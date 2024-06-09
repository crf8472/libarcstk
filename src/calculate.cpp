/**
 * \file
 *
 * \brief Implementation of the new checksum calculation API
 */

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"    // for ARId, CDDA, lba_count_t
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_IMPL_HPP__
#include "calculate_impl.hpp"
#endif
#ifndef __LIBARCSTK_ACCURATERIP_HPP__
#include "accuraterip.hpp"
#endif

#include <cstdint>     // for int32_t
#include <string>      // for to_string

namespace arcstk
{
inline namespace v_1_0_0
{


namespace details
{


// calculate_details.hpp


int32_t frames2samples(const int32_t frames)
{
	return frames * CDDA::SAMPLES_PER_FRAME;
}

int32_t samples2frames(const int32_t samples)
{
	return samples / CDDA::SAMPLES_PER_FRAME;
}

int32_t frames2bytes(const int32_t frames)
{
	return frames * CDDA::BYTES_PER_FRAME;
}

int32_t bytes2frames(const int32_t bytes)
{
	return bytes / CDDA::BYTES_PER_FRAME;
}

int32_t samples2bytes(const int32_t samples)
{
	return samples * CDDA::BYTES_PER_SAMPLE;
}

int32_t bytes2samples(const int32_t bytes)
{
	return bytes / CDDA::BYTES_PER_SAMPLE;
}

bool is_valid_track_number(const TrackNo track)
{
	return 0 < track && track <= 99;
}

bool is_valid_track(const TrackNo track, const TOC& toc)
{
	return 0 < track && track <= toc.total_tracks();
}

TrackNo track(const int32_t sample, const TOC& toc, const int32_t s_total)
{
	if (sample > s_total || sample > toc.leadout())
	{
		return 0;
	}

	auto t = TrackNo { 1 };
	for (auto o = toc.offset(t); sample < o; o = toc.offset(++t)) {/*empty*/}

	return t;
}


//


int32_t first_relevant_sample(const TrackNo track, const TOC& toc,
		const Interval<int32_t> bounds)
{
	int32_t frames = 0;

	try {

		frames = toc.offset(track);

	} catch (const std::exception& e) // TODO throw?
	{
		ARCS_LOG_WARNING << "Offset for unknown track " << track
			<< " requested, returned 0.";

		return 0;
	}

	if (1 == track)
	{
		return frames2samples(frames) + bounds.lower();
	}

	return frames2samples(frames);
}


int32_t last_relevant_sample(const TrackNo track,
		const TOC& toc, const Interval<int32_t> bounds)
{
	if (!toc.complete())
	{
		// TODO throw?
		return 0;
	}

	// TODO Validity check for TrackNo and TrackNo + 1
	if (!is_valid_track(track, toc))
	{
		// TODO throw?
		return 0;
	}

	if (track >= toc.total_tracks())
	{
		return last_relevant_sample(bounds, toc.leadout());
	}

	const TrackNo next_track = track + 1;
	int32_t frames = 0;

	try {

		frames = toc.offset(next_track);

	} catch (const std::exception& e) // TODO throw?
	{
		ARCS_LOG_WARNING << "Offset for unknown track " << next_track
			<< " requested, returned 0.";

		return 0;
	}

	return !frames ? 0 : last_relevant_sample(bounds, frames);
}


int32_t last_relevant_sample(const Interval<int32_t> bounds,
		const int32_t total_frames)
{
	// return this->skips_back()
	// 	? this->audio_size().total_samples() - 1 - this->num_skip_back()
	// 	: this->audio_size().total_samples() - 1;

	const auto total_samples { frames2samples(total_frames) };

	if (bounds.upper() >= total_samples)
	{
		return total_samples;
	}

	if (bounds.upper() == 0) /* no skip */
	{
		return total_samples - 1;
	}

	return total_samples - (total_samples - bounds.upper()) - 1;
}


// Partitioner


Partitioning Partitioner::create_partitioning(
		const int32_t offset,
		const int32_t total_samples_in_block,
		const Interval<int32_t>& total_bounds,
		const TOC* toc) const
{
	const Interval<int32_t> sample_block {
		/* first sample in block */ offset,
		/* last sample in block */  offset + total_samples_in_block - 1
	};

	// If the sample block does not contain any relevant samples,
	// just return an empty partitioning.

	if (sample_block.upper() < total_bounds.lower()
			|| sample_block.lower() > total_bounds.upper())
	{
		ARCS_LOG(DEBUG1) << "  No relevant samples in this block, skip";

		return Partitioning{};
	}

	if (!toc)
	{
		// No TOC? => Fallback to a single track partitioning
		return do_create_partitioning(sample_block, total_bounds);
	}

	return do_create_partitioning(sample_block, total_bounds, *toc);
}


std::unique_ptr<Partitioner> Partitioner::clone() const
{
	return do_clone();
}


Partition Partitioner::create_partition(
		const int32_t &begin_offset,
		const int32_t &end_offset,
		const int32_t &first,
		const int32_t &last,
		const bool    &starts_track,
		const bool    &ends_track,
		const TrackNo &track) const
{
	return Partition{begin_offset, end_offset, first, last, starts_track,
			ends_track, track};
}


// TrackPartitioner


Partitioning TrackPartitioner::do_create_partitioning(
			const Interval<int32_t>& sample_block,
			const Interval<int32_t>& total_bounds,
			const TOC& toc) const
{
	auto chunk_first_smpl = sample_block.contains(total_bounds.lower())
		? total_bounds.lower()
		: sample_block.lower();

	// If the sample index range of this block contains the last relevant
	// sample, set this as the last sample in block instead of the last
	// physical sample

	const auto block_last_smpl = sample_block.contains(total_bounds.lower())
		? total_bounds.lower()
		: sample_block.lower();

	// Will be total_tracks+1 if 1st sample is beyond global last relevant sample
	// This entails that the loop is not entered for irrelevant partitions
	auto curr_track = track(chunk_first_smpl, toc, total_bounds.upper());

	// If track > total_tracks this is global last sample
	auto chunk_last_smpl = int32_t {
		//context.last_relevant_sample(curr_track) };
		last_relevant_sample(curr_track, toc, total_bounds) };


	auto begin_offset = int32_t { 0 } ;
	auto end_offset   = int32_t { 0 } ;
	auto starts_track = bool { false } ;
	auto ends_track   = bool { false } ;

	const auto last_track = toc.total_tracks();


	// Now construct all partitions except the last (that needs clipping) in a
	// loop

	Partitioning chunks{};
	chunks.reserve(10);

	while (chunk_last_smpl < block_last_smpl and curr_track <= last_track)
	{
		//ends_track   = (chunk_last_smpl == context.last_relevant_sample(track));
		ends_track   =
			(chunk_last_smpl == last_relevant_sample(curr_track, toc, total_bounds));

		starts_track =
			//(chunk_first_smpl == context.first_relevant_sample(track));
			(chunk_first_smpl == first_relevant_sample(curr_track, toc, total_bounds));

		begin_offset = chunk_first_smpl - sample_block.lower();

		end_offset   = chunk_last_smpl  - sample_block.lower() + 1;

		chunks.emplace_back(
			this->create_partition(
				begin_offset,
				end_offset,
				chunk_first_smpl,
				chunk_last_smpl,
				starts_track,
				ends_track,
				curr_track
			)
		);

		ARCS_LOG(DEBUG1) << "  Create chunk: " << chunk_first_smpl
				<< " - " << chunk_last_smpl;

		++curr_track;

		chunk_first_smpl = chunk_last_smpl + 1;
		chunk_last_smpl  = last_relevant_sample(curr_track, toc, total_bounds);
	} // while


	// If the loop has finished or was never entered, the last partition has to
	// be prepared


	// Clip last partition to block end if necessary

	if (chunk_last_smpl > block_last_smpl)
	{
		chunk_last_smpl = block_last_smpl;

		using std::to_string;
		ARCS_LOG(DEBUG1) << "  Block ends within track "
			<< to_string(curr_track)
			<< ", clip last sample to: " << chunk_last_smpl;
	}

	// Prepare last partition

	starts_track =
		(chunk_first_smpl == first_relevant_sample(curr_track, toc, total_bounds));

	ends_track   =
		(chunk_last_smpl == last_relevant_sample(curr_track, toc, total_bounds));

	begin_offset = chunk_first_smpl - sample_block.lower();

	end_offset   = chunk_last_smpl  - sample_block.lower() + 1;

	ARCS_LOG(DEBUG1) << "  Create last chunk: " << chunk_first_smpl
				<< " - " << chunk_last_smpl;

	chunks.emplace_back(
		this->create_partition(
			begin_offset,
			end_offset,
			chunk_first_smpl,
			chunk_last_smpl,
			starts_track,
			ends_track,
			curr_track
		)
	);

	chunks.shrink_to_fit();

	return chunks;
}


Partitioning TrackPartitioner::do_create_partitioning(
			const Interval<int32_t>& sample_block,
			const Interval<int32_t>& bounds) const
{
	// Create a single partition spanning the entire block of samples,
	// but respect skipping samples at front or back.

	const auto chunk_first_smpl = sample_block.contains(bounds.lower())
		? bounds.lower()
		: sample_block.lower();

	const auto chunk_last_smpl = sample_block.contains(bounds.upper())
		? bounds.upper()
		: sample_block.upper();

	return {
		this->create_partition(
			/* begin offset */  { chunk_first_smpl - sample_block.lower() },
			/* end offset */    { chunk_last_smpl  - sample_block.lower() + 1 },
			/* chunk first */   chunk_first_smpl,
			/* chunk last */    chunk_last_smpl,
			/* starts track */  { chunk_first_smpl == bounds.lower() },
			/* ends track */    { chunk_last_smpl  == bounds.upper() },
			/* invalid track */ 0
		)
	};
}


std::unique_ptr<Partitioner> TrackPartitioner::do_clone() const
{
	return std::make_unique<TrackPartitioner>(*this);
}


// Partition


Partition::Partition(
		const int32_t &begin_offset,
		const int32_t &end_offset,
		const int32_t &first,
		const int32_t &last,
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


int32_t Partition::begin_offset() const
{
	return begin_offset_;
}


int32_t Partition::end_offset() const
{
	return end_offset_;
}


int32_t Partition::first_sample_idx() const
{
	return first_sample_idx_;
}


int32_t Partition::last_sample_idx() const
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


int Partition::track() const
{
	return track_;
}


std::size_t Partition::size() const
{
	return static_cast<std::size_t>(last_sample_idx() - first_sample_idx() + 1);
}


// CalculationState


int32_t CalculationState::sample_offset() const
{
	return sample_offset_.value();
}

void CalculationState::increment_sample_offset(const int32_t amount)
{
	sample_offset_.increment(amount);
}

std::chrono::milliseconds CalculationState::proc_time_elapsed() const
{
	return proc_time_elapsed_.value();
}

void CalculationState::increment_proc_time_elapsed(
		const std::chrono::milliseconds amount)
{
	proc_time_elapsed_.increment(amount);
}

arcstk::ChecksumSet CalculationState::current_value() const
{
	// TODO Implement
	return arcstk::ChecksumSet{};
	//return internal_state_.value();
}


// calculate_impl.hpp


bool within_bounds(const int32_t value, const AudioSize::UNIT unit)
{
	using std::to_string;

	if (value < 0)
	{
		auto ss = std::stringstream {};
		ss << "Cannot construct AudioSize from negative value: "
			<< to_string(value);
		throw std::underflow_error(ss.str());
	}

	if (value > AudioSize::max(unit))
	{
		auto ss = std::stringstream {};
		ss << "Value too big for maximum: " << to_string(value);
		throw std::overflow_error(ss.str());
	}

	return true;
}


int32_t to_bytes(const int32_t value, const AudioSize::UNIT unit) noexcept
{
	using UNIT = AudioSize::UNIT;

	switch (unit)
	{
		case UNIT::FRAMES:  return frames2bytes(value);
		case UNIT::SAMPLES: return samples2bytes(value);
		default:            return value;
	}

	return value;
}


int32_t from_bytes(const int32_t value, const AudioSize::UNIT unit) noexcept
{
	using UNIT = AudioSize::UNIT;

	switch (unit)
	{
		case UNIT::FRAMES:  return bytes2frames(value);
		case UNIT::SAMPLES: return bytes2samples(value);
		default:            return value;
	}

	return value;
}


// CalcContextImplBase


std::string CalcContextImplBase::do_filename() const noexcept
{
	return filename_;
}


void CalcContextImplBase::do_set_filename(const std::string& filename) noexcept
{
	filename_ = filename;
}


const AudioSize& CalcContextImplBase::do_audio_size() const noexcept
{
	return audiosize_;
}


void CalcContextImplBase::do_set_audio_size(const AudioSize& audio_size)
{
	audiosize_ = audio_size;
	do_hook_post_set_audio_size();
}


int32_t CalcContextImplBase::do_first_relevant_sample(const TrackNo /*t*/) const
	noexcept
{
	return 0;
}


int32_t CalcContextImplBase::do_first_relevant_sample_no_parms() const noexcept
{
	return first_relevant_sample(1);
}


int32_t CalcContextImplBase::do_last_relevant_sample(const TrackNo /*t*/) const
	noexcept
{
	return 0;
}


int32_t CalcContextImplBase::do_last_relevant_sample_no_parms() const noexcept
{
	return last_relevant_sample(this->total_tracks());
}


int32_t CalcContextImplBase::do_num_skip_front() const noexcept
{
	return num_skip_front_;
}


int32_t CalcContextImplBase::do_num_skip_back() const noexcept
{
	return num_skip_back_;
}


void CalcContextImplBase::do_notify_skips(const int32_t num_skip_front,
		const int32_t num_skip_back) noexcept
{
	num_skip_front_ = num_skip_front;
	ARCS_LOG_DEBUG << "Set context front skip: " << num_skip_front_;

	num_skip_back_  = num_skip_back;
	ARCS_LOG_DEBUG << "Set context back skip:  " << num_skip_back_;
}


void CalcContextImplBase::do_hook_post_set_audio_size()
{
	// empty
}


bool CalcContextImplBase::base_equals(const CalcContextImplBase& rhs) const
	noexcept
{
	return filename_       == rhs.filename_
		&& audiosize_      == rhs.audiosize_
		&& num_skip_front_ == rhs.num_skip_front_
		&& num_skip_back_  == rhs.num_skip_back_;
}


void CalcContextImplBase::base_swap(CalcContextImplBase& rhs) noexcept
{
	using std::swap;
	swap(filename_,       rhs.filename_);
	swap(audiosize_,      rhs.audiosize_);
	swap(num_skip_front_, rhs.num_skip_front_);
	swap(num_skip_back_,  rhs.num_skip_back_);
}


CalcContextImplBase::CalcContextImplBase(const std::string& filename,
		const int32_t num_skip_front, const int32_t num_skip_back)
	: audiosize_      { AudioSize{}    }
	, filename_       { filename       }
	, num_skip_front_ { num_skip_front }
	, num_skip_back_  { num_skip_back  }
{
	// empty
}


// SingletrackCalcContext


int32_t SingletrackCalcContext::do_first_relevant_sample(
		const TrackNo /* track */) const noexcept
{
	// It is not necessary to bounds-check the TrackNo since we do not intend
	// to throw. We just return the first relevant sample on every input except.

	// Note especially that 0 is returned iff track == 0 since the first block
	// will always start with the very first 32 bit PCM sample.

	// We have no offsets and the track parameter is irrelevant. Hence iff the
	// request adresses track 1 and skipping applies, the correct constant is
	// provided, otherwise the result is always 0.

	return this->skips_front() /* and track == 1 */
		? this->num_skip_front()
		: 0;
}


int32_t SingletrackCalcContext::do_last_relevant_sample(
		const TrackNo /* track */) const noexcept
{
	// It is not necessary to bounds-check the TrackNo since we do not intend
	// to throw. We just return the physical last sample on every input except
	// for value 1.

	// We have no offsets and the track parameter is irrelevant. Hence iff the
	// request adresses the last track and skipping applies, the correct
	// constant is provided, otherwise the result is always the last known
	// sample.

	return this->skips_back() /* and track == this->total_tracks() */ /* == 1 */
		? this->audio_size().total_samples() - 1 - this->num_skip_back()
		: this->audio_size().total_samples() - 1;
}


bool SingletrackCalcContext::do_skips_front() const noexcept
{
	return skip_front_;
}


bool SingletrackCalcContext::do_skips_back() const noexcept
{
	return skip_back_;
}


bool SingletrackCalcContext::do_is_multi_track() const noexcept
{
	return false;
}


int SingletrackCalcContext::do_total_tracks() const noexcept
{
	return 1;
}


int SingletrackCalcContext::do_track(const int32_t /* smpl */) const
noexcept
{
	return 1;
}


lba_count_t SingletrackCalcContext::do_offset(const int /* track */) const
noexcept
{
	return 0;
}


lba_count_t SingletrackCalcContext::do_length(const int /* track */) const
noexcept
{
	return 0;
}


ARId SingletrackCalcContext::do_id() const
{
	return *(make_empty_arid());
}


std::unique_ptr<CalcContext> SingletrackCalcContext::do_clone() const noexcept
{
	return std::make_unique<SingletrackCalcContext>(*this);
}


bool SingletrackCalcContext::do_equals(const CalcContext& rhs) const noexcept
{
	const SingletrackCalcContext* ctx =
		dynamic_cast<const SingletrackCalcContext*>(&rhs);

    return ctx != nullptr && *this == *ctx;
}


SingletrackCalcContext::SingletrackCalcContext(const std::string& filename)
	: SingletrackCalcContext { filename,
		false, accuraterip::NUM_SKIP_SAMPLES_FRONT,
		false, accuraterip::NUM_SKIP_SAMPLES_BACK }
{
	// empty
}


SingletrackCalcContext::SingletrackCalcContext(const std::string& filename,
		const bool skip_front, const bool skip_back)
	: SingletrackCalcContext { filename,
		skip_front, accuraterip::NUM_SKIP_SAMPLES_FRONT,
		skip_back,  accuraterip::NUM_SKIP_SAMPLES_BACK }
{
	// empty
}


SingletrackCalcContext::SingletrackCalcContext(const std::string& filename,
		const bool skip_front, const int32_t num_skip_front,
		const bool skip_back,  const int32_t num_skip_back)
	: CalcContextImplBase { filename, num_skip_front, num_skip_back }
	, skip_front_ { skip_front }
	, skip_back_  { skip_back }
{
	// empty
}


void SingletrackCalcContext::set_skip_front(const bool is_skipped) noexcept
{
	skip_front_ = is_skipped;
}


void SingletrackCalcContext::set_skip_back(const bool is_skipped) noexcept
{
	skip_back_ = is_skipped;
}


bool operator == (const SingletrackCalcContext& lhs,
		const SingletrackCalcContext& rhs) noexcept
{
	return lhs.base_equals(rhs)
		&& lhs.skip_front_ == rhs.skip_front_
		&& lhs.skip_back_  == rhs.skip_back_;
}


void swap(SingletrackCalcContext& lhs, SingletrackCalcContext& rhs) noexcept
{
	lhs.base_swap(rhs);

	using std::swap;
	swap(lhs.skip_front_, rhs.skip_front_);
	swap(lhs.skip_back_,  rhs.skip_back_);
}


// MultitrackCalcContext


int32_t MultitrackCalcContext::do_first_relevant_sample(
		const TrackNo track) const noexcept
{
	// We have offsets, so we respect the corresponding offset to any track.

	int32_t offset = 0;

	try
	{
		offset = toc().offset(track);
		// This will throw for track == 0 and for any unknown track in the TOC

	} catch (const std::exception& e)
	{
		ARCS_LOG_WARNING << "First relevant sample for unknown track "
			<< static_cast<int>(track) << " requested, returned 0.";

		return 0;
	}

	// Skipping applies at most for track 1, so we add the appropriate constant.
	if (this->skips_front() and track == 1)
	{
		return offset * CDDA::SAMPLES_PER_FRAME + this->num_skip_front();
	}

	// Standard multi track case: just the first sample of the track
	return offset * CDDA::SAMPLES_PER_FRAME;
}


int32_t MultitrackCalcContext::do_last_relevant_sample(
		const TrackNo track) const noexcept
{
	// Return last relevant sample of last track for any track number
	// greater than the last track

	if (track >= this->total_tracks())
	{
		return this->skips_back()
			? this->audio_size().total_samples() - 1 - this->num_skip_back()
			: this->audio_size().total_samples() - 1;
	}

	// We have offsets, so we respect the corresponding offset to any track

	int32_t next_offset = 0;

	try
	{
		next_offset = toc().offset(track + 1);

		// Note that this will throw for track == 0 and for any other unknown
		// track in the TOC

	} catch (const std::exception& e)
	{
		ARCS_LOG_WARNING << "Offset for unknown track "
			<< static_cast<int>(track) + 1 << " requested, returned 0.";

		return 0;
	}

	// Ensure result 0 for previous track's offset 0
	return next_offset ? next_offset * CDDA::SAMPLES_PER_FRAME - 1 : 0;
}


bool MultitrackCalcContext::do_skips_front() const noexcept
{
	return true;
}


bool MultitrackCalcContext::do_skips_back() const noexcept
{
	return true;
}


bool MultitrackCalcContext::do_is_multi_track() const noexcept
{
	return true;
}


int MultitrackCalcContext::do_total_tracks() const noexcept
{
	return toc().total_tracks();
}


int MultitrackCalcContext::do_track(const int32_t smpl)
	const noexcept
{
	if (this->audio_size().total_samples() == 0)
	{
		return 0; // FIXME throw ?
	}

	// Sample beyond last track?
	if (smpl >= this->audio_size().total_samples())
	{
		// This will return an invalid track number
		// Caller has to check result for <= total_tracks() for a valid result
		return CDDA::MAX_TRACKCOUNT + 1;
	}

	const auto last_track { this->total_tracks() };

	// Increase track number while sample is smaller than track's last relevant
	auto track = 0;
	for (int32_t last_sample_trk { this->last_relevant_sample(track) } ;
			smpl > last_sample_trk and track <= last_track ;
			++track, last_sample_trk = this->last_relevant_sample(track)) { } ;

	return track;
}


lba_count_t MultitrackCalcContext::do_offset(const int track) const noexcept
{
	return track < this->total_tracks() ? toc().offset(track + 1) : 0;
}


lba_count_t MultitrackCalcContext::do_length(const int track) const noexcept
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

	if (track >= this->total_tracks())
	{
		return 0;
	}

	// Offsets are set, but last length / leadout is unknown

	if (track == this->total_tracks() - 1)
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


std::unique_ptr<CalcContext> MultitrackCalcContext::do_clone() const noexcept
{
	return std::make_unique<MultitrackCalcContext>(*this);
}


bool MultitrackCalcContext::do_equals(const CalcContext& rhs) const noexcept
{
	const MultitrackCalcContext* ctx =
		dynamic_cast<const MultitrackCalcContext*>(&rhs);

    return ctx != nullptr && *this == *ctx;
}


void MultitrackCalcContext::do_hook_post_set_audio_size()
{
	if (audio_size().leadout_frame() != toc().leadout())
	{
		details::TOCBuilder builder;
		builder.update(toc_, audio_size().leadout_frame());
	}
}


MultitrackCalcContext::MultitrackCalcContext(const std::unique_ptr<TOC>& toc,
		const std::string& filename)
	: MultitrackCalcContext { toc, 0, 0, filename }
{
	this->set_toc(toc_);
}


MultitrackCalcContext::MultitrackCalcContext(const TOC& toc,
		const std::string& filename)
	: MultitrackCalcContext { toc, 0, 0, filename }
{
	this->set_toc(toc_);
}


MultitrackCalcContext::MultitrackCalcContext(const std::unique_ptr<TOC>& toc,
		const int32_t num_skip_front,
		const int32_t num_skip_back, const std::string& filename)
	: MultitrackCalcContext { *toc, num_skip_front, num_skip_back, filename }
{
	this->set_toc(toc_);
}


MultitrackCalcContext::MultitrackCalcContext(const TOC& toc,
		const int32_t num_skip_front,
		const int32_t num_skip_back, const std::string& filename)
	: CalcContextImplBase { filename, num_skip_front, num_skip_back }
	, toc_ { toc }
{
	this->set_toc(toc_);
}


const TOC& MultitrackCalcContext::toc() const noexcept
{
	return toc_;
}


void MultitrackCalcContext::set_toc(const TOC& toc)
{
	// NOTE: Leadout will be 0 if TOC is not complete.

	// FIXME AudioSize constructed with no bounds check
	this->set_audio_size(AudioSize { toc.leadout(), AudioSize::UNIT::FRAMES });
	// Commented out: without conversion
	//AudioSize audiosize;
	//audiosize.set_leadout_frame(toc.leadout());
	//this->set_audio_size(audiosize);

	toc_ = toc;
}


bool operator == (const MultitrackCalcContext& lhs,
		const MultitrackCalcContext& rhs) noexcept
{
	return lhs.base_equals(rhs) && lhs.toc_ == rhs.toc_;
}


void swap(MultitrackCalcContext& lhs, MultitrackCalcContext& rhs) noexcept
{
	lhs.base_swap(rhs);

	using std::swap;
	swap(lhs.toc_, rhs.toc_);
}


// PartitionProvider


PartitionProvider::PartitionProvider(const CalcContext& context,
		const Partitioner& partitioner)
	: context_     { &context }
	, partitioner_ { &partitioner }
{
		// empty
}


Partitioning PartitionProvider::operator()(const int32_t s_offset,
		const int32_t s_total) const
{
	return partitioner_->create_partitioning(s_offset, s_total,
		{
			context_->first_relevant_sample(),
			context_->last_relevant_sample()
		}
	);
}



} // namespace details


// calculate.hpp


// AudioSize


AudioSize::AudioSize()
	: AudioSize { 0, AudioSize::UNIT::BYTES }
{
	// empty
}


AudioSize::AudioSize(const int32_t value, const AudioSize::UNIT unit)
	: total_pcm_bytes_ { details::to_bytes(value, unit) }
{
	// empty
}


int32_t AudioSize::leadout_frame() const
{
	return total_frames();
}


int32_t AudioSize::total_frames() const
{
	return read_as(AudioSize::UNIT::FRAMES);
}


void AudioSize::set_total_frames(const int32_t frame_count)
{
	set_value(frame_count, AudioSize::UNIT::FRAMES);
}


int32_t AudioSize::total_samples() const
{
	return read_as(AudioSize::UNIT::SAMPLES);
}


void AudioSize::set_total_samples(const int32_t sample_count)
{
	set_value(sample_count, AudioSize::UNIT::SAMPLES);
}


int32_t AudioSize::total_pcm_bytes() const noexcept
{
	return read_as(AudioSize::UNIT::BYTES);
}


void AudioSize::set_total_pcm_bytes(const int32_t byte_count) noexcept
{
	set_value(byte_count, AudioSize::UNIT::BYTES);
}


bool AudioSize::zero() const noexcept
{
	return 0 == total_pcm_bytes();
}


int32_t AudioSize::max(const UNIT unit) noexcept
{
	static constexpr int32_t error_value { 0 };

	switch (unit)
	{
		case UNIT::FRAMES:
			return CDDA::MAX_BLOCK_ADDRESS;

		case UNIT::SAMPLES:
			return CDDA::MAX_BLOCK_ADDRESS * CDDA::SAMPLES_PER_FRAME;

		case UNIT::BYTES:
			return CDDA::MAX_BLOCK_ADDRESS * CDDA::BYTES_PER_FRAME;

		default:
			return error_value;
	}

	return error_value;
}


void AudioSize::set_value(const int32_t value, AudioSize::UNIT unit)
{
	if (details::within_bounds(value, unit))
	{
		total_pcm_bytes_ = details::to_bytes(value, unit);
	}
}


int32_t AudioSize::read_as(const AudioSize::UNIT unit) const
{
	return details::from_bytes(total_pcm_bytes_, unit);
}


AudioSize::operator bool() const noexcept
{
	return !zero();
}


void swap(AudioSize& lhs, AudioSize& rhs) noexcept
{
	using std::swap;
	swap(lhs.total_pcm_bytes_, rhs.total_pcm_bytes_);
}


//


bool operator == (const AudioSize& lhs, const AudioSize& rhs) noexcept
{
	return lhs.total_pcm_bytes() == rhs.total_pcm_bytes();
}


bool operator < (const AudioSize& lhs, const AudioSize& rhs) noexcept
{
	return lhs.total_pcm_bytes() < rhs.total_pcm_bytes();
}


// CalcContext


std::string CalcContext::filename() const noexcept
{
	return this->do_filename();
}


void CalcContext::set_filename(const std::string& filename)
{
	this->do_set_filename(filename);
}


const AudioSize& CalcContext::audio_size() const noexcept
{
	return this->do_audio_size();
}


void CalcContext::set_audio_size(const AudioSize& audio_size)
{
	this->do_set_audio_size(audio_size);
}


int32_t CalcContext::first_relevant_sample(const TrackNo track) const
	noexcept
{
	return this->do_first_relevant_sample(track);
}


int32_t CalcContext::first_relevant_sample() const noexcept
{
	return this->do_first_relevant_sample_no_parms();
}


int32_t CalcContext::last_relevant_sample(const TrackNo track) const
	noexcept
{
	return this->do_last_relevant_sample(track);
}


int32_t CalcContext::last_relevant_sample() const noexcept
{
	return this->do_last_relevant_sample_no_parms();
}


bool CalcContext::skips_front() const noexcept
{
	return this->do_skips_front();
}


bool CalcContext::skips_back() const noexcept
{
	return this->do_skips_back();
}


int32_t CalcContext::num_skip_front() const noexcept
{
	return this->do_num_skip_front();
}


int32_t CalcContext::num_skip_back() const noexcept
{
	return this->do_num_skip_back();
}


bool CalcContext::is_multi_track() const noexcept
{
	return this->do_is_multi_track();
}


void CalcContext::notify_skips(const int32_t num_skip_front,
		const int32_t num_skip_back) noexcept
{
	this->do_notify_skips(num_skip_front, num_skip_back);
}


int CalcContext::total_tracks() const noexcept
{
	return this->do_total_tracks();
}


int CalcContext::track(const int32_t smpl) const noexcept
{
	return this->do_track(smpl);
}


lba_count_t CalcContext::offset(const int track) const noexcept
{
	return this->do_offset(track);
}


lba_count_t CalcContext::length(const int track) const noexcept
{
	return this->do_length(track);
}


ARId CalcContext::id() const
{
	return this->do_id();
}


std::unique_ptr<CalcContext> CalcContext::clone() const noexcept
{
	return this->do_clone();
}


bool CalcContext::equals(const CalcContext& rhs) const noexcept
{
	return this->do_equals(rhs);
}


// make_context (bool, bool)


std::unique_ptr<CalcContext> make_context(const bool& skip_front,
		const bool& skip_back)
{
	return make_context(skip_front, skip_back, details::EmptyString);
}


// make_context (bool, bool, audiofile)


std::unique_ptr<CalcContext> make_context(const bool& skip_front,
		const bool& skip_back,
		const std::string& audiofilename)
{
	// NOTE: ARCS specific values, since ARCS2 is default checksum type
	return std::make_unique<details::SingletrackCalcContext>(audiofilename,
			skip_front, accuraterip::NUM_SKIP_SAMPLES_FRONT,
			skip_back,  accuraterip::NUM_SKIP_SAMPLES_BACK);
}


// make_context (TOC)


std::unique_ptr<CalcContext> make_context(const TOC& toc)
{
	return make_context(toc, details::EmptyString);
}


// make_context (TOC, audiofile)


std::unique_ptr<CalcContext> make_context(const TOC& toc,
		const std::string& audiofilename)
{
	// NOTE: ARCS specific values, since ARCS2 is default checksum type
	return std::make_unique<details::MultitrackCalcContext>(toc,
			accuraterip::NUM_SKIP_SAMPLES_FRONT,
			accuraterip::NUM_SKIP_SAMPLES_BACK,
			audiofilename);
}


// make_context (unique_ptr<TOC>)


std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC>& toc)
{
	return make_context(toc, details::EmptyString);
}


// make_context (unique_ptr<TOC>, audiofilename)


std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC>& toc,
		const std::string& audiofilename)
{
	return make_context(*toc, audiofilename);
}


} // namespace v_1_0_0
} // namespace arcstk

