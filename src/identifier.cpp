/**
 * \file
 *
 * \brief Implementation of a low-level API for representing AccurateRip ids
 */

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_DETAILS_HPP__
#include "identifier_details.hpp"
#endif

#include <cstdint>
#include <functional>// for function
#include <iomanip>   // for setw, setfill
#include <limits>
#include <memory>
#include <numeric>   // for accumulate
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


/**
 * \internal
 * \ingroup id
 *
 * \brief Global instance of the CDDA constants
 */
const CDDA_t CDDA;


/**
 * \brief Uniform access to a container by track
 *
 * Instead of using at() that uses a 0-based index, we need a uniform method
 * to access a container by using a 1-based index and we want to range check it.
 *
 * Type Container is required to yield its number of elements by member function
 * size() and to allow access via operator[].
 *
 * \tparam Container Container type with size() and []
 * \param c Actual container
 * \param t Number of the track to access
 *
 * \return The value for track \c t in the container \c
 */
template <typename Container>
decltype(auto) get_track(Container&& c, const TrackNo t)
{
	auto container_size = std::forward<Container>(c).size();

	// Do the range check
	if (t < 1 or static_cast<decltype(container_size)>(t) > container_size)
	{
		std::stringstream message;
		message << "Track " << t << " is out of range (yields index "
			<< (t - 1) << " but size is " << container_size << ")";

		throw std::out_of_range(message.str());
	}

	return std::forward<Container>(c)[
		static_cast<decltype(container_size)>(t - 1)];
}


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

/// \endcond


/**
 * \internal
 * \ingroup id
 *
 * \brief Private implementation of TOCBuilder.
 *
 * Note that TOCBuilder is a friend of TOC but TOCBuilder::Impl
 * is not. Hence, the actual creation of the TOC object is not
 * delegated to TOCBuilder::Impl. Instead, TOCBuilder::Impl constructs
 * the three parameters separately while TOCBuilder performs the actual
 * constructor call to TOC.
 *
 * \see TOCBuilder
 */
class TOCBuilder::Impl final
{

public:

	/**
	 * \brief Default constructor
	 */
	Impl();

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	Impl(const Impl &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	Impl(Impl &&rhs) noexcept = default;

	/**
	 * \brief Default destructor
	 */
	~Impl() noexcept = default;

	/**
	 * \brief Implements TOCBuilder::build(const TrackNo track_count, const std::vector<int32_t> &offsets, const uint32_t leadout).
	 */
	std::unique_ptr<TOC::Impl> build(const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const uint32_t leadout,
			const std::vector<std::string> &files);

	/**
	 * \brief Implements TOCBuilder::build(const TrackNo track_count, const std::vector<int32_t> &offsets, const std::vector<int32_t> &lengths, const std::vector<std::string> &files)
	 */
	std::unique_ptr<TOC::Impl> build(const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const std::vector<int32_t> &lengths,
			const std::vector<std::string> &files);

	std::unique_ptr<TOC::Impl> merge(const TOC &toc, const uint32_t leadout)
		const;

	/**
	 * \brief Copy assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TOCBuilder::Impl& operator = (const TOCBuilder::Impl &rhs);

	/**
	 * \brief Move assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TOCBuilder::Impl& operator = (TOCBuilder::Impl &&rhs) noexcept = default;


protected:

	/**
	 * \brief Service method: Builds a track count for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] track_count Number of tracks
	 *
	 * \return The intercepted track_count
	 *
	 * \throw InvalidMetadataException If the track count is not valid
	 */
	TrackNo build_track_count(const TrackNo track_count) const;

	/**
	 * \brief Service method: Builds validated offsets for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] offsets     Offsets to be validated
	 * \param[in] track_count Number of tracks
	 * \param[in] leadout     Leadout frame
	 *
	 * \return A representation of the validated offsets as unsigned integers
	 *
	 * \throw InvalidMetadataException If the offsets are not valid
	 */
	std::vector<uint32_t> build_offsets(const std::vector<int32_t> &offsets,
			const TrackNo track_count, const uint32_t leadout) const;

	/**
	 * \brief Service method: Builds validated offsets for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] offsets     Offsets to be validated
	 * \param[in] track_count Number of tracks
	 * \param[in] lengths     Lengths to be validated
	 *
	 * \return A representation of the validated offsets as unsigned integers
	 *
	 * \throw InvalidMetadataException If the offsets are not valid
	 */
	std::vector<uint32_t> build_offsets(const std::vector<int32_t> &offsets,
			const TrackNo track_count,
			const std::vector<int32_t> &lengths) const;

	/**
	 * \brief Service method: Builds validated lengths for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] sv Vector of lengths as signed integers to be validated
	 * \param[in] track_count Number of tracks
	 *
	 * \return A representation of the validated lengths as unsigned integers
	 *
	 * \throw InvalidMetadataException If the lengths are not valid
	 */
	std::vector<uint32_t> build_lengths(const std::vector<int32_t> &sv,
			const TrackNo track_count) const;

	/**
	 * \brief Service method: Builds validated leadout for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] leadout Leadout to be validated
	 *
	 * \return A representation of the validated leadout
	 *
	 * \throw InvalidMetadataException If the leadout is not valid
	 */
	uint32_t build_leadout(const uint32_t leadout) const;

	/**
	 * \brief Service method: Builds validated audio file list for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] files File list to be validated
	 *
	 * \return A representation of the validated file list
	 *
	 * \throw InvalidMetadataException If the file list is not valid
	 */
	std::vector<std::string> build_files(std::vector<std::string> files) const;


private:

	/**
	 * \brief Validator instance
	 */
	std::unique_ptr<TOCValidator> validator_;
};


/**
 * \internal
 * \ingroup id
 *
 * \brief Private implementation of TOC.
 *
 * \see TOC
 */
class TOC::Impl final
{
	// TOCBuilder::Impl::build() methods are friends of TOC::Impl
	// since they construct TOC::Impls exclusively

	friend std::unique_ptr<TOC::Impl> TOCBuilder::Impl::build(
			const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const uint32_t leadout,
			const std::vector<std::string> &files);

	friend std::unique_ptr<TOC::Impl> TOCBuilder::Impl::build(
			const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const std::vector<int32_t> &lengths,
			const std::vector<std::string> &files);

	friend std::unique_ptr<TOC::Impl> TOCBuilder::Impl::merge(
			const TOC &toc, const uint32_t leadout) const;

public:

	/**
	 * \brief Implements TOC::track_count()
	 */
	TrackNo track_count() const;

	/**
	 * \brief Implements TOC::offset(const uint8_t) const
	 */
	uint32_t offset(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::parsed_length(const uint8_t) const
	 */
	uint32_t parsed_length(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::filename(const TrackNo idx) const
	 */
	std::string filename(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::leadout()
	 */
	uint32_t leadout() const;

	/**
	 * \brief Implements TOC::complete()
	 */
	bool complete() const;

	/**
	 * \brief Implements TOC::operator==()
	 */
	bool operator == (const TOC::Impl &rhs) const;


private:

	/**
	 * \brief Implements private constructor of TOC.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 * \param[in] leadout     Leadout frame
	 * \param[in] files       File name of each track
	 */
	Impl(const TrackNo track_count,
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout,
			const std::vector<std::string> &files);

	/**
	 * \brief Implements private constructor of TOC.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 * \param[in] lengths     Lengths (in CDDA frames) of each track
	 * \param[in] files       File name of each track
	 */
	Impl(const TrackNo track_count,
			const std::vector<uint32_t> &offsets,
			const std::vector<uint32_t> &lengths,
			const std::vector<std::string> &files);

	/**
	 * \brief Number of tracks
	 */
	TrackNo track_count_;

	/**
	 * \brief Track offsets (in frames)
	 */
	std::vector<uint32_t> offsets_;

	/**
	 * \brief Track lengths (in frames)
	 */
	std::vector<uint32_t> lengths_;

	/**
	 * \brief Leadout frame
	 */
	uint32_t leadout_;

	/**
	 * \brief Audio file names
	 */
	std::vector<std::string> files_;
};

/// \cond UNDOC_FUNCTION_BODIES

TOC::Impl::Impl(const TrackNo track_count,
		const std::vector<uint32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files)
	: track_count_(track_count)
	, offsets_(offsets)
	, lengths_()
	, leadout_(leadout)
	, files_(files)
{
	// empty
}


TOC::Impl::Impl(const TrackNo track_count,
		const std::vector<uint32_t> &offsets,
		const std::vector<uint32_t> &lengths,
		const std::vector<std::string> &files)
	: track_count_(track_count)
	, offsets_(offsets)
	, lengths_(lengths)
	, leadout_(arcstk::leadout(offsets, lengths))
	, files_(files)
{
	// empty
}


TrackNo TOC::Impl::track_count() const
{
	return track_count_;
}


uint32_t TOC::Impl::offset(const TrackNo track) const
{
	return get_track(offsets_, track);
}


uint32_t TOC::Impl::parsed_length(const TrackNo track) const
{
	return get_track(lengths_, track);
}


std::string TOC::Impl::filename(const TrackNo track) const
{
	return get_track(files_, track);
}


uint32_t TOC::Impl::leadout() const
{
	return leadout_;
}


bool TOC::Impl::complete() const
{
	return leadout_ != 0;
}


bool TOC::Impl::operator == (const TOC::Impl &rhs) const
{
	if (this == &rhs)
	{
		return true;
	}

	return track_count_ == rhs.track_count_
		and offsets_    == rhs.offsets_
		and lengths_    == rhs.lengths_
		and leadout_    == rhs.leadout_
		and files_      == rhs.files_;
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
 * \brief Uniform access to a container by track
 *
 * Instead of using at() that uses a 0-based index, we need a uniform method
 * to access a container by using a 1-based index and we want to range check it.
 *
 * Type Container is required to yield its number of elements by member function
 * size() and to allow access via operator[].
 *
 * \tparam Container Container type with size() and []&
 * \param  c         Actual container
 * \param  toc       Number of the track to access
 *
 * \return The value for track \c t in the container \c c
 */
template <typename Container, typename InType>
decltype(auto) toc_get(Container&& c,
		const TOC &toc,
		InType (TOC::*accessor)(const TrackNo) const)
{
	auto container_size = c.size();

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


// ARIdBuilder


std::unique_ptr<ARId> ARIdBuilder::build(const TrackNo &track_count,
		const std::vector<int32_t> &offsets, const uint32_t leadout) const
{
	TOCBuilder builder;
	auto toc = builder.build(track_count, offsets, leadout, {/* no files */});

	return build_worker(*toc, 0);
}


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
		TOCValidator validator;
		validator.validate(toc, leadout_val);
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


uint32_t ARIdBuilder::disc_id_2(const TrackNo track_count,
		const std::vector<uint32_t> &offsets, const uint32_t leadout) const
{
	// disc id 2 is the sum of the products of offsets and the corresponding
	// 1-based track number while normalizing offsets to be >= 1

	uint32_t sum_offsets = 0;

	for (std::size_t i = 0; i < static_cast<std::size_t>(track_count); ++i)
	{
		// This will throw if offsets.size() < track_count
		sum_offsets += (offsets[i] > 0 ? offsets[i] : 1) * (i + 1);
	}

	return sum_offsets + leadout *
		(static_cast<unsigned int>(track_count) + 1u);
}


uint32_t ARIdBuilder::cddb_id(const TrackNo track_count,
		const std::vector<uint32_t> &offsets, const uint32_t leadout) const
{
	// The CDDB id is a 32bit unsigned integer, formed of a concatenation of
	// the following 3 numbers:
	// first chunk (8 bits):   checksum (sum of digit sums of offset secs + 2)
	// second chunk (16 bits): total seconds count
	// third chunk (8 bits):   number of tracks

	// Calculate first part: checksum

	uint32_t checksum = 0;
	auto frames_per_sec = static_cast<uint32_t>(CDDA.FRAMES_PER_SEC);

	for (std::size_t i = 0; i < static_cast<std::size_t>(track_count); ++i)
	{
		// This will throw if offsets.size() < track_count
		checksum += sum_digits(offsets[i] / frames_per_sec + 2u);
	}
	checksum %= 255; // normalize to 1 byte

	// Calculate second part: seconds count

	const uint32_t seconds_count = leadout / frames_per_sec
		- offsets[0] / frames_per_sec;

	return (checksum << 24u) | (seconds_count << 8u) |
		static_cast<unsigned int>(track_count);
}


uint64_t ARIdBuilder::sum_digits(const uint32_t number)
{
	return (number < 10) ? number : (number % 10) + sum_digits(number / 10);
}


// TOCBuilder::Impl


TOCBuilder::Impl::Impl()
	: validator_(std::make_unique<TOCValidator>())
{
	// empty
}


TOCBuilder::Impl::Impl(const TOCBuilder::Impl &rhs)
	: validator_(std::make_unique<TOCValidator>(*rhs.validator_)) // deep copy
{
	// empty
}


std::unique_ptr<TOC::Impl> TOCBuilder::Impl::build(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files)
{
	return std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(offsets, track_count, leadout),
		build_leadout(leadout),
		build_files(files))
	);
}


std::unique_ptr<TOC::Impl> TOCBuilder::Impl::build(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const std::vector<int32_t> &lengths,
		const std::vector<std::string> &files)
{
	return std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(offsets, track_count, lengths),
		build_lengths(lengths, track_count),
		build_files(files))
	);
}


TrackNo TOCBuilder::Impl::build_track_count(const TrackNo track_count) const
{
	validator_->validate_trackcount(track_count);

	return track_count;
}


std::vector<uint32_t> TOCBuilder::Impl::build_offsets(
		const std::vector<int32_t> &offsets, const TrackNo track_count,
		const uint32_t leadout) const
{
	validator_->validate(track_count, offsets, leadout);

	// Convert offsets to uints

	return std::vector<uint32_t>(offsets.begin(), offsets.end());
}


std::vector<uint32_t> TOCBuilder::Impl::build_offsets(
		const std::vector<int32_t> &offsets, const TrackNo track_count,
		const std::vector<int32_t> &lengths) const
{
	// Valid number of lengths ?

	if (offsets.size() != static_cast<std::size_t>(track_count))
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(track_count) << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	if (offsets.size() != lengths.size())
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(offsets.size()) << " offsets";

		throw InvalidMetadataException(ss.str());
	}

	validator_->validate_lengths(lengths);

	validator_->validate_offsets(track_count, offsets);

	// Convert offsets to uints

	return std::vector<uint32_t>(offsets.begin(), offsets.end());
}


std::vector<uint32_t> TOCBuilder::Impl::build_lengths(
		const std::vector<int32_t> &lengths, const TrackNo track_count) const
{
	// Valid number of lengths ?

	if (lengths.size() != static_cast<std::size_t>(track_count))
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(track_count) << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// If params make sense, use TOCValidator

	validator_->validate_lengths(lengths);

	// Convert ints to uints while normalizing the last length to 0

	std::vector<uint32_t> uv(lengths.begin(), lengths.end() - 1);

	auto last_length = lengths.back() < 0 ? 0 : lengths.back();
	uv.push_back(static_cast<uint32_t>(last_length));

	return uv;
}


uint32_t TOCBuilder::Impl::build_leadout(const uint32_t leadout) const
{
	validator_->validate_leadout(leadout);

	return leadout;
}


std::vector<std::string> TOCBuilder::Impl::build_files(std::vector<std::string>
		files) const
{
	// do nothing for now

	return files;
}


std::unique_ptr<TOC::Impl> TOCBuilder::Impl::merge(const TOC &toc,
		const uint32_t leadout) const
{
	validator_->validate(toc, leadout);

	// add length of last track
	std::vector<uint32_t> merged_lengths;
	auto size =
		static_cast<std::vector<uint32_t>::size_type>(toc.track_count());
	merged_lengths.reserve(size);
	merged_lengths = toc::get_parsed_lengths(toc);
	merged_lengths.push_back(leadout - toc.offset(toc.track_count()));

	std::unique_ptr<TOC::Impl> impl = std::make_unique<TOC::Impl>(TOC::Impl(
		toc.track_count(),
		toc::get_offsets(toc),
		merged_lengths,
		toc::get_filenames(toc)
	));

	impl->leadout_ = leadout;

	return impl;
}


TOCBuilder::Impl& TOCBuilder::Impl::operator = (const TOCBuilder::Impl &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	// deep copy
	validator_ = std::make_unique<TOCValidator>(*rhs.validator_);
	return *this;
}


// TOCBuilder


TOCBuilder::TOCBuilder()
	: impl_(std::make_unique<TOCBuilder::Impl>())
{
	// empty
}


TOCBuilder::TOCBuilder(const TOCBuilder &rhs)
	: impl_(std::make_unique<TOCBuilder::Impl>(*rhs.impl_)) // deep copy
{
	// empty
}


TOCBuilder::TOCBuilder(TOCBuilder &&rhs) noexcept = default;


TOCBuilder::~TOCBuilder() noexcept = default;


std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files)
{
	return std::make_unique<TOC>(
			impl_->build(track_count, offsets, leadout, files));
}


std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const std::vector<int32_t> &lengths,
		const std::vector<std::string> &files)
{
	return std::make_unique<TOC>(
			impl_->build(track_count, offsets, lengths, files));
}


std::unique_ptr<TOC> TOCBuilder::merge(const TOC &toc,
		const uint32_t leadout) const
{
	if (toc.complete())
	{
		return std::make_unique<TOC>(toc);
	}

	return std::make_unique<TOC>(impl_->merge(toc, leadout));
}


TOCBuilder& TOCBuilder::operator = (const TOCBuilder &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	// deep copy
	this->impl_ = std::make_unique<TOCBuilder::Impl>(*rhs.impl_);
	return *this;
}


TOCBuilder& TOCBuilder::operator = (TOCBuilder &&rhs) noexcept = default;


/// \endcond


// make_arid


std::unique_ptr<ARId> make_arid(const TOC &toc)
{
	ARIdBuilder builder;
	return builder.build(toc);
}


std::unique_ptr<ARId> make_arid(const TOC &toc, const uint32_t leadout)
{
	ARIdBuilder builder;
	return builder.build(toc, leadout);
}


std::unique_ptr<ARId> make_empty_arid()
{
	ARIdBuilder builder;
	return builder.build_empty_id();
}


// make_toc


std::unique_ptr<TOC> make_toc(const std::vector<int32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files)
{
	TOCBuilder builder;
	return builder.build(offsets.size(), offsets, leadout, files);
}


std::unique_ptr<TOC> make_toc(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files)
{
	TOCBuilder builder;
	return builder.build(track_count, offsets, leadout, files);
}


std::unique_ptr<TOC> make_toc(const std::vector<int32_t> &offsets,
		const std::vector<int32_t> &lengths,
		const std::vector<std::string> &files)
{
	TOCBuilder builder;
	return builder.build(offsets.size(), offsets, lengths, files);
}


std::unique_ptr<TOC> make_toc(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const std::vector<int32_t> &lengths,
		const std::vector<std::string> &files)
{
	TOCBuilder builder;
	return builder.build(track_count, offsets, lengths, files);
}


// unpublished worker functions


//uint32_t calculate_leadout(const TrackNo track_count,
//		const std::vector<uint32_t> &offsets,
//		const std::vector<uint32_t> &lengths)
//{
//	if (track_count < 1 or track_count > CDDA.MAX_TRACKCOUNT)
//	{
//		return 0;
//	}
//	auto tc = static_cast<std::vector<uint32_t>::size_type>(track_count) - 1u;
//	return (lengths.at(tc) > 0) ? offsets.at(tc) + lengths.at(tc) : 0;
//}


uint32_t leadout(const std::vector<uint32_t> &offsets,
		const std::vector<uint32_t> &lengths)
{
	return lengths.back() == 0 ? 0 : offsets.back() + lengths.back();
}


uint32_t leadout(const std::vector<uint32_t> &lengths)
{
	if (lengths.back() == 0)
	{
		return 0;
	}

	auto sum { std::accumulate(lengths.begin(), lengths.end(), 0) };
	auto max {
		static_cast<decltype(sum)>(std::numeric_limits<uint32_t>::max()) };

	if (sum > max)
	{
		return 0; // TODO
	}

	return static_cast<uint32_t>(sum);
}


} // namespace v_1_0_0

} // namespace arcstk
