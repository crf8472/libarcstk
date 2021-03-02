#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#define __LIBARCSTK_IDENTIFIER_HPP__

/**
 * \file
 *
 * \brief Public API for \link id calculating AccurateRip ids\endlink
 */

#include <cstdint>               // for uint32_t, int32_t, uint64_t
#include <initializer_list>      // for initializer_list
#include <memory>                // for unique_ptr
#include <stdexcept>             // for logic_error
#include <string>                // for string
#include <type_traits>           // for enable_if_t
#include <utility>               // for forward
#include <vector>                // for vector

#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

/// \addtogroup calc
/// @{

/**
 * \brief Type to represent amounts of LBA frames.
 *
 * A signed integer of at least 32 bit length.
 *
 * The type is required to be able to express the maximum frame count in a
 * medium. The value is CDDA.MAX_BLOCK_ADDRESS == 449.999 frames.
 *
 * The type is intended to perform arithmetic operations on it.
 */
using lba_count_t = int32_t;


/**
 * \brief Data type for track numbers.
 *
 * Valid track numbers are in the range of 1-99. Note that 0 is not a valid
 * TrackNo. Hence, a TrackNo is not suitable to represent a track count.
 *
 * A validation check is not provided, though.
 *
 * The intention of this typedef is to provide a marker for 1-based track
 * numbers in the interface.
 */
using TrackNo = int;


/**
 * \brief Constants related to the CDDA format
 */
struct CDDA_t final
{
	/**
	 * \brief CDDA: sampling rate of 44100 samples per second.
	 */
	const int SAMPLES_PER_SECOND { 44100 };

	/**
	 * \brief CDDA: 16 bits per sample.
	 */
	const int BITS_PER_SAMPLE    { 16 };

	/**
	 * \brief CDDA: stereo involves 2 channels.
	 */
	const int NUMBER_OF_CHANNELS { 2 };

	/**
	 * \brief Number of frames per second is 75.
	 */
	const int FRAMES_PER_SEC     { 75 };

	/**
	 * \brief Number of 4 bytes per sample.
	 *
	 * This follows from CDDA where
	 * 1 sample == 16 bit/sample * 2 channels / 8 bits/byte
	 */
	const int BYTES_PER_SAMPLE   { 4 };

	/**
	 * \brief Number of 588 samples per frame.
	 *
	 * This follows from CDDA where 1 frame == 44100 samples/sec / 75 frames/sec
	 */
	const int SAMPLES_PER_FRAME  { 588 };

	/**
	 * \brief Number of 2352 bytes per frame.
	 *
	 * This follows from CDDA where 1 frame == 588 samples * 4 bytes/sample
	 */
	const int BYTES_PER_FRAME    { 2352 };

	/**
	 * \brief Maximal valid track count.
	 */
	const TrackNo MAX_TRACKCOUNT { 99 };

	/**
	 * \brief Redbook maximal valid block address is 99:59.74 (MSF) which is
	 * equivalent to 449.999 frames.
	 */
	const lba_count_t MAX_BLOCK_ADDRESS { ( 99 * 60 + 59 ) * 75 + 74 };

	/**
	 * \brief Maximal valid offset value in cdda frames.
	 *
	 * Redbook defines 79:59.74 (MSF) (+leadin+leadout) as maximal play duration
	 * which is equivalent to 360.000 frames, thus the maximal offset is frame
	 * index 359.999.
	 */
	const lba_count_t MAX_OFFSET { ( 79 * 60 + 59 ) * 75 + 74 };

	/**
	 * \brief Two subsequenct offsets must have a distance of at least this
	 * number of frames.
	 *
	 * The CDDA conforming minimal track length is 4 seconcs including 2 seconds
	 * pause, thus 4 sec * 75 frames/sec == 300 frames.
	 */
	const lba_count_t MIN_TRACK_OFFSET_DIST { 300 };

	/**
	 * \brief Minimal number of frames a track contains.
	 *
	 * The CDDA conforming minmal track length is 4 seconds including 2 seconds
	 * pause but the pause does not contribute to the track lengths, thus
	 * 2 sec * 75 frames/sec == 150 frames.
	 */
	const lba_count_t MIN_TRACK_LEN_FRAMES { 150 };
};


/**
 * \brief Global instance of the CDDA constants.
 */
extern const CDDA_t CDDA;

/** @} */


/** \defgroup id AccurateRip IDs
 *
 * \brief Calculate and manage \link ARId AccurateRip identifier\endlink
 *
 * ARId is an AccurateRip identifier. It determines the request URL for an
 * album and as well its canonical savefile name. \link ARId ARIds\endlink are
 * constructed by other IDs and metadata like offsets and track count. As a
 * convenience, functions make_arid() construct the ARId of an album by its
 * TOC.
 *
 * A TOC is the validated table of content information from a compact disc.
 * \link TOC TOCs\endlink are exclusively constructed by functions make_toc()
 * that try to validate the information used to construct the TOC. The
 * validation recognizes inconsistent input data that cannot form
 * a valid TOC. If the validation fails, an InvalidMetadataException is
 * thrown.
 *
 * A NonstandardMetadataException indicates that the input is not conforming to
 * the redbook standard. This exception can occurr in tthe internal validation
 * mechanism but is currently not used in the public API.
 *
 * @{
 */

// forward declaration for operator==
class ARId; // IWYU pragma keep

bool operator == (const ARId& lhs, const ARId& rhs) noexcept;

/**
 * \brief AccurateRip-Identifier of a compact disc.
 *
 * The AccurateRip identifier determines the URL of the compact disc dataset as
 * well as the standard filename of the AccurateRip response.
 *
 * \link ARId ARIds\endlink can be constructed either from three
 * precomputed ids or from a TOC using function make_arid().
 *
 * In some cases, an ARId is syntactically required, but semantically
 * superflous. An ARId can be empty() to indicate that it carries no identifier.
 * An ARId that qualifies as empty() can be constructed by make_empty_arid().
 */
class ARId final : public Comparable<ARId>
{
public:

	friend bool operator == (const ARId& lhs, const ARId& rhs) noexcept;

	/**
	 * \brief Construct ARId.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] id_1        Id 1 of this medium
	 * \param[in] id_2        Id 2 of this medium
	 * \param[in] cddb_id     CDDB id of this medium
	 */
	ARId(const TrackNo track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs The ARId to copy
	 */
	ARId(const ARId &rhs);

	/**
	 * \brief Default move constructor.
	 *
	 * \param[in] rhs The ARId to move
	 */
	ARId(ARId &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~ARId() noexcept;

	/**
	 * \brief Return the AccurateRip request URL.
	 *
	 * \return The AccurateRip request URL
	 */
	std::string url() const noexcept;

	/**
	 * \brief Return the AccurateRip filename of the response file.
	 *
	 * \return AccurateRip filename of the response file
	 */
	std::string filename() const noexcept;

	/**
	 * \brief Return the track count.
	 *
	 * \return Track count of this medium
	 */
	TrackNo track_count() const noexcept;

	/**
	 * \brief Return the disc_id 1.
	 *
	 * \return Disc id 1 of this medium
	 */
	uint32_t disc_id_1() const noexcept;

	/**
	 * \brief Return the disc_id 2.
	 *
	 * \return Disc id 2 of this medium
	 */
	uint32_t disc_id_2() const noexcept;

	/**
	 * \brief Return the CDDB id.
	 *
	 * \return CDDB id of this medium
	 */
	uint32_t cddb_id() const noexcept;

	/**
	 * \brief Return the standard URL prefix for AccurateRip request urls.
	 *
	 * \return URL prefix for AccurateRip request URLs.
	 */
	std::string prefix() const noexcept;

	/**
	 * \brief Return TRUE iff this ARId is empty (holding no information).
	 *
	 * \return TRUE iff this ARId is empty
	 */
	bool empty() const noexcept;

	/**
	 * \brief Return a default string representation of this ARId.
	 *
	 * \return Default string representation of this ARId
	 */
	std::string to_string() const noexcept;


	ARId& operator = (const ARId &rhs);

	ARId& operator = (ARId &&rhs) noexcept;


private:

	// Forward declaration for the private implementation
	class Impl;

	/**
	 * \brief Private implementation of ARId.
	 */
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Global instance of an empty ARId.
 *
 * This is for convenience since in most cases, the creation of an empty
 * ARId can be avoided when a reference instance is at hand.
 *
 * The instance is created using make_empty_arid().
 */
extern const ARId EmptyARId;


// forward declaration for operator==
class TOC; // IWYU pragma keep

bool operator == (const TOC &lhs, const TOC &rhs) noexcept;

/**
 * \brief Table of contents of a compact disc.
 *
 * A TOC contains the number of tracks, their offsets and optionally their
 * lengths, their filenames and the index of the leadout frame. Offsets and
 * lengths are represented in LBA frames.
 *
 * A TOC is an object constructed from parsed data. It is therefore not
 * modifiable.
 *
 * \link TOC TOCs\endlink can exclusively be built by two build functions
 * called make_toc().
 * Both functions guarantee to provide either a valid TOC or to throw an
 * InvalidMetadataException. This entails that any concrete
 * TOC provides strong guarantees regarding the consistency of its content.
 *
 * Although an existing TOC is therefore always valid it might \em not be
 * complete().
 *
 * In some cases it may be sensible to construct an incomplete TOC, i.e. a TOC
 * without a leadout frame. Some toc formats (as for example CUESheet) may not
 * provide the leadout but the parsed metadata is required. In this case, the
 * leadout can be obtained from the actual audio data. However, also an
 * incomplete TOC may never be inconsistent.
 */
class TOC final : public Comparable<TOC>
{
public:

	friend bool operator == (const TOC &lhs, const TOC &rhs) noexcept;

	// Forward declaration for private implementation
	class Impl;

	/**
	 * \internal
	 * \brief Construct from private Implementation.
	 *
	 * Note that \link TOC TOCs\endlink are supposed to be constructed by a
	 * call to a builder function. This function ensures the guarantees hold for
	 * @link TOC TOCs\endlink. The logic to enforce all the necessary
	 * invariants is not to be placed in TOC.
	 *
	 * \param[in] impl The implementation of the TOC
	 */
	explicit TOC(std::unique_ptr<TOC::Impl> impl);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	TOC(const TOC &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	TOC(TOC &&rhs) noexcept;

	/**
	 * \brief Default constructor.
	 */
	~TOC() noexcept;

	/**
	 * \brief Return the number of tracks.
	 *
	 * \return Number of tracks
	 */
	TrackNo track_count() const noexcept;

	/**
	 * \brief Return the offset of the 1-based specified track in frames, i.e.
	 * <tt>offsets(i)</tt> is the offset for track \p i iff \p i is
	 * a valid track number in this TOC, otherwise 0.
	 *
	 * \param[in] idx 1-based track number
	 *
	 * \return Offset of specified track
	 */
	lba_count_t offset(const TrackNo idx) const;

	/**
	 * \brief Return the length of the 1-based specified track in frames as
	 * parsed from the toc metadata input.
	 *
	 * If the length for this track is not known, 0 is returned.
	 *
	 * \attention
	 * These lengths are the lengths as they were passed to the build function.
	 * They are not normalized or sanitized. They might be inconsistent with the
	 * offsets.
	 *
	 * \param[in] idx 1-based track number
	 *
	 * \return Length of specified track
	 */
	lba_count_t parsed_length(const TrackNo idx) const;

	/**
	 * \brief Return the file of the 1-based specified track, i.e.
	 * <tt>file(i)</tt> is the offset for track \p i iff \p i is a valid track
	 * number in this TOC.
	 *
	 * If all tracks are part of the same file, a call of <tt>file(i)</tt>
	 * will yield a string with identical content for any \p i.
	 *
	 * If the file for this track is not specified, an empty string is returned.
	 *
	 * \param[in] idx 1-based track number
	 *
	 * \return File of specified track
	 */
	std::string filename(const TrackNo idx) const;

	/**
	 * \brief Return the leadout frame LBA address.
	 *
	 * Should be either 0 if unknown, or, if known, equal to the sum of
	 * <tt>offset(track_count())</tt> and <tt>length(track_count())</tt>.
	 *
	 * \return The leadout frame index
	 */
	lba_count_t leadout() const noexcept;

	/**
	 * \brief Return TRUE iff TOC information is complete, otherwise FALSE.
	 *
	 * A TOC \c t is complete, if <tt>t.leadout() != 0</tt>, otherwise it is
	 * not complete.
	 *
	 * \return TRUE iff TOC information is complete, otherwise FALSE.
	 */
	bool complete() const noexcept;

	/**
	 * \brief Updates the leadout iff \c leadout validates.
	 *
	 * If \c leadout does not validate against the existing values,
	 * an InvalidMetadataException is thrown.
	 *
	 * \param[in] leadout The new leadout to update the TOC with
	 *
	 * \throws InvalidMetadateException iff validation fails
	 */
	void update(const lba_count_t leadout);


	TOC& operator = (const TOC &rhs);

	TOC& operator = (TOC &&rhs) noexcept;


private:

	/**
	 * \brief Private implementation of TOC.
	 */
	std::unique_ptr<TOC::Impl> impl_;
};


/**
 * \brief Reports invalid metadata for building a TOC.
 */
class InvalidMetadataException final : public std::logic_error
{
public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidMetadataException(const std::string &what_arg);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidMetadataException(const char *what_arg);
};


/**
 * \brief Reports metadata violating the redbook standard.
 *
 * Violating the redbook standard is usually not a problem for calculating
 * checksums. A common case are unusual total lengths, as for example up to 99
 * minutes.
 */
class NonstandardMetadataException final : public std::logic_error
{
public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit NonstandardMetadataException(const std::string &what_arg);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit NonstandardMetadataException(const char *what_arg);
};


/**
 * \brief Create an ARId from a
 * \link arcstk::v_1_0_0::TOC::complete() complete()\endlink TOC.
 *
 * \details
 *
 * The input is validated.
 *
 * \param[in] toc TOC to use
 *
 * \return ARId corresponding to the input TOC
 *
 * \throw InvalidMetadataException
 * If \c toc is not \link arcstk::v_1_0_0::TOC::complete() complete()\endlink.
 */
std::unique_ptr<ARId> make_arid(const TOC &toc);


/**
 * \copydoc arcstk::v_1_0_0::make_arid(const TOC &)
 */
std::unique_ptr<ARId> make_arid(const std::unique_ptr<TOC> &toc);


/**
 * \brief Create an ARId from a TOC and a specified leadout.
 *
 * The input is validated.
 *
 * Parameter \c toc is allowed to be non-\link arcstk::v_1_0_0::TOC::complete()
 * complete()\endlink. Parameter \c leadout is intended to provide the value
 * possibly missing in \c toc.
 *
 * If \c leadout is 0, \c toc.leadout() is used and \c leadout is ignored. If
 * \c leadout is not 0, \c toc.leadout() is ignored. If both values are 0
 * an InvalidMetadataException is thrown.
 *
 * If \c leadout is 0 and \c toc cannot be validated, an
 * InvalidMetadataException is thrown. If
 * \c leadout is not 0 and \c leadout and \c toc cannot be validated as
 * consistent with each other, an InvalidMetadataException is thrown.
 *
 * \param[in] toc     TOC to use
 * \param[in] leadout Leadout LBA frame
 *
 * \return ARId
 *
 * \throw InvalidMetadataException If \c toc and \c leadout are invalid
 */
std::unique_ptr<ARId> make_arid(const TOC &toc, const lba_count_t leadout);


/**
 * \copydoc arcstk::v_1_0_0::make_arid(const TOC &, const lba_count_t)
 */
std::unique_ptr<ARId> make_arid(const std::unique_ptr<TOC> &toc,
		const lba_count_t leadout);


/**
 * \brief Create an \link arcstk::v_1_0_0::ARId::empty() empty()\endlink ARId
 *
 * The implementation of this defines emptiness for ARIds.
 *
 * \return An \link arcstk::v_1_0_0::ARId::empty() empty()\endlink ARId
 */
std::unique_ptr<ARId> make_empty_arid() noexcept;


/**
 * \brief Functions assisting the management of @link TOC TOCs @endlink.
 */
namespace toc
{

/**
 * \brief Extract the offsets from a TOC to an iterable container
 *
 * \param[in] toc The TOC to get the offsets from
 *
 * \return The offsets of this TOC as an iterable container
 */
std::vector<lba_count_t> get_offsets(const TOC &toc);


/**
 * \copydoc get_offsets(const TOC&)
 */
std::vector<lba_count_t> get_offsets(const std::unique_ptr<TOC> &toc);


/**
 * \brief Extract parsed lengths from TOC in order.
 *
 * \param[in] toc The TOC
 *
 * \return List of parsed lengths from metafile
 */
std::vector<lba_count_t> get_parsed_lengths(const TOC &toc);


/**
 * \copydoc get_parsed_lengths(const TOC&)
 */
std::vector<lba_count_t> get_parsed_lengths(const std::unique_ptr<TOC> &toc);


/**
 * \brief Extract filenames from TOC in order.
 *
 * \param[in] toc The TOC
 *
 * \return List of filenames
 */
std::vector<std::string> get_filenames(const TOC &toc);


/**
 * \copydoc get_filenames(const TOC&)
 */
std::vector<std::string> get_filenames(const std::unique_ptr<TOC> &toc);

} // namespace toc

/** @} */


namespace details
{

/**
 * \internal
 * \ingroup id
 *
 * \brief Constructs \link arcstk::v_1_0_0::ARId ARIds \endlink from
 * \link arcstk::v_1_0_0::TOC TOC \endlink data.
 *
 * Constructs ARIds either from a TOC or from the triplet of track count, list
 * of offsets and leadout frame.
 *
 * ARIdBuilder validates its input and will refuse to construct invalid ARIds
 * from any data. Hence, if an ARId is returned, it is guaranteed to be correct.
 *
 * This class is considered an implementation detail.
 */
class ARIdBuilder final
{
public:

	/**
	 * \brief Build an ARId object from the specified TOC and leadout.
	 *
	 * Actual parameters \c toc and \c leadout are validated against each other.
	 *
	 * \param[in] toc     TOC to build ARId from
	 * \param[in] leadout Leadout frame
	 *
	 * \return An ARId object representing the specified information
	 *
	 * \throw InvalidMetadataException If the TOC forms no valid ARId
	 */
	static std::unique_ptr<ARId> build(const TOC &toc, const lba_count_t leadout);

	/**
	 * \brief Build an ARId object from the specified TOC.
	 *
	 * \param[in] toc TOC to build ARId from
	 *
	 * \return An ARId object representing the specified information
	 *
	 * \throw InvalidMetadataException If the TOC forms no valid ARId
	 */
	static std::unique_ptr<ARId> build(const TOC &toc);

	/**
	 * \brief Safely construct an empty ARId.
	 *
	 * An empty ARId has the invalid value 0 for the track count and also 0
	 * for disc id 1, disc id 2 and cddb id. An empty ARId is not a valid
	 * description of a CDDA medium.
	 *
	 * Building an empty ARId also provides the possibility to just provide an
	 * ARId on sites where an ARId is required without having to test for null.
	 *
	 * It may help provide an uniforming implementation of cases where
	 * an ARId in fact is expected but cannot be provided due to missing
	 * data, e.g. when processing single tracks without knowing the offset.
	 *
	 * \return An empty ARId
	 */
	static std::unique_ptr<ARId> build_empty_id() noexcept;


private:

	/**
	 * \brief Perform the actual build process.
	 *
	 * \param[in] toc         The TOC to use
	 * \param[in] leadout     Leadout frame
	 *
	 * \return An ARId object representing the specified information
	 *
	 * \throw InvalidMetadataException If the parameters form no valid ARId
	 */
	static std::unique_ptr<ARId> build_worker(const TOC &toc,
			const lba_count_t leadout);

	/**
	 * \brief Service method: Compute the disc id 1 from offsets and leadout.
	 *
	 * \param[in] offsets Offsets (in LBA frames) of each track
	 * \param[in] leadout Leadout LBA frame
	 */
	static uint32_t disc_id_1(const std::vector<lba_count_t> &offsets,
			const lba_count_t leadout) noexcept;

	/**
	 * \brief Service method: Compute the disc id 2 from offsets and leadout.
	 *
	 * \param[in] offsets Offsets (in LBA frames) of each track
	 * \param[in] leadout Leadout LBA frame
	 */
	static uint32_t disc_id_2(const std::vector<lba_count_t> &offsets,
			const lba_count_t leadout) noexcept;

	/**
	 * \brief Service method: Compute the CDDB id from offsets and leadout.
	 *
	 * The CDDB id is a 32bit unsigned integer, formed of a concatenation of
	 * the following 3 numbers:
	 * first chunk (8 bits):   checksum (sum of digit sums of offset secs + 2)
	 * second chunk (16 bits): total seconds count
	 * third chunk (8 bits):   number of tracks
	 *
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 * \param[in] leadout     Leadout LBA frame
	 */
	static uint32_t cddb_id(const std::vector<lba_count_t> &offsets,
			const lba_count_t leadout) noexcept;

	/**
	 * \brief Service method: sum up the digits of the number passed
	 *
	 * \param[in] number An unsigned integer number
	 *
	 * \return The sum of the digits of the number
	 */
	static uint64_t sum_digits(const uint32_t number) noexcept;
};

} //namespace details

} //namespace v_1_0_0

} // namespace arcstk


#ifndef __LIBARCSTK_VALIDATE_TPP__
#include "details/validate.tpp"
#endif


#ifndef __LIBARCSTK_BUILDERS_TPP__
#include "details/builders.tpp"
#endif


namespace arcstk
{
inline namespace v_1_0_0
{

/// \addtogroup id
/// @{

/**
 * \brief Defined iff T is a numerical integer type of at least 32 bit size.
 */
template <typename T>
using IsLBAType = std::enable_if_t<details::is_lba_type<T>::value>;

/**
 * \brief Defined iff T is a container whose value_type IsLBAType.
 *
 * To qualify as an LBAContainer, the class must have const versions of begin()
 * and end(), a const_iterator, a size() and a value_type that IsLBAType.
 */
template <typename T>
using IsLBAContainer = std::enable_if_t<details::is_lba_container<T>::value>;

/**
 * \brief Defined iff T is a filename type, i.e. std::string or std::wstring.
 *
 * \note
 * You may define your own string type to qualify as FilenameType by adding a
 * specialization of the internal is_filename_type template for your type.
 * \code{.cpp}
 * template <>
 * struct arcstk::details::is_filename_type<MyType> : public std::true_type { };
 * \endcode
 */
template <typename T>
using IsFilenameType = std::enable_if_t<details::is_filename_type<T>::value>;

/**
 * \brief Defined iff T is a container whose value_type IsFilenameType.
 *
 * To qualify as a FilenameContainer, the class must have const versions of
 * begin() and end(), a const_iterator, a size() and a value_type that
 * IsFilenameType.
 */
template <typename T>
using IsFilenameContainer =
	std::enable_if_t<details::is_filename_container<T>::value>;


/**
 * \brief Create a TOC object from the specified information.
 *
 * The input data is validated but the leadout is allowed to be 0. The returned
 * TOC is therefore not guaranteed to be
 * \link arcstk::v_1_0_0::TOC::complete() complete()\endlink.
 *
 * Value \c offsets.size() is assumed to be the number of total tracks.
 *
 * \tparam LBAContainer      Container type of the offsets container
 * \tparam FilenameContainer Container type of the optional filename container
 *
 * \param[in] offsets Offsets (in LBA frames) for each track
 * \param[in] leadout Leadout frame
 * \param[in] files   File name of each track
 *
 * \return A TOC object representing the specified information
 *
 * \throw InvalidMetadataException If the input forms no valid TOC
 */
template <typename LBAContainer,
	typename FilenameContainer = std::vector<std::string>,
	typename = IsLBAContainer<LBAContainer>,
	typename = IsFilenameContainer<FilenameContainer> >
inline std::unique_ptr<TOC> make_toc(LBAContainer&& offsets,
		const lba_count_t leadout,
		FilenameContainer&& files = {})
{
	using ::arcstk::details::TOCBuilder;

	return TOCBuilder::build(
			offsets.size(),
			std::forward<LBAContainer>(offsets),
			leadout,
			std::forward<FilenameContainer>(files));
}


/**
 * \brief Create a TOC object from the specified information.
 *
 * The input data is validated but the leadout is allowed to be 0. The returned
 * TOC is therefore not guaranteed to be
 * \link arcstk::v_1_0_0::TOC::complete() complete()\endlink.
 *
 * The value of \c track_count must be equal to \c offsets().size and is just
 * used for additional validation.
 *
 * \tparam LBAContainer      Container type of the offsets container
 * \tparam FilenameContainer Container type of the optional filename container
 *
 * \param[in] track_count Number of tracks in this medium
 * \param[in] offsets     Offsets (in LBA frames) for each track
 * \param[in] leadout     Leadout frame
 * \param[in] files       File name of each track
 *
 * \return A TOC object representing the specified information
 *
 * \throw InvalidMetadataException If the input forms no valid TOC
 */
template <typename LBAContainer,
	typename FilenameContainer = std::vector<std::string>,
	typename = IsLBAContainer<LBAContainer>,
	typename = IsFilenameContainer<FilenameContainer> >
std::unique_ptr<TOC> make_toc(const TrackNo track_count,
		LBAContainer&& offsets,
		const lba_count_t leadout,
		FilenameContainer&& files = {})
{
	using ::arcstk::details::TOCBuilder;

	return TOCBuilder::build(
			track_count,
			std::forward<LBAContainer>(offsets),
			leadout,
			std::forward<FilenameContainer>(files));
}


/**
 * \brief Create a TOC object from the specified information.
 *
 * The input data is validated but the length of the last track is allowed to
 * be 0. The returned TOC is therefore not guaranteed to be
 * \link arcstk::v_1_0_0::TOC::complete() complete()\endlink.
 *
 * Value \c offsets.size() is assumed to be the number of total tracks.
 *
 * Value \c offsets.size() and \c lengths.size() must be equal.
 *
 * \tparam LBAContainer1     Container type of the offsets container
 * \tparam LBAContainer2     Container type of the lengths container
 * \tparam FilenameContainer Container type of the optional filename container
 *
 * \param[in] offsets Offsets (in LBA frames) of each track
 * \param[in] lengths Lengths (in LBA frames) of each track
 * \param[in] files   File name of each track
 *
 * \return A TOC object representing the specified information
 *
 * \throw InvalidMetadataException If the input forms no valid TOC
 */
template <typename LBAContainer1, typename LBAContainer2,
	typename FilenameContainer = std::vector<std::string>,
	typename = IsLBAContainer<LBAContainer1>,
	typename = IsLBAContainer<LBAContainer2>,
	typename = IsFilenameContainer<FilenameContainer> >
std::unique_ptr<TOC> make_toc(LBAContainer1&& offsets,
		LBAContainer2&& lengths,
		FilenameContainer&& files = {})
{
	using ::arcstk::details::TOCBuilder;

	return TOCBuilder::build(
			offsets.size(),
			std::forward<LBAContainer1>(offsets),
			std::forward<LBAContainer2>(lengths),
			std::forward<FilenameContainer>(files));
}


/**
 * \brief Create a TOC object from the specified information.
 *
 * The input data is validated but the length of the last track is allowed to
 * be 0. The returned TOC is therefore not guaranteed to be
 * \link arcstk::v_1_0_0::TOC::complete() complete()\endlink.
 *
 * The value of \c track_count must be equal to \c offsets().size and is just
 * used for additional validation.
 *
 * Value \c offsets.size() and \c lengths.size() must be equal.
 *
 * \tparam LBAContainer1     Container type of the offsets container
 * \tparam LBAContainer2     Container type of the lengths container
 * \tparam FilenameContainer Container type of the optional filename container
 *
 * \param[in] track_count Number of tracks in this medium
 * \param[in] offsets     Offsets (in LBA frames) of each track
 * \param[in] lengths     Lengths (in LBA frames) of each track
 * \param[in] files       File name of each track
 *
 * \return A TOC object representing the specified information
 *
 * \throw InvalidMetadataException If the input forms no valid TOC
 */
template <typename LBAContainer1, typename LBAContainer2,
	typename FilenameContainer = std::vector<std::string>,
	typename = IsLBAContainer<LBAContainer1>,
	typename = IsLBAContainer<LBAContainer2>,
	typename = IsFilenameContainer<FilenameContainer> >
std::unique_ptr<TOC> make_toc(const TrackNo track_count,
		LBAContainer1&& offsets,
		LBAContainer2&& lengths,
		FilenameContainer&& files = {})
{
	using ::arcstk::details::TOCBuilder;

	return TOCBuilder::build(
			track_count,
			std::forward<LBAContainer1>(offsets),
			std::forward<LBAContainer2>(lengths),
			std::forward<FilenameContainer>(files));
}


/**
 * \brief Build an ARId object from the specified information.
 *
 * \tparam LBAContainer  Container type of the offsets container
 *
 * \param[in] track_count Track count
 * \param[in] offsets     Offsets
 * \param[in] leadout     Leadout frame
 *
 * \return An ARId object representing the specified information
 *
 * \throw InvalidMetadataException If the input forms no valid ARId
 */
template <typename LBAContainer, typename = IsLBAContainer<LBAContainer> >
inline std::unique_ptr<ARId> make_arid(const TrackNo track_count,
	LBAContainer&& offsets, const lba_count_t leadout)
{
	auto toc = make_toc(track_count,
			std::forward<LBAContainer>(offsets),
			leadout);

	return details::ARIdBuilder::build(*toc);
}


/**
 * \brief Build an ARId object from the specified information.
 *
 * \tparam LBAContainer  Container type of the offsets container
 *
 * \param[in] offsets     Offsets
 * \param[in] leadout     Leadout frame
 *
 * \return An ARId object representing the specified information
 *
 * \throw InvalidMetadataException If the input forms no valid ARId
 */
template <typename LBAContainer, typename = IsLBAContainer<LBAContainer> >
inline std::unique_ptr<ARId> make_arid(LBAContainer&& offsets,
		const lba_count_t leadout)
{
	auto toc = make_toc(std::forward<LBAContainer>(offsets), leadout);

	return details::ARIdBuilder::build(*toc);
}


/**
 * \brief Build an ARId object from the specified information.
 *
 * \tparam T Type of the offsets
 *
 * \param[in] track_count Track count
 * \param[in] offsets     Offsets
 * \param[in] leadout     Leadout frame
 *
 * \return An ARId object representing the specified information
 *
 * \throw InvalidMetadataException If the input forms no valid ARId
 */
template <typename T, typename = IsLBAType<T> >
inline std::unique_ptr<ARId> make_arid(const TrackNo track_count,
	std::initializer_list<T> offsets, const lba_count_t leadout)
{
	return make_arid(track_count, std::vector<T>{offsets}, leadout);
}


/**
 * \brief Build an ARId object from the specified information.
 *
 * \tparam T Type of the offsets
 *
 * \param[in] offsets     Offsets
 * \param[in] leadout     Leadout frame
 *
 * \return An ARId object representing the specified information
 *
 * \throw InvalidMetadataException If the input forms no valid ARId
 */
template <typename T, typename = IsLBAType<T> >
inline std::unique_ptr<ARId> make_arid(std::initializer_list<T> offsets,
		const lba_count_t leadout)
{
	return make_arid(std::vector<T>{offsets}, leadout);
}

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

