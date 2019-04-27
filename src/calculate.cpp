/**
 * \file
 *
 * \brief Implementation of the checksum calculation API
 */

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif

#include <algorithm>     // for copy
#include <chrono>
#include <cstdint>
#include <exception>     // for exception
#include <fstream>
#include <memory>
#include <unordered_map>
#include <vector>

#ifndef __LIBARCSTK_IDENTIFIER_DETAILS_HPP__
#include "identifier_details.hpp" // for TOCBuilder
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \internal \defgroup calcImpl Implementation
 * \ingroup calc
 * @{
 */

namespace
{

/**
 * \brief Number of samples to be skipped before the end of the last track.
 *
 * There are 5 frames to be skipped, i.e. 5 frames * 588 samples/frame
 * = 2940 samples. We derive the number of samples to be skipped at the
 * start of the first track by just subtracting 1 from this constant.
 */
constexpr uint32_t NUM_SKIP_SAMPLES_BACK  = 5/* frames */ * 588/* samples */;

/**
 * \brief Number of samples to be skipped after the start of the first track.
 *
 * There are 5 frames - 1 sample to be skipped, i.e.
 * 5 frames * 588 samples/frame - 1 sample = 2939 samples.
 */
constexpr uint32_t NUM_SKIP_SAMPLES_FRONT = NUM_SKIP_SAMPLES_BACK - 1;

} // namespace


/**
 * \brief Private implementation of AudioSize.
 *
 * \see AudioSize
 */
class AudioSize::Impl final
{

public:

	/**
	 * \brief Constructor
	 */
	Impl();

	/**
	 * \brief Implements AudioSize::set_leadout_frame(const uint32_t leadout)
	 */
	void set_leadout_frame(const uint32_t leadout);

	/**
	 * \brief Implements AudioSize::leadout_frame() const
	 */
	uint32_t leadout_frame() const;

	/**
	 * \brief Implements AudioSize::set_sample_count(const uint32_t smpl_count)
	 */
	void set_sample_count(const uint32_t smpl_count);

	/**
	 * \brief Implements AudioSize::sample_count() const
	 */
	uint32_t sample_count() const;

	/**
	 * \brief Implements AudioSize::set_pcm_byte_count(const uint64_t byte_count)
	 */
	void set_pcm_byte_count(const uint64_t byte_count);

	/**
	 * \brief Implements AudioSize::pcm_byte_count() const
	 */
	uint64_t pcm_byte_count() const;


private:

	/**
	 * \brief Data: Number of pcm sample bytes in the audio file.
	 */
	uint64_t total_pcm_bytes_;
};


/// \cond UNDOC_FUNCTION_BODIES

AudioSize::Impl::Impl()
	: total_pcm_bytes_(0)
{
	// empty
}


void AudioSize::Impl::set_leadout_frame(const uint32_t leadout)
{
	this->set_pcm_byte_count(leadout * CDDA.BYTES_PER_FRAME);
}


uint32_t AudioSize::Impl::leadout_frame() const
{
	return this->pcm_byte_count() / CDDA.BYTES_PER_FRAME;
}


void AudioSize::Impl::set_sample_count(const uint32_t sample_count)
{
	this->set_pcm_byte_count(sample_count * CDDA.BYTES_PER_SAMPLE);
}


uint32_t AudioSize::Impl::sample_count() const
{
	return this->pcm_byte_count() / CDDA.BYTES_PER_SAMPLE;
}


void AudioSize::Impl::set_pcm_byte_count(const uint64_t byte_count)
{
	total_pcm_bytes_ = byte_count;
}


uint64_t AudioSize::Impl::pcm_byte_count() const
{
	return total_pcm_bytes_;
}


// CalcContext


void CalcContext::set_audio_size(const AudioSize &audio_size)
{
	this->do_set_audio_size(audio_size);
}


const AudioSize& CalcContext::audio_size() const
{
	return this->do_audio_size();
}


void CalcContext::set_filename(const std::string &filename)
{
	this->do_set_filename(filename);
}


std::string CalcContext::filename() const
{
	return this->do_filename();
}


uint8_t CalcContext::track_count() const
{
	return this->do_track_count();
}


bool CalcContext::is_multi_track() const
{
	return this->do_is_multi_track();
}


uint32_t CalcContext::first_relevant_sample(const TrackNo track) const
{
	return this->do_first_relevant_sample(track);
}


uint32_t CalcContext::first_relevant_sample() const
{
	return this->do_first_relevant_sample();
}


uint32_t CalcContext::last_relevant_sample(const TrackNo track) const
{
	return this->do_last_relevant_sample(track);
}


uint32_t CalcContext::last_relevant_sample() const
{
	return this->do_last_relevant_sample();
}


TrackNo CalcContext::track(const uint32_t smpl) const
{
	return this->do_track(smpl);
}


uint32_t CalcContext::offset(const uint8_t track) const
{
	return this->do_offset(track);
}


uint32_t CalcContext::length(const uint8_t track) const
{
	return this->do_length(track);
}


ARId CalcContext::id() const
{
	return this->do_id();
}


bool CalcContext::skips_front() const
{
	return this->do_skips_front();
}


bool CalcContext::skips_back() const
{
	return this->do_skips_back();
}


uint32_t CalcContext::num_skip_front() const
{
	return this->do_num_skip_front();
}


uint32_t CalcContext::num_skip_back() const
{
	return this->do_num_skip_back();
}


void CalcContext::notify_skips(const uint32_t num_skip_front,
		const uint32_t num_skip_back)
{
	this->do_notify_skips(num_skip_front, num_skip_back);
}


std::unique_ptr<CalcContext> CalcContext::clone() const
{
	return this->do_clone();
}


// CalcContextBase


CalcContextBase::CalcContextBase(const std::string &filename,
		const uint32_t num_skip_front,
		const uint32_t num_skip_back)
	: audiosize_(AudioSize())
	, filename_(filename)
	, num_skip_front_(num_skip_front)
	, num_skip_back_(num_skip_back)
{
	// empty
}


CalcContextBase::~CalcContextBase() noexcept = default;


void CalcContextBase::do_set_audio_size(const AudioSize &audio_size)
{
	audiosize_ = audio_size;

	this->do_hook_post_set_audio_size();
}


void CalcContextBase::do_hook_post_set_audio_size()
{
	// empty
}


const AudioSize& CalcContextBase::do_audio_size() const
{
	return audiosize_;
}


void CalcContextBase::do_set_filename(const std::string &filename)
{
	filename_ = filename;
}


std::string CalcContextBase::do_filename() const
{
	return filename_;
}


uint32_t CalcContextBase::do_first_relevant_sample(const TrackNo /* track */)
	const
{
	return 0; // no functionality, just to be overriden
}


uint32_t CalcContextBase::do_first_relevant_sample() const
{
	return this->first_relevant_sample(1);
}


uint32_t CalcContextBase::do_last_relevant_sample(const TrackNo /* track */)
	const
{
	return 0; // no functionality, just to be overriden
}


uint32_t CalcContextBase::do_last_relevant_sample() const
{
	return this->last_relevant_sample(this->track_count());
}


uint32_t CalcContextBase::do_num_skip_front() const
{
	return num_skip_front_;
}


uint32_t CalcContextBase::do_num_skip_back() const
{
	return num_skip_back_;
}


void CalcContextBase::do_notify_skips(const uint32_t num_skip_front,
		const uint32_t num_skip_back)
{
	num_skip_front_ = num_skip_front;
	num_skip_back_  = num_skip_back;

	ARCS_LOG_DEBUG << "Set context front skip: " << num_skip_front_;
	ARCS_LOG_DEBUG << "Set context back skip:  " << num_skip_back_;
}

/// \endcond


/**
 * \brief CalcContext for singletrack mode.
 *
 * A SingletrackCalcContext is a CalcContext derived from an actual filename
 * representing a single track.
 */
class SingletrackCalcContext final : public CalcContextBase
{

public:

	/**
	 * \brief Constructor.
	 *
	 * No samples are skipped.
	 *
	 * \param[in] filename Name of the audio file
	 */
	explicit SingletrackCalcContext(const std::string &filename);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] filename   Name of the audio file
	 * \param[in] skip_front Indicate whether to skip samples at the beginning
	 * \param[in] skip_back  Indicate whether to skip samples at the end
	 */
	SingletrackCalcContext(const std::string &filename,
			const bool skip_front, const bool skip_back);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] filename   Name of the audio file
	 * \param[in] skip_front Indicate whether to skip samples at the beginning
	 * \param[in] num_skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Indicate whether to skip samples at the end
	 * \param[in] num_skip_back Amount of samples to skip at the end
	 */
	SingletrackCalcContext(const std::string &filename,
			const bool skip_front, const uint32_t num_skip_front,
			const bool skip_back,  const uint32_t num_skip_back);

	/**
	 * \brief Activate skipping of the first 2939 samples of the first track.
	 *
	 * \param[in] skip TRUE skips the first 2939 samples of the first track
	 */
	void set_skip_front(const bool skip);

	/**
	 * \brief Activate skipping of the last 2940 samples of the last track.
	 *
	 * \param[in] skip TRUE skips the last 2940 samples of the last track
	 */
	void set_skip_back(const bool skip);

private:

	uint8_t do_track_count() const final;

	bool do_is_multi_track() const final;

	uint32_t do_first_relevant_sample(const TrackNo track) const final;

	// do_first_relevant_sample() is generic in CalcContextBase

	uint32_t do_last_relevant_sample(const TrackNo track) const final;

	// do_last_relevant_sample() is generic in CalcContextBase

	TrackNo do_track(const uint32_t smpl) const final;

	uint32_t do_offset(const TrackNo track) const final;

	uint32_t do_length(const TrackNo track) const final;

	ARId do_id() const final;

	bool do_skips_front() const final;

	bool do_skips_back() const final;

	std::unique_ptr<CalcContext> do_clone() const final;

	/**
	 * \brief State: indicates whether to skip the front samples
	 */
	bool skip_front_;

	/**
	 * \brief State: indicates whether to skip the back samples
	 */
	bool skip_back_;
};

/// \cond UNDOC_FUNCTION_BODIES


SingletrackCalcContext::SingletrackCalcContext(const std::string &filename)
	: CalcContextBase(filename, 0, 0)
	, skip_front_(false)
	, skip_back_(false)
{
	// empty
}


SingletrackCalcContext::SingletrackCalcContext(const std::string &filename,
		const bool skip_front, const bool skip_back)
	: CalcContextBase(filename, 2939, 2940)
	, skip_front_(skip_front)
	, skip_back_(skip_back)
{
	// empty
}


SingletrackCalcContext::SingletrackCalcContext(const std::string &filename,
		const bool skip_front, const uint32_t num_skip_front,
		const bool skip_back,  const uint32_t num_skip_back)
	: CalcContextBase(filename, num_skip_front, num_skip_back)
	, skip_front_(skip_front)
	, skip_back_(skip_back)
{
	// empty
}


uint8_t SingletrackCalcContext::do_track_count() const
{
	return 1;
}


bool SingletrackCalcContext::do_is_multi_track() const
{
	return false;
}


uint32_t SingletrackCalcContext::do_first_relevant_sample(
		const TrackNo track) const
{
	// Illegal track request?
	if (track > CDDA.MAX_TRACKCOUNT)
	{
		return 0;
	}

	// First block will always start with the very first 32 bit PCM sample
	if (track == 0)
	{
		return 0;
	}

	// We have no offsets and the track parameter is irrelevant. Hence iff the
	// request adresses track 1 and skipping applies, the correct constant is
	// provided, otherwise the result is always 0.

	return this->skips_front() and track == 1 ? this->num_skip_front() : 0;
}


uint32_t SingletrackCalcContext::do_last_relevant_sample(
		const TrackNo track) const
{
	// Illegal track request?
	if (track > CDDA.MAX_TRACKCOUNT)
	{
		return this->audio_size().sample_count() - 1;
	}

	// We have no offsets and the track parameter is irrelevant. Hence iff the
	// request adresses the last track and skipping applies, the correct
	// constant is provided, otherwise the result is always the last known
	// sample.

	return this->skips_back() and track == this->track_count() // ==1!
				/* FIXME Is this ^^^ necessary or even correct??? */
		? this->audio_size().sample_count() - 1 - this->num_skip_back()
		: this->audio_size().sample_count() - 1;
}


TrackNo SingletrackCalcContext::do_track(const uint32_t /* smpl */) const
{
	return 1;
}


uint32_t SingletrackCalcContext::do_offset(const TrackNo /* track */) const
{
	return 0;
}


uint32_t SingletrackCalcContext::do_length(const TrackNo /* track */) const
{
	return 0;
}


ARId SingletrackCalcContext::do_id() const
{
	return *(make_empty_arid());
}


bool SingletrackCalcContext::do_skips_front() const
{
	return skip_front_;
}


void SingletrackCalcContext::set_skip_front(const bool skip)
{
	skip_front_ = skip;
}


bool SingletrackCalcContext::do_skips_back() const
{
	return skip_back_;
}


void SingletrackCalcContext::set_skip_back(const bool skip)
{
	skip_back_ = skip;
}


std::unique_ptr<CalcContext> SingletrackCalcContext::do_clone() const
{
	return std::make_unique<SingletrackCalcContext>(*this);
}

/// \endcond


/**
 * \brief CalcContext for multitrack mode.
 *
 * A MultitrackCalcContext is a CalcContext derived from a TOC and an optional
 * actual filename. It always skips the front and back samples.
 */
class MultitrackCalcContext final : public CalcContextBase
{

public:

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename . No samples
	 * are skipped.
	 *
	 * \param[in] filename Name of the file
	 * \param[in] toc      Name of the TOC
	 */
	MultitrackCalcContext(const std::string &filename, const TOC &toc);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename .
	 *
	 * \param[in] filename   Name of the file
	 * \param[in] toc        Name of the TOC
	 * \param[in] skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Amount of samples to skip at the end
	 */
	MultitrackCalcContext(const std::string &filename, const TOC &toc,
			const uint32_t skip_front, const uint32_t skip_back);

	/**
	 * \brief The TOC of the audio input file.
	 *
	 * \return The TOC information to use for the audio input
	 */
	const TOC& toc() const;

	/**
	 * \brief Set the TOC for the audio input.
	 *
	 * \param[in] toc The TOC information to use for the audio input
	 */
	void set_toc(const TOC &toc);


private:

	void do_hook_post_set_audio_size() final;

	uint8_t do_track_count() const final;

	bool do_is_multi_track() const final;

	uint32_t do_first_relevant_sample(const TrackNo track) const final;

	// do_first_relevant_sample() is generic in CalcContextBase

	uint32_t do_last_relevant_sample(const TrackNo track) const final;

	// do_last_relevant_sample() is generic in CalcContextBase

	TrackNo do_track(const uint32_t smpl) const final;

	uint32_t do_offset(const TrackNo track) const final;

	uint32_t do_length(const TrackNo track) const final;

	ARId do_id() const final;

	bool do_skips_front() const final;

	bool do_skips_back() const final;

	std::unique_ptr<CalcContext> do_clone() const final;

	/**
	 * \brief TOC representation
	 */
	TOC toc_;
};

/// \cond UNDOC_FUNCTION_BODIES


// MultitrackCalcContext


MultitrackCalcContext::MultitrackCalcContext(const std::string &filename,
		const TOC &toc)
	: CalcContextBase(filename, 0, 0)
	, toc_(toc)
{
	this->set_toc(toc_);
}


MultitrackCalcContext::MultitrackCalcContext(const std::string &filename,
		const TOC &toc,
		const uint32_t num_skip_front,
		const uint32_t num_skip_back)
	: CalcContextBase(filename, num_skip_front, num_skip_back)
	, toc_(toc)
{
	this->set_toc(toc_);
}


void MultitrackCalcContext::do_hook_post_set_audio_size()
{
	if (this->audio_size().leadout_frame() != this->toc().leadout())
	{
		TOCBuilder builder;
		auto toc { builder.merge(toc_, this->audio_size().leadout_frame()) };
		toc_ = *toc;
	}
}


uint8_t MultitrackCalcContext::do_track_count() const
{
	return toc().track_count();
}


bool MultitrackCalcContext::do_is_multi_track() const
{
	return true;
}


uint32_t MultitrackCalcContext::do_first_relevant_sample(const TrackNo track)
	const
{
	// Illegal track request?
	if (track > CDDA.MAX_TRACKCOUNT)
	{
		return 0;
	}

	// First block will always start with the very first 32 bit PCM sample
	if (track == 0)
	{
		return 0;
	}

	// Invalid track requested?
	if (track > this->track_count())
	{
		return 0;
	}

	// We have offsets, so we respect the corresponding offset to any track.

	// Skipping applies at most for track 1, so we add the appropriate constant.
	if (this->skips_front() and track == 1)
	{
		return toc().offset(1) * CDDA.SAMPLES_PER_FRAME
			+ this->num_skip_front();
	}

	// Standard multi track case: just the first sample of the track
	return toc().offset(track) * CDDA.SAMPLES_PER_FRAME;
}


uint32_t MultitrackCalcContext::do_last_relevant_sample(const TrackNo track)
	const
{
	// Illegal track request?
	if (track > CDDA.MAX_TRACKCOUNT)
	{
		return this->audio_size().sample_count() - 1;
	}

	// Invalid track requested?
	if (track > this->track_count())
	{
		// Return the last relevant sample respecting skipping
		return this->audio_size().sample_count() - 1
			- (this->skips_back() ? this->num_skip_back() : 0 );
	}

	// We have offsets, so we respect the corresponding offset to any track.

	if (this->skips_back() and track == this->track_count())
	{
		return this->audio_size().sample_count() - 1 - this->num_skip_back();
	}

	// Ensure result 0 for previous track's offset 0
	return toc().offset(track+1)
		? toc().offset(track+1) * CDDA.SAMPLES_PER_FRAME - 1
		: 0;
}


TrackNo MultitrackCalcContext::do_track(const uint32_t smpl) const
{
	if (this->audio_size().sample_count() == 0)
	{
		return 0;
	}

	// Sample beyond last track?
	if (smpl > this->audio_size().sample_count() - 1)
	{
		// This will return an invalid track number
		// Caller has to check result for <= track_count() for a valid result
		return CDDA.MAX_TRACKCOUNT + 1;
	}

	const int last_track = this->track_count();

	// Increase track number while sample is smaller than track's last relevant
	TrackNo track = 0;
	for (uint32_t last_sample_trk = this->last_relevant_sample(track) ;
			smpl > last_sample_trk and track <= last_track ;
			++track, last_sample_trk = this->last_relevant_sample(track)) { } ;

	return track;
}


uint32_t MultitrackCalcContext::do_offset(const uint8_t track) const
{
	return track < this->track_count() ? toc().offset(track + 1) : 0;
}


uint32_t MultitrackCalcContext::do_length(const uint8_t track) const
{
	// We define track i as the sample sequence whose first frame is LBA
	// offset[i] and whose last frame is LBA offset[i+1] - 1.
	//
	// This approach appends gaps between track i and i+1 as trailing
	// samples to track i. This normalization is required for computing ARCS
	// and it is the reason why we not just use the lengths parsed from the
	// metafile but let the context normalize them.
	//
	// The lengths reported by this function may differ from the lengths derived
	// from a CUEsheet or other TOC information which may have been computed by
	// 3rd party software.

	//if (offsets_.empty())
	//{
	//	return 0;
	//}

	if (track >= this->track_count())
	{
		return 0;
	}

	// Offsets are set, but last length / leadout is unknown

	if (track == this->track_count() - 1)
	{
		// We derive the length of the last track.
		// The last track has no trailing gap, therefore just subtracting
		// is consistent with appending trailing gaps to the previous track.

		return this->audio_size().leadout_frame()
			? this->audio_size().leadout_frame() - toc().offset(track + 1)
			: 0 ;
	}

	return toc().offset(track + 2) - toc().offset(track + 1);
}


ARId MultitrackCalcContext::do_id() const
{
	std::unique_ptr<ARId> id;

	try
	{
		id = make_arid(toc(), this->audio_size().leadout_frame());

	} catch (const InvalidMetadataException& e)
	{
		ARCS_LOG_WARNING << "Could not build ARId, cause: '" << e.what()
			<< "', will build empty ARId instead";

		id = make_empty_arid();
	}

	return *id;
}


bool MultitrackCalcContext::do_skips_front() const
{
	return true;
}


bool MultitrackCalcContext::do_skips_back() const
{
	return true;
}


void MultitrackCalcContext::set_toc(const TOC &toc)
{
	// NOTE: Leadout will be 0 if TOC is not complete.

	AudioSize audiosize;
	audiosize.set_leadout_frame(toc.leadout());
	this->set_audio_size(audiosize);

	toc_ = toc;
}


const TOC& MultitrackCalcContext::toc() const
{
	return toc_;
}


std::unique_ptr<CalcContext> MultitrackCalcContext::do_clone() const
{
	return std::make_unique<MultitrackCalcContext>(*this);
}


// CalcState


CalcState::~CalcState() noexcept = default;


// CalcStateARCSBase


CalcStateARCSBase::CalcStateARCSBase()
	: actual_skip_front_(0)
	, actual_skip_back_(0)
{
	// empty
}


CalcStateARCSBase::~CalcStateARCSBase() noexcept = default;


void CalcStateARCSBase::init_with_skip()
{
	actual_skip_front_ = NUM_SKIP_SAMPLES_FRONT;
	actual_skip_back_  = NUM_SKIP_SAMPLES_BACK;

	this->init(1 + NUM_SKIP_SAMPLES_FRONT);
}


void CalcStateARCSBase::init_without_skip()
{
	actual_skip_front_ = 0;
	actual_skip_back_  = 0;

	this->init(1);
}


uint32_t CalcStateARCSBase::num_skip_front() const
{
	return actual_skip_front_;
}


uint32_t CalcStateARCSBase::num_skip_back() const
{
	return actual_skip_back_;
}


void CalcStateARCSBase::update(PCMForwardIterator &begin, PCMForwardIterator &end)
{
	ARCS_LOG_DEBUG << "    First multiplier is: " << this->mult();
	this->do_update(begin, end);
	ARCS_LOG_DEBUG << "    Last multiplier was: " << (this->mult() - 1);
}

/// \endcond


/**
 * \brief CalcState for calculation of ARCSv1.
 */
class CalcStateV1 final : public CalcStateARCSBase
{

public:

	/**
	 * \brief Default constructor.
	 */
	CalcStateV1();

	void save(const TrackNo track) override;

	int track_count() const override;

	checksum::type type() const override;

	ChecksumSet result(const TrackNo track) const override;

	ChecksumSet result() const override;

	void reset() override;

	void wipe() override;

	uint32_t mult() const override;

	std::unique_ptr<CalcState> clone() const override;


protected:

	/**
	 * \brief Worker: find Checksum for specified track or 0
	 *
	 * \param[in] track Track number or 0
	 *
	 * \return The Checksum for this track
	 */
	Checksum find(const uint8_t track) const;

	/**
	 * \brief Worker: compose a ChecksumSet from a single Checksum
	 *
	 * \param[in] checksum The Checksum
	 *
	 * \return The ChecksumSet containing \c checksum
	 */
	ChecksumSet compose(const Checksum &checksum) const;


private:

	void init(const uint32_t mult) override;

	void do_update(PCMForwardIterator &begin, PCMForwardIterator &end) override;

	/**
	 * \brief The multiplier to compute the ARCS values v1 and v2. Starts with 1
	 * on the first sample after the pregap of the first track.
	 */
	uint_fast64_t multiplier_;

	/**
	 * \brief State: subtotal of ARCS v1 (accumulates lower bits of each product).
	 */
	uint_fast32_t subtotal_v1_;

	/**
	 * \brief Internal representation of the calculated ARCS values
	 */
	std::unordered_map<TrackNo, uint32_t> arcss_;
};


/// \cond UNDOC_FUNCTION_BODIES


CalcStateV1::CalcStateV1()
	: multiplier_(1)
	, subtotal_v1_(0)
	, arcss_()
{
	// empty
}


void CalcStateV1::do_update(PCMForwardIterator &begin, PCMForwardIterator &end)
{
	for (auto pos = begin; pos != end; ++pos, ++multiplier_)
	{
		subtotal_v1_ += (multiplier_ * (*pos)) & LOWER_32_BITS_;
	}
}


void CalcStateV1::save(const TrackNo track)
{
	const auto rc { arcss_.insert(std::make_pair(track, subtotal_v1_)) };

	if (not rc.second)
	{
		ARCS_LOG_WARNING << "Checksum for track "
			<< std::to_string(track) << " was not saved";
	}

	this->reset();
}


int CalcStateV1::track_count() const
{
	return arcss_.size();
}


checksum::type CalcStateV1::type() const
{
	return checksum::type::ARCS1;
}


ChecksumSet CalcStateV1::result(const TrackNo track) const
{
	const auto arcs1_value { this->find(track) };
	return compose(arcs1_value);
}


ChecksumSet CalcStateV1::result() const
{
	const auto arcs1_value { this->find(0) };
	return compose(arcs1_value);
}


void CalcStateV1::reset()
{
	multiplier_  = 1;
	subtotal_v1_ = 0;
}


void CalcStateV1::wipe()
{
	this->reset();
	arcss_.clear();
}


uint32_t CalcStateV1::mult() const
{
	return multiplier_;
}


std::unique_ptr<CalcState> CalcStateV1::clone() const
{
	return std::make_unique<CalcStateV1>(*this);
}


void CalcStateV1::init(const uint32_t mult)
{
	this->wipe();

	multiplier_ = mult;
}


Checksum CalcStateV1::find(const uint8_t track) const
{
	const auto value { arcss_.find(track) };

	if (value == arcss_.end())
	{
		return Checksum{};
	}

	return Checksum { value->second };
}


ChecksumSet CalcStateV1::compose(const Checksum &checksum) const
{
	ChecksumSet checksums;

	const auto rc { checksums.insert(checksum::type::ARCS1, checksum) };

	if (not rc.second)
	{
		ARCS_LOG_WARNING << "No value found for type "
			<< checksum::type_name(checksum::type::ARCS1);

		return ChecksumSet{};
	}

	return checksums;
}

/// \endcond


/**
 * \brief CalcState for calculation of ARCSv2 and ARCSv1.
 */
class CalcStateV1andV2 final : public CalcStateARCSBase
{

public:

	/**
	 * \brief Default constructor
	 */
	CalcStateV1andV2();

	void save(const TrackNo track) override;

	int track_count() const override;

	checksum::type type() const override;

	ChecksumSet result(const TrackNo track) const override;

	ChecksumSet result() const override;

	void reset() override;

	void wipe() override;

	uint32_t mult() const override;

	std::unique_ptr<CalcState> clone() const override;


protected:

	ChecksumSet find(const uint8_t track) const;


private:

	void init(const uint32_t mult) override;

	void do_update(PCMForwardIterator &begin, PCMForwardIterator &end) override;

	/**
	 * \brief The multiplier to compute the ARCS values v1 and v2. Starts with 1
	 * on the first sample after the pregap of the first track.
	 */
	uint_fast64_t multiplier_;

	/**
	 * \brief State: subtotal of ARCS v1 (accumulates lower bits of each product).
	 */
	uint_fast32_t subtotal_v1_;

	/**
	 * \brief State: subtotal of ARCS v2 (accumulates higher bits of each product).
	 * The ARCS v2 is the sum of subtotal_v1_ and subtotal_v2_.
	 */
	uint_fast32_t subtotal_v2_;

	/**
	 * \brief State: product of sample and index multiplier
	 */
	uint_fast64_t update64_;

	/**
	 * \brief Internal representation of the calculated ARCS values
	 */
	std::unordered_map<TrackNo, std::pair<uint32_t, uint32_t>> arcss_;
};

/// \cond UNDOC_FUNCTION_BODIES


CalcStateV1andV2::CalcStateV1andV2()
	: multiplier_(1)
	, subtotal_v1_(0)
	, subtotal_v2_(0)
	, update64_(0)
	, arcss_()
{
	// empty
}


void CalcStateV1andV2::do_update(PCMForwardIterator &begin,
		PCMForwardIterator &end)
{
	for (auto pos = begin; pos != end; ++pos, ++multiplier_)
	{
		update64_ = multiplier_ * (*pos);
		subtotal_v1_ +=  update64_ & LOWER_32_BITS_;
		subtotal_v2_ += (update64_ >> 32u);
	}
}


void CalcStateV1andV2::save(const TrackNo track)
{
	const auto rc = arcss_.insert(
		//std::pair<TrackNo, std::pair<uint_fast32_t, uint_fast32_t> >(
		std::make_pair(
			track,
			//std::pair<uint_fast32_t, uint_fast32_t>(
			std::make_pair(
				subtotal_v1_,
				subtotal_v1_ + subtotal_v2_
			)
		)
	);

	if (not rc.second)
	{
		ARCS_LOG_WARNING << "Checksum for track "
			<< std::to_string(track) << " was not saved";
	}

	this->reset();
}


int CalcStateV1andV2::track_count() const
{
	return arcss_.size();
}


checksum::type CalcStateV1andV2::type() const
{
	return checksum::type::ARCS2;
}


ChecksumSet CalcStateV1andV2::result(const TrackNo track) const
{
	return this->find(track);
}


ChecksumSet CalcStateV1andV2::result() const
{
	return this->find(0);
}


void CalcStateV1andV2::reset()
{
	multiplier_  = 1;
	subtotal_v1_ = 0;
	subtotal_v2_ = 0;

	// For completeness. Value does not affect updating
	update64_ = 0;
}


void CalcStateV1andV2::wipe()
{
	this->reset();
	arcss_.clear();
}


uint32_t CalcStateV1andV2::mult() const
{
	return multiplier_;
}


std::unique_ptr<CalcState> CalcStateV1andV2::clone() const
{
	return std::make_unique<CalcStateV1andV2>(*this);
}


void CalcStateV1andV2::init(const uint32_t mult)
{
	this->wipe();

	multiplier_ = mult;
}


ChecksumSet CalcStateV1andV2::find(const uint8_t track) const
{
	const auto value { arcss_.find(track) };

	if (value == arcss_.end())
	{
		return ChecksumSet{};
	}

	ChecksumSet checksums;

	auto rc {
		checksums.insert(checksum::type::ARCS2,
				Checksum { value->second.second })
	};

	if (not rc.second)
	{
		ARCS_LOG_WARNING << "Insertion to result failed for type "
			<< checksum::type_name(checksum::type::ARCS2);
	}

	rc = checksums.insert(checksum::type::ARCS1,
			Checksum { value->second.first});

	if (not rc.second)
	{
		ARCS_LOG_WARNING << "Insertion to result failed for type "
			<< checksum::type_name(checksum::type::ARCS1);
	}

	return checksums;
}

/// \endcond


/**
 * \brief Private implementation of Calculation.
 *
 * This class is not intended as a base class for inheritance.
 *
 * \see Calculation
 */
class Calculation::Impl final
{

public:

	/**
	 * \brief Constructor with type and context.
	 *
	 * \param[in] type Checksum Type
	 * \param[in] ctx  Context
	 */
	Impl(const checksum::type type, std::unique_ptr<CalcContext> ctx);

	/**
	 * \brief Constructor with context and checksum::type::ARCS2.
	 *
	 * \param[in] ctx Context
	 */
	explicit Impl(std::unique_ptr<CalcContext> ctx);

	/**
	 * \brief Copy constructor.
	 *
	 * \param rhs The Calculation::Impl to copy
	 */
	Impl(const Impl &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param rhs The Calculation::Impl to move
	 */
	Impl(Impl &&rhs) noexcept = default;

	/**
	 * \brief Default destructor.
	 */
	~Impl() noexcept = default;

	/**
	 * \brief Implements Calculation::set_context().
	 */
	void set_context(std::unique_ptr<CalcContext> context);

	/**
	 * \brief Implements Calculation::context().
	 */
	const CalcContext& context() const;

	/**
	 * \brief Implements Calculation::type()
	 */
	checksum::type type() const;

	/**
	 * \brief Implements Calculation::update()
	 */
	void update(PCMForwardIterator &begin, PCMForwardIterator &end);

	/**
	 * \brief Implements Calculation::update_audiosize(const AudioSize &audiosize).
	 */
	void update_audiosize(const AudioSize &audiosize);

	/**
	 * \brief Implements Calculation::complete().
	 */
	bool complete() const;

	/**
	 * \brief Implements Calculation::sample_counter().
	 */
	uint32_t sample_counter() const;

	/**
	 * \brief Implements Calculation::result().
	 */
	Checksums result() const;

	/**
	 * \brief Set the Partitioner for this instance.
	 *
	 * \param[in] partitioner The Partitioner for this instance
	 */
	void set_partitioner(std::unique_ptr<Partitioner> partitioner);

	/**
	 * \brief Read the Partitioner of this instance.
	 *
	 * \return The Partitioner of this instance
	 */
	const Partitioner& partitioner() const;

	/**
	 * \brief Read the state of this instance.
	 *
	 * \return the CalcState of this instance to read
	 */
	const CalcState& state() const;

	/**
	 * \brief Implements Calculation::set_type()
	 */
	void set_type(const checksum::type type);

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Impl& operator = (const Impl &rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Impl& operator = (Impl &&rhs) noexcept;


protected:

	/**
	 * \brief Initializes state according to context (multi- or singletrack) and
	 * transfers the skipping amounts back to the context.
	 */
	void sync_state_and_context();

	/**
	 * \brief Set \c context as new context or, if nullptr, the default context.
	 *
	 * \param[in] ctx The new context, if non-empty
	 */
	void set_context_or_default(std::unique_ptr<CalcContext> ctx);

	/**
	 * \brief Log statistics about a Partition.
	 *
	 * \param[in] i     Chunk counter
	 * \param[in] n     Number of chunks in block
	 * \param[in] chunk Chunk to log
	 */
	void log_partition(const uint16_t i, const uint16_t n,
			const Partition &chunk) const;


private:

	/**
	 * \brief State: 1-based global index of the sample to be processed as next.
	 */
	uint32_t smpl_offset_;

	/**
	 * \brief Internal stream context.
	 */
	std::unique_ptr<CalcContext> context_;

	/**
	 * \brief Internal computation state.
	 */
	std::unique_ptr<CalcState> state_;

	/**
	 * \brief Internal partitioner
	 */
	std::unique_ptr<Partitioner> partitioner_;

	/**
	 * \brief Accumulated time elapsed by processing blocks
	 */
	std::chrono::milliseconds proc_time_elapsed_;
};

/// \cond UNDOC_FUNCTION_BODIES


Calculation::Impl::Impl(const checksum::type type,
		std::unique_ptr<CalcContext> ctx)
	: smpl_offset_(0)
	, context_(nullptr)
	, state_(nullptr)
	, partitioner_(nullptr)
	, proc_time_elapsed_(std::chrono::milliseconds::zero())
{
	this->set_type(type);
	this->set_context_or_default(std::move(ctx));
}


Calculation::Impl::Impl(std::unique_ptr<CalcContext> ctx)
	: smpl_offset_(0)
	, context_(nullptr)
	, state_(std::make_unique<CalcStateV1andV2>()) // default
	, partitioner_(nullptr)
	, proc_time_elapsed_(std::chrono::milliseconds::zero())
{
	this->set_context_or_default(std::move(ctx));
}


Calculation::Impl::Impl(const Impl& rhs)
	: smpl_offset_(rhs.smpl_offset_)
	, context_(rhs.context_->clone())
	, state_(rhs.state_->clone())
	, partitioner_(rhs.partitioner_->clone())
	, proc_time_elapsed_(rhs.proc_time_elapsed_)
{
	// empty
}


void Calculation::Impl::set_context(std::unique_ptr<CalcContext> context)
{
	if (!context)
	{
		return;
	}

	this->context_ = std::move(context);
	this->smpl_offset_ = 0;

	this->sync_state_and_context();

	// Initialize partitioners for multi- or singletrack mode

	if (context_->is_multi_track())
	{
		this->set_partitioner(std::make_unique<MultitrackPartitioner>());
	} else
	{
		this->set_partitioner(std::make_unique<SingletrackPartitioner>());
	}
}


const CalcContext& Calculation::Impl::context() const
{
	return *context_;
}


void Calculation::Impl::set_type(const checksum::type type)
{
	try
	{
		state_ = state::make(type);

	} catch (const std::exception& e)
	{
		ARCS_LOG_ERROR << e.what();

		std::stringstream msg;
		msg << "Could not load CalcState for requested type "
			<< checksum::type_name(type) << ": ";
		msg << e.what();

		throw std::invalid_argument(msg.str());
	}

	this->sync_state_and_context();
}


checksum::type Calculation::Impl::type() const
{
	return state_->type();
}


void Calculation::Impl::update_audiosize(const AudioSize &audiosize)
{
	context_->set_audio_size(audiosize);
}


bool Calculation::Impl::complete() const
{
	// Dangerous. smpl_offset_ will stop on the right value only iff
	// the size of the last input processed (which may have a smaller size
	// than its predecessors) was accurately set before passing it to update()

	return this->smpl_offset_ == context().audio_size().sample_count();
}


void Calculation::Impl::update(PCMForwardIterator &begin,
		PCMForwardIterator &end)
{
	const auto samples_in_block     { std::distance(begin, end) };
	const auto last_sample_in_block { smpl_offset_ + samples_in_block - 1 };

	ARCS_LOG_DEBUG << "  Offset:  " << smpl_offset_ << " samples";
	ARCS_LOG_DEBUG << "  Size:    " << samples_in_block << " samples";
	ARCS_LOG_DEBUG << "  Indices: " <<
		smpl_offset_ << " - " << last_sample_in_block;


	// Create a partitioning following the track bounds in this block

	auto partitioning {
		partitioner_->create_partitioning(smpl_offset_, samples_in_block,
				context())
	};

	ARCS_LOG_DEBUG << "  Partitions:  " << partitioning.size();

	const bool is_last_relevant_block {
		Interval(smpl_offset_, last_sample_in_block).contains(
			context().last_relevant_sample())
	};


	// Update the internal CalcState with each partition in this partitioning

	uint16_t partition_counter        { 0 };
	uint32_t relevant_samples_counter { 0 };

	const auto start_time { std::chrono::steady_clock::now() };
	for (const auto& partition : partitioning)
	{
		++partition_counter;
		relevant_samples_counter += partition.size();

		this->log_partition(partition_counter, partitioning.size(), partition);

		// Update the calculation state with the current chunk

		PCMForwardIterator chunk_begin { begin + partition.begin_offset() };
		PCMForwardIterator chunk_end   { begin + partition.end_offset()   };

		state_->update(chunk_begin, chunk_end);

		// If the current partition ends a track, save the ARCSs for this track

		if (partition.ends_track())
		{
			state_->save(partition.track());

			ARCS_LOG_DEBUG << "    Completed track: "
				<< std::to_string(partition.track());
		}
	}
	smpl_offset_ += samples_in_block;
	const auto end_time { std::chrono::steady_clock::now() };


	// Do the logging

	ARCS_LOG_DEBUG << "  Number of relevant samples in this block: "
			<< relevant_samples_counter;

	{
		const auto block_time_elapsed {
			std::chrono::duration_cast<std::chrono::milliseconds>
				(end_time - start_time)
		};

		proc_time_elapsed_ += block_time_elapsed;

		ARCS_LOG_DEBUG << "  Milliseconds elapsed by processing this block: "
			<<	block_time_elapsed.count();
	}

	if (is_last_relevant_block)
	{
		ARCS_LOG(DEBUG1) << "Calculation complete.";
		ARCS_LOG(DEBUG1) << "Total samples counted:  " << smpl_offset_;
		ARCS_LOG(DEBUG1) << "Total samples declared: " <<
			context().audio_size().sample_count();
		ARCS_LOG(DEBUG1) << "Milliseconds elapsed by calculating ARCSs: "
			<< proc_time_elapsed_.count();
	}
}


uint32_t Calculation::Impl::sample_counter() const
{
	return smpl_offset_;
}


Checksums Calculation::Impl::result() const
{
	if (not context_ or not state_)
	{
		return Checksums(0);
	}

	auto track_count { state_->track_count()  };
	auto result      { Checksums(track_count) };

	// FIXME Does not make sense, a loop from 0 to track_count should be correct
	// for both cases

	if (context_->is_multi_track())
	{
		// multitrack

		for (uint8_t i = 0; i < track_count; ++i)
		{
			ChecksumSet checksums { context_->length(i) };

			checksums.merge(state_->result(i + 1));

			result[i] = checksums;
		}
	} else
	{
		// singletrack

		ChecksumSet checksums { context_->audio_size().leadout_frame() };

		checksums.merge(state_->result()); // alias for result(0)

		result[0] = checksums;
	}

	return result;
}


void Calculation::Impl::set_partitioner(
			std::unique_ptr<Partitioner> partitioner)
{
	partitioner_ = std::move(partitioner);
}


const Partitioner& Calculation::Impl::partitioner() const
{
	return *partitioner_;
}


const CalcState& Calculation::Impl::state() const
{
	return *state_;
}


Calculation::Impl& Calculation::Impl::operator = (const Calculation::Impl& rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	// This needs deep copies of each and every member

	// First ensure that copying suceeds before changing internal state

	auto context_copy     { rhs.context_->clone()     };
	auto state_copy       { rhs.state_->clone()       };
	auto partitioner_copy { rhs.partitioner_->clone() };

	// No exception so far, do the assignments

	this->context_           = std::move(context_copy);
	this->state_             = std::move(state_copy);
	this->partitioner_       = std::move(partitioner_copy);
	this->smpl_offset_       = rhs.smpl_offset_;
	this->proc_time_elapsed_ = rhs.proc_time_elapsed_;

	return *this;
}


Calculation::Impl& Calculation::Impl::operator = (
			Calculation::Impl &&rhs) noexcept = default;


void Calculation::Impl::sync_state_and_context()
{
	if (not context_ or not state_)
	{
		return;
	}

	// notify state about skipping requirements

	if (context_->skips_front())
	{
		ARCS_LOG(DEBUG1) << "Init with skip";

		state_->init_with_skip();

		ARCS_LOG(DEBUG1) << "State init front skip: "
			<< state_->num_skip_front();
		ARCS_LOG(DEBUG1) << "State init back skip: "
			<< state_->num_skip_back();

	} else
	{
		ARCS_LOG(DEBUG1) << "Init without skip";

		state_->init_without_skip();
	}

	// notify context about skipping amounts

	context_->notify_skips(state_->num_skip_front(), state_->num_skip_back());
}


void Calculation::Impl::set_context_or_default(std::unique_ptr<CalcContext> ctx)
{
	this->set_context(std::move(ctx));

	if (not context_)
	{
		this->set_context(
				std::make_unique<SingletrackCalcContext>(std::string()));
	}
}


void Calculation::Impl::log_partition(const uint16_t i,
		const uint16_t n, const Partition &chunk) const
{
	ARCS_LOG_DEBUG << "  CHUNK " << i << "/" << n;

	const uint32_t chunk_first_smpl_idx { chunk.first_sample_idx() };
	const uint32_t chunk_last_smpl_idx  { chunk.last_sample_idx()  };

	const uint32_t samples_in_chunk {
		chunk_last_smpl_idx - chunk_first_smpl_idx + 1
		// chunk_first_smpl_idx counts as relevant therefore + 1
	};

	const bool chunk_starts_track { chunk.starts_track() };

	ARCS_LOG_DEBUG << "    Samples " << chunk_first_smpl_idx
			<< " - "              << chunk_last_smpl_idx
			<< " (Track "         << std::to_string(chunk.track()) << ", "
			<< (chunk.ends_track()
				? (chunk_starts_track
					? "complete"
					: "last part")
				: (chunk_starts_track
					? "first part"
					: "intermediate part"))
			<< ")";

	ARCS_LOG_DEBUG << "    Number of relevant samples in chunk:  "
		<< samples_in_chunk;
}


// .hpp


// AudioSize


AudioSize::AudioSize()
	: impl_(std::make_unique<AudioSize::Impl>())
{
	//empty
}


AudioSize::AudioSize(const AudioSize &rhs)
	: impl_(std::make_unique<AudioSize::Impl>(*rhs.impl_))
{
	//empty
}


AudioSize::AudioSize(AudioSize &&rhs) noexcept = default;


AudioSize::~AudioSize() noexcept = default;


void AudioSize::set_leadout_frame(const uint32_t leadout)
{
	impl_->set_leadout_frame(leadout);
}


uint32_t AudioSize::leadout_frame() const
{
	return impl_->leadout_frame();
}


void AudioSize::set_sample_count(const uint32_t sample_count)
{
	impl_->set_sample_count(sample_count);
}


uint32_t AudioSize::sample_count() const
{
	return impl_->sample_count();
}


void AudioSize::set_pcm_byte_count(const uint64_t byte_count)
{
	impl_->set_pcm_byte_count(byte_count);
}


uint64_t AudioSize::pcm_byte_count() const
{
	return impl_->pcm_byte_count();
}


bool AudioSize::null() const
{
	return 0 == impl_->pcm_byte_count();
}


AudioSize& AudioSize::operator = (AudioSize rhs)
{
	std::swap(*impl_, *rhs.impl_);
	return *this;
}


AudioSize& AudioSize::operator = (AudioSize &&rhs) noexcept = default;


// CalcContext


CalcContext::~CalcContext() noexcept = default;


// Calculation


Calculation::Calculation(const checksum::type type,
		std::unique_ptr<CalcContext> ctx)
	: impl_(std::make_unique<Calculation::Impl>(type, std::move(ctx)))
{
	// empty
}


Calculation::Calculation(std::unique_ptr<CalcContext> ctx)
	: impl_(std::make_unique<Calculation::Impl>(std::move(ctx)))
{
	// empty
}


Calculation::Calculation(const Calculation &rhs)
	: impl_(std::make_unique<Calculation::Impl>(*rhs.impl_))
{
	// empty
}


Calculation::Calculation(Calculation &&rhs) noexcept = default;


Calculation::~Calculation() noexcept = default;


void Calculation::set_context(std::unique_ptr<CalcContext> context)
{
	impl_->set_context(std::move(context));
}


const CalcContext& Calculation::context() const
{
	return impl_->context();
}


void Calculation::set_type(const checksum::type type)
{
	impl_->set_type(type);
}


checksum::type Calculation::type() const
{
	return impl_->type();
}


void Calculation::update(PCMForwardIterator begin, PCMForwardIterator end)
{
	ARCS_LOG_DEBUG << "PROCESS BLOCK";

	if (end == begin)
	{
		ARCS_LOG_WARNING << "No samples to update calculation. Return";
		return;
	}

	if (impl_->context().audio_size().pcm_byte_count() == 0)
	{
		ARCS_LOG_ERROR << "Context says there are 0 bytes to process";
		return;
	}

	impl_->update(begin, end);

	ARCS_LOG_DEBUG << "END BLOCK";
}


void Calculation::update_audiosize(const AudioSize &audiosize)
{
	impl_->update_audiosize(audiosize);
}


bool Calculation::complete() const
{
	return impl_->complete();
}


uint32_t Calculation::sample_counter() const
{
	return impl_->sample_counter();
}


Checksums Calculation::result() const
{
	return impl_->result();
}


Calculation& Calculation::operator = (Calculation rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	impl_ = std::make_unique<Calculation::Impl>(*rhs.impl_);
	return *this;
}


Calculation& Calculation::operator = (Calculation &&rhs) noexcept = default;


// Checksums


Checksums::Checksums(const std::size_t size)
	: sets_( std::make_unique<ChecksumSet[]>(size) )
	, size_( size )
{
	// empty
}


Checksums::Checksums(const Checksums &rhs)
	: sets_( new ChecksumSet[rhs.size_] )
	, size_(rhs.size_)
{
	std::copy(rhs.begin(), rhs.end(), this->begin());
}


Checksums::Checksums(Checksums &&rhs) noexcept = default;


Checksums::iterator Checksums::begin()
{
	return sets_.get();
}


Checksums::iterator Checksums::end()
{
	return std::next(sets_.get(), size_);
}


Checksums::const_iterator Checksums::begin() const
{
	return sets_.get();
}


Checksums::const_iterator Checksums::end() const
{
	return std::next(sets_.get(), size_);
}


Checksums::const_iterator Checksums::cbegin() const
{
	return this->begin();
}


Checksums::const_iterator Checksums::cend() const
{
	return this->end();
}


ChecksumSet& Checksums::operator [] (const uint32_t index)
{
	// Confer Meyers, Scott: Effective C++, 3rd ed.,
	// Item 3, Section "Avoiding Duplication in const and Non-const member
	// Functions", p. 23ff
	return const_cast<ChecksumSet&>(
			(*static_cast<const Checksums*>(this))[index]);
}


const ChecksumSet& Checksums::operator [] (const uint32_t index) const
{
	return (sets_.get())[index];
}


std::size_t Checksums::size() const
{
	return size_;
}


Checksums& Checksums::operator = (const Checksums &rhs)
{
	Checksums sums(rhs);
	std::swap(this->sets_, sums.sets_);
	std::swap(this->size_, sums.size_);
	return *this;
}


Checksums& Checksums::operator = (Checksums &&rhs) noexcept = default;


// InvalidAudioException


InvalidAudioException::InvalidAudioException(const std::string &what_arg)
	: std::logic_error(what_arg)
{
	// empty
}


InvalidAudioException::InvalidAudioException(const char *what_arg)
	: std::logic_error(what_arg)
{
	// empty
}


// make_context (audiofile, bool, bool)


std::unique_ptr<CalcContext> make_context(const std::string &audiofilename,
		const bool &skip_front, const bool &skip_back)
{
	// Note: ARCS specific values, since ARCS2 is default checksum type
	return std::make_unique<SingletrackCalcContext>(audiofilename,
			skip_front, NUM_SKIP_SAMPLES_FRONT,
			skip_back,  NUM_SKIP_SAMPLES_BACK);
}


// make_context (audiofile, TOC)


std::unique_ptr<CalcContext> make_context(const std::string &audiofilename,
		const TOC &toc)
{
	// Note: ARCS specific values, since ARCS2 is default checksum type
	return std::make_unique<MultitrackCalcContext>(audiofilename, toc,
			NUM_SKIP_SAMPLES_FRONT, NUM_SKIP_SAMPLES_BACK);
}

/// \endcond

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

