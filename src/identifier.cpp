/**
 * \file
 *
 * \brief Implementation of a low-level API for representing AccurateRip ids
 */

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif

#include <cstdint>
#include <functional>// for function
#include <iomanip>   // for setw, setfill
#include <limits>
#include <memory>
#include <sstream>   // for stringstream
#include <stdexcept> // for logic_error
#include <string>
#include <type_traits>
#include <utility>   // for forward
#include <vector>

#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{

namespace details
{
inline namespace v_1_0_0
{

// ARIdBuilder


std::unique_ptr<ARId> ARIdBuilder::build(const TOC &toc, const uint32_t leadout)
	const
{
	return build_worker(toc, leadout);
}


std::unique_ptr<ARId> ARIdBuilder::build(const TOC &toc) const
{
	return build_worker(toc, 0);
}


std::unique_ptr<ARId> ARIdBuilder::build_empty_id() const noexcept
{
	try
	{
		return std::make_unique<ARId>(0, 0, 0, 0);

	} catch (const std::exception& e)
	{
		ARCS_LOG_ERROR << "Exception while creating empty ARId: " << e.what();
	}

	return nullptr;
}


std::unique_ptr<ARId> ARIdBuilder::build_worker(const TOC &toc,
		const uint32_t leadout) const
{
	// Override TOC leadout with optional non-null extra leadout

	uint32_t leadout_val { leadout };

	if (leadout_val > 0)
	{
		TOCValidator::validate(toc, leadout_val);
	} else
	{
		leadout_val = toc.leadout();
	}

	auto offsets = toc::get_offsets(toc);

	return std::make_unique<ARId>(
			toc.track_count(),
			this->disc_id_1(offsets, leadout_val),
			this->disc_id_2(offsets, leadout_val),
			this->cddb_id  (offsets, leadout_val)
	);
}


uint32_t ARIdBuilder::disc_id_1(const std::vector<uint32_t> &offsets,
		const uint32_t leadout) const
{
	// disc id 1 is just the sum off all offsets + the leadout frame

	uint32_t sum_offsets = 0;

	for (const auto &o : offsets) { sum_offsets += o; }

	return sum_offsets + leadout;
}


uint32_t ARIdBuilder::disc_id_2(const std::vector<uint32_t> &offsets,
		const uint32_t leadout) const
{
	// disc id 2 is the sum of the products of offsets and the corresponding
	// 1-based track number while normalizing offsets to be >= 1

	uint32_t accum = 0;

	uint16_t track = 1;
	for (const auto &o : offsets) { accum += (o > 0 ? o : 1) * track; track++; }

	return accum + leadout /* must be > 0*/ * track;
}


uint32_t ARIdBuilder::cddb_id(const std::vector<uint32_t> &offsets,
		const uint32_t leadout) const
{
	const auto fps = static_cast<uint32_t>(CDDA.FRAMES_PER_SEC);
	uint32_t accum = 0;

	for (const auto &o : offsets)
	{
		accum += sum_digits(o / fps + 2u);
	}
	accum %= 255; // normalize to 1 byte

	const uint32_t     total_seconds = leadout / fps - offsets[0] / fps;
	const unsigned int track_count   = offsets.size();

	return (accum << 24u) | (total_seconds << 8u) | track_count;
}


uint64_t ARIdBuilder::sum_digits(const uint32_t number)
{
	return (number < 10) ? number : (number % 10) + sum_digits(number / 10);
}

} // namespace v_1_0_0

} // namespace details


inline namespace v_1_0_0
{


/**
 * \internal
 * \ingroup id
 *
 * \brief Global instance of the CDDA constants
 */
const CDDA_t CDDA;


/**
 * \internal
 * \ingroup id
 *
 * \brief Private implementation of ARId
 *
 * \see ARId
 */
class ARId::Impl final
{

public:

	/**
	 * \brief URL prefix for accessing AccurateRip
	 */
	const std::string AR_URL_PREFIX = "http://www.accuraterip.com/accuraterip/";

	/**
	 * \brief Implements ARId::ARId().
	 */
	Impl(const TrackNo track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id);

	/**
	 * \brief Implements ARId::url()
	 */
	std::string url() const;

	/**
	 * \brief Implements ARId::filename()
	 */
	std::string filename() const;

	/**
	 * \brief Implements ARId::track_count()
	 */
	TrackNo track_count() const;

	/**
	 * \brief Implements ARId::disc_id_1()
	 */
	uint32_t disc_id_1() const;

	/**
	 * \brief Implements ARId::disc_id_2()
	 */
	uint32_t disc_id_2() const;

	/**
	 * \brief Implements ARId::cddb_id()
	 */
	uint32_t cddb_id() const;

	/**
	 * \brief Implements ARId::empty()
	 */
	bool empty() const;

	/**
	 * \brief Implements ARId::to_string()
	 */
	std::string to_string() const;

	/**
	 * \brief Implements ARId::operator == (const ARId &rhs) const
	 */
	bool operator == (const Impl &rhs) const;

	/**
	 * \brief Implements ARId::operator != (const ARId &rhs) const
	 */
	bool operator != (const Impl &rhs) const;


protected:

	/**
	 * \brief Service method: Compute the AccurateRip response filename
	 *
	 * Used by ARId::Impl::filename().
	 *
	 * \param[in] track_count   Number of tracks in this medium
	 * \param[in] id_1          Id 1 of this medium
	 * \param[in] id_2          Id 2 of this medium
	 * \param[in] cddb_id       CDDB id of this medium
	 *
	 * \return AccurateRip response filename
	 */
	std::string construct_filename(const TrackNo track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id) const;

	/**
	 * \brief Service method: Compute the AccurateRip request URL
	 *
	 * Used by ARId::Impl::url().
	 *
	 * \param[in] track_count   Number of tracks in this medium
	 * \param[in] id_1          Id 1 of this medium
	 * \param[in] id_2          Id 2 of this medium
	 * \param[in] cddb_id       CDDB id of this medium
	 *
	 * \return AccurateRip request URL
	 */
	std::string construct_url(const TrackNo track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id) const;


private:

	/**
	 * \brief Number of tracks
	 */
	TrackNo track_count_;

	/**
	 * \brief Disc id no. 1
	 */
	uint32_t disc_id1_;

	/**
	 * \brief Disc id no. 2
	 */
	uint32_t disc_id2_;

	/**
	 * \brief CDDB disc id
	 */
	uint32_t cddb_id_;
};

/// \cond UNDOC_FUNCTION_BODIES


ARId::Impl::Impl(const TrackNo track_count, const uint32_t id_1,
		const uint32_t id_2, const uint32_t cddb_id)
	: track_count_(track_count)
	, disc_id1_   (id_1)
	, disc_id2_   (id_2)
	, cddb_id_    (cddb_id)
{
	// empty
}


std::string ARId::Impl::url() const
{
	return this->construct_url(track_count_, disc_id1_, disc_id2_, cddb_id_);
}


std::string ARId::Impl::filename() const
{
	return this->construct_filename(track_count_, disc_id1_, disc_id2_,
			cddb_id_);
}


TrackNo ARId::Impl::track_count() const
{
	return track_count_;
}


uint32_t ARId::Impl::disc_id_1() const
{
	return disc_id1_;
}


uint32_t ARId::Impl::disc_id_2() const
{
	return disc_id2_;
}


uint32_t ARId::Impl::cddb_id() const
{
	return cddb_id_;
}


bool ARId::Impl::empty() const
{
	return 0 == (this->disc_id_1() + this->disc_id_2() + this->cddb_id());
}


std::string ARId::Impl::to_string() const
{
	std::stringstream id;

	id << std::dec
		<< std::setw(3) << std::setfill('0') << this->track_count()
		<< "-"
		<< std::hex << std::nouppercase
		<< std::setw(8) << std::setfill('0') << this->disc_id_1()
		<< "-"
		<< std::setw(8) << std::setfill('0') << this->disc_id_2()
		<< "-"
		<< std::setw(8) << std::setfill('0') << this->cddb_id();

	return id.str();
}


bool ARId::Impl::operator == (const ARId::Impl& rhs) const
{
	return track_count_ == rhs.track_count_
		&& disc_id1_    == rhs.disc_id1_
		&& disc_id2_    == rhs.disc_id2_
		&& cddb_id_     == rhs.cddb_id_;
}


bool ARId::Impl::operator != (const ARId::Impl& rhs) const
{
	return !(*this == rhs);
}


std::string ARId::Impl::construct_filename(const TrackNo track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) const
{
	std::stringstream ss;

	std::ios_base::fmtflags dec_flags = ss.flags();
	dec_flags &= ~ss.basefield;
	dec_flags &= ~ss.adjustfield;
	dec_flags |= ss.right;
	dec_flags |= ss.dec;

	std::ios_base::fmtflags hex_flags = ss.flags();
	hex_flags &= ~ss.basefield;
	hex_flags &= ~ss.adjustfield;
	hex_flags |= ss.right;
	hex_flags |= ss.hex;

	ss.flags(dec_flags);
	ss << "dBAR" << '-' << std::setw(3) << std::setfill('0') << track_count;

	ss.flags(hex_flags);
	ss << std::nouppercase
	   << '-' << std::setw(8) << std::setfill('0') << id_1
	   << '-' << std::setw(8) << std::setfill('0') << id_2
	   << '-' << std::setw(8) << std::setfill('0') << cddb_id
	   << ".bin";

	return ss.str();
}


std::string ARId::Impl::construct_url(const TrackNo track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) const
{
	std::stringstream ss;

	std::ios_base::fmtflags hex_flags = ss.flags();
	hex_flags &= ~ss.basefield;
	hex_flags &= ~ss.adjustfield;
	hex_flags |= ss.right;
	hex_flags |= ss.hex;

	ss.flags(hex_flags);
	ss << AR_URL_PREFIX
	   << std::setw(1) << (id_1       & 0xFu)
	   << '/'          << (id_1 >> 4u & 0xFu)
	   << '/'          << (id_1 >> 8u & 0xFu)
	   << '/' << this->construct_filename(track_count, id_1, id_2, cddb_id);

	return ss.str();
}


// TOC


TOC::TOC(std::unique_ptr<TOC::Impl> impl)
	: impl_(std::move(impl))
{
	// empty
}


TOC::TOC(const TOC &rhs)
	: impl_(std::make_unique<TOC::Impl>(*rhs.impl_))
{
	// empty
}


TOC::TOC(TOC &&rhs) noexcept = default;


TOC::~TOC() noexcept = default;


TrackNo TOC::track_count() const
{
	return impl_->track_count();
}


uint32_t TOC::offset(const TrackNo idx) const
{
	return impl_->offset(idx);
}


uint32_t TOC::parsed_length(const TrackNo idx) const
{
	return impl_->parsed_length(idx);
}


std::string TOC::filename(const TrackNo idx) const
{
	return impl_->filename(idx);
}


uint32_t TOC::leadout() const
{
	return impl_->leadout();
}


bool TOC::complete() const
{
	return impl_->complete();
}


void TOC::update(std::unique_ptr<TOC::Impl> impl)
{
	impl_ = std::move(impl);
}


bool TOC::operator == (const TOC &rhs) const
{
	return this == &rhs
		or this->impl_  == rhs.impl_
		or *this->impl_ == *rhs.impl_;
}


TOC& TOC::operator = (const TOC &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	// deep copy
	impl_ = std::make_unique<TOC::Impl>(*rhs.impl_);
	return *this;
}


TOC& TOC::operator = (TOC &&rhs) noexcept = default;


// ARId


ARId::ARId(const TrackNo track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id)
	: impl_(std::make_unique<ARId::Impl>(track_count, id_1, id_2, cddb_id))
{
	// empty
}


ARId::ARId(const ARId &id)
	: impl_(std::make_unique<ARId::Impl>(*id.impl_))
{
	// empty
}


ARId::ARId(ARId &&rhs) noexcept = default;


ARId::~ARId() noexcept = default;


std::string ARId::url() const
{
	return impl_->url();
}


std::string ARId::filename() const
{
	return impl_->filename();
}


TrackNo ARId::track_count() const
{
	return impl_->track_count();
}


uint32_t ARId::disc_id_1() const
{
	return impl_->disc_id_1();
}


uint32_t ARId::disc_id_2() const
{
	return impl_->disc_id_2();
}


uint32_t ARId::cddb_id() const
{
	return impl_->cddb_id();
}


std::string ARId::prefix() const
{
	return impl_->AR_URL_PREFIX;
}


bool ARId::empty() const
{
	return impl_->empty();
}


std::string ARId::to_string() const
{
	return impl_->to_string();
}


bool ARId::operator == (const ARId& rhs) const
{
	return *this->impl_ == *rhs.impl_;
}


bool ARId::operator != (const ARId& rhs) const
{
	return not (*this->impl_ == *rhs.impl_);
}


ARId& ARId::operator = (const ARId &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	// deep copy
	impl_ = std::make_unique<ARId::Impl>(*rhs.impl_);
	return *this;
}


ARId& ARId::operator = (ARId &&rhs) noexcept = default;


// InvalidMetadataException


InvalidMetadataException::InvalidMetadataException(const std::string &what_arg)
	: std::logic_error(what_arg)
{
	// empty
}


InvalidMetadataException::InvalidMetadataException(const char *what_arg)
	: std::logic_error(what_arg)
{
	// empty
}


// NonstandardMetadataException


NonstandardMetadataException::NonstandardMetadataException(
		const std::string &what_arg)
	: std::logic_error(what_arg)
{
	// empty
}


NonstandardMetadataException::NonstandardMetadataException(const char *what_arg)
	: std::logic_error(what_arg)
{
	// empty
}


namespace toc
{

/**
 * \internal
 *
 * \brief Implementation details of namespace toc
 */
namespace details
{

/**
 * \internal
 * \brief Uniform access to a container by track
 *
 * Instead of using at() that uses a 0-based index, we need a uniform method
 * to access a container by using a 1-based index and we want to range check it.
 *
 * Type Container is required to yield its number of elements by member function
 * size() and to allow assignment via operator[].
 *
 * \tparam Container Container type with \c size() and \c []&
 *
 * \param[in,out] c         Actual container
 * \param[in]     toc       Number of the track to access
 * \param[in]     accessor  TOC member function to iterate
 *
 * \return The values \c accessor yields in the order the occurr in \c toc
 */
template <typename Container, typename InType> // FIXME requirements
decltype(auto) toc_get(Container&& c,
		const TOC &toc,
		InType (TOC::*accessor)(const TrackNo) const)
{
	auto container_size { c.size() };

	auto track_count = static_cast<decltype(container_size)>(toc.track_count());
	for (decltype(container_size) t = 1; t <= track_count; ++t)
	{
		c[t - 1] = (toc.*accessor)(t);
		// FIXME Uniform container insertion? std::inserter?
	}

	return c;
}

// Example usage:
//
//template<typename Container>
//static typename std::enable_if<details::is_lba_container<Container>::value,
//	void>::type
//append(Container& to, const Container& from)
//{
//    using std::begin;
//    using std::end;
//    to.insert(end(to), begin(from), end(from));
//}

} // namespace details


std::vector<uint32_t> get_offsets(const TOC &toc)
{
	std::vector<uint32_t> target;
	target.resize(static_cast<decltype(target)::size_type>(toc.track_count()));

	return details::toc_get(target, toc, &TOC::offset);
}


std::vector<uint32_t> get_parsed_lengths(const TOC &toc)
{
	std::vector<uint32_t> target;
	target.resize(static_cast<decltype(target)::size_type>(toc.track_count()));

	return details::toc_get(target, toc, &TOC::parsed_length);
}


std::vector<std::string> get_filenames(const TOC &toc)
{
	std::vector<std::string> target;
	target.resize(static_cast<decltype(target)::size_type>(toc.track_count()));

	return details::toc_get(target, toc, &TOC::filename);
}

} // namespace toc


// make_arid


std::unique_ptr<ARId> make_arid(const TOC &toc)
{
	details::ARIdBuilder builder;
	return builder.build(toc);
}


std::unique_ptr<ARId> make_arid(const TOC &toc, const uint32_t leadout)
{
	details::ARIdBuilder builder;
	return builder.build(toc, leadout);
}


std::unique_ptr<ARId> make_empty_arid()
{
	details::ARIdBuilder builder;
	return builder.build_empty_id();
}

/// \endcond

} // namespace v_1_0_0

} // namespace arcstk

