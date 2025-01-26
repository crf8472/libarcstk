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

#include <cstdint>        // for uint32_t, int32_t
#include <memory>         // for unique_ptr
#include <vector>         // for vector

namespace arcstk
{
inline namespace v_1_0_0
{

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
	 * \brief Units for size declaration
	 */
	enum class UNIT
	{
		SAMPLES, FRAMES, BYTES
	};

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
	// FIXME This allows setting a value without a bounds check

	/**
	 * \brief Return the LBA leadout frame.
	 *
	 * Note that the number is 1-based.
	 *
	 * \return LBA leadout frame
	 */
	int32_t leadout_frame() const;

	/**
	 * \brief Return the total number of LBA frames.
	 *
	 * Maximum value is CDDA::MAX_BLOCK_ADDRESS which is a value of
	 * 449.999 LBA frames on a disc.
	 *
	 * \return Total number of LBA frames
	 */
	int32_t total_frames() const;

	/**
	 * \brief Set the total number of LBA frames.
	 *
	 * \param[in] frame_count Total number of LBA frames
	 *
	 * \throw std::underflow_error If value is negative
	 * \throw std::overflow_error  If value is greater than the unit maximum
	 */
	void set_total_frames(const int32_t frame_count);

	/**
	 * \brief Return the total number of 32 bit PCM samples.
	 *
	 * Maximum value is CDDA::MAX_BLOCK_ADDRESS * CDDA::SAMPLES_PER_FRAME
	 * which is a value of 264.599.412 stereo samples on a disc.
	 *
	 * \return The total number of 32 bit PCM samples
	 */
	int32_t total_samples() const;

	/**
	 * \brief Set the total number of 32 bit PCM samples.
	 *
	 * This also determines the leadout frame and the number of PCM bytes.
	 *
	 * \param[in] sample_count Total number of 32 bit PCM samples
	 *
	 * \throw std::underflow_error If value is negative
	 * \throw std::overflow_error  If value is greater than the unit maximum
	 */
	void set_total_samples(const int32_t sample_count);

	/**
	 * \brief Return the total number of bytes holding 32 bit PCM samples.
	 *
	 * Maximum value is CDDA::MAX_BLOCK_ADDRESS * CDDA::BYTES_PER_SAMPLE which
	 * is a value of 1.058.397.648 bytes on a disc.
	 *
	 * \return The total number of bytes holding 32 bit PCM samples
	 */
	int32_t total_pcm_bytes() const noexcept;

	/**
	 * \brief Set the total number of bytes holding decoded 32 bit PCM samples
	 *
	 * This also determines the leadout frame and the total number of 32 bit PCM
	 * samples.
	 *
	 * \param[in] byte_count Total number of bytes holding 32 bit PCM samples
	 *
	 * \throw std::underflow_error If value is negative
	 * \throw std::overflow_error  If value is greater than the unit maximum
	 */
	void set_total_pcm_bytes(const int32_t byte_count) noexcept;

	/**
	 * \brief Return \c TRUE if the AudioSize is 0.
	 *
	 * \return \c TRUE if the AudioSize is 0
	 */
	bool zero() const noexcept;

	/**
	 * \brief Return maximum size for the specified unit.
	 */
	static int32_t max(const UNIT unit) noexcept;

	/**
	 * \brief Return \c TRUE iff this AudioSize is zero(), otherwise \c FALSE.
	 *
	 * \return Return \c TRUE iff this AudioSize is zero(), otherwise \c FALSE.
	 */
	explicit operator bool() const noexcept;


	friend void swap(AudioSize& lhs, AudioSize& rhs) noexcept;

private:

	/**
	 * \brief Set the internal value, possibly converting from \c unit.
	 *
	 * \param[in] value Value to set
	 * \param[in] unit  Unit to convert to bytes from
	 */
	void set_value(const int32_t value, AudioSize::UNIT unit);

	/**
	 * \brief Get internal size in the specified unit.
	 *
	 * \param[in] unit  Unit to convert bytes amount to
	 */
	int32_t read_as(const AudioSize::UNIT unit) const;
};

bool operator == (const AudioSize& lhs, const AudioSize& rhs) noexcept;

bool operator  < (const AudioSize& lhs, const AudioSize& rhs) noexcept;


/**
 * \brief Leadout on index 0, followed by the sequence of offsets.
 *
 * Every number denotes an amount of frames.
 */
using ToCData = std::vector<AudioSize>;

namespace toc
{

ToCData construct(const int32_t leadout, const std::vector<int32_t>& offsets);

void set_leadout(const AudioSize& leadout, ToCData& data);

AudioSize leadout(const ToCData& data);

std::vector<AudioSize> offsets(const ToCData& data);

int total_tracks(const ToCData& data);

bool complete(const ToCData& data);

}


// ToC


/**
 * \brief Table of contents of a compact disc.
 */
class ToC final : public Comparable<ToC>
{
public:

	ToC(const int32_t leadout, const std::vector<int32_t>& offsets,
			const std::vector<std::string>& filenames);

	ToC(const int32_t leadout, const std::vector<int32_t>& offsets);

	ToC(const std::vector<int32_t>& offsets,
			const std::vector<std::string>& filenames);

	ToC(const std::vector<int32_t>& offsets);


	ToC(const ToC& rhs);
	ToC& operator = (const ToC& rhs);
	ToC(ToC&& rhs) noexcept;
	ToC& operator = (ToC&& rhs) noexcept;
	~ToC() noexcept;


	int total_tracks() const noexcept;
	AudioSize leadout() const noexcept;
	void set_leadout(const AudioSize l) noexcept;
	std::vector<AudioSize>   offsets() const;
	std::vector<std::string> filenames() const;
	bool is_single_file() const noexcept;
	bool complete() const noexcept;


	friend bool operator == (const ToC& lhs, const ToC& rhs) noexcept;
	friend void swap(ToC& lhs, ToC& rhs) noexcept;

private:

	class Impl;
	std::unique_ptr<Impl> impl_;
};


std::unique_ptr<ToC> make_toc(const int32_t leadout,
		const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames);

std::unique_ptr<ToC> make_toc(const int32_t leadout,
		const std::vector<int32_t>& offsets);

std::unique_ptr<ToC> make_toc(const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames);

std::unique_ptr<ToC> make_toc(const std::vector<int32_t>& offsets);

} // namespace v_1_0_0
} // namespace arcstk

#endif

