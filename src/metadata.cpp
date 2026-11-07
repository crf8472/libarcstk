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

#include <algorithm>     // for find_if, for_each, transform, copy
#include <array>         // for array
#include <cstdint>       // for int32_t
#include <iterator>      // for begin, cbegin, cend, end, inserter
#include <memory>        // for make_unique
#include <ostream>       // for ostream
#include <sstream>       // for ostringstream
#include <stdexcept>     // for invalid_argument
#include <string>        // for string
#include <set>           // for set
#include <type_traits>   // for underlying_type
#include <utility>       // for move, swap
#include <vector>        // for vector

namespace arcstk
{
inline namespace v_1_0_0
{

// metadata_details.hpp


namespace toc
{
namespace details
{

void print(std::ostream& out, const ToCData& toc_data)
{
	if (!out.good())
	{
		// Maybe set badbit: out.setstate(std::ios_base::badbit);
		return;
	}

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


void validate_offsets(const ToCData& toc_data)
{
	// leadout is validated, but allowed to be zero

	using Req = MetadataRequirement;

	const auto total_tracks { toc::total_tracks(toc_data) };
	static const auto OUT_OF_TRACKS = unsigned { CDDA::MAX_TRACKCOUNT + 1 };

	if (total_tracks == 0)
	{
		auto ss = std::ostringstream {};
		ss << "Number of tracks " << total_tracks << " is zero";

		toc::req::on_invalid_tocdata(ss.str(), Req::TOTAL_TRACKS_IS_POSITIVE,
				static_cast<std::int32_t>(total_tracks),
				OUT_OF_TRACKS /* no index pos */);
	}

	if (total_tracks > CDDA::MAX_TRACKCOUNT)
	{
		auto ss = std::ostringstream {};
		ss << "Number of tracks " << total_tracks
			<< " is bigger than maximum of " << CDDA::MAX_TRACKCOUNT;

		toc::req::on_invalid_tocdata(ss.str(),
				Req::TOTAL_TRACKS_IS_NOT_GREATER_THAN_MAX,
				static_cast<std::int32_t>(total_tracks),
				OUT_OF_TRACKS /* no index pos */);
	}

	auto curr_offset = std::int32_t { 0 }; // current offset
	auto prev_offset = curr_offset;   // previous offset
	auto length      = std::int32_t { 0 }; // current track length

	auto track = ToCData::size_type { 1 }; // track number
	auto c     = ToCData::size_type { 0 }; // count comparisons

	while (c < total_tracks) // size() > 0 must be ensured
	{
		// organize values (do not use offset() since we access index 0)

		prev_offset = toc_data[track].frames();

		++track;
		track %= toc_data.size(); // after last track, flip back to 0

		curr_offset = toc_data[track].frames();

		++c;

		// validation

		if (curr_offset < 0)
		{
			auto ss = std::ostringstream {};
			ss << "Negative value " << curr_offset << " is not a valid offset";

			toc::req::on_invalid_tocdata(ss.str(), Req::OFFSETS_ARE_NONNEGATIVE,
					curr_offset, track);
		}

		if (const auto ovf = arcstk::details::exceeds_maximum(curr_offset);
				ovf > 0)
		{
			auto ss = std::ostringstream {};

			if (ovf == CDDA::MAX_BLOCK_ADDRESS)// may cause length < 0
			{
				ss << "Offset " << curr_offset << " exceeds physical maximum";
			}

			// Commented out: not invalid, just non-standard

			// if (ovf == arcstk::details::MAX_OFFSET_99)
			// {
			// 	ss << "Offset " << curr_offset << " exceeds redbook 99";
			// }
			//
			// if (ovf == arcstk::details::MAX_OFFSET_90)
			// {
			// 	ss << "Offset " << curr_offset << " exceeds redbook 90";
			// }
			//
			// if (ovf == CDDA::MAX_OFFSET)
			// {
			// 	ss << "Offset " << curr_offset
			// 		<< " exceeds legal maximum offset";
			// }

			toc::req::on_invalid_tocdata(ss.str(),
					Req::OFFSETS_ARE_NOT_GREATER_THAN_MAX, curr_offset, track);
		}

		// OFFSETS_ARE_NOT_GREATER_THAN_MAX fires intentionally before
		// OFFSETS_ARE_STRICTLY_WELLORDERED, if both would be triggered

		// curr_offset does never point to track 1, thus it is only allowed to
		// be 0 if it points to leadout. For zero leadout only, length is 0.

		length = (curr_offset) ? curr_offset - prev_offset : 0;

		if (length < 0)
		{
			toc::req::on_invalid_tocdata(Req::OFFSETS_ARE_STRICTLY_WELLORDERED,
				curr_offset, track);
		}

		if (length == 0 && track > 1)
		{
			toc::req::on_invalid_tocdata(Req::OFFSETS_ARE_STRICTLY_WELLORDERED,
				curr_offset, track);
		}

		if (length > 0)
		{
			if (length < CDDA::MIN_TRACK_LEN_FRAMES /* 150 */)
			{
				auto ss = std::ostringstream {};
				ss << "Track " << track << " has less length than minimum";

				toc::req::on_invalid_tocdata(ss.str(),
					Req::OFFSETS_PRODUCE_MIN_LENGTHS, curr_offset, track);
			}

			if (length < CDDA::MIN_TRACK_OFFSET_DIST /* 300 */)
			{
				auto ss = std::ostringstream {};
				ss << "The start offset of track " << track
					<< " does not have the required minimal offset distance to"
					" its predecessor track (is "
					<< length
					<< " frames but must be at least "
					<< CDDA::MIN_TRACK_OFFSET_DIST
					<< " frames)";

				toc::req::on_invalid_tocdata(ss.str(),
					Req::OFFSETS_HAVE_MIN_DIST, curr_offset, track);
			}
		}
	} // while
}


void validate_offsets_and_leadout(const ToCData& toc_data)
{
	using Req = MetadataRequirement;

	if (toc::leadout(toc_data).zero())
	{
		toc::req::on_invalid_tocdata(Req::LEADOUT_IS_POSITIVE, 0, 0);
	}

	validate_offsets(toc_data);
}


void validate_filenames(const ToCData& toc_data,
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

		toc::req::on_invalid_tocdata(ss.str(),
				MetadataRequirement::TOTAL_FILENAMES_MATCH_TOTAL_TRACKS,
				static_cast<std::int32_t>(total_filenames),
				CDDA::MAX_TRACKCOUNT + 1);
	}
}

} // namespace details

namespace req
{

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
		"LEADOUT_IS_POSITIVE",
		"TOTAL_FILENAMES_MATCH_TOTAL_TRACKS"
	};

	using index_type = typename std::underlying_type_t<MetadataRequirement>;

	return names.at(static_cast<index_type>(r));
}


void on_invalid_tocdata(const std::string& msg, const MetadataRequirement r,
		const std::int32_t v, const ToCData::size_type i)
{
	throw InvalidMetadataException { msg, r, v, i };
}


void on_invalid_tocdata(const MetadataRequirement r, const std::int32_t v,
		const ToCData::size_type i)
{
	throw InvalidMetadataException { r, v, i };
}


std::string default_error_message(const MetadataRequirement r,
		const std::int32_t v, const ToCData::size_type i)
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

} // namespace req
} // namespace toc


namespace details
{

std::int32_t exceeds_maximum(const std::int32_t offset)
{
	// in order, from highest to lowest
	static const std::array<std::int32_t, 5> MAX_FRAMES = {
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
			[&offset](const std::int32_t v) -> bool
			{
				return offset > v;
			})
	};

	// Return the highest maximum exceeded by offset or a negative value
	return (cend(MAX_FRAMES) == max) ? offset/*means < 0*/ : *max;
}

} // namespace details


// metadata.hpp


// ToCData


namespace toc
{

ToCData construct(const std::int32_t leadout,
		const std::vector<std::int32_t>& offsets)
{
	auto toc = ToCData{};
	toc.reserve(1 + offsets.size());

	set_leadout({ leadout, UNIT::FRAMES }, toc);
	set_offsets(offsets,                   toc);

	return toc;
}


ToCData construct(const AudioSize& leadout,
		const std::vector<AudioSize>& offsets)
{
	auto toc = ToCData{};
	toc.reserve(1 + offsets.size());

	set_leadout(leadout, toc);
	set_offsets(offsets, toc);

	return toc;
}


std::size_t total_tracks(const ToCData& data) noexcept
{
	return data.size() > 0 ? data.size() - 1 : 0;
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


AudioSize leadout(const ToCData& data) noexcept
{
	return data.empty() ? AudioSize { 0, UNIT::FRAMES } : data.front();
}


AudioSize offset(const ToCData::size_type track, const ToCData& data)
{
	if (track == 0)
	{
		throw std::out_of_range {
			"Requested offset for leadout: has no offset" };
	}

	if (track >= data.size())
	{
		auto msg = std::ostringstream {};
		msg << "Requested offset for track " << track << " but last track is "
			<< toc::total_tracks(data);

		throw std::out_of_range { msg.str() };
	}

	return data.at(track);
}


AudioSize length(const ToCData::size_type track, const ToCData& data)
{
	if (track == 0)
	{
		throw std::out_of_range {
			"Requested length for leadout: has no length" };
	}

	if (track >= data.size())
	{
		auto msg = std::ostringstream {};
		msg << "Requested length for track " << track << " but last track is "
			<< total_tracks(data);

		throw std::out_of_range { msg.str() };
	}

	const auto next_track = (track + 1 == data.size()) ? 0 : track + 1;

	if (next_track == 0) // leadout 0 conforming to validate_offsets()
	{
		return { 0, UNIT::BYTES };
	}

	return { data.at(next_track).frames() - data.at(track).frames(),
		UNIT::FRAMES };
}


void set_leadout(const AudioSize& leadout, ToCData& data)
{
	if (data.size() < 1)
	{
		data.resize(1);
	}

	data[0] = leadout;
}


void set_offset(const ToCData::size_type track, const std::int32_t offset,
		ToCData& data)
{
	if (track == 0)
	{
		throw std::out_of_range {
			"Requested to set offset for leadout: gets no offset" };
	}

	if (track >= data.size())
	{
		auto msg = std::ostringstream {};
		msg << "Requested to set offset for track " << track
			<< " but last track is "
			<< toc::total_tracks(data);

		throw std::out_of_range { msg.str() };
	}

	data[track] = AudioSize { offset, UNIT::FRAMES };
}


void set_offsets(const std::vector<std::int32_t>& offsets, ToCData& data)
{
	using std::begin;
	using std::cbegin;
	using std::cend;

	data.resize(offsets.size() + 1);
	std::transform(cbegin(offsets), cend(offsets),
			std::next(begin(data)),
			[](const std::int32_t o) -> AudioSize
			{
				return { o, UNIT::FRAMES };
			});
}


void set_offsets(const std::vector<AudioSize>& offsets, ToCData& data)
{
	using std::begin;
	using std::cbegin;
	using std::cend;

	data.resize(offsets.size() + 1);
	std::copy(cbegin(offsets), cend(offsets), std::next(begin(data)));
}


std::vector<AudioSize> lengths(const ToCData& data)
{
	auto lengths { std::vector<AudioSize>{} };

	auto curr_offset = std::int32_t { 0 };
	auto prev_offset = curr_offset;

	auto track = ToCData::size_type { 1 }; // track number
	auto c     = ToCData::size_type { 0 }; // count comparisons

	const auto total_tracks { toc::total_tracks(data) };
	while (c < total_tracks) // ensure size() > 0
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


void validate(const ToCData& toc_data)
{
	details::validate_offsets(toc_data);
}


void validate_with_completeness(const ToCData& toc_data)
{
	details::validate_offsets_and_leadout(toc_data);
}


bool is_empty(const ToCData& toc_data)
{
	return toc_data.empty();
}


std::string to_string(const ToCData& toc_data)
{
	auto stream = std::ostringstream {};
	details::print(stream, toc_data);
	return stream.str();
}

} // namespace toc


// ToC::Impl


ToC::Impl::Impl(ToCData toc, std::vector<std::string> filenames)
	: toc_       { std::move(toc)       }
	, filenames_ { std::move(filenames) }
{
	// empty
}


std::size_t ToC::Impl::total_tracks() const noexcept
{
	return toc::total_tracks(toc_);
}


std::vector<AudioSize> ToC::Impl::offsets() const
{
	return toc::offsets(toc_);
}


AudioSize ToC::Impl::leadout() const noexcept
{
	return toc::leadout(toc_);
}


std::vector<std::string> ToC::Impl::filenames() const
{
	return filenames_;
}


void ToC::Impl::set_offsets(const std::vector<AudioSize>& offsets)
{
	toc::set_offsets(offsets, toc_);
}


void ToC::Impl::set_leadout(const AudioSize& l)
{
	toc::set_leadout(l, toc_);
}


void ToC::Impl::set_filenames(const std::vector<std::string>& filenames)
{
	filenames_ = filenames;
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

	const std::set<std::string> names(cbegin(filenames_), cend(filenames_));

	return names.size() == 1;
}


void ToC::Impl::validate() const
{
	toc::validate(toc_); // with or without leadout

	if (!filenames_.empty())
	{
		toc::details::validate_filenames(toc_, filenames_);
	}
}


bool ToC::Impl::complete() const noexcept
{
	return toc::complete(toc_);
}


void ToC::Impl::print(std::ostream& out) const
{
	toc::details::print(out, toc_);
}


bool ToC::Impl::empty() const noexcept
{
	return toc::is_empty(toc_);
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


ToC::ToC()
	: impl_ { std::make_unique<ToC::Impl>() }
{
	// empty
}


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
	ToC tmp { rhs };
	swap(tmp);
	return *this;
}


ToC::ToC(ToC&& rhs) noexcept = default;


ToC& ToC::operator = (ToC&& rhs) noexcept = default;


ToC::~ToC() noexcept = default; // Pimpl requirement


std::size_t ToC::total_tracks() const noexcept
{
	return impl_->total_tracks();
}


std::vector<AudioSize> ToC::offsets() const
{
	return impl_->offsets();
}


AudioSize ToC::leadout() const noexcept
{
	return impl_->leadout();
}


std::vector<std::string> ToC::filenames() const
{
	return impl_->filenames();
}


void ToC::set_offsets(const std::vector<AudioSize>& offsets)
{
	impl_->set_offsets(offsets);
}


void ToC::set_leadout(const AudioSize& l)
{
	impl_->set_leadout(l);
}


void ToC::set_filenames(const std::vector<std::string>& filenames)
{
	impl_->set_filenames(filenames);
}


void ToC::validate() const
{
	impl_->validate();
}


bool ToC::valid() const
{
	try
	{
		validate();

	} catch (const InvalidMetadataException&)
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


void ToC::swap(ToC& rhs) noexcept
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


ToC make_toc(const std::int32_t leadout,
		const std::vector<std::int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	return ToC { toc::construct(leadout, offsets), filenames };
}


ToC make_toc(const std::int32_t leadout,
		const std::vector<std::int32_t>& offsets)
{
	return ToC { toc::construct(leadout, offsets) };
}


ToC make_toc(const std::vector<std::int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	return make_toc(0, offsets, filenames);
}


ToC make_toc(const std::vector<std::int32_t>& offsets)
{
	return make_toc(0, offsets);
}


// validated_toc


ToC validated_toc(const std::int32_t leadout,
		const std::vector<std::int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	const auto toc_data { toc::construct(leadout, offsets) };

	toc::validate(toc_data);
	toc::details::validate_filenames(toc_data, filenames);

	return ToC { toc_data, filenames };
}


ToC validated_toc(const std::int32_t leadout,
		const std::vector<std::int32_t>& offsets)
{
	const auto toc_data { toc::construct(leadout, offsets) };

	toc::validate(toc_data);

	return ToC { toc_data };
}


ToC validated_toc(const std::vector<std::int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	const auto toc_data { toc::construct(0, offsets) };

	toc::validate(toc_data);
	toc::details::validate_filenames(toc_data, filenames);

	return ToC { toc_data, filenames };
}


ToC validated_toc(const std::vector<std::int32_t>& offsets)
{
	const auto toc_data { toc::construct(0, offsets) };

	toc::validate(toc_data);

	return ToC { toc_data };
}


// ToC::operator <<


std::ostream& operator << (std::ostream& out, const ToC& toc)
{
	toc.impl_->print(out);
	return out;
}


// InvalidMetadataException


InvalidMetadataException::InvalidMetadataException(const std::string& what_arg,
	const MetadataRequirement r, const std::int32_t v,
	const ToCData::size_type i)
	: std::runtime_error { what_arg }
	, req_   { r }
	, value_ { v }
	, index_ { i }
{
	// empty
}


InvalidMetadataException::InvalidMetadataException(const MetadataRequirement r,
	const std::int32_t v, const ToCData::size_type i)
	: std::runtime_error { toc::req::default_error_message(r, v, i) }
	, req_   { r }
	, value_ { v }
	, index_ { i }
{
	// empty
}


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

MetadataRequirement InvalidMetadataException::requirement() const noexcept
{
	return req_;
}

std::int32_t InvalidMetadataException::violating_value() const noexcept
{
	return value_;
}

ToCData::size_type InvalidMetadataException::pos() const noexcept
{
	return index_;
}

} // namespace v_1_0_0
} // namespace arcstk

