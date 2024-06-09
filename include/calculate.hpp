#ifndef __LIBARCSTK_CALCULATE_HPP__
#define __LIBARCSTK_CALCULATE_HPP__

/**
 * \file
 *
 * \brief Calculation interface.
 */

#include <cstdint>   // for int32_t

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"             // for Checksum, ChecksumSet, Checksums
#endif
#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"             // for Comparable, TotallyOrdered
#endif

namespace arcstk
{
inline namespace v_1_0_0
{


/**
 * \brief Type to represent a 32 bit PCM stereo sample.
 *
 * An unsigned integer of 32 bit length.
 *
 * The type is not intended to do arithmetic operations on it.
 *
 * Bitwise operators are required to work as on unsigned types.
 */
using sample_t = uint32_t;


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
	AudioSize();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] value Size value
	 * \param[in] unit  Unit for \c value
	 */
	AudioSize(const int32_t value, const UNIT unit);
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


	friend void swap(AudioSize& lhs, AudioSize& rhs) noexcept;

	explicit operator bool() const noexcept;

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
 * \brief Interface for information about the current audio input.
 *
 * The CalcContext provides the metadata required to perform the Calculation.
 *
 * The CalcContext determines whether the input is interpreted as single- or
 * multitrack and whether it has to skip samples on the beginning of the first
 * or the end of the last track.
 *
 * CalcContext also provides service methods for identifying the first and
 * last sample of a track relevant for Calculation or to get the track for a
 * given sample.
 *
 * CalcContext offers also access to its underlying TOC data.
 *
 * CalcContext instances are created exclusively via the variants of
 * make_context().
 *
 * A CalcContext is intended as a "read-only" object, it should never be
 * required to modify the CalcContext of an audio input. The only exception
 * is notify_skips() which depends on the concrete algorithm that is only
 * known within Calculation. Thus, Calculation must inform its CalcContext about
 * it.
 *
 * \note
 * Currently it should not be required for the user to subclass CalcContext. All
 * subclasses required for calculating ARCSv1 and ARCSv2 in single- and
 * multitrack scenarios are already provided by libarcstk. Note that subclassing
 * CalcContext would require to also add a new variant of make_context() that
 * can yield an instance of this subclass.
 */
class CalcContext
{
public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~CalcContext() noexcept = default;

	/**
	 * \brief Name of current audio file.
	 *
	 * \return Name of the audio file that is currently processed
	 */
	std::string filename() const noexcept;

	/**
	 * \brief Set the name of the current audio file.
	 *
	 * \param[in] filename Name of the audio file that is to be processed
	 */
	void set_filename(const std::string& filename);

	/**
	 * \brief Return the size of the referenced audio file
	 *
	 * \return The size of the referenced audio file
	 */
	const AudioSize& audio_size() const noexcept;

	// TODO AudioSize audio_size() const noexcept;

	/**
	 * \brief Set the AudioSize of the context.
	 *
	 * This contains the information about the leadout frame. This information
	 * must be known before Calculation::update is called on the last
	 * block.
	 *
	 * \param[in] audio_size AudioSize
	 */
	void set_audio_size(const AudioSize& audio_size);

	/**
	 * \brief Get 0-based index of the first relevant sample of the specified
	 * 1-based track.
	 *
	 * Note that parameter \c track is 1-based, which means that
	 * \c first_relevant_sample(2) returns the first 0-based sample of the
	 * actual track 2 (and not track 3).
	 *
	 * \param[in] track The 1-based track number
	 *
	 * \return Index of the first sample contributing to the track's ARCS
	 */
	int32_t first_relevant_sample(const TrackNo track) const noexcept;

	/**
	 * \brief Get 0-based index of the first relevant sample of the current
	 * audio input.
	 *
	 * Which sample is actually the first relevant one depends on
	 * <ul>
	 *   <li>the offset of the first track and</li>
	 *   <li>whether samples in the beginning of the first track are to be
	 *       skipped.</li>
	 * </ul>
	 *
	 * Always equivalent with
	 * @link CalcContext::first_relevant_sample() first_relevant_sample(1) @endlink.
	 *
	 * \return Index of the first sample contributing to the first track's ARCS
	 */
	int32_t first_relevant_sample() const noexcept;

	/**
	 * \brief Get 0-based index of the last relevant sample of the specified
	 * 1-based track.
	 *
	 * Note that parameter \c track is 1-based, which means that
	 * last_relevant_sample(2) returns the last 0-based sample of the actual
	 * track 2 (and not track 3).
	 *
	 * If no offsets are set, the output will always be identical to
	 * @link CalcContext::last_relevant_sample() last_relevant_sample() @endlink.
	 *
	 * For <tt>track == 0</tt>, the last sample of the offset before track 1 is
	 * returned. This may of course be 0 iff the offset of track 1 is 0.
	 *
	 * \param[in] track 1-based track number, accepts 0 as offset of track 1
	 *
	 * \return Index of last sample contributing to the specified track's ARCS
	 */
	int32_t last_relevant_sample(const TrackNo track) const noexcept;

	/**
	 * \brief Get 0-based index of the last relevant sample of the current
	 * audio input.
	 *
	 * Which sample is actually the last relevant one depends on whether
	 * samples in the end of the last track are to be skipped.
	 *
	 * Always equivalent with
	 * @link CalcContext::last_relevant_sample(const TrackNo track) const last_relevant_sample(this->total_tracks()) @endlink.
	 *
	 * \return Index of the last sample contributing to the last track's ARCS
	 */
	int32_t last_relevant_sample() const noexcept;

	/**
	 * \brief Returns \c TRUE iff this context will skip the first 2939 samples of
	 * the first track.
	 *
	 * \return \c TRUE iff context will signal to skip the first samples.
	 */
	bool skips_front() const noexcept;

	/**
	 * \brief Returns \c TRUE iff this context will skip the last 2940 samples
	 * (5 LBA frames) of the last track.
	 *
	 * \return \c TRUE iff context will signal to skip the last samples.
	 */
	bool skips_back() const noexcept;

	/**
	 * \brief Returns the amount of samples to skip at the beginning of the
	 * first track.
	 *
	 * If the audio input contains only one track, this track is the first track.
	 *
	 * The value is either 2939 or 0.
	 *
	 * The skipping is only active iff this instance skips_front().
	 *
	 * \return The number of samples to skip at the beginning of the first track
	 */
	int32_t num_skip_front() const noexcept;

	/**
	 * \brief Returns the amount of samples to skip at the end of the last
	 * track.
	 *
	 * If the audio input contains only one track, this track is the last track.
	 *
	 * The value is either 2940 or 0.
	 *
	 * The skipping is only active iff this instance skips_back().
	 *
	 * \return The number of samples to skip at the end of the last track
	 */
	int32_t num_skip_back() const noexcept;

	/**
	 * \brief Returns \c TRUE if this instances indicates a processing for multiple
	 * tracks on a single input file.
	 *
	 * If this is \c FALSE, no chunks will be available. Multitrack mode is
	 * activated by setting a TOC.
	 *
	 * \attention
	 * Note that this is independent from <tt>total_tracks()</tt>. A
	 * TOC containing only one track would have a CalcContext in
	 * which <tt>total_tracks()</tt> is \c 1 but <tt>is_multi_track()</tt> is
	 * \c TRUE. Method <tt>is_multi_track()</tt> specifies the processing mode
	 * while <tt>total_tracks()</tt> just provides information about the TOC.
	 *
	 * \return \c TRUE if this context specifies multitrack mode, otherwise \c FALSE
	 */
	bool is_multi_track() const noexcept;

	/**
	 * \brief Notify the instance about configured skipping amounts at the
	 * beginning of the first track and the end of the last track.
	 *
	 * Whether actual skipping takes place can be determined by
	 * skips_front() and skips_back().
	 *
	 * \param[in] num_skip_front Actual amount of skipped samples at the beginning
	 * \param[in] num_skip_back  Actual amount of skipped samples at the end
	 */
	void notify_skips(const int32_t num_skip_front,
			const int32_t num_skip_back) noexcept;

	/**
	 * \brief Convenience method: Total number of tracks.
	 *
	 * This number will aways be a non-negative integer up to \c 99 .
	 *
	 * Since the track count should be accessed in a uniform way, regardless
	 * whether we actually have multiple tracks or only one.
	 *
	 * \attention
	 * Note that this is independent from <tt>is_multi_track()</tt>. A
	 * TOC containing only one track would have a CalcContext in
	 * which <tt>total_tracks()</tt> is \c 1 but <tt>is_multi_track()</tt> is
	 * \c TRUE. Method <tt>is_multi_track()</tt> specifies the processing mode
	 * while <tt>total_tracks()</tt> just provides information about the TOC.
	 *
	 * \return The number of tracks represented in this file
	 */
	int total_tracks() const noexcept;

	/**
	 * \brief Returns 1-based track number of the track containing the specified
	 * 0-based sample.
	 *
	 * If total_samples() is 0, the method will return 0 regardless of
	 * the actual value of \c smpl.
	 *
	 * If \c smpl is bigger than <tt>total_samples() - 1</tt>, the method will
	 * return an invalid high track number ("infinite").
	 *
	 * The user has therefore to check for the validity of the result by
	 * checking whether <tt>0 < result <= CDDA::MAX_TRACKCOUNT</tt>.
	 *
	 * \param[in] smpl The sample to get the track for
	 *
	 * \return Track number of the track containing sample \c smpl
	 */
	int track(const int32_t smpl) const noexcept;

	/**
	 * \brief Return the offset of the specified 0-based track from the TOC.
	 *
	 * Note that <tt>offset(i) == toc().offset(i+1)</tt> for all
	 * <tt>i: 0 <= i < toc().total_tracks()</tt>.
	 *
	 * \param[in] track The 0-based track to get the offset for
	 *
	 * \return The offset for the specified 0-based track
	 */
	lba_count_t offset(const int track) const noexcept;

	/**
	 * \brief Return the normalized length of the specified 0-based track.
	 *
	 * Note that <tt>length(i) == offset(i+2) - offset(i+1)</tt> for all
	 * <tt>i: 0 <= i < toc().total_tracks() - 1</tt>.
	 *
	 * For the last track <tt>t = toc().total_tracks() - 1</tt>, the value of
	 * <tt>length(t)</tt> may be \c 0 if the TOC is incomplete and the instance
	 * is not yet updated with the concrete AudioSize, otherwise it is
	 * <tt>length(t) == leadout_frame() - offset(t+1)</tt>.
	 *
	 * \param[in] track The 0-based track to get the length for
	 *
	 * \return The length for the specified 0-based track
	 */
	lba_count_t length(const int track) const noexcept;

	/**
	 * \brief Return the ARId of the current medium, if known.
	 *
	 * The value returned will only be significant iff non-empty offsets and
	 * non-zero total PCM byte count are available. Otherwise the ARId
	 * returned will be \c empty() .
	 *
	 * \return The ARId of the current medium
	 */
	ARId id() const;

	/**
	 * \brief Clone this CalcContext object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return A clone of this instance
	 */
	std::unique_ptr<CalcContext> clone() const noexcept;

	/**
	 * \brief TRUE iff \c rhs is equal to this instance..
	 *
	 * \param[in] rhs Right hand side of equality comparison
	 *
	 * \return TRUE iff \c rhs is equal to this instance, otherwise FALSE
	 */
	bool equals(const CalcContext& rhs) const noexcept;

private:

	/**
	 * \brief Implements filename() const.
	 *
	 * \return Name of the audio file that is currently processed
	 */
	virtual std::string do_filename() const noexcept
	= 0;

	/**
	 * \brief Implements set_filename(const std::string &filename).
	 *
	 * \param[in] filename Name of the audio file that is to be processed
	 */
	virtual void do_set_filename(const std::string &filename) noexcept
	= 0;

	/**
	 * \brief Implements audio_size() const.
	 *
	 * \return The total number of bytes of the PCM samples
	 */
	virtual const AudioSize& do_audio_size() const noexcept
	= 0;

	/**
	 * \brief Implements set_audio_size(const AudioSize &audio_size).
	 *
	 * \param[in] audio_size AudioSize
	 */
	virtual void do_set_audio_size(const AudioSize &audio_size)
	= 0;

	/**
	 * \brief Implements first_relevant_sample(const TrackNo track) const.
	 *
	 * \param[in] track The 1-based track number
	 *
	 * \return Index of the first sample contributing to the track's ARCS
	 */
	virtual int32_t do_first_relevant_sample(const TrackNo track) const
	noexcept
	= 0;

	/**
	 * \brief Implements first_relevant_sample() const.
	 *
	 * \return Index of the first sample contributing to the first track's ARCS
	 */
	virtual int32_t do_first_relevant_sample_no_parms() const noexcept
	= 0;

	/**
	 * \brief Implements last_relevant_sample(const TrackNo track) const.
	 *
	 * \param[in] track 1-based track number, accepts 0 as offset of track 1
	 *
	 * \return Index of last sample contributing to the specified track's ARCS
	 */
	virtual int32_t do_last_relevant_sample(const TrackNo track) const
	noexcept
	= 0;

	/**
	 * \brief Implements last_relevant_sample() const.
	 *
	 * \return Index of the last sample contributing to the last track's ARCS
	 */
	virtual int32_t do_last_relevant_sample_no_parms() const noexcept
	= 0;

	/**
	 * \brief Implements skips_front() const.
	 *
	 * \return \c TRUE iff context will signal to skip the first samples.
	 */
	virtual bool do_skips_front() const noexcept
	= 0;

	/**
	 * \brief Implements skips_back() const.
	 *
	 * \return \c TRUE iff context will signal to skip the last samples.
	 */
	virtual bool do_skips_back() const noexcept
	= 0;

	/**
	 * \brief Implements num_skip_front() const.
	 *
	 * \return The number of samples to skip at the beginning of the first track
	 */
	virtual int32_t do_num_skip_front() const noexcept
	= 0;

	/**
	 * \brief Implements num_skip_back() const.
	 *
	 * \return The number of samples to skip at the end of the last track
	 */
	virtual int32_t do_num_skip_back() const noexcept
	= 0;

	/**
	 * \brief Implements is_multi_track() const.
	 *
	 * \return \c TRUE if this context specifies multitrack mode, otherwise \c FALSE
	 */
	virtual bool do_is_multi_track() const noexcept
	= 0;

	/**
	 * \brief Implements notify_skips(const int32_t num_skip_front, const int32_t num_skip_back).
	 *
	 * \param[in] num_skip_front Actual amount of skipped samples at the beginning
	 * \param[in] num_skip_back  Actual amount of skipped samples at the end
	 */
	virtual void do_notify_skips(const int32_t num_skip_front,
			const int32_t num_skip_back) noexcept
	= 0;


	/**
	 * \brief Implements total_tracks() const.
	 *
	 * \return The number of tracks represented in this file
	 */
	virtual int do_total_tracks() const noexcept
	= 0;

	/**
	 * \brief Implements track(const int32_t smpl) const.
	 *
	 * \param[in] smpl The sample to get the track for
	 *
	 * \return Track number of the track containing sample \c smpl
	 */
	virtual int do_track(const int32_t smpl) const noexcept
	= 0;

	/**
	 * \brief Implements offset(const int track) const.
	 *
	 * \param[in] track The 0-based track to get the offset for
	 *
	 * \return The offset for the specified 0-based track
	 */
	virtual lba_count_t do_offset(const int track) const noexcept
	= 0;

	/**
	 * \brief Implements length(const int track) const.
	 *
	 * \param[in] track The 0-based track to get the length for
	 *
	 * \return The length for the specified 0-based track
	 */
	virtual lba_count_t do_length(const int track) const noexcept
	= 0;

	/**
	 * \brief Implements id() const.
	 *
	 * \return The ARId of the current medium
	 */
	virtual ARId do_id() const
	= 0;

	/**
	 * \brief Implements clone() const.
	 *
	 * \return A clone of this instance
	 */
	virtual std::unique_ptr<CalcContext> do_clone() const noexcept
	= 0;

	virtual bool do_equals(const CalcContext& rhs) const noexcept
	= 0;
};


/**
 * \brief Create a CalcContext from two skip flags.
 *
 * This addresses a situation where a single track has to be checksummed.
 *
 * \param[in] skip_front    Tell wether to skip the front samples
 * \param[in] skip_back     Tell wether to skip the back samples
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const bool &skip_front,
		const bool &skip_back);

/**
 * \brief Create a CalcContext from an audio filename and two skip flags.
 *
 * This addresses a situation where a single track has to be checksummed.
 *
 * The file will not be opened, it is just declared as part of the metadata.
 *
 * \param[in] skip_front    Tell wether to skip the front samples
 * \param[in] skip_back     Tell wether to skip the back samples
 * \param[in] audiofilename The name of the audiofile
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const bool &skip_front,
		const bool &skip_back,
		const std::string &audiofilename);

/**
 * \brief Create a CalcContext from a TOC.
 *
 * \param[in] toc The TOC to use
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const TOC &toc);

/**
 * \brief Create a CalcContext from an audio filename and a TOC.
 *
 * The file will not be opened, it is just declared as part of the metadata.
 *
 * \param[in] toc           The TOC to use
 * \param[in] audiofilename The name of the audiofile, empty be default
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const TOC &toc,
		const std::string &audiofilename);

/**
 * \copydoc arcstk::v_1_0_0::make_context(const TOC&)
 */
std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC> &toc);

/**
 * \copydoc arcstk::v_1_0_0::make_context(const TOC&, const std::string&)
 */
std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC> &toc,
		const std::string &audiofilename);

// TODO Calculation

} // namespace v_1_0_0
} // namespace arcstk

#endif

