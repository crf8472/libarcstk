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

inline namespace v_1_0_0
{

namespace details
{

// ARIdBuilder


std::unique_ptr<ARId> ARIdBuilder::build(const TOC &toc, const uint32_t leadout)
{
	return build_worker(toc, leadout);
}


std::unique_ptr<ARId> ARIdBuilder::build(const TOC &toc)
{
	return build_worker(toc, 0);
}


std::unique_ptr<ARId> ARIdBuilder::build_empty_id() noexcept
{
	try
	{
		return std::make_unique<ARId>(0, 0, 0, 0);
		// This site defines emptyness for ARIds, it used by make_empty_arid()

	} catch (const std::exception& e)
	{
		ARCS_LOG_ERROR << "Exception while creating empty ARId: " << e.what();
	}

	return nullptr;
}


std::unique_ptr<ARId> ARIdBuilder::build_worker(const TOC &toc,
		const lba_count leadout)
{
	// Override TOC leadout with optional non-null extra leadout

	auto leadout_val = lba_count { leadout };

	if (leadout_val > 0)
	{
		TOCValidator::validate(toc, leadout_val);
	} else
	{
		leadout_val = toc.leadout();
	}

	auto offsets { toc::get_offsets(toc) };

	return std::make_unique<ARId>(
			toc.track_count(),
			disc_id_1(offsets, leadout_val),
			disc_id_2(offsets, leadout_val),
			cddb_id  (offsets, leadout_val)
	);
}


uint32_t ARIdBuilder::disc_id_1(const std::vector<uint32_t> &offsets,
		const lba_count leadout) noexcept
{
	// disc id 1 is just the sum off all offsets + the leadout frame

	auto sum_offsets = uint32_t { 0 };

	for (const auto &o : offsets) { sum_offsets += o; }

	return sum_offsets + static_cast<uint32_t>(leadout);
}


uint32_t ARIdBuilder::disc_id_2(const std::vector<uint32_t> &offsets,
		const lba_count leadout) noexcept
{
	// disc id 2 is the sum of the products of offsets and the corresponding
	// 1-based track number while normalizing offsets to be >= 1

	auto accum = uint32_t { 0 };

	auto track = uint16_t { 1 };
	for (const auto &o : offsets) { accum += (o > 0 ? o : 1) * track; track++; }

	return accum + static_cast<uint32_t>(leadout) /* must be > 0*/ * track;
}


uint32_t ARIdBuilder::cddb_id(const std::vector<uint32_t> &offsets,
		const lba_count leadout) noexcept
{
	const auto fps { static_cast<uint32_t>(CDDA.FRAMES_PER_SEC) };
	auto accum = uint32_t { 0 };

	for (const auto &o : offsets)
	{
		accum += sum_digits(o / fps + 2u);
	}
	accum %= 255; // normalize to 1 byte

	const uint32_t total_seconds {
		static_cast<uint32_t>(leadout) / fps  -  offsets[0] / fps };

	// since 0 <= offsets.size <= 99 narrowing is no problem
	const auto track_count = uint32_t { static_cast<uint32_t>(offsets.size()) };

	return (accum << 24u) | (total_seconds << 8u) | track_count;
}


uint64_t ARIdBuilder::sum_digits(const uint32_t number) noexcept
{
	return (number < 10) ? number : (number % 10) + sum_digits(number / 10);
}

} // namespace details


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
	const std::string AR_URL_PREFIX {
		"http://www.accuraterip.com/accuraterip/" };

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
	std::string url() const noexcept;

	/**
	 * \brief Implements ARId::filename()
	 */
	std::string filename() const noexcept;

	/**
	 * \brief Implements ARId::track_count()
	 */
	TrackNo track_count() const noexcept;

	/**
	 * \brief Implements ARId::disc_id_1()
	 */
	uint32_t disc_id_1() const noexcept;

	/**
	 * \brief Implements ARId::disc_id_2()
	 */
	uint32_t disc_id_2() const noexcept;

	/**
	 * \brief Implements ARId::cddb_id()
	 */
	uint32_t cddb_id() const noexcept;

	/**
	 * \brief Implements ARId::empty()
	 */
	bool empty() const noexcept;

	/**
	 * \brief Implements ARId::to_string()
	 */
	std::string to_string() const noexcept;

	/**
	 * \brief Implements ARId::operator == (const ARId &rhs) const
	 */
	bool equals(const Impl &rhs) const noexcept;


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
			const uint32_t cddb_id) const noexcept;

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
			const uint32_t cddb_id) const noexcept;


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
	: track_count_ { track_count }
	, disc_id1_    { id_1 }
	, disc_id2_    { id_2 }
	, cddb_id_     { cddb_id }
{
	// empty
}


std::string ARId::Impl::url() const noexcept
{
	return this->construct_url(track_count_, disc_id1_, disc_id2_, cddb_id_);
}


std::string ARId::Impl::filename() const noexcept
{
	return this->construct_filename(track_count_, disc_id1_, disc_id2_,
			cddb_id_);
}


TrackNo ARId::Impl::track_count() const noexcept
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


std::string ARId::Impl::to_string() const noexcept
{
	auto id = std::stringstream {};

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


bool ARId::Impl::equals(const ARId::Impl& rhs) const noexcept
{
	return track_count_ == rhs.track_count_
		&& disc_id1_    == rhs.disc_id1_
		&& disc_id2_    == rhs.disc_id2_
		&& cddb_id_     == rhs.cddb_id_;
}


std::string ARId::Impl::construct_filename(const TrackNo track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) const noexcept
{
	auto ss = std::stringstream {};

	auto dec_flags = std::ios_base::fmtflags { ss.flags() };
	dec_flags &= ~ss.basefield;
	dec_flags &= ~ss.adjustfield;
	dec_flags |= ss.right;
	dec_flags |= ss.dec;

	auto hex_flags = std::ios_base::fmtflags { ss.flags() };
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
		const uint32_t cddb_id) const noexcept
{
	auto ss = std::stringstream {};

	auto hex_flags = std::ios_base::fmtflags { ss.flags() };
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
	: impl_ { std::move(impl) }
{
	// empty
}


TOC::TOC(const TOC &rhs)
	: impl_ { std::make_unique<TOC::Impl>(*rhs.impl_) }
{
	// empty
}


TOC::TOC(TOC &&rhs) noexcept = default;


TOC::~TOC() noexcept = default;


TrackNo TOC::track_count() const noexcept
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


uint32_t TOC::leadout() const noexcept
{
	return impl_->leadout();
}


bool TOC::complete() const noexcept
{
	return impl_->complete();
}


void TOC::reimplement(std::unique_ptr<TOC::Impl> impl)
{
	impl_ = std::move(impl);
}


TOC& TOC::operator = (const TOC &rhs)
{
	if (this != &rhs)
	{
		// deep copy
		impl_ = std::make_unique<TOC::Impl>(*rhs.impl_);
	}

	return *this;
}


TOC& TOC::operator = (TOC &&rhs) noexcept = default;


// operators TOC


bool operator == (const TOC &lhs, const TOC &rhs) noexcept
{
	return &lhs == &rhs
		or lhs.impl_ == rhs.impl_
		or lhs.impl_->equals(*rhs.impl_);
}


// ARId


ARId::ARId(const TrackNo track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id)
	: impl_ { std::make_unique<ARId::Impl>(track_count, id_1, id_2, cddb_id) }
{
	// empty
}


ARId::ARId(const ARId &id)
	: impl_ { std::make_unique<ARId::Impl>(*id.impl_) }
{
	// empty
}


ARId::ARId(ARId &&rhs) noexcept = default;


ARId::~ARId() noexcept = default;


std::string ARId::url() const noexcept
{
	return impl_->url();
}


std::string ARId::filename() const noexcept
{
	return impl_->filename();
}


TrackNo ARId::track_count() const noexcept
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
	return impl_->AR_URL_PREFIX;
}


bool ARId::empty() const noexcept
{
	return impl_->empty();
}


std::string ARId::to_string() const noexcept
{
	return impl_->to_string();
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


// operators ARId


bool operator == (const ARId &lhs, const ARId &rhs) noexcept
{
	return &lhs == &rhs or lhs.impl_->equals(*rhs.impl_);
}


// InvalidMetadataException


InvalidMetadataException::InvalidMetadataException(const std::string &what_arg)
	: std::logic_error { what_arg }
{
	// empty
}


InvalidMetadataException::InvalidMetadataException(const char *what_arg)
	: std::logic_error { what_arg }
{
	// empty
}


// NonstandardMetadataException


NonstandardMetadataException::NonstandardMetadataException(
		const std::string &what_arg)
	: std::logic_error { what_arg }
{
	// empty
}


NonstandardMetadataException::NonstandardMetadataException(const char *what_arg)
	: std::logic_error { what_arg }
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
 *
 * \brief Uniform access to a container by track
 *
 * Instead of using at() that uses a 0-based index, we need a uniform method
 * to access a container by using a 1-based index and we want to range check it.
 *
 * Type Container is required to yield its number of elements by member function
 * size() and to allow assignment via operator[].
 *
 * \tparam Container Container type with size() and operator []&
 * \tparam InType    The type \c accessor returns
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

	auto track_count {
		static_cast<decltype(container_size)>(toc.track_count()) };
	for (decltype(container_size) t { 1 }; t <= track_count; ++t)
	{
		c[t - 1] = (toc.*accessor)(t);
		// FIXME Uniform container insertion? std::inserter?
	}

	return c;
}

} // namespace details


std::vector<uint32_t> get_offsets(const TOC &toc)
{
	std::vector<uint32_t> target;
	target.resize(static_cast<decltype(target)::size_type>(toc.track_count()));

	return details::toc_get(target, toc, &TOC::offset);
}


std::vector<uint32_t> get_offsets(const std::unique_ptr<TOC> &toc)
{
	return get_offsets(*toc);
}


std::vector<uint32_t> get_parsed_lengths(const TOC &toc)
{
	std::vector<uint32_t> target;
	target.resize(static_cast<decltype(target)::size_type>(toc.track_count()));

	return details::toc_get(target, toc, &TOC::parsed_length);
}


std::vector<uint32_t> get_parsed_lengths(const std::unique_ptr<TOC> &toc)
{
	return get_parsed_lengths(*toc);
}


std::vector<std::string> get_filenames(const TOC &toc)
{
	std::vector<std::string> target;
	target.resize(static_cast<decltype(target)::size_type>(toc.track_count()));

	return details::toc_get(target, toc, &TOC::filename);
}


std::vector<std::string> get_filenames(const std::unique_ptr<TOC> &toc)
{
	return get_filenames(*toc);
}

} // namespace toc


// make_arid


std::unique_ptr<ARId> make_arid(const TOC &toc)
{
	return details::ARIdBuilder::build(toc);
}


std::unique_ptr<ARId> make_arid(const std::unique_ptr<TOC> &toc)
{
	return make_arid(*toc);
}


std::unique_ptr<ARId> make_arid(const TOC &toc, const uint32_t leadout)
{
	return details::ARIdBuilder::build(toc, leadout);
}


std::unique_ptr<ARId> make_arid(const std::unique_ptr<TOC> &toc,
		const uint32_t leadout)
{
	return make_arid(*toc, leadout);
}


std::unique_ptr<ARId> make_empty_arid() noexcept
{
	return details::ARIdBuilder::build_empty_id();
}

const ARId EmptyARId = *make_empty_arid();

/// \endcond

} // namespace v_1_0_0

} // namespace arcstk

