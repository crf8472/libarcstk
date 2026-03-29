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

#include <algorithm>     // for for_each, transform
#include <iterator>      // for begin, cbegin, cend, end
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


void validate_leadout_impl(const int32_t leadout)
{
	// Note: zero leadout is validated

	validate::is_legal_offset(leadout);

	if (leadout > 0 && leadout < CDDA::MIN_TRACK_OFFSET_DIST)
	{
		auto ss = std::ostringstream {};
		ss << "Leadout " << leadout << " is smaller than minimum track length";
		validate::throw_on_invalid_tocdata(ss.str());
	}
}


void validate_filenames_impl(const ToCData& toc_data,
		const std::vector<std::string>& filenames)
{
	if (filenames.size() != toc::total_tracks(toc_data))
	{
		auto ss = std::ostringstream {};
		ss << "Passed number of filenames (="
			<< filenames.size()
			<< ") is not equal to total number of tracks (="
			<< toc::total_tracks(toc_data)
			<< ")";
		validate::throw_on_invalid_tocdata(ss.str());
	}
}


namespace validate
{

void is_legal_offset(const int32_t offset)
{
	using std::to_string;

	if (offset < 0)
	{
		auto ss = std::ostringstream {};
		ss << "Negative value " << offset << " is not an offset";
		throw_on_invalid_tocdata(ss.str());
	}

	if (offset > CDDA::MAX_BLOCK_ADDRESS)
	{
		auto ss = std::ostringstream {};
		ss << "Offset " << offset << " exceeds physical maximum";
		throw_on_invalid_tocdata(ss.str());
	}

	if (offset > MAX_OFFSET_99)
	{
		auto ss = std::ostringstream {};
		ss << "Offset exceeds physical range of 99 min ("
				<< to_string(MAX_OFFSET_99) << " offset)";
		throw_on_invalid_tocdata(ss.str());
	}

	if (offset > MAX_OFFSET_90)
	{
		auto ss = std::ostringstream {};
		ss << "Offset exceeds frame "
			<< to_string(MAX_OFFSET_90) << " (90 min)";
		throw_on_invalid_tocdata(ss.str());
	}

	if (offset > CDDA::MAX_OFFSET)
	{
		auto ss = std::ostringstream {};
		ss << "Offset " << offset << " exceeds redbook maximum";
		throw_on_invalid_tocdata(ss.str());
	}
}


void is_legal_length(const int32_t length)
{
	if (length < CDDA::MIN_TRACK_LEN_FRAMES)
	{
		using std::to_string;

		throw_on_invalid_tocdata(to_string(length));
	}
}


void validate_leadout(const ToCData& toc_data)
{
	const auto leadout { toc::leadout(toc_data).frames() };

	details::validate_leadout_impl(leadout);
}


void validate_nonzero_leadout(const ToCData& toc_data)
{
	const auto leadout { toc::leadout(toc_data).frames() };

	if (!leadout)
	{
		auto ss = std::ostringstream {};
		ss << "Leadout " << leadout << " is missing (is actually zero)";
		throw_on_invalid_tocdata(ss.str());
	}

	details::validate_leadout_impl(leadout);
}


void validate_offsets(const ToCData& toc_data)
{
	const auto offsets { toc::offsets(toc_data) };

	if (offsets.size() > CDDA::MAX_TRACKCOUNT)
	{
		auto ss = std::ostringstream {};
		ss << "Number of tracks " << offsets.size()
			<< " is bigger than maximum of " << CDDA::MAX_TRACKCOUNT;
		throw_on_invalid_tocdata(ss.str());
	}

	using std::cbegin;
	using std::cend;

	auto pred { cbegin(offsets) };

	is_legal_offset(pred->frames());

	auto track { 1 };
	std::for_each(cbegin(offsets) + 1, cend(offsets),
		[&pred,&track](const AudioSize& a)
		{
			is_legal_offset(a.frames());

			// Check required minimal distance to predecessor

			++track;
			if (pred->frames() - a.frames() < CDDA::MIN_TRACK_OFFSET_DIST)
			{
				auto ss = std::ostringstream {};
				ss << "The start offset of track " << track
					<< " does not have the required minimal offset distance to "
					" its predessessor track (is "
					<< (pred->frames() - a.frames())
					<< " frames but required is at least "
					<< CDDA::MIN_TRACK_OFFSET_DIST
					<< " frames)";
				throw_on_invalid_tocdata(ss.str());
			}
			++pred;
		}
	);
}


void validate_lengths(const ToCData& toc_data)
{
	const auto lengths { toc::lengths(toc_data) };

	using std::cbegin;
	using std::cend;

	auto track { 1 };
	std::for_each(cbegin(lengths), cend(lengths),
		[&track](const AudioSize& a)
		{
			try
			{
				is_legal_length(a.frames());
			}
			catch (const InvalidMetadataException& e)
			{
				using std::to_string;
				auto ss = std::ostringstream {};
				ss << "Illegal length: Track " << track
					<< " is too short (length is " << e.what() << " frames)";

				throw_on_invalid_tocdata(ss.str());
			}

			++track;
		}
	);
}


void throw_on_invalid_tocdata(const std::string& msg)
{
	throw InvalidMetadataException { msg };
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
	using std::to_string;

	return to_string(frames()) + " LBA frames";
}


// ToCData


namespace toc
{

ToCData construct(const int32_t leadout, const std::vector<int32_t>& offsets)
{
	const auto unit { UNIT::FRAMES };
	auto toc = ToCData(1 + offsets.size());

	// Write leadout to first index position

	using std::begin;
	using std::end;

	auto ptr { begin(toc) };
	*ptr = AudioSize { leadout, unit };
	++ptr;

	// Write tracks in ascending order to index positions 1..n

	using std::cbegin;
	using std::cend;

	std::transform(cbegin(offsets), cend(offsets), ptr,
			[](const int32_t o) -> AudioSize
			{
				return AudioSize { o, unit };
			});

	toc.shrink_to_fit();

	return toc;
}


ToCData construct(const AudioSize& leadout,
		const std::vector<AudioSize>& offsets)
{
	auto toc = offsets;

	using std::begin;
	toc.insert(begin(toc), leadout);

	toc.shrink_to_fit(); // For safety. Required?

	return toc;
}


void set_leadout(const AudioSize& leadout, ToCData& data)
{
	if (data.size() < 1)
	{
		data.resize(1);
	}

	data[0] = leadout;
}


AudioSize leadout(const ToCData& data)
{
	return data.empty() ? AudioSize { 0, UNIT::FRAMES } : data.front();
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


unsigned total_tracks(const ToCData& data)
{
	return data.size() - 1;
}


bool complete(const ToCData& data)
{
	return !data.empty()  &&  !data[0].zero()  &&  total_tracks(data) > 0;
}


void validate_with_completeness(const ToCData& toc_data)
{
	details::validate::validate_nonzero_leadout(toc_data);
	details::validate::validate_offsets(toc_data);
	details::validate::validate_lengths(toc_data);
}


void validate_without_completeness(const ToCData& toc_data)
{
	details::validate::validate_leadout(toc_data);
	details::validate::validate_offsets(toc_data);
	details::validate::validate_lengths(toc_data);
}

} // namespace toc


// ToC::Impl


ToC::Impl::Impl(const ToCData& toc, const std::vector<std::string>& filenames)
	: toc_       { toc       }
	, filenames_ { filenames }
{
	// empty
}


ToC::Impl::Impl(const Impl& rhs)
	: toc_       { rhs.toc_       }
	, filenames_ { rhs.filenames_ }
{
	// empty
}


ToC::Impl& ToC::Impl::operator = (const Impl& rhs)
{
	using std::swap;

	Impl tmp { rhs };
	swap(*this, tmp);
	return *this;
}


ToC::Impl::Impl(Impl&& rhs) noexcept
	: toc_        { std::move(rhs.toc_)       }
	, filenames_  { std::move(rhs.filenames_) }
{
	// empty
}


ToC::Impl& ToC::Impl::operator = (Impl&& rhs) noexcept
{
	toc_       = std::move(rhs.toc_);
	filenames_ = std::move(rhs.filenames_);
	return *this;
}


ToC::Impl::~Impl() noexcept = default;


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


void ToC::Impl::swap(Impl& rhs) noexcept
{
	using std::swap;

	swap(this->toc_,       rhs.toc_);
	swap(this->filenames_, rhs.filenames_);
}


bool ToC::Impl::equals(const Impl& rhs) noexcept
{
	return this->toc_ == rhs.toc_ && this->filenames_ == rhs.filenames_;
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
	impl_ = std::make_unique<ToC::Impl>(*rhs.impl_);
	return *this;
}


ToC::ToC(ToC&& rhs) noexcept
	: impl_ { std::move(rhs.impl_) }
{
	// empty
}


ToC& ToC::operator = (ToC&& rhs) noexcept
{
	impl_ = std::move(rhs.impl_);
	return *this;
}


ToC::~ToC() noexcept = default;


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


void ToC::swap(const ToC& rhs) const noexcept
{
	impl_->swap(*rhs.impl_);
}


bool ToC::equals(const ToC& rhs) const noexcept
{
	return impl_->equals(*rhs.impl_);
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
	toc::validate_with_completeness(toc_data);

	return ToC { toc_data, filenames };
}


ToC validated_toc(const int32_t leadout, const std::vector<int32_t>& offsets)
{
	const auto toc_data { toc::construct(leadout, offsets) };

	toc::validate_with_completeness(toc_data);

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


// InvalidMetadataException


InvalidMetadataException::InvalidMetadataException(const std::string& what_arg)
	: std::runtime_error { what_arg }
{
	// empty
}


InvalidMetadataException::InvalidMetadataException(const char *what_arg)
	: std::runtime_error { what_arg }
{
	// empty
}

} // namespace v_1_0_0
} // namespace arcstk

