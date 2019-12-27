#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#define __LIBARCSTK_IDENTIFIER_HPP__

/**
 * \file
 *
 * \brief Public API for \link id calculating AccurateRip ids\endlink
 */

#include <cstdint>
#include <memory>
#include <stdexcept>  // for logic_error
#include <string>
#include <vector>

namespace arcstk
{
inline namespace v_1_0_0
{


/// \addtogroup calc
/// @{


/**
 * \brief Data type for track numbers.
 *
 * Note that track numbers are 1-based. Hence, a TrackNo can also be used to
 * represent a track count. A track count has a legal maximum of 99.
 * Note that \c 0 is not a valid TrackNo.
 */
using TrackNo = int;


/**
 * \brief Type to represent a PCM 32 bit sample (2 channels x 16 bit).
 */
using sample_type = uint32_t;


/**
 * \brief Constants related to the CDDA format
 */
struct CDDA_t
{

	/**
	 * \brief CDDA: sampling rate of 44100 samples per second.
	 */
	const int SAMPLES_PER_SECOND = 44100;

	/**
	 * \brief CDDA: 16 bits per sample.
	 */
	const int BITS_PER_SAMPLE    = 16;

	/**
	 * \brief CDDA: stereo involves 2 channels.
	 */
	const int NUMBER_OF_CHANNELS = 2;

	/**
	 * \brief Number of frames per second is 75.
	 */
	const int FRAMES_PER_SEC     = 75;

	/**
	 * \brief Number of 4 bytes per sample.
	 *
	 * This follows from CDDA where
	 * 1 sample == 16 bit/sample * 2 channels / 8 bits/byte
	 */
	const int BYTES_PER_SAMPLE   = 4;

	/**
	 * \brief Number of 588 samples per frame.
	 *
	 * This follows from CDDA where 1 frame == 44100 samples/sec / 75 frames/sec
	 */
	const int SAMPLES_PER_FRAME  = 588;

	/**
	 * \brief Number of 2352 bytes per frame.
	 *
	 * This follows from CDDA where 1 frame == 588 samples * 4 bytes/sample
	 */
	const int BYTES_PER_FRAME    = 2352;

	/**
	 * \brief Maximal valid track count.
	 */
	const TrackNo MAX_TRACKCOUNT      = 99;

	/**
	 * \brief Redbook maximal valid block address is 99:59.74 (MSF) which is
	 * equivalent to 449.999 frames.
	 */
	const uint32_t MAX_BLOCK_ADDRESS  = ( 99 * 60 + 59 ) * 75 + 74;

	/**
	 * \brief Maximal valid offset value in cdda frames.
	 *
	 * Redbook defines 79:59.74 (MSF) (+leadin+leadout) as maximal play duration
	 * which is equivalent to 360.000 frames, thus the maximal offset is frame
	 * index 359.999.
	 */
	const uint32_t MAX_OFFSET         = ( 79 * 60 + 59 ) * 75 + 74;

	/**
	 * \brief Two subsequenct offsets must have a distance of at least this
	 * number of frames.
	 *
	 * The CDDA conforming minimal track length is 4 seconcs including 2 seconds
	 * pause, thus 4 sec * 75 frames/sec == 300 frames.
	 */
	const uint32_t MIN_TRACK_OFFSET_DIST = 300;

	/**
	 * \brief Minimal number of frames a track contains.
	 *
	 * The CDDA conforming minmal track length is 4 seconds including 2 seconds
	 * pause but the pause does not contribute to the track lengths, thus
	 * 2 sec * 75 frames/sec == 150 frames.
	 */
	const uint32_t MIN_TRACK_LEN_FRAMES = 150;

};

using CDDA_t = struct CDDA_t;


/**
 * \brief Global instance of the CDDA constants.
 */
extern const CDDA_t CDDA;

/** @} */

} // namespace v_1_0_0


inline namespace v_1_0_0
{

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
 * A TOC is the verified table of content information from a compact disc.
 * \link TOC TOCs\endlink are exclusively constructed by functions make_toc()
 * that try to validate the information used to construct the TOC. The
 * verification recognizes inconsistent input data that cannot form
 * a valid TOC. If the verification fails, an InvalidMetadataException is
 * thrown.
 *
 * @{
 */


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
class ARId final
{

public:

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
	std::string url() const;

	/**
	 * \brief Return the AccurateRip filename of the response file.
	 *
	 * \return AccurateRip filename of the response file
	 */
	std::string filename() const;

	/**
	 * \brief Return the track count.
	 *
	 * \return Track count of this medium
	 */
	TrackNo track_count() const;

	/**
	 * \brief Return the disc_id 1.
	 *
	 * \return Disc id 1 of this medium
	 */
	uint32_t disc_id_1() const;

	/**
	 * \brief Return the disc_id 2.
	 *
	 * \return Disc id 2 of this medium
	 */
	uint32_t disc_id_2() const;

	/**
	 * \brief Return the CDDB id.
	 *
	 * \return CDDB id of this medium
	 */
	uint32_t cddb_id() const;

	/**
	 * \brief Return the standard URL prefix for AccurateRip request urls.
	 *
	 * \return URL prefix for AccurateRip request URLs.
	 */
	std::string prefix() const;

	/**
	 * \brief Return TRUE iff this ARId is empty (holding no information).
	 *
	 * \return TRUE iff this ARId is empty
	 */
	bool empty() const;

	/**
	 * \brief Return a default string representation of this ARId.
	 *
	 * \return Default string representation of this ARId
	 */
	std::string to_string() const;

	/**
	 * \brief Returns TRUE iff this ARId holds identical values as \c rhs,
	 * otherwise FALSE.
	 *
	 * \param[in] rhs The right hand side of the comparison
	 *
	 * \return TRUE iff the right hand side is equal to the left hand side,
	 * otherwise false
	 */
	bool operator == (const ARId& rhs) const;

	/**
	 * \brief Returns TRUE if this ARId does not holds identical values as
	 * \c rhs, otherwise FALSE.
	 *
	 * \param[in] rhs The right hand side of the comparison
	 *
	 * \return TRUE iff the right hand side is not equal to the left hand side,
	 * otherwise false
	 */
	bool operator != (const ARId& rhs) const;

	/**
	 * \brief Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARId& operator = (const ARId &rhs);

	/**
	 * \brief Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
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
 * \brief Table of contents of a compact disc.
 *
 * A TOC contains the number of tracks, their offsets and optionally their
 * lengths, their filenames and the index of the leadout frame. Offsets and
 * lengths are represented in LBA frames.
 *
 * A TOC is an object constructed from parsed data. It is therefore not
 * modifiable.
 *
 * \link TOC TOCs\endlink can exclusively be built by to two build functions
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
class TOC final
{

public:

	// Forward declaration for private implementation
	class Impl;

	/**
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
	TrackNo track_count() const;

	/**
	 * \brief Return the offset of the 1-based specified track in frames, i.e.
	 * <tt>offsets(i)</tt> is the offset for track \p i iff \p i is
	 * a valid track number in this TOC, otherwise 0.
	 *
	 * \param[in] idx 1-based track number
	 *
	 * \return Offset of specified track
	 */
	uint32_t offset(const TrackNo idx) const;

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
	uint32_t parsed_length(const TrackNo idx) const;

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
	 * \todo Not sure about the leadout for CDs with a data track.
	 *
	 * \return The leadout frame index
	 */
	uint32_t leadout() const;

	/**
	 * \brief Return TRUE iff TOC information is complete, otherwise FALSE.
	 *
	 * A TOC \c t is complete, if <tt>t.leadout() != 0</tt>, otherwise it is
	 * not complete.
	 *
	 * \return TRUE iff TOC information is complete, otherwise FALSE.
	 */
	bool complete() const;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TOC& operator = (const TOC &rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TOC& operator = (TOC &&rhs) noexcept;

	/**
	 * Equality operator
	 *
	 * \param[in] rhs The right hand side of the comparison
	 *
	 * \return The right hand side of the comparison
	 */
	bool operator == (const TOC &rhs) const; // TODO free function


private:

	/**
	 * \brief Private implementation of TOC.
	 */
	std::unique_ptr<TOC::Impl> impl_;
};


/**
 * \brief Reports insufficient or invalid metadata for building a TOC.
 *
 * \todo Metadata files are usually parsed text files, hence position
 * information about the error may be useful.
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
 * \brief Create an ARId from a \link TOC::complete() complete()\endlink TOC.
 *
 * \details
 *
 * The input is validated.
 *
 * \param[in] toc TOC to use
 *
 * \return ARId
 *
 * \throws InvalidMetadataException
 * If \c toc is not \link TOC::complete() complete()\endlink.
 *
 * \see make_arid(const TOC &toc, const uint32_t leadout)
 * \see make_empty_arid()
 */
std::unique_ptr<ARId> make_arid(const TOC &toc);


/**
 * \brief Create an ARId from a TOC and a specified leadout.
 *
 * The input is validated.
 *
 * Parameter \c toc is allowed to be non-\link TOC::complete()
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
 * \throws InvalidMetadataException If \c toc and \c leadout are invalid
 *
 * \see make_arid(const TOC &toc)
 * \see make_empty_arid()
 */
std::unique_ptr<ARId> make_arid(const TOC &toc, const uint32_t leadout);


/**
 * \brief Creates an empty ARId
 *
 * \return An \link ARId::empty() empty()\endlink ARId
 *
 * \see make_arid(const TOC &toc, const uint32_t leadout)
 * \see make_arid(const TOC &toc)
 */
std::unique_ptr<ARId> make_empty_arid();


/**
 * \brief Create a \link TOC::complete() complete()\endlink TOC object from the
 * specified information.
 *
 * The input data is validated and the returned TOC is guaranteed to be
 * complete().
 *
 * \param[in] offsets     Offsets (in LBA frames) for each track
 * \param[in] leadout     Leadout frame
 * \param[in] files       File name of each track
 *
 * \return A complete() TOC object representing the specified information
 *
 * \throw InvalidMetadataException If the input data forms no valid and
 * complete() TOC
 *
 * \todo
 * The make_toc() function requiring a leadout does not accept a leadout of 0.
 * This behaviour is different from make_toc() with lengths, where the
 * last length is allowed to be 0. It is sensible to not enforce completeness
 * if no leadout is known. But it is inconsistent insofar as the client may
 * never create an incomplete TOC by using leadout 0, only by using a length of
 * 0 for the last track. It's reasonable but inconsistent.
 */
std::unique_ptr<TOC> make_toc(const std::vector<int32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files = {});


/**
 * \brief Create a \link TOC::complete() complete()\endlink TOC object from the
 * specified information.
 *
 * The input data is validated and the returned TOC is guaranteed to be
 * complete(). The value of \c track_count must be equal to \c offsets().size
 * and is just used for additional validation.
 *
 * \param[in] track_count Number of tracks in this medium
 * \param[in] offsets     Offsets (in LBA frames) for each track
 * \param[in] leadout     Leadout frame
 * \param[in] files       File name of each track
 *
 * \return A complete() TOC object representing the specified information
 *
 * \throw InvalidMetadataException If the input data forms no valid and
 * complete() TOC
 *
 * \todo
 * The make_toc() function requiring a leadout does not accept a leadout of 0.
 * This behaviour is different from make_toc() with lengths, where the
 * last length is allowed to be 0. It is sensible to not enforce completeness
 * if no leadout is known. But it is inconsistent insofar as the client may
 * never create an incomplete TOC by using leadout 0, only by using a length of
 * 0 for the last track. It's reasonable but inconsistent.
 */
std::unique_ptr<TOC> make_toc(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files = {});


/**
 * \brief Create a TOC object from the specified information.
 *
 * \note
 * The input data is validated but the length of the last track is allowed to
 * be 0. The returned TOC is therefore \em not guaranteed to be
 * \link TOC::complete() complete()\endlink. If the length of the last track is
 * a valid value, the resulting TOC will be complete() though.
 *
 * \param[in] offsets     Offsets (in LBA frames) of each track
 * \param[in] lengths     Lengths (in LBA frames) of each track
 * \param[in] files       File name of each track
 *
 * \return A TOC object representing the specified information
 *
 * \throw InvalidMetadataException If the input data forms no valid TOC
 */
std::unique_ptr<TOC> make_toc(const std::vector<int32_t> &offsets,
		const std::vector<int32_t> &lengths,
		const std::vector<std::string> &files = {});


/**
 * \brief Create a TOC object from the specified information.
 *
 * The value of \c track_count must be equal to \c offsets().size and is just
 * used for additional validation.
 *
 * \note
 * The input data is validated but the length of the last track is allowed to
 * be 0. The returned TOC is therefore \em not guaranteed to be
 * \link TOC::complete() complete()\endlink. If the length of the last track is
 * a valid value, the resulting TOC will be complete() though.
 *
 * \param[in] track_count Number of tracks in this medium
 * \param[in] offsets     Offsets (in LBA frames) of each track
 * \param[in] lengths     Lengths (in LBA frames) of each track
 * \param[in] files       File name of each track
 *
 * \return A TOC object representing the specified information
 *
 * \throw InvalidMetadataException If the input data forms no valid TOC
 */
std::unique_ptr<TOC> make_toc(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const std::vector<int32_t> &lengths,
		const std::vector<std::string> &files = {});


/**
 * \brief Functions assisting the management of @link TOC TOCs @endlink.
 *
 * \todo Avoid redundancy of 3 implementations. A template solution could do
 * that - or just use copy possibilities offered by \c std. Anyway, solution
 * should be generic for all 3 cases.
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
std::vector<uint32_t> get_offsets(const TOC &toc);


/**
 * \brief Extract parsed lengths from TOC in order.
 *
 * \param[in] toc The TOC
 *
 * \return List of parsed lengths from metafile
 */
std::vector<uint32_t> get_parsed_lengths(const TOC &toc);


/**
 * \brief Extract filenames from TOC in order.
 *
 * \param[in] toc The TOC
 *
 * \return List of filenames
 */
std::vector<std::string> get_filenames(const TOC &toc);

}

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

