/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of a low-level API for representing AccurateRip ids
 */

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"
#endif
#ifndef LIBARCSTK_IDENTIFIER_DETAILS_HPP_
#include "identifier_details.hpp"
#endif

#include <cstdint>           // for int32_t, uint32_t, uint64_t
#include <iomanip>           // for operator<<, setw, setfill
#include <memory>            // for unique_ptr, make_unique, operator==
#include <sstream>           // for operator<<, basic_ostream, basic_strings...
#include <string>            // for string, operator<<, char_traits
#include <vector>            // for vector, vector<>::size_type

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"      // for AudioSize, CDDA, ToC
#endif


namespace arcstk
{
inline namespace v_1_0_0
{

const ARId EmptyARId = ARId { 0, 0, 0, 0 }; // defines emptiness for ARId


// identifier_details.hpp

namespace details
{


uint32_t disc_id_1(const std::vector<int32_t>& offsets, const int32_t leadout)
	noexcept
{
	// disc id 1 is just the sum off all offsets + the leadout frame

	auto accum = int32_t { 0 };

	for (const auto& o : offsets)
	{
		accum += o;
	}

	return static_cast<uint32_t>(accum + leadout);
}


uint32_t disc_id_2(const std::vector<int32_t>& offsets, const int32_t leadout)
	noexcept
{
	// disc id 2 is the sum of the products of offsets and the corresponding
	// 1-based track number while normalizing offsets to be >= 1

	auto accum = int32_t { 0 };
	auto track { 1 };

	for (const auto& o : offsets)
	{
		accum += (o > 0 ? o : 1) * track;
		++track;
	}

	return static_cast<uint32_t>(accum + leadout * track);
}


uint32_t cddb_id(const std::vector<int32_t>& offsets, const int32_t leadout)
{
	const auto fps { static_cast<uint32_t>(CDDA::FRAMES_PER_SEC) };


	const auto start_audio = uint32_t { offsets.empty()
		? 0
		: static_cast<uint32_t>(offsets.front()) };

	const auto total_seconds = uint32_t {
		static_cast<uint32_t>(leadout) / fps  -  start_audio / fps };
			//static_cast<uint32_t>(offsets[0]) / fps };

	auto accum = uint32_t { 0 };
	for (const auto& o : offsets)
	{
		accum += sum_digits(static_cast<uint32_t>(o) / fps + 2u);
	}
	accum %= 0xFF; // normalize to 1 byte


	// since 0 <= offsets.size <= 99 narrowing is no problem
	const auto track_count = static_cast<uint32_t>(offsets.size());


	return (accum << 24u) | (total_seconds << 8u) | track_count;
}


uint64_t sum_digits(const uint32_t number) noexcept
{
	return (number < 10) ? number : (number % 10) + sum_digits(number / 10);
}


unsigned normalize_trackcount(const std::size_t track_count) noexcept
{
	if (track_count > CDDA::MAX_TRACKCOUNT) { return CDDA::MAX_TRACKCOUNT; }

	return static_cast<unsigned>(track_count);
}


unsigned normalize_trackcount(const int track_count) noexcept
{
	/* legal track_count is between 0 and CDDA::MAX_TRACKCOUNT */

	if (track_count < 0) { return 0; }

	if (track_count > CDDA::MAX_TRACKCOUNT) { return CDDA::MAX_TRACKCOUNT; }

	return static_cast<unsigned>(track_count);
}


std::string construct_filename(const unsigned track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	return "dBAR-" + construct_id(track_count, id_1, id_2, cddb_id) + ".bin";
}


std::string construct_url(const unsigned track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	return construct_url(track_count, id_1, id_2, cddb_id,
			ACCURATERIP::request_url_prefix());
}


std::string construct_url(const unsigned track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id,
		const std::string& prefix) noexcept
{
	auto ss = std::ostringstream {};

	auto hex_flags = std::ios_base::fmtflags { ss.flags() };
	hex_flags &= ~ss.adjustfield; // unset 'left' or 'internal'
	hex_flags |= ss.right;        // set 'right' only
	hex_flags &= ~ss.basefield;   // unset 'dec' and 'oct'
	hex_flags |= ss.hex;          // set 'hex' only
	hex_flags &= ~ss.uppercase;   // unset 'uppercase'
	hex_flags &= ~ss.showbase;    // unset 'showbase'

	ss.flags(hex_flags);

	ss  << prefix /* MUST end with '/' */
		<< std::setw(1) << (id_1       & 0xFu) << '/'
		<< std::setw(1) << (id_1 >> 4u & 0xFu) << '/'
		<< std::setw(1) << (id_1 >> 8u & 0xFu) << '/';

	return ss.str() + construct_filename(track_count, id_1, id_2, cddb_id);
}


std::string construct_id(const unsigned track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	auto id = std::ostringstream {};

	auto fmt_flags = std::ios_base::fmtflags { id.flags() };
	fmt_flags &= ~id.adjustfield; // unset 'left' and 'internal'
	fmt_flags |= id.right;        // set 'right' only
	fmt_flags &= ~id.basefield;   // unset 'dec' and 'oct'
	fmt_flags &= ~id.uppercase;   // unset 'uppercase'
	fmt_flags &= ~id.showbase;    // unset 'showbase'

	id.flags(fmt_flags);

	id  << std::dec
		<< std::setw(3) << std::setfill('0') << track_count
		<< std::hex
		<< '-' << std::setw(8) << std::setfill('0') << id_1
		<< '-' << std::setw(8) << std::setfill('0') << id_2
		<< '-' << std::setw(8) << std::setfill('0') << cddb_id;

	return id.str();
}


ARId make_arid(const std::vector<int32_t>& offsets, const int32_t leadout)
{
	return ARId {
			offsets.size(),
			details::disc_id_1(offsets, leadout),
			details::disc_id_2(offsets, leadout),
			details::cddb_id  (offsets, leadout)
	};
}

} // namespace details


// ARId::Impl


ARId::Impl::Impl(const unsigned track_count, const uint32_t id_1,
		const uint32_t id_2, const uint32_t cddb_id) noexcept
	: track_count_ { track_count }
	, disc_id1_    { id_1 }
	, disc_id2_    { id_2 }
	, cddb_id_     { cddb_id }
{
	// empty
}


std::string ARId::Impl::url() const noexcept
{
	return details::construct_url(track_count_, disc_id1_, disc_id2_, cddb_id_);
}


std::string ARId::Impl::filename() const noexcept
{
	return details::construct_filename(track_count_, disc_id1_, disc_id2_,
			cddb_id_);
}


unsigned ARId::Impl::track_count() const noexcept
{
	return track_count_;
}


uint32_t ARId::Impl::disc_id_1() const noexcept
{
	return disc_id1_;
}


uint32_t ARId::Impl::disc_id_2() const noexcept
{
	return disc_id2_;
}


uint32_t ARId::Impl::cddb_id() const noexcept
{
	return cddb_id_;
}


bool ARId::Impl::empty() const noexcept
{
	return this->equals(*EmptyARId.impl_);
}


void ARId::Impl::swap(Impl& rhs) noexcept
{
	using std::swap;

	swap(this->track_count_, rhs.track_count_);
	swap(this->disc_id1_,    rhs.disc_id1_);
	swap(this->disc_id2_,    rhs.disc_id2_);
	swap(this->cddb_id_,     rhs.cddb_id_);
}


bool ARId::Impl::equals(const ARId::Impl& rhs) const noexcept
{
	return     this->track_count_ == rhs.track_count_
			&& this->disc_id1_    == rhs.disc_id1_
			&& this->disc_id2_    == rhs.disc_id2_
			&& this->cddb_id_     == rhs.cddb_id_;
}


std::string ARId::Impl::to_string() const noexcept
{
	return details::construct_id(track_count_, disc_id1_, disc_id2_, cddb_id_);
}


// ARId


ARId::ARId(const std::size_t track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id)
	: impl_ { std::make_unique<ARId::Impl>(
			details::normalize_trackcount(track_count), id_1, id_2, cddb_id) }
{
	// empty
}


ARId::ARId(const ARId& id)
	: impl_ { std::make_unique<ARId::Impl>(*id.impl_) }
{
	// empty
}


ARId::ARId(ARId&& rhs) noexcept = default;


ARId::~ARId() noexcept = default;


std::string ARId::url() const
{
	return this->empty() ? std::string{} : impl_->url();
}


std::string ARId::filename() const
{
	return this->empty() ? std::string{} : impl_->filename();
}


unsigned ARId::track_count() const noexcept
{
	return impl_->track_count();
}


uint32_t ARId::disc_id_1() const noexcept
{
	return impl_->disc_id_1();
}


uint32_t ARId::disc_id_2() const noexcept
{
	return impl_->disc_id_2();
}


uint32_t ARId::cddb_id() const noexcept
{
	return impl_->cddb_id();
}


std::string ARId::prefix() const noexcept
{
	return ACCURATERIP::request_url_prefix();
}


bool ARId::empty() const noexcept
{
	return impl_->empty();
}


ARId::operator bool() const noexcept
{
	return !empty();
}


void ARId::swap(ARId& rhs) noexcept
{
	impl_->swap(*rhs.impl_);
}


bool ARId::equals(const ARId& rhs) const noexcept
{
	return impl_->equals(*rhs.impl_);
}


std::string ARId::to_string() const
{
	return this->empty() ? std::string{} : impl_->to_string();
}


ARId& ARId::operator = (const ARId& rhs)
{
	if (&rhs != this)
	{
		auto tmp = std::make_unique<ARId::Impl>(*rhs.impl_);
		impl_ = std::move(tmp);
	}
	return *this;
}


ARId& ARId::operator = (ARId&& rhs) noexcept = default;


// make_arid


ARId make_arid(const std::vector<AudioSize>& offsets, const AudioSize& leadout)
{
	return details::make_arid(convert<UNIT::FRAMES>(offsets), leadout.frames());
}


ARId make_arid(const ToC& toc, const AudioSize& leadout)
{
	return details::make_arid(convert<UNIT::FRAMES>(toc.offsets()),
			leadout.frames());
}


ARId make_arid(const ToC& toc)
{
	return details::make_arid(convert<UNIT::FRAMES>(toc.offsets()),
			toc.leadout().frames());
}


ARId validated_arid(const std::vector<AudioSize>& offsets,
		const AudioSize& leadout)
{
	toc::validate_with_completeness(toc::construct(leadout, offsets));

	return make_arid(offsets, leadout);
}


ARId validated_arid(const ToC& toc, const AudioSize& leadout)
{
	return validated_arid(toc.offsets(), leadout);
}


ARId validated_arid(const ToC& toc)
{
	return validated_arid(toc.offsets(), toc.leadout());
}


// make_empty_arid


ARId make_empty_arid()
{
	return EmptyARId;
}


// ACCURATERIP


std::string ACCURATERIP::request_url_prefix_ =
		ACCURATERIP::default_request_url_prefix();


std::string ACCURATERIP::request_url_prefix() noexcept
{
	return ACCURATERIP::request_url_prefix_;
}


std::string ACCURATERIP::default_request_url_prefix() noexcept
{
	/* this defines the default URL prefix for AccurateRip requests */
	return "http://www.accuraterip.com/accuraterip/";
}


void ACCURATERIP::set_request_url_prefix(const std::string& prefix) noexcept
{
	ACCURATERIP::request_url_prefix_ = prefix;
}


void ACCURATERIP::reset_request_url_prefix() noexcept
{
	ACCURATERIP::set_request_url_prefix(
			ACCURATERIP::default_request_url_prefix());
}


std::string ACCURATERIP::default_arcs_format(const uint32_t number)
{
	auto ss = std::ostringstream {};

	auto hex_flags = std::ios_base::fmtflags { ss.flags() };
	hex_flags &= ~ss.adjustfield; // unset 'left' or 'internal'
	hex_flags |= ss.right;        // set 'right' only
	hex_flags &= ~ss.basefield;   // unset 'dec' and 'oct'
	hex_flags |= ss.hex;          // set 'hex' only
	hex_flags |= ss.uppercase;    // set 'uppercase'
	hex_flags &= ~ss.showbase;    // unset 'showbase'

	ss.flags(hex_flags);
	ss << std::setw(8) << std::setfill('0') << number;

	return ss.str();
}


std::string ACCURATERIP::default_id_format(const uint32_t number)
{
	auto ss = std::ostringstream {};

	auto hex_flags = std::ios_base::fmtflags { ss.flags() };
	hex_flags &= ~ss.adjustfield; // unset 'left' or 'internal'
	hex_flags |= ss.right;        // set 'right' only
	hex_flags &= ~ss.basefield;   // unset 'dec' and 'oct'
	hex_flags |= ss.hex;          // set 'hex' only
	hex_flags &= ~ss.uppercase;   // unset 'uppercase'
	hex_flags &= ~ss.showbase;    // unset 'showbase'

	ss.flags(hex_flags);
	ss << std::setw(8) << std::setfill('0') << number;

	return ss.str();
}

} // namespace v_1_0_0
} // namespace arcstk

