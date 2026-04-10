/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of metadata.hpp.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"
#endif
#ifndef LIBARCSTK_METADATA_DETAILS_HPP_
#include "metadata_details.hpp"
#endif

#include <algorithm>     // for for_each, transform, copy
#include <array>         // for array
#include <iterator>      // for begin, cbegin, cend, end, back_inserter
#include <sstream>       // for ostringstream
#include <stdexcept>     // for invalid_argument
#include <string>        // for vector
#include <unordered_set> // for unordered_set
#include <utility>       // for move, swap
#include <vector>        // for string


namespace arcstk
{
inline namespace v_1_0_0
{

// metadata_details.hpp


namespace details
{


int32_t convert_to_bytes(const int32_t value, const UNIT unit) noexcept
{
	switch (unit)
	{
		case UNIT::FRAMES:  return convert<UNIT::FRAMES,  UNIT::BYTES>(value);
		case UNIT::SAMPLES: return convert<UNIT::SAMPLES, UNIT::BYTES>(value);
		default:            return value;
	}

	return value;
}


void validate_filenames_impl(const ToCData& toc_data,
		const std::vector<std::string>& filenames)
{
	if (const auto total_filenames = filenames.size();
			total_filenames != toc::total_tracks(toc_data))
	{
		auto ss = std::ostringstream {};
		ss << "Passed number of filenames (="
			<< total_filenames
			<< ") is not equal to total number of tracks (="
			<< toc::total_tracks(toc_data)
			<< ")";

		validate::on_invalid_tocdata(ss.str(),
				MetadataRequirement::TOTAL_FILENAMES_MATCH_TOTAL_TRACKS,
				static_cast<int32_t>(total_filenames),
				CDDA::MAX_TRACKCOUNT + 1);
	}
}


void print(std::ostream& out, const ToCData& toc_data)
{
	using std::cbegin;
	using std::cend;

	const auto sz = toc_data.size();

	if (sz < 2)
	{
		return;
	}

	// offsets

	if (sz == 2)
	{
		out << toc_data.back().frames();
	} else
	{
		const auto last_track = cend(toc_data) - 1;

		std::for_each(cbegin(toc_data) + 1, last_track,
			[&out](const ToCData::value_type& offset)
			{
				out << offset.frames() << ',';
			});

		out << last_track->frames();
	}

	// leadout

	out << ' ' << '(' << toc_data.front().frames() << ')';
}


void print(std::ostream& out, const AudioSize& s)
{
	out << s.frames() << " LBA frames";
}


namespace validate
{

int32_t exceeds_maximum(const int32_t offset)
{
	// in order, from highest to lowest
	static const std::array<int32_t, 5> MAX_FRAMES = {
		CDDA::MAX_BLOCK_ADDRESS,
		MAX_OFFSET_99,
		MAX_OFFSET_90,
		CDDA::MAX_OFFSET,
		0
	};

	using std::cbegin;
	using std::cend;

	const auto max {
		std::find_if(cbegin(MAX_FRAMES), cend(MAX_FRAMES),
			[&offset](const int32_t v) -> bool
			{
				return offset > v;
			})
	};

	// Return the highest maximum exceeded by offset or a negative value
	return (cend(MAX_FRAMES) == max) ? offset/*means < 0*/ : *max;
}


void validate_offsets_leadout(const ToCData& toc_data)
{
	using Req = MetadataRequirement;

	if (toc::leadout(toc_data).zero())
	{
		on_invalid_tocdata(Req::LEADOUT_IS_NONNEGATIVE, 0, 0);
	}

	validate_offsets(toc_data);
}


void validate_offsets(const ToCData& toc_data)
{
	// leadout is validated, but allowed to be zero

	using Req = MetadataRequirement;

	const auto total_tracks { toc::total_tracks(toc_data) };
	static const auto MORE_THAN_MAX = unsigned { 100 };

	if (total_tracks <= 0)
	{
		auto ss = std::ostringstream {};
		ss << "Number of tracks " << total_tracks << " is not non-negative";

		on_invalid_tocdata(ss.str(), Req::TOTAL_TRACKS_IS_POSITIVE,
				static_cast<int32_t>(total_tracks),
				MORE_THAN_MAX /* no index pos */);
	}

	if (total_tracks > CDDA::MAX_TRACKCOUNT)
	{
		auto ss = std::ostringstream {};
		ss << "Number of tracks " << total_tracks
			<< " is bigger than maximum of " << CDDA::MAX_TRACKCOUNT;

		on_invalid_tocdata(ss.str(), Req::TOTAL_TRACKS_IS_NOT_GREATER_THAN_MAX,
				static_cast<int32_t>(total_tracks),
				MORE_THAN_MAX /* no index pos */);
	}

	auto curr_offset = int32_t { 0 }; // current offset
	auto prev_offset = curr_offset;   // previous offset
	auto length      = int32_t { 0 }; // current track length

	auto track = ToCData::size_type { 1 }; // track number
	auto c     = ToCData::size_type { 0 }; // count comparisons

	const auto offsets { toc::offsets(toc_data) };

	while (c < total_tracks)
	{
		// organize values (do not use offset() since we access index 0)

		prev_offset = toc_data[track].frames();

		++track;
		track %= toc_data.size(); // after last track, flip back to 0

		curr_offset = toc_data[track].frames();

		++c;

		// validation

		if (curr_offset > CDDA::MAX_BLOCK_ADDRESS)// may cause length < 0
		{
			auto ss = std::ostringstream {};
			ss << "Offset " << curr_offset << " exceeds physical maximum";

			on_invalid_tocdata(ss.str(), Req::OFFSETS_ARE_NOT_GREATER_THAN_MAX,
					curr_offset, track);
		}

		if (curr_offset < 0)
		{
			auto ss = std::ostringstream {};
			ss << "Negative value " << curr_offset << " is not a valid offset";

			on_invalid_tocdata(ss.str(), Req::OFFSETS_ARE_NONNEGATIVE,
					curr_offset, track);
		}

		// OFFSETS_ARE_NOT_GREATER_THAN_MAX fires intentionally before
		// OFFSETS_ARE_STRICTLY_WELLORDERED, if both would be triggered

		// curr_offset does never point to track 1, thus it is only allowed to
		// be 0 if it points to leadout. For zero leadout only, length is 0.

		length = (curr_offset) ? curr_offset - prev_offset : 0;

		if (length < 0)
		{
			on_invalid_tocdata(Req::OFFSETS_ARE_STRICTLY_WELLORDERED,
				curr_offset, track);
		}

		if (length == 0 && track > 1)
		{
			on_invalid_tocdata(Req::OFFSETS_ARE_STRICTLY_WELLORDERED,
				curr_offset, track);
		}

		if (length > 0)
		{
			if (length < CDDA::MIN_TRACK_LEN_FRAMES /* 150 */)
			{
				auto ss = std::ostringstream {};
				ss << "Track " << track << " has less length than minimum";

				on_invalid_tocdata(ss.str(), Req::OFFSETS_PRODUCE_MIN_LENGTHS,
					curr_offset, track);
			}

			if (length < CDDA::MIN_TRACK_OFFSET_DIST /* 300 */)
			{
				auto ss = std::ostringstream {};
				ss << "The start offset of track " << track
					<< " does not have the required minimal offset distance to "
					" its predessessor track (is "
					<< length
					<< " frames but must be at least "
					<< CDDA::MIN_TRACK_OFFSET_DIST
					<< " frames)";

				on_invalid_tocdata(ss.str(), Req::OFFSETS_HAVE_MIN_DIST,
					curr_offset, track);
			}
		}
	} // while
}

void on_invalid_tocdata(const std::string& msg, const MetadataRequirement r,
		const int32_t v, const ToCData::size_type i)
{
	throw InvalidMetadataException { msg, r, v, i };
}

void on_invalid_tocdata(const MetadataRequirement r, const int32_t v,
		const ToCData::size_type i)
{
	throw InvalidMetadataException { r, v, i };
}

std::string name(const MetadataRequirement r)
{
	static const std::array<std::string, 9> names =
	{
		"OFFSETS_ARE_NONNEGATIVE",
		"OFFSETS_ARE_NOT_GREATER_THAN_MAX",
		"OFFSETS_ARE_STRICTLY_WELLORDERED",
		"OFFSETS_HAVE_MIN_DIST",
		"OFFSETS_PRODUCE_MIN_LENGTHS",
		"TOTAL_TRACKS_IS_POSITIVE",
		"TOTAL_TRACKS_IS_NOT_GREATER_THAN_MAX",
		"LEADOUT_IS_NONNEGATIVE",
		"TOTAL_FILENAMES_MATCH_TOTAL_TRACKS"
	};

	using index_type = typename std::underlying_type<MetadataRequirement>::type;

	return names.at(static_cast<index_type>(r));
}

std::string default_error_message(const MetadataRequirement r,
		const int32_t v, const ToCData::size_type i)
{
	auto ss = std::ostringstream {};
	ss << "Requirement "
		<< name(r)
		<< " was violated by value "
		<< v
		<< " on index "
		<< i;

	return ss.str();
}

} // namespace validate
} // namespace details


// metadata.hpp


// AudioSize


AudioSize::AudioSize() noexcept
	: AudioSize { 0, UNIT::BYTES }
{
	// empty
}


AudioSize::AudioSize(const int32_t value, const UNIT unit) noexcept
	: total_pcm_bytes_ { details::convert_to_bytes(value, unit) }
{
	// empty
}


int32_t AudioSize::frames() const noexcept
{
	return convert<UNIT::BYTES, UNIT::FRAMES>(total_pcm_bytes_);
}


void AudioSize::set_frames(const int32_t frames) noexcept
{
	total_pcm_bytes_ = convert<UNIT::FRAMES, UNIT::BYTES>(frames);
}


int32_t AudioSize::samples() const noexcept
{
	return convert<UNIT::BYTES, UNIT::SAMPLES>(total_pcm_bytes_);
}


void AudioSize::set_samples(const int32_t samples) noexcept
{
	total_pcm_bytes_ = convert<UNIT::SAMPLES, UNIT::BYTES>(samples);
}


int32_t AudioSize::bytes() const noexcept
{
	return total_pcm_bytes_;
}


void AudioSize::set_bytes(const int32_t bytes) noexcept
{
	total_pcm_bytes_ = bytes;
}


bool AudioSize::zero() const noexcept
{
	return 0 == bytes();
}


AudioSize::operator bool() const noexcept
{
	return !zero();
}


void AudioSize::swap(AudioSize& rhs) noexcept
{
	using std::swap;

	swap(this->total_pcm_bytes_, rhs.total_pcm_bytes_);
}


bool AudioSize::equals(const AudioSize& rhs) const noexcept
{
	return this->total_pcm_bytes_ == rhs.total_pcm_bytes_;
}


std::string AudioSize::to_string() const
{
	auto ss = std::ostringstream {};
	ss << *this;
	return ss.str();
}


std::ostream& operator << (std::ostream& out, const AudioSize& i)
{
	details::print(out, i);
	return out;
}


// ToCData


namespace toc
{

ToCData construct(const int32_t leadout, const std::vector<int32_t>& offsets)
{
	const auto unit { UNIT::FRAMES };

	auto toc = ToCData{};
	toc.reserve(1 + offsets.size());

	// Write leadout to first index position

	toc.push_back({ leadout, unit });

	// Write offsets in ascending order to index positions 1..n

	using std::cbegin;
	using std::cend;

	std::transform(cbegin(offsets), cend(offsets),
			std::back_inserter(toc),
			[](const int32_t o) -> AudioSize
			{
				return { o, unit };
			});

	//toc.shrink_to_fit(); // Commented out, possibly unnecessary

	return toc;
}


ToCData construct(const AudioSize& leadout,
		const std::vector<AudioSize>& offsets)
{
	auto toc = ToCData{};
	toc.reserve(1 + offsets.size());

	// Write leadout to first index position

	toc.push_back(leadout);

	// Write offsets in ascending order to index positions 1..n

	using std::cbegin;
	using std::cend;

	std::copy(cbegin(offsets), cend(offsets), std::back_inserter(toc));

	//toc.shrink_to_fit(); // Commented out, possibly unnecessary

	return toc;
}


unsigned total_tracks(const ToCData& data)
{
	return data.size() - 1;
}


std::vector<AudioSize> offsets(const ToCData& data)
{
	if (data.size() < 2)
	{
		return {};
	}

	using std::cbegin;
	using std::cend;

	return { cbegin(data) + 1, cend(data) };
}


AudioSize leadout(const ToCData& data)
{
	return data.empty() ? AudioSize { 0, UNIT::FRAMES } : data.front();
}


AudioSize offset(const ToCData::size_type track, const ToCData& data)
{
	if (track == 0)
	{
		throw std::out_of_range { "" };
	};

	return data.at(track);
}


AudioSize length(const ToCData::size_type track, const ToCData& data)
{
	const auto next_track = (track + 1 == data.size()) ? 0 : track + 1;

	return { data[next_track].frames() - data[track].frames(), UNIT::FRAMES };
}


void set_leadout(const AudioSize& leadout, ToCData& data)
{
	if (data.size() < 1)
	{
		data.resize(1);
	}

	data[0] = leadout;
}


void set_offset(const ToCData::size_type track, const int32_t offset,
		ToCData& data)
{
	data[track] = AudioSize { offset, UNIT::FRAMES };
}


std::vector<AudioSize> lengths(const ToCData& data)
{
	auto lengths { std::vector<AudioSize>{} };

	auto curr_offset = int32_t { 0 };
	auto prev_offset = curr_offset;

	auto track = ToCData::size_type { 1 }; // track number
	auto c     = ToCData::size_type { 0 }; // count comparisons

	const auto total_tracks { toc::total_tracks(data) };
	while (c < total_tracks)
	{
		prev_offset = data[track].frames();

		++track;
		track %= data.size(); // after last track, flip back to 0

		curr_offset = data[track].frames();

		if (curr_offset) // skip zero leadout
		{
			lengths.emplace_back(curr_offset - prev_offset, UNIT::FRAMES);
		}

		++c;
	}

	return lengths;
}


bool complete(const ToCData& data)
{
	return !data.empty()  &&  !data[0].zero()  &&  total_tracks(data) > 0;
}


void validate_with_completeness(const ToCData& toc_data)
{
	details::validate::validate_offsets_leadout(toc_data);
}


void validate_without_completeness(const ToCData& toc_data)
{
	details::validate::validate_offsets(toc_data);
}


std::string to_string(const ToCData& toc_data)
{
	auto stream = std::ostringstream {};
	details::print(stream, toc_data);
	return stream.str();
}

} // namespace toc


// ToC::Impl


ToC::Impl::Impl(const ToCData& toc, const std::vector<std::string>& filenames)
	: toc_       { toc       }
	, filenames_ { filenames }
{
	// empty
}


unsigned ToC::Impl::total_tracks() const noexcept
{
	return toc::total_tracks(toc_);
}


void ToC::Impl::set_leadout(const AudioSize& l) noexcept
{
	toc::set_leadout(l, toc_);
}


AudioSize ToC::Impl::leadout() const noexcept
{
	return toc::leadout(toc_);
}


std::vector<AudioSize> ToC::Impl::offsets() const
{
	return toc::offsets(toc_);
}


std::vector<std::string> ToC::Impl::filenames() const
{
	return filenames_;
}


bool ToC::Impl::has_filenames() const noexcept
{
	return !filenames_.empty();
}


bool ToC::Impl::is_single_file() const noexcept
{
	if (filenames_.size() == 1)
	{
		return true;
	}

	using std::cbegin;
	using std::cend;
	std::unordered_set<std::string> names(cbegin(filenames_), cend(filenames_));

	return names.size() == 1;
}


void ToC::Impl::validate() const
{
	if (complete())
	{
		toc::validate_without_completeness(toc_);
	} else
	{
		toc::validate_with_completeness(toc_);
	}

	if (!filenames_.empty())
	{
		details::validate_filenames_impl(toc_, filenames_);
	}
}


bool ToC::Impl::complete() const noexcept
{
	return toc::complete(toc_);
}


void ToC::Impl::print(std::ostream& out)
{
	details::print(out, toc_);
}


bool ToC::Impl::empty() const noexcept
{
	return toc_.empty();
}


void ToC::Impl::swap(Impl& rhs) noexcept
{
	using std::swap;

	swap(this->toc_,       rhs.toc_);
	swap(this->filenames_, rhs.filenames_);
}


bool ToC::Impl::equals(const Impl& rhs) const noexcept
{
	return this->toc_ == rhs.toc_ && this->filenames_ == rhs.filenames_;
}


std::string ToC::Impl::to_string() const
{
	return toc::to_string(toc_);
}


// ToC


ToC::ToC(const ToCData& toc_data, const std::vector<std::string>& filenames)
	: impl_ { std::make_unique<ToC::Impl>(toc_data, filenames) }
{
	// empty
}


ToC::ToC(const ToCData& toc_data)
	: impl_ { std::make_unique<ToC::Impl>(
					toc_data, std::vector<std::string>{/* empty */}) }
{
	// empty
}


ToC::ToC(const ToC& rhs)
	: impl_ { std::make_unique<ToC::Impl>(*rhs.impl_) }
{
	// empty
}


ToC& ToC::operator = (const ToC& rhs)
{
	if (&rhs == this)
	{
		auto tmp = std::make_unique<ToC::Impl>(*rhs.impl_);
		impl_ = std::move(tmp);
	}
	return *this;
}


ToC::ToC(ToC&& rhs) noexcept = default;


ToC& ToC::operator = (ToC&& rhs) noexcept = default;


ToC::~ToC() noexcept = default; // Pimpl requirement


unsigned ToC::total_tracks() const noexcept
{
	return impl_->total_tracks();
}


void ToC::set_leadout(const AudioSize& l) noexcept
{
	impl_->set_leadout(l);
}


AudioSize ToC::leadout() const noexcept
{
	return impl_->leadout();
}


std::vector<AudioSize> ToC::offsets() const
{
	return impl_->offsets();
}


std::vector<std::string> ToC::filenames() const
{
	return impl_->filenames();
}


void ToC::validate() const
{
	impl_->validate();
}


bool ToC::valid() const noexcept
{
	try
	{
		validate();

	} catch (...)
	{
		return false;
	}

	return true;
}


bool ToC::complete() const noexcept
{
	return impl_->complete();
}


bool ToC::empty() const noexcept
{
	return impl_->empty();
}


ToC::operator bool() const noexcept
{
	return !empty();
}


void ToC::swap(const ToC& rhs) const noexcept
{
	impl_->swap(*rhs.impl_);
}


bool ToC::equals(const ToC& rhs) const noexcept
{
	return impl_->equals(*rhs.impl_);
}


std::string ToC::to_string() const
{
	return impl_->to_string();
}


// make_toc


ToC make_toc(const int32_t leadout, const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	return ToC { toc::construct(leadout, offsets), filenames };
}


ToC make_toc(const int32_t leadout, const std::vector<int32_t>& offsets)
{
	return ToC { toc::construct(leadout, offsets) };
}


ToC make_toc(const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	return make_toc(0, offsets, filenames);
}


ToC make_toc(const std::vector<int32_t>& offsets)
{
	return make_toc(0, offsets);
}


// validated_toc


ToC validated_toc(const int32_t leadout, const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	const auto toc_data { toc::construct(leadout, offsets) };

	details::validate_filenames_impl(toc_data, filenames);

	if (leadout)
	{
		toc::validate_with_completeness(toc_data);
	} else
	{
		toc::validate_without_completeness(toc_data);
	}

	return ToC { toc_data, filenames };
}


ToC validated_toc(const int32_t leadout, const std::vector<int32_t>& offsets)
{
	const auto toc_data { toc::construct(leadout, offsets) };

	if (leadout)
	{
		toc::validate_with_completeness(toc_data);
	} else
	{
		toc::validate_without_completeness(toc_data);
	}

	return ToC { toc_data };
}


ToC validated_toc(const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	const auto toc_data { toc::construct(0, offsets) };

	details::validate_filenames_impl(toc_data, filenames);
	toc::validate_without_completeness(toc_data);

	return ToC { toc_data, filenames };
}


ToC validated_toc(const std::vector<int32_t>& offsets)
{
	const auto toc_data { toc::construct(0, offsets) };

	toc::validate_without_completeness(toc_data);

	return ToC { toc_data };
}


// ToC::operator <<


std::ostream& operator << (std::ostream& out, const ToC& toc)
{
	toc.impl_->print(out);
	return out;
}


// EmptyToC


const ToC EmptyToC = ToC { {}, {} };


// InvalidMetadataException


InvalidMetadataException::InvalidMetadataException(const std::string& what_arg,
	const MetadataRequirement r, const int32_t v, const ToCData::size_type i)
	: std::runtime_error { what_arg }
	, req_   { r }
	, value_ { v }
	, index_ { i }
{
	// empty
}


InvalidMetadataException::InvalidMetadataException(const MetadataRequirement r,
	const int32_t v, const ToCData::size_type i)
	: std::runtime_error { details::validate::default_error_message(r, v, i) }
	, req_   { r }
	, value_ { v }
	, index_ { i }
{
	// empty
}


InvalidMetadataException::InvalidMetadataException(const std::string& what_arg)
	: std::runtime_error { what_arg }
	, req_   { 0 }
	, value_ { 0 }
	, index_ { 0 }
{
	// empty
}


InvalidMetadataException::InvalidMetadataException(const char *what_arg)
	: std::runtime_error { what_arg }
	, req_   { 0 }
	, value_ { 0 }
	, index_ { 0 }
{
	// empty
}

MetadataRequirement InvalidMetadataException::requirement() const
{
	return req_;
}

int32_t InvalidMetadataException::violating_value() const
{
	return value_;
}

ToCData::size_type InvalidMetadataException::pos() const
{
	return index_;
}

} // namespace v_1_0_0
} // namespace arcstk

