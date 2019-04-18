/**
 * \file identifier.cpp Implementation of a low-level API for representing AccurateRip ids
 */


#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_DETAILS_HPP__
#include "identifier_details.hpp"
#endif

#include <cstdint>
#include <iomanip>   // for setw, setfill
#include <memory>
#include <sstream>   // for stringstream
#include <stdexcept> // for logic_error
#include <string>
#include <vector>

#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif


namespace arcstk
{

inline namespace v_1_0_0
{

/**
 * \brief Global instance of the CDDA constants
 */
const CDDA_t CDDA;

/// \cond IMPL_ONLY
/// \internal \addtogroup idImpl
/// @{

/**
 * \brief Private implementation of ARId
 *
 * \see ARId
 */
class ARId::Impl final
{

public:

	/**
	 * Implements ARId::ARId().
	 */
	Impl(const uint16_t track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id);

	/**
	 * Implements ARId::url()
	 */
	std::string url() const;

	/**
	 * Implements ARId::filename()
	 */
	std::string filename() const;

	/**
	 * Implements ARId::track_count()
	 */
	uint16_t track_count() const;

	/**
	 * Implements ARId::disc_id_1()
	 */
	uint32_t disc_id_1() const;

	/**
	 * Implements ARId::disc_id_2()
	 */
	uint32_t disc_id_2() const;

	/**
	 * Implements ARId::cddb_id()
	 */
	uint32_t cddb_id() const;

	/**
	 * Implements ARId::empty()
	 */
	bool empty() const;

	/**
	 * Implements ARId::to_string()
	 */
	std::string to_string() const;

	/**
	 * Implements ARId::operator == (const ARId &rhs) const
	 */
	bool operator == (const Impl &rhs) const;

	/**
	 * Implements ARId::operator != (const ARId &rhs) const
	 */
	bool operator != (const Impl &rhs) const;


protected:

	/**
	 * Service method: Compute the AccurateRip response filename
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
	std::string construct_filename(const uint16_t track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id) const;

	/**
	 * Service method: Compute the AccurateRip request URL
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
	std::string construct_url(const uint16_t track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id) const;


private:

	/**
	 * URL prefix for accessing AccurateRip
	 */
	const std::string AR_URL_PREFIX = "http://www.accuraterip.com/accuraterip/";

	/**
	 * Number of tracks
	 */
	uint16_t track_count_;

	/**
	 * Disc id no. 1
	 */
	uint32_t disc_id1_;

	/**
	 * Disc id no. 2
	 */
	uint32_t disc_id2_;

	/**
	 * CDDB disc id
	 */
	uint32_t cddb_id_;
};


/// @}
/// \endcond
// IMPL_ONLY


ARId::Impl::Impl(const uint16_t track_count, const uint32_t id_1,
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


uint16_t ARId::Impl::track_count() const
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


std::string ARId::Impl::construct_filename(const uint16_t track_count,
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
	ss << "dBAR"
	   << '-' << std::setw(3) << std::setfill('0')
			<< static_cast<uint32_t>(track_count);

	ss.flags(hex_flags);
	ss << std::nouppercase
	   << '-' << std::setw(8) << std::setfill('0') << id_1
	   << '-' << std::setw(8) << std::setfill('0') << id_2
	   << '-' << std::setw(8) << std::setfill('0') << cddb_id
	   << ".bin";

	return ss.str();
}


std::string ARId::Impl::construct_url(const uint16_t track_count,
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


/// \cond IMPL_ONLY
/// \internal \addtogroup idImpl
/// @{


namespace details
{


/**
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
	 * Default constructor
	 */
	Impl();

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	Impl(const Impl &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	Impl(Impl &&rhs) noexcept = default;

	/**
	 * Default destructor
	 */
	~Impl() noexcept = default;

	/**
	 * Implements TOCBuilder::build(const uint32_t track_count, const std::vector<int32_t> &offsets, const uint32_t leadout);
	 */
	std::unique_ptr<TOC::Impl> build(const uint32_t track_count,
			const std::vector<int32_t> &offsets,
			const uint32_t leadout);

	/**
	 * Implements TOCBuilder::build(const uint32_t track_count, const std::vector<int32_t> &offsets, const std::vector<int32_t> &lengths, const std::vector<std::string> &files)
	 */
	std::unique_ptr<TOC::Impl> build(const uint32_t track_count,
			const std::vector<int32_t> &offsets,
			const std::vector<int32_t> &lengths,
			const std::vector<std::string> &files);

	std::unique_ptr<TOC::Impl> merge(const TOC &toc, const uint32_t leadout)
		const;

	/**
	 * Copy assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TOCBuilder::Impl& operator = (const TOCBuilder::Impl &rhs);

	/**
	 * Move assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TOCBuilder::Impl& operator = (TOCBuilder::Impl &&rhs) noexcept = default;


protected:

	/**
	 * Service method: Builds a track count for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] track_count Number of tracks
	 *
	 * \return The intercepted track_count
	 *
	 * \throw InvalidMetadataException If the track count is not valid
	 */
	uint16_t build_track_count(const uint16_t track_count) const;

	/**
	 * Service method: Builds validated offsets for a TOC object.
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
			const uint16_t track_count, const uint32_t leadout) const;

	/**
	 * Service method: Builds validated offsets for a TOC object.
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
			const uint16_t track_count,
			const std::vector<int32_t> &lengths) const;

	/**
	 * Service method: Builds validated lengths for a TOC object.
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
			const uint16_t track_count) const;

	/**
	 * Service method: Builds validated leadout for a TOC object.
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
	 * Service method: Builds validated audio file list for a TOC object.
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
	 * Validator instance
	 */
	std::unique_ptr<details::TOCValidator> validator_;
};


} // namespace details


/**
 * \brief Private implementation of TOC.
 *
 * \see TOC
 */
class TOC::Impl final
{
	// TOCBuilder::Impl::build() methods are friends of TOC::Impl
	// since they construct TOC::Impls exclusively

	friend std::unique_ptr<TOC::Impl> details::TOCBuilder::Impl::build(
			const uint32_t track_count,
			const std::vector<int32_t> &offsets,
			const std::vector<int32_t> &lengths,
			const std::vector<std::string> &files);

	friend std::unique_ptr<TOC::Impl> details::TOCBuilder::Impl::build(
			const uint32_t track_count,
			const std::vector<int32_t> &offsets,
			const uint32_t leadout);

	friend std::unique_ptr<TOC::Impl> details::TOCBuilder::Impl::merge(
			const TOC &toc, const uint32_t leadout) const;

public:

	/**
	 * Implements TOC::track_count()
	 */
	uint16_t track_count() const;

	/**
	 * Implements TOC::offset(const uint8_t) const
	 */
	uint32_t offset(const TrackNo idx) const;

	/**
	 * Implements TOC::parsed_length(const uint8_t) const
	 */
	uint32_t parsed_length(const TrackNo idx) const;

	/**
	 * Implements TOC::filename(const TrackNo idx) const
	 */
	std::string filename(const TrackNo idx) const;

	/**
	 * Implements TOC::leadout()
	 */
	uint32_t leadout() const;

	/**
	 * Implements TOC::complete()
	 */
	bool complete() const;

	/**
	 * Implements TOC::operator==()
	 */
	bool operator == (const TOC::Impl &rhs) const;


private:

	/**
	 * Implements private constructor of TOC.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 * \param[in] leadout     Leadout frame
	 */
	Impl(const uint32_t track_count,
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout);

	/**
	 * Implements private constructor of TOC.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 * \param[in] lengths     Lengths (in CDDA frames) of each track
	 * \param[in] files       File name of each track
	 */
	Impl(const uint32_t track_count,
			const std::vector<uint32_t> &offsets,
			const std::vector<uint32_t> &lengths,
			const std::vector<std::string> &files);

	/**
	 * Number of tracks
	 */
	uint16_t track_count_;

	/**
	 * Track offsets (in frames)
	 */
	std::vector<uint32_t> offsets_;

	/**
	 * Track lengths (in frames)
	 */
	std::vector<uint32_t> lengths_;

	/**
	 * Leadout frame
	 */
	uint32_t leadout_;

	/**
	 * Audio file names
	 */
	std::vector<std::string> files_;
};


/// @}
/// \endcond
// IMPL_ONLY


// TOC::Impl


TOC::Impl::Impl(const uint32_t track_count,
		const std::vector<uint32_t> &offsets,
		const uint32_t leadout)
	: track_count_(track_count)
	, offsets_(offsets)
	, lengths_()
	, leadout_(leadout)
	, files_()
{
	// empty
}


TOC::Impl::Impl(const uint32_t track_count,
		const std::vector<uint32_t> &offsets,
		const std::vector<uint32_t> &lengths,
		const std::vector<std::string> &files)
	: track_count_(track_count)
	, offsets_(offsets)
	, lengths_(lengths)
	, leadout_( // compute leadout, if length of last track is known
			(lengths_.at(track_count - 1) > 0)
				? offsets_.at(track_count - 1) + lengths_.at(track_count - 1)
				: 0)
	, files_(files)
{
	// empty
}


uint16_t TOC::Impl::track_count() const
{
	return track_count_;
}


uint32_t TOC::Impl::offset(const TrackNo idx) const
{
	return idx < 1 || idx > offsets_.size() ? 0 : offsets_[idx - 1];
}


uint32_t TOC::Impl::parsed_length(const TrackNo idx) const
{
	return idx < 1 || idx > lengths_.size() ? 0 : lengths_[idx - 1];
}


std::string TOC::Impl::filename(const TrackNo idx) const
{
	return idx < 1 || idx > files_.size() ? std::string() : files_[idx - 1];
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


uint16_t TOC::track_count() const
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


ARId::ARId(const uint16_t track_count,
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


uint16_t ARId::track_count() const
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


/// \cond IMPL_ONLY
/// \internal \addtogroup idImpl
/// @{


namespace toc
{

std::vector<uint32_t> get_offsets(const TOC &toc)
{
	std::size_t size = toc.track_count();

	std::vector<uint32_t> offsets;
	offsets.reserve(size);

	for (unsigned int i = 1; i <= size; ++i)
	{
		offsets.emplace_back(toc.offset(i));
	}

	return offsets;
}


std::vector<uint32_t> get_parsed_lengths(const TOC &toc)
{
	std::size_t size = toc.track_count();

	std::vector<uint32_t> parsed_lengths;
	parsed_lengths.reserve(size);

	for (unsigned int i = 1; i <= size; ++i)
	{
		parsed_lengths.emplace_back(toc.parsed_length(i));
	}

	return parsed_lengths;
}


std::vector<std::string> get_filenames(const TOC &toc)
{
	std::vector<std::string> fnames;

	std::size_t file_count { toc.track_count() };
	fnames.reserve(file_count);

	for (unsigned int i = 1; i <= file_count; ++i)
	{
		fnames.emplace_back(toc.filename(i));
	}

	return fnames;
}

} // namespace toc


namespace details
{

/**
 * \brief Private implementation of ARIdBuilder
 *
 * \see ARIdBuilder
 */
class ARIdBuilder::Impl final
{

public:

	/**
	 * Implements ARIdBuilder::build(const TOC &toc, const uint32_t leadout) const
	 */
	std::unique_ptr<ARId> build(const TOC &toc, const uint32_t leadout) const;

	/**
	 * Implements ARIdBuilder::build_empty_id()
	 */
	std::unique_ptr<ARId> build_empty_id() const noexcept;


protected:

	/**
	 * Service method: Compute the disc id 1 from a vector of offsets.
	 * Vector offsets contains the frame offsets as parsed from the CUE sheet
	 * with the leadout frame added as an additional element on the back
	 * position.
	 *
	 * \param[in] track_count   Number of tracks in this medium
	 * \param[in] offsets       Offsets (in CDDA frames) of each track
	 * \param[in] leadout Leadout CDDA frame
	 */
	uint32_t disc_id_1(const uint16_t track_count,
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * Service method: Compute the disc id 2 from a vector of offsets.
	 * Vector offsets contains the frame offsets as parsed from the CUE sheet
	 * with the leadout frame added as an additional element on the back
	 * position.
	 *
	 * \param[in] track_count   Number of tracks in this medium
	 * \param[in] offsets       Offsets (in CDDA frames) of each track
	 * \param[in] leadout Leadout CDDA frame
	 */
	uint32_t disc_id_2(const uint16_t track_count,
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * Service method: Compute the CDDB disc id from a vector of offsets.
	 * Vector offsets contains the frame offsets as parsed from the CUE sheet
	 * with the leadout frame added as an additional element on the back
	 * position.
	 *
	 * \param[in] track_count   Number of tracks in this medium
	 * \param[in] offsets       Offsets (in CDDA frames) of each track
	 * \param[in] leadout Leadout CDDA frame
	 */
	uint32_t cddb_id(const uint16_t track_count,
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * Service method: sum up the digits of the number passed
	 *
	 * \param[in] number An unsigned integer number
	 *
	 * \return The sum of the digits of the number
	 */
	static uint64_t sum_digits(const uint32_t number);
};


std::unique_ptr<ARId> ARIdBuilder::Impl::build(const TOC &toc,
		const uint32_t leadout) const
{
	// Override TOC leadout with optional non-null extra leadout

	uint32_t leadout_val { leadout };

	if (leadout_val > 0)
	{
		details::TOCValidator validator;
		validator.validate(toc, leadout_val);
	} else
	{
		leadout_val = toc.leadout();
	}

	auto offsets = toc::get_offsets(toc);

	return std::make_unique<ARId>(
			toc.track_count(),
			this->disc_id_1(toc.track_count(), offsets, leadout_val),
			this->disc_id_2(toc.track_count(), offsets, leadout_val),
			this->cddb_id  (toc.track_count(), offsets, leadout_val)
	);
}


std::unique_ptr<ARId> ARIdBuilder::Impl::build_empty_id() const noexcept
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

// FIXME Is track_count required?
uint32_t ARIdBuilder::Impl::disc_id_1(const uint16_t /* track_count */,
		const std::vector<uint32_t> &offsets, const uint32_t leadout) const
{
	// disc id 1 is just the sum off all offsets + the leadout frame

	uint32_t sum_offsets = 0;

	for (const auto &o : offsets) { sum_offsets += o; }

	return sum_offsets + leadout;
}


uint32_t ARIdBuilder::Impl::disc_id_2(const uint16_t track_count,
		const std::vector<uint32_t> &offsets, const uint32_t leadout) const
{
	// disc id 2 is the sum of the products of offsets and the corresponding
	// 1-based track number while normalizing offsets to be >= 1

	uint32_t sum_offsets = 0;

	for (uint32_t i = 0; i < track_count; ++i)
	{
		// This will throw if offsets.size() < track_count
		sum_offsets += (offsets[i] > 0 ? offsets[i] : 1) * (i + 1);
	}

	return sum_offsets + leadout * (track_count + 1);
}


uint32_t ARIdBuilder::Impl::cddb_id(const uint16_t track_count,
		const std::vector<uint32_t> &offsets, const uint32_t leadout) const
{
	// The CDDB id is a 32bit unsigned integer, formed of a concatenation of
	// the following 3 numbers:
	// first chunk (8 bits):   checksum (sum of digit sums of offset secs + 2)
	// second chunk (16 bits): total seconds count
	// third chunk (8 bits):   number of tracks

	// Calculate first part: checksum

	uint32_t checksum = 0;

	for (uint32_t i = 0; i < track_count; ++i)
	{
		// This will throw if offsets.size() < track_count
		checksum += ARIdBuilder::Impl::sum_digits(
				offsets[i] / CDDA.FRAMES_PER_SEC + 2);
	}
	checksum %= 255; // normalize to 1 byte

	// Calculate second part: seconds count

	const uint32_t seconds_count = leadout / CDDA.FRAMES_PER_SEC
		- offsets[0] / CDDA.FRAMES_PER_SEC;

	return (checksum << 24u) | (seconds_count << 8u) | track_count;
}


uint64_t ARIdBuilder::Impl::sum_digits(const uint32_t number)
{
	return (number < 10) ? number : (number % 10) + sum_digits(number / 10);
}


// ARIdBuilder


ARIdBuilder::ARIdBuilder()
	: impl_(std::make_unique<ARIdBuilder::Impl>())
{
	// empty
}


ARIdBuilder::ARIdBuilder(const ARIdBuilder &builder)
	: impl_(std::make_unique<ARIdBuilder::Impl>(*builder.impl_)) // deep copy
{
	// empty
}


ARIdBuilder::ARIdBuilder(ARIdBuilder &&builder) noexcept = default;


ARIdBuilder::~ARIdBuilder() noexcept = default;


std::unique_ptr<ARId> ARIdBuilder::build(const uint32_t &track_count,
		const std::vector<int32_t> &offsets, const uint32_t leadout) const
{
	TOCBuilder builder;
	auto toc = builder.build(track_count, offsets, leadout);

	return impl_->build(*toc, 0);
}


std::unique_ptr<ARId> ARIdBuilder::build(const TOC &toc) const
{
	return impl_->build(toc, 0);
}


std::unique_ptr<ARId> ARIdBuilder::build(const TOC &toc, const uint32_t leadout)
	const
{
	return impl_->build(toc, leadout);
}


std::unique_ptr<ARId> ARIdBuilder::build_empty_id() noexcept
{
	return impl_->build_empty_id();
}


ARIdBuilder& ARIdBuilder::operator = (const ARIdBuilder &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	// deep copy
	impl_= std::make_unique<ARIdBuilder::Impl>(*rhs.impl_);
	return *this;
}


ARIdBuilder& ARIdBuilder::operator = (ARIdBuilder &&rhs) noexcept = default;


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


std::unique_ptr<TOC::Impl> TOCBuilder::Impl::build(const uint32_t track_count,
		const std::vector<int32_t> &offsets,
		const uint32_t leadout)
{
	return std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(offsets, track_count, leadout),
		build_leadout(leadout))
	);
}


std::unique_ptr<TOC::Impl> TOCBuilder::Impl::build(const uint32_t track_count,
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


uint16_t TOCBuilder::Impl::build_track_count(const uint16_t track_count) const
{
	validator_->validate_trackcount(track_count);

	return track_count;
}


std::vector<uint32_t> TOCBuilder::Impl::build_offsets(
		const std::vector<int32_t> &offsets, const uint16_t track_count,
		const uint32_t leadout) const
{
	validator_->validate(track_count, offsets, leadout);

	// Convert offsets to uints

	std::vector<uint32_t> uv(offsets.size());
	uv.assign(offsets.begin(), offsets.end());

	return uv;
}


std::vector<uint32_t> TOCBuilder::Impl::build_offsets(
		const std::vector<int32_t> &offsets, const uint16_t track_count,
		const std::vector<int32_t> &lengths) const
{
	// Valid number of lengths ?

	if (lengths.size() != track_count)
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(track_count) << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	if (lengths.size() != offsets.size())
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(offsets.size()) << " offsets";

		throw InvalidMetadataException(ss.str());
	}

	validator_->validate_lengths(lengths);

	validator_->validate_offsets(track_count, offsets);

	// Convert offsets to uints

	std::vector<uint32_t> uv(offsets.size());
	uv.assign(offsets.begin(), offsets.end());

	return uv;
}


std::vector<uint32_t> TOCBuilder::Impl::build_lengths(
		const std::vector<int32_t> &lengths, const uint16_t track_count) const
{
	// Valid number of lengths ?

	if (lengths.size() != track_count) // number of lenghts invalid
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(track_count) << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// If params make sense, use TOCValidator

	validator_->validate_lengths(lengths);

	// Convert ints to uints while normalizing the last length to 0

	std::vector<uint32_t> uv(lengths.size());
	uv.assign(lengths.begin(), lengths.end() - 1);
	uv.push_back(lengths.back() < 0 ? 0 : lengths.back());

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
	merged_lengths.reserve(toc.track_count());
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


std::unique_ptr<TOC> TOCBuilder::build(const uint32_t track_count,
		const std::vector<int32_t> &offsets,
		const uint32_t leadout)
{
	return std::make_unique<TOC>(
			impl_->build(track_count, offsets, leadout));
}


std::unique_ptr<TOC> TOCBuilder::build(const uint32_t track_count,
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


// TOCValidator


void TOCValidator::validate(const uint16_t track_count,
		const std::vector<int32_t> &offsets, const uint32_t leadout) const
{
	this->validate_leadout(leadout);

	// Validation: Leadout in Valid Distance after Last Offset?

	if (static_cast<int64_t>(leadout)
			< offsets.back() + CDDA.MIN_TRACK_LEN_FRAMES)
	{
		std::stringstream ss;
		ss << "Leadout frame " << leadout
			<< " is too near to last offset " << offsets.back()
			<< ". Minimal distance is " << CDDA.MIN_TRACK_LEN_FRAMES
			<< " frames." << " Bail out.";

		ARCS_LOG_ERROR << ss.str();

		throw InvalidMetadataException(ss.str());
	}

	this->validate_offsets(track_count, offsets);
}


void TOCValidator::validate(const TOC &toc, const uint32_t leadout) const
{
	this->validate_leadout(leadout);

	auto last_offset { toc.offset(toc.track_count()) };
	this->have_min_dist(last_offset, leadout);
}


void TOCValidator::validate_offsets(const uint16_t track_count,
		const std::vector<int32_t> &offsets) const
{
	this->validate_trackcount(track_count);

	// Validation: Track count Consistent with Number of Offsets?

	if (track_count != offsets.size())
	{
		std::stringstream ss;
		ss << "Track count does not match offset count." << " Bail out.";

		ARCS_LOG_ERROR << ss.str();

		throw InvalidMetadataException(ss.str());
	}

	this->validate_offsets(offsets);
}


void TOCValidator::validate_offsets(const std::vector<int32_t> &offsets) const
{
	// Number of offsets in legal range?

	if (offsets.empty())
	{
		std::stringstream ss;
		ss << "No offsets were given. Bail out.";

		ARCS_LOG_ERROR << ss.str();

		throw InvalidMetadataException(ss.str());
	}

	if (offsets.size() > CDDA.MAX_TRACKCOUNT)
	{
		std::stringstream ss;
		ss << "Offsets are only possible for at most "
			<< CDDA.MAX_TRACKCOUNT << " tracks";

		ARCS_LOG_ERROR << ss.str();

		throw InvalidMetadataException(ss.str());
	}

	// Explicitly allow the offset of the first track to be 0

	if (offsets[0] < 0)
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with negative offset for first track: "
			<< std::to_string(offsets[0]);

		throw InvalidMetadataException(ss.str());
	}

	// Check whether all subsequent Offsets have minimum distance

	for (uint8_t i = 1; i < offsets.size(); ++i)
	{
		// Is offset in a CDDA-legal range?

		if (offsets[i] > static_cast<int64_t>(CDDA.MAX_OFFSET))
		{
			std::stringstream ss;
			ss << "Offset " << std::to_string(offsets[i])
				<< " for track " << std::to_string(i);

			if (offsets[i] > static_cast<int64_t>(MAX_OFFSET_99))
			{
				ss << " exceeds physical range of 99 min ("
					<< std::to_string(MAX_OFFSET_99) << " frames)";

				ARCS_LOG_WARNING << ss.str();

			} else if (offsets[i] > static_cast<int64_t>(MAX_OFFSET_90))
			{
				ss << " exceeds physical range of 90 min ("
					<< std::to_string(MAX_OFFSET_90) << " frames)";

				ARCS_LOG_ERROR << ss.str();
			} else
			{
				ss << " exceeds redbook maximum duration of "
					<< std::to_string(CDDA.MAX_OFFSET);

				ARCS_LOG_INFO << ss.str();
			}

			throw InvalidMetadataException(ss.str());
		}

		// Has offset for current track at least minimum distance after
		// offset for last track?

		this->have_min_dist(offsets[i-1], offsets[i]);
	} // for
}


void TOCValidator::validate_lengths(const std::vector<int32_t> &lengths) const
{
	// Number of lengths in legal range?

	if (lengths.empty())
	{
		std::stringstream ss;
		ss << "No lengths were given. Bail out.";

		ARCS_LOG_ERROR << ss.str();

		throw InvalidMetadataException(ss.str());
	}

	if (lengths.size() > CDDA.MAX_TRACKCOUNT)
	{
		std::stringstream ss;
		ss << "Lengths are only possible for at most "
			<< CDDA.MAX_TRACKCOUNT << " tracks";

		ARCS_LOG_ERROR << ss.str();

		throw InvalidMetadataException(ss.str());
	}

	// Length values are valid?

	uint32_t sum_lengths = 0;

	// Skip last length, if it is not known (e.g. 0 or -1)
	int tracks = (lengths.back() < 1) ? lengths.size() - 1 : lengths.size();

	for (int i = 0; i < tracks; ++i)
	{
		if (lengths[i] < CDDA.MIN_TRACK_LEN_FRAMES)
		{
			std::stringstream ss;
			ss << "Cannot construct TOC with illegal length "
				<< std::to_string(lengths[i]) << " for track "
				<< std::to_string(i+1);

			throw InvalidMetadataException(ss.str());
		}

		sum_lengths += lengths[i];
	}

	// Sum of all lengths in legal range ?

	if (sum_lengths > CDDA.MAX_OFFSET)
	{
		std::stringstream ss;
		ss << "Total length " << std::to_string(sum_lengths);

		if (sum_lengths > MAX_OFFSET_99) // more than 99 min? => throw
		{
			ss << " exceeds physical range of 99 min ("
				<< std::to_string(MAX_OFFSET_99) << " frames)";

			ARCS_LOG_ERROR << ss.str();

			throw InvalidMetadataException(ss.str());

		} else if (sum_lengths > MAX_OFFSET_90) // more than 90 min? => warn
		{
			ss << " exceeds redbook maximum of "
				<< std::to_string(MAX_OFFSET_90) << " frames (90 min)";

			ARCS_LOG_WARNING << ss.str();

		} else // more than redbook originally defines? => info
		{
			ss << " exceeds redbook maximum of "
				<< std::to_string(CDDA.MAX_OFFSET);

			ARCS_LOG_INFO << ss.str();
		}
	}
}


void TOCValidator::validate_leadout(const uint32_t leadout) const
{
	// Greater than Minimum ?

	if (static_cast<int64_t>(leadout) < CDDA.MIN_TRACK_OFFSET_DIST)
	{
		std::stringstream ss;

		if (leadout == 0)
		{
			ss << "Leadout must not be 0";
		} else
		{
			ss << "Leadout " << leadout
				<< " is smaller than minimum track length";
		}

		ARCS_LOG_ERROR << ss.str();

		throw InvalidMetadataException(ss.str());
	}

	// Less than Maximum ?

	if (leadout > CDDA.MAX_BLOCK_ADDRESS)
	{
		std::stringstream ss;
		ss << "Leadout " << leadout << " exceeds physical maximum";

		ARCS_LOG_ERROR << ss.str();

		throw InvalidMetadataException(ss.str());
	}

	// Warning ?

	if (leadout > CDDA.MAX_OFFSET)
	{
		ARCS_LOG_WARNING << "Leadout " << leadout
			<< " exceeds redbook maximum";
	}
}


void TOCValidator::validate_trackcount(const uint16_t track_count) const
{
	if (track_count < 1 or track_count > 99)
	{
		std::stringstream ss;
		ss << "Cannot construct TOC from invalid track count: "
		   << std::to_string(track_count);

		throw InvalidMetadataException(ss.str());
	}
}


void TOCValidator::have_min_dist(const uint32_t prev_track,
		const uint32_t next_track) const
{
	if (next_track < prev_track + CDDA.MIN_TRACK_OFFSET_DIST)
	{
		std::stringstream ss;
		ss << "Track " << next_track
			<< " is too near to last track offset " << prev_track
			<< ". Minimal distance is " << CDDA.MIN_TRACK_LEN_FRAMES
			<< " frames." << " Bail out.";

		throw InvalidMetadataException(ss.str());
	}
}


} // namespace arcstk::details


/// @}
/// \endcond
// IMPL_ONLY


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


// make_toc


std::unique_ptr<TOC> make_toc(const uint32_t track_count,
		const std::vector<int32_t> &offsets,
		const uint32_t leadout)
{
	details::TOCBuilder builder;
	return builder.build(track_count, offsets, leadout);
}


std::unique_ptr<TOC> make_toc(const uint32_t track_count,
		const std::vector<int32_t> &offsets,
		const std::vector<int32_t> &lengths,
		const std::vector<std::string> &files)
{
	details::TOCBuilder builder;
	return builder.build(track_count, offsets, lengths, files);
}


} // namespace v_1_0_0

} // namespace arcstk

