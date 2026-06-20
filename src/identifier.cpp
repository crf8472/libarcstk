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

#include <cstdint>           // for int32_t, uint32_t
#include <memory>            // for unique_ptr, make_unique
#include <ostream>           // for ostream
#include <string>            // for string
#include <vector>            // for vector

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"   // for print_id, disc_id_1, disc_id_2, cddb_id
#endif
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


// identifier_details.hpp

namespace arid
{

void print(std::ostream& out, const ARId& id)
{
	accuraterip::id::print(out,
			id.total_tracks(), id.disc_id_1(), id.disc_id_2(), id.cddb_id());
}


ARId make(const std::vector<int32_t>& offsets, const int32_t leadout)
{
	return ARId {
			offsets.size(),
			accuraterip::id::disc_id_1(offsets, leadout),
			accuraterip::id::disc_id_2(offsets, leadout),
			accuraterip::id::cddb_id  (offsets, leadout)
	};
}

} // namespace arid


namespace details
{

unsigned normalize_total_tracks(const std::size_t total_tracks) noexcept
{
	if (total_tracks > CDDA::MAX_TRACKCOUNT) { return CDDA::MAX_TRACKCOUNT; }

	return static_cast<unsigned>(total_tracks);
}

} // namespace details


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
	, disc_id_1_    { id_1         }
	, disc_id_2_    { id_2         }
	, cddb_id_      { cddb_id      }
{
	// empty
}


std::string ARId::Impl::url() const noexcept
{
	return accuraterip::id::construct_url(total_tracks_, disc_id_1_, disc_id_2_,
			cddb_id_);
}


std::string ARId::Impl::filename() const noexcept
{
	return accuraterip::id::construct_filename(total_tracks_, disc_id_1_,
			disc_id_2_, cddb_id_);
}


unsigned ARId::Impl::total_tracks() const noexcept
{
	return total_tracks_;
}


uint32_t ARId::Impl::disc_id_1() const noexcept
{
	return disc_id_1_;
}


uint32_t ARId::Impl::disc_id_2() const noexcept
{
	return disc_id_2_;
}


uint32_t ARId::Impl::cddb_id() const noexcept
{
	return cddb_id_;
}


bool ARId::Impl::empty() const noexcept
{
	// Note: this checks actually for the result the actual implementation of
	// the default ctor provides!
	return !(total_tracks_ | disc_id_1_ | disc_id_2_ | cddb_id_ );
}


void ARId::Impl::swap(Impl& rhs) noexcept
{
	using std::swap;

	swap(this->total_tracks_, rhs.total_tracks_);
	swap(this->disc_id_1_,    rhs.disc_id_1_);
	swap(this->disc_id_2_,    rhs.disc_id_2_);
	swap(this->cddb_id_,      rhs.cddb_id_);
}


bool ARId::Impl::equals(const ARId::Impl& rhs) const noexcept
{
	return     this->total_tracks_ == rhs.total_tracks_
			&& this->disc_id_1_    == rhs.disc_id_1_
			&& this->disc_id_2_    == rhs.disc_id_2_
			&& this->cddb_id_      == rhs.cddb_id_;
}


std::string ARId::Impl::to_string() const
{
	return accuraterip::id::construct_id(total_tracks_, disc_id_1_, disc_id_2_,
			cddb_id_);
}


// ARId


ARId::ARId() // TODO redundant? could just be defaulted
	: ARId { 0, 0, 0, 0 }
{
	// empty
}


ARId::ARId(const std::size_t total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id)
	: impl_ {
		std::make_unique<ARId::Impl>(
			details::normalize_total_tracks(total_tracks), id_1, id_2, cddb_id)
	}
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
	toc::validate(toc::construct(leadout, offsets));

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

} // namespace v_1_0_0
} // namespace arcstk

