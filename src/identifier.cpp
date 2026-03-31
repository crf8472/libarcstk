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
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"      // for Checksum
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

const ARId EmptyARId = ARId { 0, 0, 0, 0 }; // defines emptiness for ARId


// identifier_details.hpp

namespace arid
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
	const auto total_tracks = static_cast<uint32_t>(offsets.size());


	return (accum << 24u) | (total_seconds << 8u) | total_tracks;
}


uint64_t sum_digits(const uint32_t number) noexcept
{
	return (number < 10) ? number : (number % 10) + sum_digits(number / 10);
}


unsigned normalize_total_tracks(const std::size_t total_tracks) noexcept
{
	if (total_tracks > CDDA::MAX_TRACKCOUNT) { return CDDA::MAX_TRACKCOUNT; }

	return static_cast<unsigned>(total_tracks);
}


std::string construct_filename(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	return "dBAR-" + construct_id(total_tracks, id_1, id_2, cddb_id) + ".bin";
}


std::string construct_url(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	return construct_url(total_tracks, id_1, id_2, cddb_id,
			ACCURATERIP::request_url_prefix());
}


std::string construct_url(const unsigned total_tracks,
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

	return ss.str() + construct_filename(total_tracks, id_1, id_2, cddb_id);
}


std::string construct_id(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	auto out = std::ostringstream {};
	print(out, total_tracks, id_1, id_2, cddb_id);
	return out.str();
}


void print(std::ostream& out, const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id, const std::string& delim)
{
	static const auto fmt_flags =
		[](const std::ostream& stream) -> std::ios_base::fmtflags
		{
			auto flags = std::ios_base::fmtflags { stream.flags() };

			flags &= ~stream.adjustfield; // unset 'left' or 'internal'
			flags |= stream.right;        // set 'right' only
			flags &= ~stream.basefield;   // unset 'dec', 'hex', and 'oct'
			flags &= ~stream.uppercase;   // unset 'uppercase'
			flags &= ~stream.showbase;    // unset 'showbase'

			return flags;
		};

	const auto prev_flags = std::ios_base::fmtflags { out.flags() };

	out.flags(fmt_flags(out));

	out << std::dec
		<< std::setw(3) << std::setfill('0') << total_tracks
		<< std::hex
		<< delim << std::setw(8) << std::setfill('0') << id_1
		<< delim << std::setw(8) << std::setfill('0') << id_2
		<< delim << std::setw(8) << std::setfill('0') << cddb_id;

	out.flags(prev_flags);
}


void print(std::ostream& out, const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id)
{
	print(out, total_tracks, id_1, id_2, cddb_id, "-");
}


void print(std::ostream& out, const ARId& id)
{
	print(out, id.total_tracks(), id.disc_id_1(), id.disc_id_2(), id.cddb_id());
}


ARId make(const std::vector<int32_t>& offsets, const int32_t leadout)
{
	return ARId {
			offsets.size(),
			disc_id_1(offsets, leadout),
			disc_id_2(offsets, leadout),
			cddb_id  (offsets, leadout)
	};
}

} // namespace arid


// ARId::operator <<


std::ostream& operator << (std::ostream& out, const ARId& arid)
{
	arid::print(out, arid);
	return out;
}


// ARId::Impl


ARId::Impl::Impl(const unsigned total_tracks, const uint32_t id_1,
		const uint32_t id_2, const uint32_t cddb_id) noexcept
	: total_tracks_ { total_tracks }
	, disc_id1_     { id_1 }
	, disc_id2_     { id_2 }
	, cddb_id_      { cddb_id }
{
	// empty
}


std::string ARId::Impl::url() const noexcept
{
	return arid::construct_url(total_tracks_, disc_id1_, disc_id2_, cddb_id_);
}


std::string ARId::Impl::filename() const noexcept
{
	return arid::construct_filename(total_tracks_, disc_id1_, disc_id2_,
			cddb_id_);
}


unsigned ARId::Impl::total_tracks() const noexcept
{
	return total_tracks_;
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

	swap(this->total_tracks_, rhs.total_tracks_);
	swap(this->disc_id1_,    rhs.disc_id1_);
	swap(this->disc_id2_,    rhs.disc_id2_);
	swap(this->cddb_id_,     rhs.cddb_id_);
}


bool ARId::Impl::equals(const ARId::Impl& rhs) const noexcept
{
	return     this->total_tracks_ == rhs.total_tracks_
			&& this->disc_id1_     == rhs.disc_id1_
			&& this->disc_id2_     == rhs.disc_id2_
			&& this->cddb_id_      == rhs.cddb_id_;
}


std::string ARId::Impl::to_string() const
{
	return arid::construct_id(total_tracks_, disc_id1_, disc_id2_, cddb_id_);
}


// ARId


ARId::ARId(const std::size_t total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id)
	: impl_ { std::make_unique<ARId::Impl>(
			arid::normalize_total_tracks(total_tracks), id_1, id_2, cddb_id) }
{
	// empty
}


ARId::ARId(const ARId& id)
	: impl_ { std::make_unique<ARId::Impl>(*id.impl_) }
{
	// empty
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


ARId::ARId(ARId&& rhs) noexcept = default;


ARId& ARId::operator = (ARId&& rhs) noexcept = default;


ARId::~ARId() noexcept = default; // Pimpl requirement


std::string ARId::url() const
{
	return this->empty() ? std::string{} : impl_->url();
}


std::string ARId::filename() const
{
	return this->empty() ? std::string{} : impl_->filename();
}


unsigned ARId::total_tracks() const noexcept
{
	return impl_->total_tracks();
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


// make_arid


ARId make_arid(const std::vector<AudioSize>& offsets, const AudioSize& leadout)
{
	return arid::make(convert<UNIT::FRAMES>(offsets), leadout.frames());
}


ARId make_arid(const ToC& toc, const AudioSize& leadout)
{
	return arid::make(convert<UNIT::FRAMES>(toc.offsets()), leadout.frames());
}


ARId make_arid(const ToC& toc)
{
	return arid::make(convert<UNIT::FRAMES>(toc.offsets()),
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
	return Checksum { number }.to_string();
}

} // namespace v_1_0_0
} // namespace arcstk

