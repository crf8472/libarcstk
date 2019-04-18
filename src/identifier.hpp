#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#define __LIBARCSTK_IDENTIFIER_HPP__


/**
 * \file identifier.hpp API for constructing and representing AccurateRip ids
 *
 * ARId is an AccurateRip identifier. It can be constructed either from
 * three precomputed ids or from a TOC using function make_arid(). A
 * TOC is the verified table of content information from a compact disc.
 * <tt>TOC</tt>s are exclusively constructed by make_toc() that tries to
 * validate the information used to construct the TOC. If this information
 * cannot be verified, an InvalidMetadataException is thrown.
 */


#include <cstdint>
#include <memory>
#include <stdexcept>  // for logic_error
#include <string>
#include <vector>


namespace arcstk
{

/// \addtogroup calc
/// @{

inline namespace v_1_0_0
{


/**
 * \brief Data type for track numbers.
 *
 * Note that track numbers are 1-based and \c 0 is not a valid TrackNo.
 */
using TrackNo = uint8_t;


/**
 * \brief Constants related to the CDDA format
 */
struct CDDA_t {

	/**
	 * CDDA: sampling rate of 44100 samples per second
	 */
	const uint32_t SAMPLES_PER_SECOND = 44100;

	/**
	 * CDDA: 16 bits per sample
	 */
	const uint32_t BITS_PER_SAMPLE    = 16;

	/**
	 * CDDA: stereo involves 2 channels
	 */
	const uint32_t NUMBER_OF_CHANNELS = 2;

	/**
	 * Number of frames per second is 75
	 */
	const uint32_t FRAMES_PER_SEC     = 75;

	/**
	 * Number of 4 bytes per sample
	 *
	 * This follows from CDDA where
	 * 1 sample == 16 bit/sample * 2 channels / 8 bits/byte
	 */
	const uint32_t BYTES_PER_SAMPLE   = 4;

	/**
	 * Number of 588 samples per frame
	 *
	 * This follows from CDDA where 1 frame == 44100 samples/sec / 75 frames/sec
	 */
	const uint32_t SAMPLES_PER_FRAME  = 588;

	/**
	 * Number of 2352 bytes per frame
	 *
	 * This follows from CDDA where 1 frame == 588 samples * 4 bytes/sample
	 */
	const uint32_t BYTES_PER_FRAME    = 2352;

	/**
	 * Maximal valid track count.
	 */
	const uint16_t MAX_TRACKCOUNT     = 99;

	/**
	 * Redbook maximal valid block address is 99:59.74 (MSF) which is
	 * equivalent to 449.999 frames.
	 */
	const uint32_t MAX_BLOCK_ADDRESS  = ( 99 * 60 + 59 ) * 75 + 74;

	/**
	 * Maximal valid offset value in cdda frames.
	 *
	 * Redbook defines 79:59.74 (MSF) (+leadin+leadout) as maximal play duration
	 * which is equivalent to 360.000 frames, thus the maximal offset is frame
	 * index 359.999.
	 */
	const uint32_t MAX_OFFSET         = ( 79 * 60 + 59 ) * 75 + 74;

	/**
	 * Two subsequenct offsets must have a distance of at least this number of
	 * frames.
	 *
	 * The CDDA conforming minimal track length is 4 seconcs including 2 seconds
	 * pause, thus 4 sec * 75 frames/sec == 300 frames.
	 */
	const int32_t MIN_TRACK_OFFSET_DIST = 300;

	/**
	 * Minimal number of frames a track contains.
	 *
	 * The CDDA conforming minmal track length is 4 seconds including 2 seconds
	 * pause but the pause does not contribute to the track lengths, thus
	 * 2 sec * 75 frames/sec == 150 frames.
	 */
	const int32_t MIN_TRACK_LEN_FRAMES = 150;

};

using CDDA_t = struct CDDA_t;


/**
 * Global instance of the CDDA constants.
 */
extern const CDDA_t CDDA;

} // namespace v_1_0_0

/// @}

/// \defgroup id AccurateRip IDs
/// @{

inline namespace v_1_0_0
{


/**
 * \brief AccurateRip-Identifier of a compact disc.
 *
 * The identifier determines the URL of the compact disc dataset as well as the
 * filename of the AccurateRip response.
 */
class ARId final
{

public:

	/**
	 * Construct ARId from \c track_count and the corresponding ids.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] id_1        Id 1 of this medium
	 * \param[in] id_2        Id 2 of this medium
	 * \param[in] cddb_id     CDDB id of this medium
	 */
	ARId(const uint16_t track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id);

	/**
	 * Copy constructor.
	 *
	 * \param[in] rhs The ARId to copy
	 */
	ARId(const ARId &rhs);

	/**
	 * Move constructor.
	 *
	 * \param[in] rhs The ARId to move
	 */
	ARId(ARId &&rhs) noexcept;

	/**
	 * Default destructor.
	 */
	~ARId() noexcept;

	/**
	 * Return the AccurateRip request URL.
	 *
	 * \return The AccurateRip request URL
	 */
	std::string url() const;

	/**
	 * Return the AccurateRip filename of the response file.
	 *
	 * \return AccurateRip filename of the response file
	 */
	std::string filename() const;

	/**
	 * Return the track count.
	 *
	 * \return Track count of this medium
	 */
	uint16_t track_count() const;

	/**
	 * Return the disc_id 1.
	 *
	 * \return Disc id 1 of this medium
	 */
	uint32_t disc_id_1() const;

	/**
	 * Return the disc_id 2.
	 *
	 * \return Disc id 2 of this medium
	 */
	uint32_t disc_id_2() const;

	/**
	 * Return the CDDB id.
	 *
	 * \return CDDB id of this medium
	 */
	uint32_t cddb_id() const;

	/**
	 * Return a default string representation of this ARId
	 *
	 * \return Default string representation of this ARId
	 */
	std::string to_string() const;

	/**
	 * Return TRUE iff this ARId is empty (holding no information).
	 *
	 * \return TRUE iff this ARId is empty
	 */
	bool empty() const;

	/**
	 * Returns TRUE iff this ARId holds identical values as \c rhs,
	 * otherwise FALSE.
	 *
	 * \param[in] rhs The right hand side of the comparison
	 *
	 * \return TRUE iff the right hand side is equal to the left hand side,
	 * otherwise false
	 */
	bool operator == (const ARId& rhs) const;

	/**
	 * Returns TRUE if this ARId does not holds identical values as \c rhs,
	 * otherwise FALSE.
	 *
	 * \param[in] rhs The right hand side of the comparison
	 *
	 * \return TRUE iff the right hand side is not equal to the left hand side,
	 * otherwise false
	 */
	bool operator != (const ARId& rhs) const;

	/**
	 * Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARId& operator = (const ARId &rhs);

	/**
	 * Move assignment operator.
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
	 * Private implementation of ARId
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
 * <tt>TOC</tt>s can exclusively be built by function <tt>make_toc()</tt>. This
 * function guarantees to provide either a valid TOC or to throw an
 * InvalidMetadataException. This entails that any concrete
 * <tt>TOC</tt>s makes strong guarantees regarding the consitency of its
 * content.
 */
class TOC final
{

public:

	// Forward declaration for private implementation
	class Impl;

	/**
	 * Construct from private Implementation.
	 *
	 * Note that <tt>TOC</tt>s are supposed to be constructed by a call to
	 * make_toc(). This function ensures the guarantees hold for
	 * <tt>TOC</tt>s. The logic to enforce all the necessary invariants is not
	 * to be placed in TOC.
	 *
	 * \param[in] impl The implementation of the TOC
	 */
	explicit TOC(std::unique_ptr<TOC::Impl> impl);

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	TOC(const TOC &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	TOC(TOC &&rhs) noexcept;

	/**
	 * Default constructor
	 */
	~TOC() noexcept;

	/**
	 * Return the number of tracks.
	 *
	 * \return Number of tracks
	 */
	uint16_t track_count() const;

	/**
	 * Return the offset of the 1-based specified track in frames, i.e.
	 * <tt>offsets(i)</tt> is the offset for track \p i iff \p i is
	 * a valid track number in this TOC, otherwise 0.
	 *
	 * \param[in] idx 1-based track number
	 *
	 * \return Offset of specified track
	 */
	uint32_t offset(const TrackNo idx) const;

	/**
	 * Return the length of the 1-based specified track in frames as
	 * parsed from the toc metadata input.
	 *
	 * If the length for this track is not known, 0 is returned.
	 *
	 * \param[in] idx 1-based track number
	 *
	 * \return Length of specified track
	 */
	uint32_t parsed_length(const TrackNo idx) const;

	/**
	 * Return the file of the 1-based specified track, i.e. <tt>file(i)</tt> is
	 * the offset for track \p i iff \p i is a valid track number in
	 * this TOC.
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
	 * Return the leadout frame LBA address.
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
	 * Return TRUE iff TOC information is complete, otherwise FALSE.
	 *
	 * A TOC \c t is complete, if <tt>t.leadout() != 0</tt>, otherwise it is
	 * not complete.
	 *
	 * \return TRUE iff TOC information is complete, otherwise FALSE.
	 */
	bool complete() const;

	/**
	 * Copy assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TOC& operator = (const TOC &rhs);

	/**
	 * Move assignment
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
	 * Private implementation of TOC
	 */
	std::unique_ptr<TOC::Impl> impl_;
};


/**
 * \brief Create an ARId from a TOC.
 *
 * Object \c toc must have a non-zero leadout, otherwise an
 * InvalidMetadataException is thrown.
 *
 * \param[in] toc TOC to use
 *
 * \return ARId
 *
 * \throw InvalidMetadataException If \c toc has <tt>toc.leadout() == 0</tt>
 */
std::unique_ptr<ARId> make_arid(const TOC &toc);


/**
 * \brief Create an ARId from a TOC and a separately specified leadout.
 *
 * If \c leadout is 0 , \c toc.leadout() is used. One of both values must be
 * non-zero, otherwise an InvalidMetadataException is thrown. If \c toc
 * and \c leadout cannot be validated as consistent, an
 * InvalidMetadataException is thrown.
 *
 * \param[in] toc     TOC to use
 * \param[in] leadout Leadout LBA frame
 *
 * \return ARId
 *
 * \throw InvalidMetadataException If \c toc and \c leadout are invalid
 */
std::unique_ptr<ARId> make_arid(const TOC &toc, const uint32_t leadout);


/**
 * \brief Creates an empty ARId
 *
 * \return An empty ARId
 */
std::unique_ptr<ARId> make_empty_arid();


/**
 * \brief Create a TOC object from the specified information.
 *
 * The returned TOC is guaranteed to be complete.
 *
 * \param[in] track_count Number of tracks in this medium
 * \param[in] offsets     Offsets (in LBA frames) for each track
 * \param[in] leadout     Leadout frame
 *
 * \return A TOC object representing the specified information
 *
 * \throw InvalidMetadataException If the input data forms no valid TOC
 */
std::unique_ptr<TOC> make_toc(const uint32_t track_count,
		const std::vector<int32_t> &offsets,
		const uint32_t leadout);


/**
 * \brief Create a TOC object from the specified information.
 *
 * The returned TOC is not guaranteed to be complete since the length of
 * the last track is allowed to be 0 which entails that the leadout will be
 * missing.
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
std::unique_ptr<TOC> make_toc(const uint32_t track_count,
		const std::vector<int32_t> &offsets,
		const std::vector<int32_t> &lengths,
		const std::vector<std::string> &files);


/**
 * \brief Reports insufficient or invalid metadata for building a TOC.
 *
 * \todo For metadata files, position information about the error maybe useful
 */
class InvalidMetadataException final : public std::logic_error
{

public:

	/**
	 * Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidMetadataException(const std::string &what_arg);

	/**
	 * Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidMetadataException(const char *what_arg);
};


/**
 * \brief Functions assissting the management of @link TOC TOCs @endlink.
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


} // namespace v_1_0_0

/// @}

} // namespace arcstk

#endif

