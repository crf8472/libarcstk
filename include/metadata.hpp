#ifndef __LIBARCSTK_METADATA_HPP__
#define __LIBARCSTK_METADATA_HPP__
/**
 * \file
 *
 * \brief Classes and functions for metadata of a compact disc.
 */

#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"   // for Comparable, TotallyOrdered
#endif

#include <algorithm>      // for transform
#include <cstdint>        // for uint32_t, int32_t
#include <memory>         // for unique_ptr
#include <stdexcept>      // for invalid_argument
#include <vector>         // for vector

namespace arcstk
{
inline namespace v_1_0_0
{

/** \defgroup meta Compact Disc and Audio Metadata
 *
 * A ToC is the table of content information from a compact disc. It contains
 * the track offsets and the leadout of the compact disc.
 *
 * An AudioSize is a representation of an amount of audio information that can
 * be evaluated as frames, samples or bytes. Passing AudioSize objects helps to
 * avoid calculating with the wrong unit, e.g. w/ samples when frames are
 * required.
 *
 * CDDA provides a set of cdda related constants that are used on validating and
 * parsing audio information.
 *
 * An InvalidMetadataException indicates that no valid ToC can be constructed
 * from the input provided.
 *
 * A NonstandardMetadataException indicates that the input is not conforming to
 * the redbook standard. This exception can occurr in the internal validation
 * mechanism but is currently not used in the public API.
 *
 * @{
 */

/**
 * \brief Constants related to the CDDA format.
 */
struct CDDA final
{
	/**
	 * \brief CDDA: sampling rate of 44100 samples per second.
	 */
	constexpr static int SAMPLES_PER_SECOND { 44100 };

	/**
	 * \brief CDDA: 16 bits per sample.
	 */
	constexpr static int BITS_PER_SAMPLE    { 16 };

	/**
	 * \brief CDDA: stereo involves 2 channels.
	 */
	constexpr static int NUMBER_OF_CHANNELS { 2 };

	/**
	 * \brief Total number of frames per second is 75.
	 */
	constexpr static int FRAMES_PER_SEC     { 75 };

	/**
	 * \brief Total number of bytes per sample is 4.
	 *
	 * This follows from CDDA where
	 * 1 sample == 16 bit/sample * 2 channels / 8 bits/byte
	 */
	constexpr static int BYTES_PER_SAMPLE   { 4 };

	/**
	 * \brief Total number of samples per frame is 588.
	 *
	 * This follows from CDDA where 1 frame == 44100 samples/sec / 75 frames/sec
	 */
	constexpr static int SAMPLES_PER_FRAME  { 588 };

	/**
	 * \brief Total number of bytes per frame is 2352.
	 *
	 * This follows from CDDA where 1 frame == 588 samples * 4 bytes/sample
	 */
	constexpr static int BYTES_PER_FRAME    { 2352 };

	/**
	 * \brief Maximal valid track count is 99.
	 */
	constexpr static int MAX_TRACKCOUNT { 99 };

	/**
	 * \brief Redbook maximal value for a valid LBA frame index is 449.999.
	 *
	 * Redbook defines 99:59.74 (MSF) as maximal valid block adress. This is
	 * equivalent to 449.999 frames.
	 */
	constexpr static int32_t MAX_BLOCK_ADDRESS { ( 99 * 60 + 59 ) * 75 + 74 };

	/**
	 * \brief Redbook maximal valid offset value is 359.999 LBA frames.
	 *
	 * Redbook defines 79:59.74 (MSF) (+leadin+leadout) as maximal play
	 * duration. This is equivalent to 360.000 frames, thus the maximal valid
	 * offset is LBA frame index 359.999.
	 */
	constexpr static int32_t MAX_OFFSET { ( 79 * 60 + 59 ) * 75 + 74 };

	/**
	 * \brief Two subsequenct offsets must have a distance of at least 300 LBA
	 * frames.
	 *
	 * The CDDA conforming minimal track length is 4 seconcs including 2 seconds
	 * pause, thus 4 sec * 75 frames/sec == 300 frames.
	 */
	constexpr static int32_t MIN_TRACK_OFFSET_DIST { 300 };

	/**
	 * \brief Minimal number of LBA frames a track contains is 150.
	 *
	 * The CDDA conforming minmal track length is 4 seconds including 2 seconds
	 * pause but the pause does not contribute to the track lengths, thus
	 * 2 sec * 75 frames/sec == 150 frames.
	 */
	constexpr static int32_t MIN_TRACK_LEN_FRAMES { 150 };
};


/**
 * \brief Represents an audio unit.
 *
 * Units are frames, samples or bytes.
 */
enum class UNIT : int
{
	FRAMES  = 1,
	SAMPLES = CDDA::SAMPLES_PER_FRAME,
	BYTES   = CDDA::BYTES_PER_FRAME
};


/**
 * \brief Return the maximum cdda conforming value for the specified UNIT.
 */
template <enum UNIT U>
constexpr int32_t cdda_max() noexcept;

// full specializations

template <>
inline constexpr int32_t cdda_max<UNIT::FRAMES>() noexcept
{
	return CDDA::MAX_BLOCK_ADDRESS;
}

template <>
inline constexpr int32_t cdda_max<UNIT::SAMPLES>() noexcept
{
	return cdda_max<UNIT::FRAMES>() * CDDA::SAMPLES_PER_FRAME;
}

template <>
inline constexpr int32_t cdda_max<UNIT::BYTES>() noexcept
{
	return cdda_max<UNIT::FRAMES>() * CDDA::BYTES_PER_FRAME;
}


/**
 * \brief Uniform access to the size of the input audio information.
 *
 * Some decoders provide the number of frames, others the number of samples and
 * maybe in some situations just the number of bytes of the sample stream is
 * known. To avoid implementing the appropriate conversion for each decoder,
 * AudioSize provides an interface for uniform representation to this
 * information. Any of the informations provided will determine all of the
 * others.
 *
 * An AudioSize converts to TRUE if it is greater than 0. An AudioSize of 0
 * converts to FALSE.
 */
class AudioSize final : TotallyOrdered<AudioSize>
{
	/**
	 * \brief Data: Total number of pcm sample bytes in the audio file.
	 */
	int32_t total_pcm_bytes_;

public:

	/**
	 * \brief Constructor.
	 *
	 * Constructs an AudioSize of zero().
	 */
	AudioSize() noexcept;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] value Size value
	 * \param[in] unit  Unit for \c value
	 */
	AudioSize(const int32_t value, const UNIT unit) noexcept;

	/**
	 * \brief Size in LBA frames.
	 *
	 * \return The size in LBA frames
	 */
	int32_t frames() const noexcept;

	/**
	 * \brief Update this size by an amount of LBA frames
	 *
	 * \param[in] frames Updated size to set as an amount of LBA frames
	 */
	void set_frames(const int32_t frames) noexcept;

	/**
	 * \brief Size in stereo PCM samples.
	 *
	 * \return The size in stereo PCM samples
	 */
	int32_t samples() const noexcept;

	/**
	 * \brief Update this size by an amount of stereo PCM samples.
	 *
	 * \param[in] samples Updated size to set as an amount of stereo PCM samples
	 */
	void set_samples(const int32_t samples) noexcept;

	/**
	 * \brief Size in bytes.
	 *
	 * \return The size in bytes
	 */
	int32_t bytes() const noexcept;

	/**
	 * \brief Update this size by an amount of bytes.
	 *
	 * \param[in] samples Updated size to set as an amount of bytes
	 */
	void set_bytes(const int32_t bytes) noexcept;

	/**
	 * \brief Return \c TRUE if the AudioSize is equivalent to zero.
	 *
	 * \return \c TRUE if the AudioSize is zero
	 */
	bool zero() const noexcept;

	/**
	 * \brief Return \c TRUE iff this AudioSize is zero(), otherwise \c FALSE.
	 *
	 * \return Return \c TRUE iff this AudioSize is zero(), otherwise \c FALSE.
	 */
	explicit operator bool() const noexcept;


	friend void swap(AudioSize& lhs, AudioSize& rhs) noexcept;
};

bool operator == (const AudioSize& lhs, const AudioSize& rhs) noexcept;

bool operator  < (const AudioSize& lhs, const AudioSize& rhs) noexcept;

std::string to_string(const AudioSize& s);


/**
 * \brief ToC data from a file, e.g. offsets and leadout.
 *
 * Leadout is on index 0, followed by the sequence of offsets. Offset indices
 * therefore correspond to track numbers, i.e. index 7 is offset of track 7.
 *
 * It is guaranteed to be iterable and accessible by operator [].
 */
using ToCData = std::vector<AudioSize>;


/**
 * \brief Functions for managing ToCData instances.
 */
namespace toc
{

/**
 * \brief Construct ToCData from leadout and offsets.
 *
 * All <tt>int32_t</tt> data types denote amounts of LBA frames.
 *
 * \param[in] leadout Leadout frame
 * \param[in] offsets Offset frames in order of tracks
 *
 * \return Formatted ToCData object
 */
ToCData construct(const int32_t leadout, const std::vector<int32_t>& offsets);

/**
 * \brief Set the leadout of a ToC object.
 *
 * \param[in] leadout Leadout to set
 * \param[in] data    ToCData to update
 */
void set_leadout(const AudioSize& leadout, ToCData& data);

/**
 * \brief Leadout of a ToC object.
 *
 * \param[in] data ToCData to read from
 *
 * \return Leadout of a ToC object
 */
AudioSize leadout(const ToCData& data);

/**
 * \brief Offsets of a ToC object.
 *
 * \param[in] data ToCData to read from
 *
 * \return Offsets of a ToC object
 */
std::vector<AudioSize> offsets(const ToCData& data);

/**
 * \brief Lengths of tracks object.
 *
 * \param[in] data ToCData to read from
 *
 * \return Track lengths
 */
std::vector<AudioSize> lengths(const ToCData& data);

/**
 * \brief Total tracks of a ToC object.
 *
 * \param[in] data ToCData to read from
 *
 * \return Total tracks of a ToC object
 */
int total_tracks(const ToCData& data);

/**
 * \brief TRUE iff non-zero leadout and non-empty sequence of offsets are
 * specified.
 *
 * This does not entail a validation of the specified values.
 *
 * \return TRUE iff non-zero leadout and non-empty offsets exist
 */
bool complete(const ToCData& data);

/**
 * \brief Validate ToCData object.
 *
 * \param[in] toc_data ToCData object to be validated
 *
 * \throws invalid_argument If validation fails
 */
void validate(const ToCData& toc_data);

} // namespace toc


/**
 * \brief Table of contents of a compact disc.
 */
class ToC final : public Comparable<ToC>
{
public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] toc_data   The ToC data to construct a ToC from
	 * \param[in] filenames  Audio filenames
	 */
	ToC(const ToCData& toc_data, const std::vector<std::string>& filenames);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] toc_data The ToC data to construct a ToC from
	 */
	explicit ToC(const ToCData& toc_data);

	// copy
	ToC(const ToC& rhs);
	ToC& operator = (const ToC& rhs);

	// move
	ToC(ToC&& rhs) noexcept;
	ToC& operator = (ToC&& rhs) noexcept;

	~ToC() noexcept;

	/**
	 * \brief Total tracks in this ToC.
	 *
	 * \return Total tracks
	 */
	int total_tracks() const noexcept;

	/**
	 * \brief Leadout LBA frame of this ToC.
	 *
	 * If the leadout is unknown, the AudioSize returned is <tt>zero()</tt>.
	 *
	 * \return Leadout LBA frame
	 */
	AudioSize leadout() const noexcept;

	/**
	 * \brief Set the leadout LBA frame of this ToC.
	 *
	 * \param[in] leadout The leadout frame to set
	 */
	void set_leadout(const AudioSize l) noexcept;

	/**
	 * \brief Offsets of this ToC.
	 *
	 * \return Offsets
	 */
	std::vector<AudioSize> offsets() const;

	/**
	 * \brief Filenames of this ToC.
	 *
	 * \return Filenames
	 */
	std::vector<std::string> filenames() const;

	/**
	 * \brief TRUE iff the ToC specifies exactly one audiofile, otherwise FALSE.
	 *
	 * However, the <tt>filenames()</tt> returned may be more than 1, but they
	 * will be all identical. This is possible when the parsed metadata source
	 * specifies a file for each track.
	 *
	 * This will be FALSE in case where multiple non-identical filenames were
	 * specified by the metadata source.
	 *
	 * \return TRUE iff the ToC has exactly one audio file, otherwise FALSE
	 */
	bool is_single_file() const noexcept;

	/**
	 * \brief TRUE iff offsets and non-zero leadout are present, otherwise FALS.
	 *
	 * \return TRUE iff ToC contains complete toc information otherwise FALSE
	 */
	bool complete() const noexcept;

	friend void swap(ToC& lhs, ToC& rhs) noexcept;

	friend bool operator == (const ToC& lhs, const ToC& rhs) noexcept;

private:

	class Impl;
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Constructor.
 *
 * \param[in] leadout   Leadout frame
 * \param[in] offsets   Offset frames
 * \param[in] filenames Audio filenames
 */
std::unique_ptr<ToC> make_toc(const int32_t leadout,
		const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames);

/**
 * \brief Constructor.
 *
 * \param[in] leadout   Leadout frame
 * \param[in] offsets   Offset frames
 */
std::unique_ptr<ToC> make_toc(const int32_t leadout,
		const std::vector<int32_t>& offsets);

/**
 * \brief Constructor.
 *
 * \param[in] offsets   Offset frames
 * \param[in] filenames Audio filenames
 */
std::unique_ptr<ToC> make_toc(const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames);

/**
 * \brief Constructor.
 *
 * \param[in] offsets   Offset frames
 */
std::unique_ptr<ToC> make_toc(const std::vector<int32_t>& offsets);


/**
 * \brief Reports invalid metadata for constructing a ToC.
 */
class InvalidMetadataException final : public std::runtime_error
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
 * AccurateRip checksums. A common case are unusual total lengths, as for
 * example up to 99 minutes per disc.
 *
 * \attention
 * This exception occurrs only internally in the current API version, but is
 * never thrown to the client. This may change in future versions.
 */
class NonstandardMetadataException final : public std::runtime_error
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

/** @} */

} // namespace v_1_0_0
} // namespace arcstk

#endif

