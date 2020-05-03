/**
 * \file
 *
 * \brief Implementation of the checksum calculation API
 */

#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif

#include <array>                  // for array
#include <chrono>                 // for milliseconds, duration_cast, operator-
#include <cstdint>                // for uint32_t, uint_fast32_t, uint8_t
#include <initializer_list>       // for initializer_list
#include <iomanip>                // for operator<<, setfill, setw
#include <iterator>               // for distance
#include <limits>                 // for numeric_limits
#include <map>                    // for map
#include <memory>                 // for unique_ptr, make_unique, unique_ptr...
#include <set>                    // for set
#include <sstream>                // for operator<<, basic_ostream, basic_os...
#include <stdexcept>              // for overflow_error, invalid_argument, ...
#include <string>                 // for char_traits, operator<<, string
#include <type_traits>            // for __underlying_type_impl<>::type, und...
#include <unordered_map>          // for unordered_map, operator==, _Node_co...
#include <utility>                // for pair, move, make_pair, swap
#include <vector>                 // for vector


#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

#ifndef __LIBARCSTK_APPENDABLESEQ_HPP__
#include "appendableseq.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

using arcstk::v_1_0_0::details::Interval;
using arcstk::v_1_0_0::details::Partition;
using arcstk::v_1_0_0::details::Partitioner;
using arcstk::v_1_0_0::details::MultitrackPartitioner;
using arcstk::v_1_0_0::details::SingletrackPartitioner;
//using arcstk::v_1_0_0::details::CalcContextBase; // unused
using arcstk::v_1_0_0::details::CalcState;
//using arcstk::v_1_0_0::details::CalcStateARCSBase; // unused
using arcstk::v_1_0_0::details::CalcStateV1;
using arcstk::v_1_0_0::details::CalcStateV1andV2;


namespace
{

/**
 * \brief Number of samples to be skipped before the end of the last track.
 *
 * There are 5 frames to be skipped, i.e. 5 frames * 588 samples/frame
 * = 2940 samples. We derive the number of samples to be skipped at the
 * start of the first track by just subtracting 1 from this constant.
 */
constexpr sample_count NUM_SKIP_SAMPLES_BACK  = 5/* frames */ * 588/* samples */;

/**
 * \brief Number of samples to be skipped after the start of the first track.
 *
 * There are 5 frames - 1 sample to be skipped, i.e.
 * 5 frames * 588 samples/frame - 1 sample = 2939 samples.
 */
constexpr sample_count NUM_SKIP_SAMPLES_FRONT = NUM_SKIP_SAMPLES_BACK - 1;

} // namespace


/**
 * \internal
 * \ingroup calc
 *
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
	Impl() noexcept;

	/**
	 * \brief Constructor
	 *
	 * \param[in] value Absolute size
	 * \param[in] unit  The unit of the declaring value
	 */
	Impl(const long int value, const AudioSize::UNIT unit) noexcept;

	/**
	 * \brief Implements AudioSize::set_leadout_frame(const uint32_t leadout)
	 */
	void set_total_frames(const lba_count leadout) noexcept;

	/**
	 * \brief Implements AudioSize::leadout_frame() const
	 */
	lba_count total_frames() const noexcept;

	/**
	 * \brief Implements AudioSize::set_total_samples(const sample_count smpl_count)
	 */
	void set_total_samples(const sample_count smpl_count) noexcept;

	/**
	 * \brief Implements AudioSize::total_samples() const
	 */
	sample_count total_samples() const noexcept;

	/**
	 * \brief Implements AudioSize::set_pcm_byte_count(const uint32_t byte_count)
	 */
	void set_total_pcm_bytes(const uint32_t byte_count) noexcept;

	/**
	 * \brief Implements AudioSize::pcm_byte_count() const
	 */
	uint32_t total_pcm_bytes() const noexcept;

	/**
	 * \brief Equality
	 *
	 * \param[in] rhs The instance to compare
	 */
	bool equals(const AudioSize::Impl &rhs) const noexcept;


private:

	/**
	 * \brief Convert \c value to the corrsponding number of bytes.
	 *
	 * \param[in] value Value to convert
	 * \param[in] unit  Unit of the value
	 *
	 * \return The equivalent number of bytes.
	 *
	 * \throw std::overflow_error If value is bigger than the legal unit maximum
	 * \throw std::underflow_error If value is negative
	 */
	uint32_t to_bytes(const long int value, const AudioSize::UNIT unit);

	/**
	 * \brief Convert \c frame_count to the corrsponding number of bytes.
	 *
	 * \param[in] frame_count Number of LBA frames to convert
	 *
	 * \return The equivalent number of bytes.
	 *
	 * \throw std::overflow_error If value is bigger than the legal unit maximum
	 */
	uint32_t frames_to_bytes(const lba_count frame_count);

	/**
	 * \brief Convert \c frame_count to the corrsponding number of bytes.
	 *
	 * \param[in] sample_count Number of PCM 32 bit samples to convert
	 *
	 * \return The equivalent number of bytes.
	 *
	 * \throw std::overflow_error If value is bigger than the legal unit maximum
	 */
	uint32_t samples_to_bytes(const sample_count sample_count);

	/**
	 * \brief Data: Number of pcm sample bytes in the audio file.
	 */
	uint32_t total_pcm_bytes_;
};


AudioSize::Impl::Impl() noexcept
	: total_pcm_bytes_ { 0 }
{
	// empty
}


AudioSize::Impl::Impl(const long int value, const AudioSize::UNIT unit) noexcept
	: total_pcm_bytes_ { to_bytes(value, unit) }
{
	// empty
}


uint32_t AudioSize::Impl::to_bytes(const long int value,
		const AudioSize::UNIT unit)
{
	if (value < 0)
	{
		auto ss = std::stringstream {};
		ss << "Cannot construct AudioSize from negative value: "
			<< std::to_string(value);

		throw std::underflow_error(ss.str());
	}

	if (value > std::numeric_limits<uint32_t>::max())
	{
		auto ss = std::stringstream {};
		ss << "Value too big for AudioSize: " << std::to_string(value);

		throw std::overflow_error(ss.str());
	}

	if (AudioSize::UNIT::FRAMES == unit)
	{
		return this->frames_to_bytes(value);
	}
	else if (AudioSize::UNIT::SAMPLES == unit)
	{
		return this->samples_to_bytes(value);
	}

	return value;
}


uint32_t AudioSize::Impl::frames_to_bytes(const lba_count frame_count)
{
	if (frame_count > static_cast<lba_count>(CDDA.MAX_BLOCK_ADDRESS))
	{
		auto ss = std::stringstream {};
		ss << "Frame count too big for AudioSize: "
			<< std::to_string(frame_count);

		throw std::overflow_error(ss.str());
	}

	return static_cast<uint32_t>(frame_count * CDDA.BYTES_PER_FRAME);
}


uint32_t AudioSize::Impl::samples_to_bytes(const sample_count smpl_count)
{
	static const sample_count MAX_SAMPLES {
		CDDA.SAMPLES_PER_FRAME * CDDA.MAX_BLOCK_ADDRESS };

	if (smpl_count > MAX_SAMPLES)
	{
		auto ss = std::stringstream {};
		ss << "Sample count too big for AudioSize: "
			<< std::to_string(smpl_count);

		throw std::overflow_error(ss.str());
	}

	return static_cast<uint32_t>(smpl_count * CDDA.BYTES_PER_SAMPLE);
}


void AudioSize::Impl::set_total_frames(const lba_count frame_count) noexcept
{
	this->set_total_pcm_bytes(this->frames_to_bytes(frame_count));
}


lba_count AudioSize::Impl::total_frames() const noexcept
{
	return static_cast<lba_count>(
		this->total_pcm_bytes() / static_cast<uint32_t>(CDDA.BYTES_PER_FRAME));
}


void AudioSize::Impl::set_total_samples(const sample_count smpl_count) noexcept
{
	this->set_total_pcm_bytes(this->samples_to_bytes(smpl_count));
}


sample_count AudioSize::Impl::total_samples() const noexcept
{
	return static_cast<sample_count>(
		this->total_pcm_bytes() / static_cast<uint32_t>(CDDA.BYTES_PER_SAMPLE));
}


void AudioSize::Impl::set_total_pcm_bytes(const uint32_t byte_count) noexcept
{
	total_pcm_bytes_ = byte_count;
}


uint32_t AudioSize::Impl::total_pcm_bytes() const noexcept
{
	return total_pcm_bytes_;
}


bool AudioSize::Impl::equals(const AudioSize::Impl &rhs) const noexcept
{
	return total_pcm_bytes_ == rhs.total_pcm_bytes_;
}


// CalcContext


void CalcContext::set_audio_size(const AudioSize &audio_size) noexcept
{
	this->do_set_audio_size(audio_size);
}


const AudioSize& CalcContext::audio_size() const noexcept
{
	return this->do_audio_size();
}


void CalcContext::set_filename(const std::string &filename) noexcept
{
	this->do_set_filename(filename);
}


std::string CalcContext::filename() const noexcept
{
	return this->do_filename();
}


uint8_t CalcContext::track_count() const noexcept
{
	return this->do_track_count();
}


bool CalcContext::is_multi_track() const noexcept
{
	return this->do_is_multi_track();
}


sample_count CalcContext::first_relevant_sample(const TrackNo track) const noexcept
{
	return this->do_first_relevant_sample(track);
}


sample_count CalcContext::first_relevant_sample() const noexcept
{
	return this->do_first_relevant_sample_0();
}


sample_count CalcContext::last_relevant_sample(const TrackNo track) const noexcept
{
	return this->do_last_relevant_sample(track);
}


sample_count CalcContext::last_relevant_sample() const noexcept
{
	return this->do_last_relevant_sample_0();
}


TrackNo CalcContext::track(const sample_count smpl) const noexcept
{
	return this->do_track(smpl);
}


lba_count CalcContext::offset(const uint8_t track) const noexcept
{
	return this->do_offset(track);
}


lba_count CalcContext::length(const uint8_t track) const noexcept
{
	return this->do_length(track);
}


ARId CalcContext::id() const noexcept
{
	return this->do_id();
}


bool CalcContext::skips_front() const noexcept
{
	return this->do_skips_front();
}


bool CalcContext::skips_back() const noexcept
{
	return this->do_skips_back();
}


sample_count CalcContext::num_skip_front() const noexcept
{
	return this->do_num_skip_front();
}


sample_count CalcContext::num_skip_back() const noexcept
{
	return this->do_num_skip_back();
}


void CalcContext::notify_skips(const sample_count num_skip_front,
		const sample_count num_skip_back) noexcept
{
	this->do_notify_skips(num_skip_front, num_skip_back);
}


std::unique_ptr<CalcContext> CalcContext::clone() const noexcept
{
	return this->do_clone();
}


namespace details
{


// Partition


Partition::Partition(
		const sample_count &begin_offset,
		const sample_count &end_offset,
		const sample_count &first,
		const sample_count &last,
		const bool     &starts_track,
		const bool     &ends_track,
		const TrackNo  &track
	)
	: begin_offset_ { begin_offset }
	, end_offset_ { end_offset }
	, first_sample_idx_ { first }
	, last_sample_idx_ { last }
	, starts_track_ { starts_track }
	, ends_track_ { ends_track }
	, track_ { track }
{
	// empty
}


sample_count Partition::begin_offset() const
{
	return begin_offset_;
}


sample_count Partition::end_offset() const
{
	return end_offset_;
}


sample_count Partition::first_sample_idx() const
{
	return first_sample_idx_;
}


sample_count Partition::last_sample_idx() const
{
	return last_sample_idx_;
}


bool Partition::starts_track() const
{
	return starts_track_;
}


bool Partition::ends_track() const
{
	return ends_track_;
}


TrackNo Partition::track() const
{
	return track_;
}


sample_count Partition::size() const
{
	return last_sample_idx() - first_sample_idx() + 1;
}


// Interval


Interval::Interval(const sample_count a, const sample_count b)
	: a_ { a }
	, b_ { b }
{
	// empty
}


bool Interval::contains(const sample_count i) const
{
	if (a_ <= b_)
	{
		return a_ <= i and i <= b_;
	}

	return a_ >= i and i >= b_;
}


// Partitioner


Partitioner::~Partitioner() noexcept = default;


Partitioning Partitioner::create_partitioning(
		const sample_count offset,
		const sample_count number_of_samples,
		const CalcContext &context) const
{
	// If the sample block does not contain any relevant samples,
	// just return an empty partition list

	const auto block_end { last_sample_idx(offset, number_of_samples) };

	const auto first_smpl { context.first_relevant_sample(1) };
	// avoids -Wstrict-overflow firing in if-clause

	if (block_end < first_smpl or offset > context.last_relevant_sample())
	{
		ARCS_LOG(DEBUG1) << "  No relevant samples in this block, skip";

		return Partitioning();
	}

	return this->do_create_partitioning(offset, number_of_samples, context);
}


sample_count Partitioner::last_sample_idx(const sample_count offset,
		const sample_count sample_count) const
{
	return offset + sample_count - 1;
}


Partition Partitioner::create_partition(
		const sample_count     &begin_offset,
		const sample_count     &end_offset,
		const sample_count     &first,
		const sample_count     &last,
		const bool         &starts_track,
		const bool         &ends_track,
		const TrackNo      &track) const
{
	return Partition(begin_offset, end_offset, first, last, starts_track,
			ends_track, track);
}


// MultitrackPartitioner


std::unique_ptr<Partitioner> MultitrackPartitioner::clone() const
{
	return std::make_unique<MultitrackPartitioner>(*this);
}


Partitioning MultitrackPartitioner::do_create_partitioning(
		const sample_count offset,
		const sample_count number_of_samples,
		const CalcContext &context) const
{
	const auto total_samples = sample_count { number_of_samples };

	Interval sample_block {
		offset, this->last_sample_idx(offset, total_samples)
	};

	// If the sample index range of this block contains the last relevant
	// sample, set this as the last sample in block instead of the last
	// physical sample

	auto block_last_smpl = sample_count {
		this->last_sample_idx(offset, total_samples) };

	if (sample_block.contains(context.last_relevant_sample()))
	{
		block_last_smpl = context.last_relevant_sample();
	}

	// If the sample index range of this block contains the first relevant
	// sample, set this as the first sample of the first partition instead of
	// the first physical sample

	auto chunk_first_smpl = sample_count { offset };

	if (sample_block.contains(context.first_relevant_sample(1)))
	{
		chunk_first_smpl = context.first_relevant_sample(1);
	}

	// Will be track_count+1 if 1st sample is beyond global last relevant sample
	// This entails that the loop is not entered for irrelevant partitions
	auto track = TrackNo { context.track(chunk_first_smpl) };

	// If track > track_count this is global last sample
	auto chunk_last_smpl = sample_count { context.last_relevant_sample(track) };

	auto begin_offset = sample_count { 0 } ;
	auto end_offset   = sample_count { 0 } ;
	auto starts_track = bool { false } ;
	auto ends_track   = bool { false } ;

	const auto last_track = uint8_t { context.track_count() };


	// Now construct all partitions except the last (that needs clipping) in a
	// loop

	Partitioning chunks{};
	chunks.reserve(10);

	while (chunk_last_smpl < block_last_smpl and track <= last_track)
	{
		ends_track   = (chunk_last_smpl == context.last_relevant_sample(track));

		starts_track =
			(chunk_first_smpl == context.first_relevant_sample(track));

		begin_offset = chunk_first_smpl - offset;

		end_offset   = chunk_last_smpl  - offset + 1;

		chunks.push_back(
			this->create_partition(
				begin_offset,
				end_offset,
				chunk_first_smpl,
				chunk_last_smpl,
				starts_track,
				ends_track,
				track
			)
		);

		ARCS_LOG(DEBUG1) << "  Create chunk: " << chunk_first_smpl
				<< " - " << chunk_last_smpl;

		++track;

		chunk_first_smpl = chunk_last_smpl + 1;
		chunk_last_smpl  = context.last_relevant_sample(track);
	} // while


	// If the loop has finished or was never entered, the last partition has to
	// be prepared


	// Clip last partition to block end if necessary

	if (chunk_last_smpl > block_last_smpl)
	{
		chunk_last_smpl = block_last_smpl;

		ARCS_LOG(DEBUG1) << "  Block ends within track "
			<< std::to_string(track)
			<< ", clip last sample to: " << chunk_last_smpl;
	}

	// Prepare last partition

	starts_track = (chunk_first_smpl == context.first_relevant_sample(track));

	ends_track   = (chunk_last_smpl == context.last_relevant_sample(track));

	begin_offset = chunk_first_smpl - offset;

	end_offset   = chunk_last_smpl  - offset + 1;

	ARCS_LOG(DEBUG1) << "  Create last chunk: " << chunk_first_smpl
				<< " - " << chunk_last_smpl;

	chunks.push_back(
		this->create_partition(
			begin_offset,
			end_offset,
			chunk_first_smpl,
			chunk_last_smpl,
			starts_track,
			ends_track,
			track
		)
	);

	chunks.shrink_to_fit();

	return chunks;
}


// SingletrackPartitioner


std::unique_ptr<Partitioner> SingletrackPartitioner::clone() const
{
	return std::make_unique<SingletrackPartitioner>(*this);
}


Partitioning SingletrackPartitioner::do_create_partitioning(
		const sample_count offset,
		const sample_count number_of_samples,
		const CalcContext &context) const
{
	const auto total_samples = sample_count { number_of_samples };

	Interval sample_block {
		offset, this->last_sample_idx(offset, total_samples)
	};

	// If the sample index range of this block contains the last relevant
	// sample, set this as the last sample in block instead of the last
	// physical sample

	auto chunk_last_smpl = sample_count { this->last_sample_idx(offset, total_samples) };

	if (sample_block.contains(context.last_relevant_sample()))
	{
		chunk_last_smpl = context.last_relevant_sample();
	}

	// If the sample index range of this block contains the first relevant
	// sample, set this as the first sample of the first partition instead of
	// the first physical sample

	auto chunk_first_smpl = sample_count { offset };

	if (sample_block.contains(context.first_relevant_sample(1)))
	{
		chunk_first_smpl = context.first_relevant_sample(1);
	}

	// Create a single partition spanning the entire sample block, but respect
	// skipping samples at front or back

	// Is this the last partition in the current track?

	const bool ends_track {
		chunk_last_smpl == context.last_relevant_sample()
	};

	// Is this the first partition of the current track in the current block?

	const bool starts_track {
		chunk_first_smpl == context.first_relevant_sample(1)
	};

	// Determine first sample in partition (easy for singletrack: 0)

	const auto begin_offset = sample_count { chunk_first_smpl - offset };

	// Determine last sample in partition (easy for singletrack: total_samples)

	const auto end_offset = sample_count { chunk_last_smpl - offset + 1 };

	Partitioning chunks;
	chunks.push_back(
		this->create_partition(
			begin_offset,
			end_offset,
			chunk_first_smpl,
			chunk_last_smpl,
			starts_track,
			ends_track,
			0
		));

	return chunks;
}


// CalcContextBase


CalcContextBase::CalcContextBase(const std::string &filename,
		const sample_count num_skip_front,
		const sample_count num_skip_back)
	: audiosize_ { AudioSize() }
	, filename_ { filename }
	, num_skip_front_ { num_skip_front }
	, num_skip_back_ { num_skip_back }
{
	// empty
}


CalcContextBase::~CalcContextBase() noexcept = default;


void CalcContextBase::do_set_audio_size(const AudioSize &audio_size) noexcept
{
	audiosize_ = audio_size;

	this->do_hook_post_set_audio_size();
}


void CalcContextBase::do_hook_post_set_audio_size() noexcept
{
	// empty
}


bool CalcContextBase::equals(const CalcContextBase &rhs) const noexcept
{
	return audiosize_ == rhs.audiosize_
		and filename_ == rhs.filename_
		and num_skip_front_ == rhs.num_skip_front_
		and num_skip_back_ == rhs.num_skip_back_;
}


const AudioSize& CalcContextBase::do_audio_size() const noexcept
{
	return audiosize_;
}


void CalcContextBase::do_set_filename(const std::string &filename) noexcept
{
	filename_ = filename;
}


std::string CalcContextBase::do_filename() const noexcept
{
	return filename_;
}


sample_count CalcContextBase::do_first_relevant_sample(const TrackNo /* track */)
	const noexcept
{
	return 0; // no functionality, just to be overriden
}


sample_count CalcContextBase::do_first_relevant_sample_0() const noexcept
{
	return this->first_relevant_sample(1);
}


sample_count CalcContextBase::do_last_relevant_sample(const TrackNo /* track */)
	const noexcept
{
	return 0; // no functionality, just to be overriden
}


sample_count CalcContextBase::do_last_relevant_sample_0() const noexcept
{
	return this->last_relevant_sample(this->track_count());
}


sample_count CalcContextBase::do_num_skip_front() const noexcept
{
	return num_skip_front_;
}


sample_count CalcContextBase::do_num_skip_back() const noexcept
{
	return num_skip_back_;
}


void CalcContextBase::do_notify_skips(const sample_count num_skip_front,
		const sample_count num_skip_back) noexcept
{
	num_skip_front_ = num_skip_front;
	num_skip_back_  = num_skip_back;

	ARCS_LOG_DEBUG << "Set context front skip: " << num_skip_front_;
	ARCS_LOG_DEBUG << "Set context back skip:  " << num_skip_back_;
}


// operators for SingletrackCalcContexts


bool operator == (const SingletrackCalcContext &lhs,
		const SingletrackCalcContext &rhs) noexcept
{
	return lhs.equals(rhs)
		and lhs.skip_front_ == rhs.skip_front_
		and lhs.skip_back_  == rhs.skip_back_;
}


// SingletrackCalcContext


SingletrackCalcContext::SingletrackCalcContext(const std::string &filename)
	: CalcContextBase { filename, 0, 0 }
	, skip_front_ { false }
	, skip_back_  { false }
{
	// empty
}


SingletrackCalcContext::SingletrackCalcContext(const std::string &filename,
		const bool skip_front, const bool skip_back)
	: CalcContextBase { filename, 2939, 2940 }
	, skip_front_ { skip_front }
	, skip_back_  { skip_back }
{
	// empty
}


SingletrackCalcContext::SingletrackCalcContext(const std::string &filename,
		const bool skip_front, const sample_count num_skip_front,
		const bool skip_back,  const sample_count num_skip_back)
	: CalcContextBase { filename, num_skip_front, num_skip_back }
	, skip_front_ { skip_front }
	, skip_back_  { skip_back }
{
	// empty
}


uint8_t SingletrackCalcContext::do_track_count() const noexcept
{
	return 1;
}


bool SingletrackCalcContext::do_is_multi_track() const noexcept
{
	return false;
}


sample_count SingletrackCalcContext::do_first_relevant_sample(
		const TrackNo track) const noexcept
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


sample_count SingletrackCalcContext::do_last_relevant_sample(
		const TrackNo track) const noexcept
{
	// Illegal track request?
	if (track > CDDA.MAX_TRACKCOUNT)
	{
		return this->audio_size().total_samples() - 1;
	}

	// We have no offsets and the track parameter is irrelevant. Hence iff the
	// request adresses the last track and skipping applies, the correct
	// constant is provided, otherwise the result is always the last known
	// sample.

	return this->skips_back() and track == this->track_count() // ==1!
				/* FIXME Is this ^^^ necessary or even correct??? */
		? this->audio_size().total_samples() - 1 - this->num_skip_back()
		: this->audio_size().total_samples() - 1;
}


TrackNo SingletrackCalcContext::do_track(const sample_count /* smpl */) const
noexcept
{
	return 1;
}


lba_count SingletrackCalcContext::do_offset(const uint8_t /* track */) const
noexcept
{
	return 0;
}


lba_count SingletrackCalcContext::do_length(const uint8_t /* track */) const
noexcept
{
	return 0;
}


ARId SingletrackCalcContext::do_id() const noexcept
{
	return *(make_empty_arid());
}


bool SingletrackCalcContext::do_skips_front() const noexcept
{
	return skip_front_;
}


void SingletrackCalcContext::set_skip_front(const bool skip) noexcept
{
	skip_front_ = skip;
}


bool SingletrackCalcContext::do_skips_back() const noexcept
{
	return skip_back_;
}


void SingletrackCalcContext::set_skip_back(const bool skip) noexcept
{
	skip_back_ = skip;
}


std::unique_ptr<CalcContext> SingletrackCalcContext::do_clone() const noexcept
{
	return std::make_unique<SingletrackCalcContext>(*this);
}


// operators for MultitrackCalcContexts


bool operator == (const MultitrackCalcContext &lhs,
		const MultitrackCalcContext &rhs) noexcept
{
	return lhs.equals(rhs) and lhs.toc_ == rhs.toc_;
}


// MultitrackCalcContext


MultitrackCalcContext::MultitrackCalcContext(const TOC &toc,
		const std::string &filename)
	: CalcContextBase { filename, 0, 0 }
	, toc_ { toc }
{
	this->set_toc(toc_);
}


MultitrackCalcContext::MultitrackCalcContext(const std::unique_ptr<TOC> &toc,
		const std::string &filename)
	: CalcContextBase { filename, 0, 0 }
	, toc_ { *toc }
{
	this->set_toc(toc_);
}


MultitrackCalcContext::MultitrackCalcContext(const TOC &toc,
		const sample_count num_skip_front,
		const sample_count num_skip_back, const std::string &filename)
	: CalcContextBase { filename, num_skip_front, num_skip_back }
	, toc_ { toc }
{
	this->set_toc(toc_);
}


MultitrackCalcContext::MultitrackCalcContext(const std::unique_ptr<TOC> &toc,
		const sample_count num_skip_front,
		const sample_count num_skip_back, const std::string &filename)
	: CalcContextBase { filename, num_skip_front, num_skip_back }
	, toc_ { *toc }
{
	this->set_toc(toc_);
}


void MultitrackCalcContext::do_hook_post_set_audio_size() noexcept
{
	if (this->audio_size().leadout_frame() != this->toc().leadout())
	{
		details::TOCBuilder builder;
		builder.update(toc_, this->audio_size().leadout_frame());
	}
}


uint8_t MultitrackCalcContext::do_track_count() const noexcept
{
	return toc().track_count();
}


bool MultitrackCalcContext::do_is_multi_track() const noexcept
{
	return true;
}


sample_count MultitrackCalcContext::do_first_relevant_sample(const TrackNo track)
	const noexcept
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
		return toc().offset(1) * CDDA.SAMPLES_PER_FRAME + this->num_skip_front();
	}

	// Standard multi track case: just the first sample of the track
	return toc().offset(track) * CDDA.SAMPLES_PER_FRAME;
}


sample_count MultitrackCalcContext::do_last_relevant_sample(const TrackNo track)
	const noexcept
{
	// Illegal track request?
	if (track > CDDA.MAX_TRACKCOUNT)
	{
		return this->audio_size().total_samples() - 1;
	}

	// Invalid track requested?
	if (track > this->track_count())
	{
		// Return the last relevant sample respecting skipping
		return this->audio_size().total_samples() - 1
			- (this->skips_back() ? this->num_skip_back() : 0 );
	}

	// We have offsets, so we respect the corresponding offset to any track.

	if (this->skips_back() and track == this->track_count())
	{
		return this->audio_size().total_samples() - 1 - this->num_skip_back();
	}

	// Ensure result 0 for previous track's offset 0
	return toc().offset(track + 1)
		? toc().offset(track + 1) * CDDA.SAMPLES_PER_FRAME - 1
		: 0;
}


TrackNo MultitrackCalcContext::do_track(const sample_count smpl) const noexcept
{
	if (this->audio_size().total_samples() == 0)
	{
		return 0;
	}

	// Sample beyond last track?
	if (this->audio_size().total_samples() - smpl < 1)
	{
		// This will return an invalid track number
		// Caller has to check result for <= track_count() for a valid result
		return CDDA.MAX_TRACKCOUNT + 1;
	}

	const auto last_track { this->track_count() };

	// Increase track number while sample is smaller than track's last relevant
	auto track = TrackNo { 0 };
	for (sample_count last_sample_trk { this->last_relevant_sample(track) } ;
			smpl > last_sample_trk and track <= last_track ;
			++track, last_sample_trk = this->last_relevant_sample(track)) { } ;

	return track;
}


lba_count MultitrackCalcContext::do_offset(const uint8_t track) const noexcept
{
	return track < this->track_count() ? toc().offset(track + 1) : 0;
}


lba_count MultitrackCalcContext::do_length(const uint8_t track) const noexcept
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


ARId MultitrackCalcContext::do_id() const noexcept
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


bool MultitrackCalcContext::do_skips_front() const noexcept
{
	return true;
}


bool MultitrackCalcContext::do_skips_back() const noexcept
{
	return true;
}


void MultitrackCalcContext::set_toc(const TOC &toc) noexcept
{
	// NOTE: Leadout will be 0 if TOC is not complete.

	this->set_audio_size(AudioSize { toc.leadout(), AudioSize::UNIT::FRAMES });
	// Commented out: without conversion
	//AudioSize audiosize;
	//audiosize.set_leadout_frame(toc.leadout());
	//this->set_audio_size(audiosize);

	toc_ = toc;
}


const TOC& MultitrackCalcContext::toc() const noexcept
{
	return toc_;
}


std::unique_ptr<CalcContext> MultitrackCalcContext::do_clone() const noexcept
{
	return std::make_unique<MultitrackCalcContext>(*this);
}


// CalcState


CalcState::~CalcState() noexcept = default;


// CalcStateARCSBase


CalcStateARCSBase::CalcStateARCSBase()
	: actual_skip_front_ { 0 }
	, actual_skip_back_  { 0 }
{
	// empty
}


CalcStateARCSBase::~CalcStateARCSBase() noexcept = default;


void CalcStateARCSBase::init_with_skip() noexcept
{
	actual_skip_front_ = NUM_SKIP_SAMPLES_FRONT;
	actual_skip_back_  = NUM_SKIP_SAMPLES_BACK;

	this->init(1 + NUM_SKIP_SAMPLES_FRONT);
}


void CalcStateARCSBase::init_without_skip() noexcept
{
	actual_skip_front_ = 0;
	actual_skip_back_  = 0;

	this->init(1);
}


sample_count CalcStateARCSBase::num_skip_front() const noexcept
{
	return actual_skip_front_;
}


sample_count CalcStateARCSBase::num_skip_back() const noexcept
{
	return actual_skip_back_;
}


void CalcStateARCSBase::update(SampleInputIterator &begin,
		SampleInputIterator &end)
{
	ARCS_LOG_DEBUG << "    First multiplier is: " << this->mult();
	this->do_update(begin, end);
	ARCS_LOG_DEBUG << "    Last multiplier was: " << (this->mult() - 1);
}


/**
 * \internal
 * \ingroup calc
 *
 * \brief CalcState for calculation of ARCSv1.
 */
class CalcStateV1 final : public CalcStateARCSBase
{

public:

	/**
	 * \brief Default constructor.
	 */
	CalcStateV1();

	void save(const TrackNo track) noexcept final;

	int track_count() const noexcept final;

	checksum::type type() const noexcept final;

	ChecksumSet result(const TrackNo track) const noexcept final;

	ChecksumSet result() const noexcept final;

	void reset() noexcept final;

	void wipe() noexcept final;

	uint32_t mult() const noexcept final;

	std::unique_ptr<CalcState> clone() const noexcept final;


protected:

	/**
	 * \brief Worker: find Checksum for specified track or 0
	 *
	 * \param[in] track Track number or 0
	 *
	 * \return The Checksum for this track
	 */
	Checksum find(const uint8_t track) const noexcept;

	/**
	 * \brief Worker: compose a ChecksumSet from a single Checksum
	 *
	 * \param[in] checksum The Checksum
	 *
	 * \return The ChecksumSet containing \c checksum
	 */
	ChecksumSet compose(const Checksum &checksum) const noexcept;


private:

	void init(const uint32_t mult) noexcept final;

	void do_update(SampleInputIterator &begin, SampleInputIterator &end) final;

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


CalcStateV1::CalcStateV1()
	: multiplier_ { 1 }
	, subtotal_v1_ { 0 }
	, arcss_ {}
{
	// empty
}


void CalcStateV1::do_update(SampleInputIterator &begin,
		SampleInputIterator &end)
{
	for (auto pos = begin; pos != end; ++pos, ++multiplier_)
	{
		subtotal_v1_ += (multiplier_ * (*pos)) & LOWER_32_BITS_;
	}
}


void CalcStateV1::save(const TrackNo track) noexcept
{
	const auto rc { arcss_.insert(std::make_pair(track, subtotal_v1_)) };

	if (not rc.second)
	{
		ARCS_LOG_WARNING << "Checksum for track "
			<< std::to_string(track) << " was not saved";
	}

	this->reset();
}


int CalcStateV1::track_count() const noexcept
{
	return arcss_.size();
}


checksum::type CalcStateV1::type() const noexcept
{
	return checksum::type::ARCS1;
}


ChecksumSet CalcStateV1::result(const TrackNo track) const noexcept
{
	const auto arcs1_value { this->find(track) };
	return compose(arcs1_value);
}


ChecksumSet CalcStateV1::result() const noexcept
{
	const auto arcs1_value { this->find(0) };
	return compose(arcs1_value);
}


void CalcStateV1::reset() noexcept
{
	multiplier_  = 1;
	subtotal_v1_ = 0;
}


void CalcStateV1::wipe() noexcept
{
	this->reset();
	arcss_.clear();
}


uint32_t CalcStateV1::mult() const noexcept
{
	return multiplier_;
}


std::unique_ptr<CalcState> CalcStateV1::clone() const noexcept
{
	return std::make_unique<CalcStateV1>(*this);
}


void CalcStateV1::init(const uint32_t mult) noexcept
{
	this->wipe();

	multiplier_ = mult;
}


Checksum CalcStateV1::find(const uint8_t track) const noexcept
{
	const auto value { arcss_.find(track) };

	if (value == arcss_.end())
	{
		return Checksum{};
	}

	return Checksum { value->second };
}


ChecksumSet CalcStateV1::compose(const Checksum &checksum) const noexcept
{
	ChecksumSet checksums;

	const auto rc { checksums.insert(checksum::type::ARCS1, checksum) };

	if (not rc.second)
	{
		ARCS_LOG_WARNING << "Could not insert value for type "
			<< checksum::type_name(checksum::type::ARCS1);

		return ChecksumSet{};
	}

	return checksums;
}


/**
 * \internal
 * \ingroup calc
 *
 * \brief CalcState for calculation of ARCSv2 and ARCSv1.
 */
class CalcStateV1andV2 final : public CalcStateARCSBase
{

public:

	/**
	 * \brief Default constructor
	 */
	CalcStateV1andV2();

	void save(const TrackNo track) noexcept final;

	int track_count() const noexcept final;

	checksum::type type() const noexcept final;

	ChecksumSet result(const TrackNo track) const noexcept final;

	ChecksumSet result() const noexcept final;

	void reset() noexcept final;

	void wipe() noexcept final;

	uint32_t mult() const noexcept final;

	std::unique_ptr<CalcState> clone() const noexcept final;


protected:

	ChecksumSet find(const uint8_t track) const noexcept;


private:

	void init(const uint32_t mult) noexcept final;

	void do_update(SampleInputIterator &begin, SampleInputIterator &end) final;

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


CalcStateV1andV2::CalcStateV1andV2()
	: multiplier_ { 1 }
	, subtotal_v1_ { 0 }
	, subtotal_v2_ { 0 }
	, update64_ { 0 }
	, arcss_ {}
{
	// empty
}


void CalcStateV1andV2::do_update(SampleInputIterator &begin,
		SampleInputIterator &end)
{
	for (auto pos = begin; pos != end; ++pos, ++multiplier_)
	{
		update64_ = multiplier_ * (*pos);
		subtotal_v1_ +=  update64_ & LOWER_32_BITS_;
		subtotal_v2_ += (update64_ >> 32u);
	}
}


void CalcStateV1andV2::save(const TrackNo track) noexcept
{
	const auto rc { arcss_.insert(
		std::make_pair(
			track,
			std::make_pair(
				subtotal_v1_,
				subtotal_v1_ + subtotal_v2_
			)
		)
	) } ;

	if (not rc.second)
	{
		ARCS_LOG_WARNING << "Checksum for track "
			<< std::to_string(track) << " was not saved";
	}

	this->reset();
}


int CalcStateV1andV2::track_count() const noexcept
{
	return arcss_.size();
}


checksum::type CalcStateV1andV2::type() const noexcept
{
	return checksum::type::ARCS2;
}


ChecksumSet CalcStateV1andV2::result(const TrackNo track) const noexcept
{
	return this->find(track);
}


ChecksumSet CalcStateV1andV2::result() const noexcept
{
	return this->find(0);
}


void CalcStateV1andV2::reset() noexcept
{
	multiplier_  = 1;
	subtotal_v1_ = 0;
	subtotal_v2_ = 0;

	// For completeness. Value does not affect updating
	update64_ = 0;
}


void CalcStateV1andV2::wipe() noexcept
{
	this->reset();
	arcss_.clear();
}


uint32_t CalcStateV1andV2::mult() const noexcept
{
	return multiplier_;
}


std::unique_ptr<CalcState> CalcStateV1andV2::clone() const noexcept
{
	return std::make_unique<CalcStateV1andV2>(*this);
}


void CalcStateV1andV2::init(const uint32_t mult) noexcept
{
	this->wipe();

	multiplier_ = mult;
}


ChecksumSet CalcStateV1andV2::find(const uint8_t track) const noexcept
{
	const auto value { arcss_.find(track) };

	if (value == arcss_.end())
	{
		return ChecksumSet{};
	}

	ChecksumSet sums;

	const auto rc_v2 {
		sums.insert(checksum::type::ARCS2, Checksum { value->second.second })
	};

	if (not rc_v2.second)
	{
		ARCS_LOG_WARNING << "Insertion to result failed for type "
			<< checksum::type_name(checksum::type::ARCS2);
	}

	const auto rc_v1 {
		sums.insert(checksum::type::ARCS1, Checksum { value->second.first })
	};

	if (not rc_v1.second)
	{
		ARCS_LOG_WARNING << "Insertion to result failed for type "
			<< checksum::type_name(checksum::type::ARCS1);
	}

	return sums;
}

} // namespace details


// Checksums::Impl

using ChecksumsImplBase = details::AppendableSequence<ChecksumSet>;

/**
 * \brief Private implementation of Checksums.
 *
 * \see Checksums
 */
class Checksums::Impl final : public ChecksumsImplBase
{

public:

	using ChecksumsImplBase::ChecksumsImplBase;


private:

	std::unique_ptr<ChecksumsImplBase> do_create(
			const ChecksumsImplBase::size_type size) const override;
};


std::unique_ptr<ChecksumsImplBase> Checksums::Impl::do_create(
		const ChecksumsImplBase::size_type size) const
{
	return std::make_unique<Checksums::Impl>(size);
}


// Checksums


Checksums::Checksums(size_type size)
	: impl_ { std::make_unique<Checksums::Impl>(size) }
{
	// empty
}


Checksums::Checksums(std::unique_ptr<Checksums::Impl> impl)
	: impl_ { std::move(impl) }
{
	// empty
}


Checksums::Checksums(std::initializer_list<ChecksumSet> tracks)
	: impl_ { std::make_unique<Checksums::Impl>(tracks) }
{
	// empty
}


Checksums::Checksums(const Checksums &rhs)
	: impl_ { std::make_unique<Checksums::Impl>(*rhs.impl_) }
{
	// empty
}


Checksums::Checksums(Checksums &&rhs) noexcept = default;


Checksums::~Checksums() noexcept = default;


void Checksums::append(const ChecksumSet &checksum)
{
	impl_->append(checksum);
}
//Checksums::append(ChecksumSet &&rhs) noexcept


Checksums::const_iterator Checksums::begin() const noexcept
{
	return impl_->cbegin();
}


Checksums::const_iterator Checksums::end() const noexcept
{
	return impl_->cend();
}


Checksums::const_iterator Checksums::cbegin() const noexcept
{
	return impl_->cbegin();
}


Checksums::const_iterator Checksums::cend() const noexcept
{
	return impl_->cend();
}


const ChecksumSet& Checksums::at(const Checksums::size_type index) const
{
	return impl_->at(index);
}


const ChecksumSet& Checksums::operator [] (const Checksums::size_type index)
	const
{
	return impl_->operator[](index);
}


Checksums::size_type Checksums::size() const noexcept
{
	return impl_->size();
}


Checksums& Checksums::operator = (Checksums rhs)
{
	using std::swap;

	swap(*this, rhs); // unqualified call, finds friend swap() via ADL
	return *this;
}


Checksums& Checksums::operator = (Checksums &&rhs) noexcept = default;


void swap(Checksums &lhs, Checksums &rhs) noexcept
{
	lhs.impl_->swap(*rhs.impl_);
}


bool operator == (const Checksums &lhs, const Checksums &rhs) noexcept
{
	return lhs.impl_->equals(*rhs.impl_);
}


/**
 * \internal
 * \ingroup calc
 *
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
	void set_context(std::unique_ptr<CalcContext> context) noexcept;

	/**
	 * \brief Implements Calculation::context().
	 */
	const CalcContext& context() const noexcept;

	/**
	 * \brief Implements Calculation::type()
	 */
	checksum::type type() const noexcept;

	/**
	 * \brief Implements Calculation::update()
	 */
	void update(SampleInputIterator &begin, SampleInputIterator &end);

	/**
	 * \brief Implements Calculation::update_audiosize(const AudioSize &audiosize).
	 */
	void update_audiosize(const AudioSize &audiosize) noexcept;

	/**
	 * \brief Implements Calculation::complete().
	 */
	bool complete() const noexcept;

	/**
	 * \brief Implements Calculation::samples_expected().
	 */
	int64_t samples_expected() const noexcept;

	/**
	 * \brief Implements Calculation::samples_processed().
	 */
	int64_t samples_processed() const noexcept;

	/**
	 * \brief Implements Calculation::samples_todo().
	 */
	int64_t samples_todo() const noexcept;

	/**
	 * \brief Implements Calculation::result().
	 */
	Checksums result() const noexcept;

	/**
	 * \brief Set the Partitioner for this instance.
	 *
	 * \param[in] partitioner The Partitioner for this instance
	 */
	void set_partitioner(std::unique_ptr<Partitioner> partitioner) noexcept;

	/**
	 * \brief Read the Partitioner of this instance.
	 *
	 * \return The Partitioner of this instance
	 */
	const Partitioner& partitioner() const noexcept;

	/**
	 * \brief Read the state of this instance.
	 *
	 * \return the CalcState of this instance to read
	 */
	const CalcState& state() const noexcept;

	/**
	 * \brief Implements Calculation::set_type()
	 */
	void set_type(const checksum::type type);


	Impl& operator = (const Impl &rhs);

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
	void set_context_or_default(std::unique_ptr<CalcContext> ctx) noexcept;

	/**
	 * \brief Log statistics about a Partition.
	 *
	 * \param[in] i     Chunk counter
	 * \param[in] n     Number of chunks in block
	 * \param[in] chunk Chunk to log
	 */
	void log_partition(const uint16_t i, const uint16_t n,
			const Partition &chunk) const noexcept;


private:

	/**
	 * \brief State: 1-based global index of the sample to be processed as next.
	 */
	sample_count smpl_offset_;

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


Calculation::Impl::Impl(const checksum::type type,
		std::unique_ptr<CalcContext> ctx)
	: smpl_offset_ { 0 }
	, context_ { nullptr }
	, state_ { nullptr }
	, partitioner_ { nullptr }
	, proc_time_elapsed_ { std::chrono::milliseconds::zero() }
{
	this->set_type(type);
	this->set_context_or_default(std::move(ctx));
}


Calculation::Impl::Impl(std::unique_ptr<CalcContext> ctx)
	: smpl_offset_ { 0 }
	, context_ { nullptr }
	, state_ { std::make_unique<CalcStateV1andV2>() } // default
	, partitioner_ { nullptr }
	, proc_time_elapsed_ { std::chrono::milliseconds::zero() }
{
	this->set_context_or_default(std::move(ctx));
}


Calculation::Impl::Impl(const Impl& rhs)
	: smpl_offset_ { rhs.smpl_offset_ }
	, context_ { rhs.context_->clone() }
	, state_ { rhs.state_->clone() }
	, partitioner_ { rhs.partitioner_->clone() }
	, proc_time_elapsed_ { rhs.proc_time_elapsed_ }
{
	// empty
}


void Calculation::Impl::set_context(std::unique_ptr<CalcContext> context)
noexcept
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


const CalcContext& Calculation::Impl::context() const noexcept
{
	return *context_;
}


void Calculation::Impl::set_type(const checksum::type type)
{
	try
	{
		state_ = details::state::make(type);

	} catch (const std::exception& e)
	{
		ARCS_LOG_ERROR << e.what();

		auto msg = std::stringstream {};
		msg << "Could not load CalcState for requested type "
			<< checksum::type_name(type) << ": ";
		msg << e.what();

		throw std::invalid_argument(msg.str());
	}

	this->sync_state_and_context();
}


checksum::type Calculation::Impl::type() const noexcept
{
	return state_->type();
}


void Calculation::Impl::update_audiosize(const AudioSize &audiosize) noexcept
{
	context_->set_audio_size(audiosize);
}


bool Calculation::Impl::complete() const noexcept
{
	return (this->samples_expected() - this->samples_processed()) < 1;

	// Calculation is not complete only while there are less samples processed
	// than expected.

	// smpl_offset_ will "stop" on the correct value only iff
	// the size of the last input block processed (which may have a smaller size
	// than its predecessors) was accurately set before passing the block to
	// update().
	// One could do:
	// return this->smpl_offset_ == context().audio_size().total_samples();
	// but this would mean that the calculation is not complete when more
	// samples were processed than expected. (Nonetheless, this may or may not
	// indicate an actual error.) But in a sense, the calculation is completed
	// as soon as the number of estimated samples is processed.
}


void Calculation::Impl::update(SampleInputIterator &begin,
		SampleInputIterator &end)
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

	auto partition_counter = uint16_t { 0 };
	auto relevant_samples_counter = sample_count { 0 };

	const auto start_time { std::chrono::steady_clock::now() };
	for (const auto& partition : partitioning)
	{
		++partition_counter;
		relevant_samples_counter += partition.size();

		this->log_partition(partition_counter, partitioning.size(), partition);

		// Update the calculation state with the current partition/chunk

		auto part_begin = SampleInputIterator { begin + partition.begin_offset() };
		auto part_end = SampleInputIterator { begin + partition.end_offset()   };
		// FIXME Do not allocate in loop

		state_->update(part_begin, part_end);

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
		ARCS_LOG(DEBUG1) << "Total samples counted:  " <<
			this->samples_processed();
		ARCS_LOG(DEBUG1) << "Total samples declared: " <<
			this->samples_expected();
		ARCS_LOG(DEBUG1) << "Milliseconds elapsed by calculating ARCSs: "
			<< proc_time_elapsed_.count();
	}
}


int64_t Calculation::Impl::samples_expected() const noexcept
{
	return context().audio_size().total_samples();
}


int64_t Calculation::Impl::samples_processed() const noexcept
{
	return smpl_offset_;
}


int64_t Calculation::Impl::samples_todo() const noexcept
{
	return this->samples_expected() - this->samples_processed();
}


Checksums Calculation::Impl::result() const noexcept
{
	if (not context_ or not state_)
	{
		return Checksums(0);
	}

	auto track_count { state_->track_count()  };

	if (track_count < 0)
	{
		track_count = 0;
		// TODO throw something
	}

	auto checksums { std::make_unique<Checksums::Impl>(
			static_cast<Checksums::size_type>(track_count)) };

	// FIXME Does not make sense, a loop from 0 to track_count should be correct
	// for both cases

	if (context_->is_multi_track())
	{
		// multitrack

		for (auto i = uint8_t { 0 }; i < track_count; ++i)
		{
			auto track = ChecksumSet { context_->length(i) };

			track.merge(state_->result(i + 1));

			checksums->append(track);
		}
	} else
	{
		// singletrack

		auto track = ChecksumSet { context_->audio_size().leadout_frame() };

		track.merge(state_->result()); // alias for result(0)

		checksums->append(track);
	}

	return Checksums(std::move(checksums));
}


void Calculation::Impl::set_partitioner(
			std::unique_ptr<Partitioner> partitioner) noexcept
{
	partitioner_ = std::move(partitioner);
}


const Partitioner& Calculation::Impl::partitioner() const noexcept
{
	return *partitioner_;
}


const CalcState& Calculation::Impl::state() const noexcept
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
noexcept
{
	this->set_context(std::move(ctx));

	if (not context_)
	{
		this->set_context(
			std::make_unique<details::SingletrackCalcContext>(
				details::EmptyString));
	}
}


void Calculation::Impl::log_partition(const uint16_t i,
		const uint16_t n, const Partition &chunk) const noexcept
{
	ARCS_LOG_DEBUG << "  CHUNK " << i << "/" << n;

	const auto chunk_first_smpl_idx = sample_count { chunk.first_sample_idx() };
	const auto chunk_last_smpl_idx = sample_count { chunk.last_sample_idx()  };

	const sample_count samples_in_chunk {
		chunk_last_smpl_idx - chunk_first_smpl_idx + 1
		// chunk_first_smpl_idx counts as relevant therefore + 1
	};

	const auto chunk_starts_track = bool { chunk.starts_track() };

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


AudioSize::AudioSize() noexcept
	: impl_ { std::make_unique<AudioSize::Impl>() }
{
	//empty
}


AudioSize::AudioSize(const long int value, const UNIT unit) noexcept
	: impl_ { std::make_unique<AudioSize::Impl>(value, unit) }
{
	// empty
}


AudioSize::AudioSize(const AudioSize &rhs)
	: impl_ { std::make_unique<AudioSize::Impl>(*rhs.impl_) }
{
	//empty
}


AudioSize::AudioSize(AudioSize &&rhs) noexcept = default;


AudioSize::~AudioSize() noexcept = default;


void AudioSize::set_leadout_frame(const lba_count leadout) noexcept
{
	impl_->set_total_frames(leadout);
}


lba_count AudioSize::leadout_frame() const noexcept
{
	return impl_->total_frames();
}


void AudioSize::set_total_samples(const sample_count smpl_count) noexcept
{
	impl_->set_total_samples(smpl_count);
}


sample_count AudioSize::total_samples() const noexcept
{
	return impl_->total_samples();
}


void AudioSize::set_pcm_byte_count(const uint32_t byte_count) noexcept
{
	impl_->set_total_pcm_bytes(byte_count);
}


uint32_t AudioSize::pcm_byte_count() const noexcept
{
	return impl_->total_pcm_bytes();
}


bool AudioSize::null() const noexcept
{
	return 0 == impl_->total_pcm_bytes();
}


AudioSize& AudioSize::operator = (AudioSize rhs)
{
	std::swap(*impl_, *rhs.impl_);
	return *this;
}


AudioSize& AudioSize::operator = (AudioSize &&rhs) noexcept = default;


// operators for AudioSize


bool operator == (const AudioSize &lhs, const AudioSize &rhs) noexcept
{
	return lhs.impl_->equals(*rhs.impl_);
}


bool operator < (const AudioSize &lhs, const AudioSize &rhs) noexcept
{
	return lhs.pcm_byte_count() < rhs.pcm_byte_count();
}


// CalcContext


CalcContext::~CalcContext() noexcept = default;


// Calculation


Calculation::Calculation(const checksum::type type,
		std::unique_ptr<CalcContext> ctx)
	: impl_ { std::make_unique<Calculation::Impl>(type, std::move(ctx)) }
{
	// empty
}


Calculation::Calculation(std::unique_ptr<CalcContext> ctx)
	: impl_ { std::make_unique<Calculation::Impl>(std::move(ctx)) }
{
	// empty
}


Calculation::Calculation(const Calculation &rhs)
	: impl_ { std::make_unique<Calculation::Impl>(*rhs.impl_) }
{
	// empty
}


Calculation::Calculation(Calculation &&rhs) noexcept = default;


Calculation::~Calculation() noexcept = default;


void Calculation::set_context(std::unique_ptr<CalcContext> context) noexcept
{
	impl_->set_context(std::move(context));
}


const CalcContext& Calculation::context() const noexcept
{
	return impl_->context();
}


void Calculation::set_type(const checksum::type type)
{
	impl_->set_type(type);
}


checksum::type Calculation::type() const noexcept
{
	return impl_->type();
}


void Calculation::update(SampleInputIterator begin, SampleInputIterator end)
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


void Calculation::update_audiosize(const AudioSize &audiosize) noexcept
{
	impl_->update_audiosize(audiosize);
}


bool Calculation::complete() const noexcept
{
	return impl_->complete();
}


int64_t Calculation::samples_expected() const noexcept
{
	return impl_->samples_expected();
}


int64_t Calculation::samples_processed() const noexcept
{
	return impl_->samples_processed();
}


int64_t Calculation::samples_todo() const noexcept
{
	return impl_->samples_todo();
}


Checksums Calculation::result() const noexcept
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




// InvalidAudioException


InvalidAudioException::InvalidAudioException(const std::string &what_arg)
	: std::logic_error { what_arg }
{
	// empty
}


InvalidAudioException::InvalidAudioException(const char *what_arg)
	: std::logic_error { what_arg }
{
	// empty
}


// make_context (bool, bool, audiofile)


std::unique_ptr<CalcContext> make_context(const bool &skip_front,
		const bool &skip_back)
{
	return make_context(skip_front, skip_back, details::EmptyString);
}


std::unique_ptr<CalcContext> make_context(const bool &skip_front,
		const bool &skip_back,
		const std::string &audiofilename)
{
	// Note: ARCS specific values, since ARCS2 is default checksum type
	return std::make_unique<details::SingletrackCalcContext>(audiofilename,
			skip_front, NUM_SKIP_SAMPLES_FRONT,
			skip_back,  NUM_SKIP_SAMPLES_BACK);
}


// make_context (TOC, audiofile)


std::unique_ptr<CalcContext> make_context(const TOC &toc)
{
	return make_context(toc, details::EmptyString);
}


std::unique_ptr<CalcContext> make_context(const TOC &toc,
		const std::string &audiofilename)
{
	// Note: ARCS specific values, since ARCS2 is default checksum type
	return std::make_unique<details::MultitrackCalcContext>(toc,
			NUM_SKIP_SAMPLES_FRONT,
			NUM_SKIP_SAMPLES_BACK,
			audiofilename);
}


std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC> &toc)
{
	return make_context(toc, details::EmptyString);
}


std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC> &toc,
		const std::string &audiofilename)
{
	return make_context(*toc, audiofilename);
}


// Checksum


Checksum::Checksum()
	: value_ { 0 }
{
	// empty
}


Checksum::Checksum(const uint32_t value)
	: value_ { value }
{
	// empty
}


uint32_t Checksum::value() const noexcept
{
	return value_;
}


bool Checksum::empty() const noexcept
{
	return 0 == value_;
}


Checksum& Checksum::operator = (const uint32_t rhs)
{
	value_ = rhs;
	return *this;
}


// Checksum operators


bool operator == (const Checksum &lhs, const Checksum &rhs) noexcept
{
	return (lhs.empty() and rhs.empty()) or lhs.value() == rhs.value();
}


/// \internal \addtogroup calcImpl
/// @{


// ChecksumSet::Impl


/**
 * \brief Private implementation of ChecksumSet.
 *
 * \see ChecksumSet
 * \see details::ChecksumMap
 */
class ChecksumSet::Impl final
{
public:

	/**
	 * \brief Constructor
	 *
	 * \param[in] length Length (in LBA frames) of this track
	 */
	explicit Impl(const lba_count length);

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	Impl(const Impl &rhs);

	/**
	 * \brief Implements ChecksumSet::merge(const ChecksumSet &)
	 */
	void merge(const Impl &rhs);

	/**
	 * \brief Equality
	 *
	 * \param[in] rhs The instance to compare
	 */
	bool equals(const Impl &rhs) const;

	/**
	 * \brief Length (in LBA frames) of this track.
	 *
	 * \return Length of this track in LBA frames
	 */
	lba_count length() const noexcept;

	/**
	 * \brief Return the internal map
	 *
	 * \return The internal map
	 */
	details::ChecksumMap<checksum::type>& map();


	Impl& operator = (const Impl &rhs);


private:

	/**
	 * \brief Internal representation of the length (in frames).
	 */
	lba_count length_;

	/**
	 * \brief Internal representation of the checksums.
	 */
	details::ChecksumMap<checksum::type> checksum_map_;
};


/** @} */


ChecksumSet::Impl::Impl(const lba_count length)
	: length_ { length }
	, checksum_map_ {}
{
	// empty
}


ChecksumSet::Impl::Impl(const ChecksumSet::Impl &rhs)
	: length_ { rhs.length_ }
	, checksum_map_ { rhs.checksum_map_ }
{
	// empty
}


bool ChecksumSet::Impl::equals(const Impl &rhs) const
{
	return length_ == rhs.length_ and checksum_map_ == rhs.checksum_map_;
}


void ChecksumSet::Impl::merge(const Impl &rhs)
{
	if (this->length() != 0 and rhs.length() != 0)
	{
		// A set with no length may be merged without constraint
		// but a non-zero length of different value indicates merge
		// of different tracks.

		if (this->length() != rhs.length())
		{
			throw std::domain_error(
					"Refuse to merge checksums of different track");
		}
	}

	checksum_map_.merge(rhs.checksum_map_);
}


lba_count ChecksumSet::Impl::length() const noexcept
{
	return length_;
}


details::ChecksumMap<checksum::type>& ChecksumSet::Impl::map()
{
	return checksum_map_;
}


ChecksumSet::Impl& ChecksumSet::Impl::operator = (const ChecksumSet::Impl& rhs)
= default;


bool operator == (const ChecksumSet &lhs, const ChecksumSet &rhs)
{
	return lhs.impl_->equals(*rhs.impl_);
}


// ChecksumSet


ChecksumSet::ChecksumSet()
	: impl_ { std::make_unique<ChecksumSet::Impl>(0) }
{
	// empty
}


ChecksumSet::ChecksumSet(const lba_count length)
	: impl_ { std::make_unique<ChecksumSet::Impl>(length) }
{
	// empty
}


ChecksumSet::ChecksumSet(const ChecksumSet &rhs)
	: impl_ { std::make_unique<ChecksumSet::Impl>(*rhs.impl_) }
{
	// empty
}


ChecksumSet::ChecksumSet(ChecksumSet &&rhs) noexcept = default;


ChecksumSet::~ChecksumSet() noexcept = default;


ChecksumSet::size_type ChecksumSet::size() const
{
	return impl_->map().size();
}


bool ChecksumSet::empty() const
{
	return impl_->map().empty();
}


ChecksumSet::const_iterator ChecksumSet::cbegin() const
{
	return impl_->map().cbegin();
}


ChecksumSet::const_iterator ChecksumSet::cend() const
{
	return impl_->map().cend();
}


ChecksumSet::const_iterator ChecksumSet::begin() const
{
	return impl_->map().cbegin();
}


ChecksumSet::const_iterator ChecksumSet::end() const
{
	return impl_->map().cend();
}


ChecksumSet::iterator ChecksumSet::begin()
{
	return impl_->map().begin();
}


ChecksumSet::iterator ChecksumSet::end()
{
	return impl_->map().end();
}


lba_count ChecksumSet::length() const noexcept
{
	return impl_->length();
}


bool ChecksumSet::contains(const checksum::type &type) const
{
	return impl_->map().contains(type);
}


void ChecksumSet::erase(const checksum::type &type)
{
	return impl_->map().erase(type);
}


void ChecksumSet::clear()
{
	return impl_->map().clear();
}


Checksum ChecksumSet::get(const checksum::type type) const
{
	auto rc { impl_->map().find(type) };

	if (rc == impl_->map().end())
	{
		return Checksum(0);
	}

	return *rc;
}


std::set<checksum::type> ChecksumSet::types() const
{
	return impl_->map().keys();
}


std::pair<ChecksumSet::iterator, bool> ChecksumSet::insert(
		const checksum::type type, const Checksum &checksum)
{
	return impl_->map().insert(type, checksum);
}


void ChecksumSet::merge(const ChecksumSet &rhs)
{
	impl_->merge(*rhs.impl_);
}


ChecksumSet& ChecksumSet::operator = (const ChecksumSet &rhs)
{
	impl_ = std::make_unique<ChecksumSet::Impl>(*rhs.impl_);
	return *this;
}


namespace checksum
{

/**
 * \internal
 *
 * \brief Implementation details of namespace checksum
 */
namespace details
{

/**
 * \internal
 *
 * \brief Checksum type names.
 *
 * The order of names in this aggregate must match the order of types in
 * enum class checksum::type, otherwise function type_name() will fail.
 */
static const std::array<std::string, 2> names {
	"ARCSv1",
	"ARCSv2",
	// "THIRD_TYPE" ,
	// "FOURTH_TYPE" ...
};

/**
 * \internal
 *
 * \brief Creates a hexadecimal string representation of a 32bit checksum.
 *
 * \param[in] checksum The Checksum to represent
 * \param[in] upper    TRUE indicates to print digits A-F in uppercase
 * \param[in] base     TRUE indicates to print base '0x'
 *
 * \return A hexadecimal representation of the \c checksum as a string
 */
std::string to_hex_str(const Checksum &checksum, const bool upper,
		const bool base);

/**
 * \internal
 *
 * \brief Return the numeric value of a >=C++11 enum class value
 *
 * \return The numeric constant of an enum class value
 */
template <typename E>
auto as_integral_value(E const value)
		-> typename std::underlying_type<E>::type
{
	return static_cast<typename std::underlying_type<E>::type>(value);
}


std::string to_hex_str(const Checksum &checksum, const bool upper,
		const bool base)
{
	auto ss = std::stringstream {};
	ss << std::hex
		<< (base  ? std::showbase  : std::noshowbase  )
		<< (upper ? std::uppercase : std::nouppercase )
		<< std::setw(8) << std::setfill('0')
		<< checksum.value();
	return ss.str();
}

} // namespace checksum::details


std::string type_name(const type t)
{
	using details::names;

	return names.at(std::log2(details::as_integral_value(t)));
}


} // namespace checksum

} // namespace v_1_0_0

} // namespace arcstk

