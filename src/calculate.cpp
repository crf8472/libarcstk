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

#include <algorithm>   // for max, min, transform
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


std::vector<int32_t> get_offset_sample_indices(const TOC& toc)
{
	auto points { toc::get_offsets(toc) };

	using std::begin;
	using std::end;

	std::transform(begin(points), end(points), begin(points),
			[](const int32_t i){ return frames2samples(i); } );

	return points;
}


bool is_valid_track_number(const int track)
{
	return 0 < track && track <= 99;
}


bool is_valid_track(const int track, const TOC& toc)
{
	return 0 < track && track <= toc.total_tracks();
}


int track(const int32_t sample, const TOC& toc, const int32_t s_total)
{
	const auto offsets { get_offset_sample_indices(toc) };

	if (sample > s_total
			|| sample > frames2samples(toc.leadout()) || sample < offsets[0])
	{
		return 0;
	}

	const auto tracks  { static_cast<std::size_t>(toc.total_tracks()) };
	auto t = decltype( offsets )::size_type { 0 };
	while (t < tracks && sample >= offsets[t]) { ++t; };

	return static_cast<TrackNo>(t);
}


int32_t first_relevant_sample(const int track, const TOC& toc,
		const Interval<int32_t>& bounds)
{
	static const int32_t INVALID = 0;

	// TODO Validity check for TrackNo and TrackNo + 1
	if (!is_valid_track(track, toc))
	{
		// TODO throw?
		return INVALID;
	}

	auto frames = int32_t { 0 };

	try {

		frames = toc.offset(track);

	} catch (const std::exception& e) // TODO throw?
	{
		ARCS_LOG_WARNING << "Offset for unknown track " << track
			<< " requested, returned 0.";

		return INVALID;
	}

	if (!frames)
	{
		return INVALID;
	}

	const auto samples { frames2samples(frames) };

	if (1 == track)
	{
		return samples + bounds.lower(); // FIXME This is not bounds!!
		// THIS would be bounds:
		// return bounds.contain(samples) ? samples : bounds.lower();
	}

	return samples;
}


int32_t last_relevant_sample(const int track,
		const TOC& toc, const Interval<int32_t>& bounds)
{
	static const int32_t INVALID = 0;

	if (!toc.complete())
	{
		// TODO throw?
		return INVALID;
	}

	// TODO Validity check for TrackNo and TrackNo + 1
	if (!is_valid_track(track, toc))
	{
		// TODO throw?
		return INVALID;
	}

	if (track >= toc.total_tracks())
	{
		//return last_in_bounds(bounds, frames2samples(toc.leadout()));
		return std::min(bounds.upper(), frames2samples(toc.leadout()));
	}

	const auto next_track = TrackNo { track + 1 };
	auto frames = int32_t { 0 };

	try {

		frames = toc.offset(next_track);

	} catch (const std::exception& e) // TODO throw?
	{
		ARCS_LOG_WARNING << "Offset for unknown track " << next_track
			<< " requested, returned 0.";

		return INVALID;
	}

	return !frames ? INVALID :
		/* Last sample of previous track, if in bounds */
		//last_in_bounds(bounds, frames2samples(frames) - 1);
		std::min(bounds.upper(), frames2samples(frames) - 1);
}


// get_partitioning


Partitioning get_partitioning(
		const Interval<int32_t>&    interval,
		const Interval<int32_t>&    legal,
		const std::vector<int32_t>& points)
{
	const auto real_lower = std::max(
			{ legal.lower(), interval.lower(), points[0] });

	const auto real_upper = std::min(legal.upper(), interval.upper());

	// Both, real_lower and real_upper lie in segments between two of points[].
	// Identify those segments.

	auto b = std::size_t { 0 };
	auto e = std::size_t { 0 };
	for (const auto& p : points)
	{
		if (real_lower >= p) { ++b; };
		if (real_upper >= p) { ++e; } else break;
	}

	if (b > e)  { /* TODO throw; */ return {}; }

	// Now, b-1 and e-1 are the indices of segments in which the bounds lie.
	// All segments between these two, i.e. in the interval [b+1,e-2] can be
	// just read off of points[].

	// Add first partition
	auto partitions = std::vector<Partition> {
		Partition { real_lower, points[b],
			//real_lower, points[b] - 1, /* redundant */
			real_lower == points[b - 1], true, static_cast<TrackNo>(b)
		}
	};

	if (b == e) { return partitions; }

	// Add further partitions
	for (auto i { b }; i < e - 1; ++i)
	{
		partitions.emplace_back(points[i], points[i + 1],
			//points[i], points[i + 1] - 1, /* redundant */
			true, true, i + 1
		);
	}

	// Add last partition
	partitions.emplace_back(points[e - 1], real_upper,
		//points[e - 1], real_upper - 1, /* redundant */
		true, real_upper == points[e], static_cast<TrackNo>(e)
	);

	return partitions;
}


// Partitioner

/*
Partitioner::Partitioner(const int32_t total_samples)
	: Partitioner { total_samples, 0, 0, {} }
{
	// empty
}


Partitioner::Partitioner(const int32_t total_samples,
		const int32_t skip_front, const int32_t skip_back)
	: Partitioner { total_samples, skip_front, skip_back, {} }
{
	// empty
}
*/

Partitioner::Partitioner(const int32_t total_samples,
		const std::vector<int32_t>& points,
		const int32_t skip_front,
		const int32_t skip_back)
	: total_samples_ { total_samples }
	, points_        { points        }
	, skip_front_    { skip_front    }
	, skip_back_     { skip_back     }
{
	// empty
}


Partitioning Partitioner::create_partitioning(
		const int32_t offset,
		const int32_t total_samples_in_block) const
{
	const Interval<int32_t> interval {
		/* first phys. sample in block */ offset,
		/* last  phys. sample in block */ offset + total_samples_in_block - 1
	};

	const Interval<int32_t> legal {
		skip_front(),
		total_samples() - skip_back()
	};

	// If the sample block does not contain any relevant samples,
	// just return an empty partitioning.

	if (interval.upper() < legal.lower() || interval.lower() > legal.upper())
	{
		ARCS_LOG(DEBUG1) << "  No relevant samples in this block, skip";
		return Partitioning {};
	}

	if (points_.empty())
	{
		return do_create_partitioning(interval, legal);
	}

	return do_create_partitioning(interval, legal, points_);
}


int32_t Partitioner::total_samples() const
{
	return total_samples_;
}


void Partitioner::set_total_samples(const int32_t total_samples)
{
	total_samples_ = total_samples;
}


int32_t Partitioner::skip_front() const
{
	return skip_front_;
}


int32_t Partitioner::skip_back() const
{
	return skip_back_;
}


std::unique_ptr<Partitioner> Partitioner::clone() const
{
	return do_clone();
}


// make_partitioner


std::unique_ptr<Partitioner> make_partitioner(const TOC& toc,
		const int32_t total_frames)
{
	if (total_frames <= 0)
	{
		if (toc.complete())
		{
			return make_partitioner(toc);
		}

		// TODO throw
	}

	return std::make_unique<TrackPartitioner>(frames2samples(total_frames),
			get_offset_sample_indices(toc), true, true);
}


std::unique_ptr<Partitioner> make_partitioner(const TOC& toc)
{
	if (!toc.complete())
	{
		// TODO throw
	}

	return std::make_unique<TrackPartitioner>(frames2samples(toc.leadout()),
			get_offset_sample_indices(toc), true, true);
}


// TrackPartitioner


TrackPartitioner::TrackPartitioner(const int32_t total_samples,
			const std::vector<int32_t>& points,
			const int32_t skip_front, const int32_t skip_back)
	: Partitioner(total_samples, points, skip_front, skip_back)
{
	// empty
}


Partitioning TrackPartitioner::do_create_partitioning(
		const Interval<int32_t>&    interval, /* block of samples */
		const Interval<int32_t>&    legal,    /* legal range of samples */
		const std::vector<int32_t>& points    /* track points */ )  const
{
	return get_partitioning(interval, legal, points);
}


Partitioning TrackPartitioner::do_create_partitioning(
			const Interval<int32_t>& interval,
			const Interval<int32_t>& legal) const
{
	// Create a single partition spanning the entire block of samples,
	// but respect skipping samples at front or back.

	const auto chunk_first = interval.contains(legal.lower())
		? legal.lower()
		: interval.lower();

	const auto chunk_last = interval.contains(legal.upper())
		? legal.upper()
		: interval.upper();

	return { Partition {
		/* begin offset */  { chunk_first - interval.lower()     },
		/* end offset */    { chunk_last  - interval.lower() + 1 },
		/* chunk first */   //chunk_first, // redundant
		/* chunk last */    //chunk_last,  // redundant
		/* starts track */  { chunk_first == legal.lower() },
		/* ends track */    { chunk_last  == legal.upper() },
		/* invalid track */ 0
	}};
}


std::unique_ptr<Partitioner> TrackPartitioner::do_clone() const
{
	return std::make_unique<TrackPartitioner>(*this);
}


// Partition


Partition::Partition(
		const int32_t &begin_offset,
		const int32_t &end_offset,
		//const int32_t &first,
		//const int32_t &last,
		const bool     &starts_track,
		const bool     &ends_track,
		const TrackNo  &track
	)
	: begin_offset_ { begin_offset }
	, end_offset_ { end_offset }
	//, first_sample_idx_ { first }
	//, last_sample_idx_ { last }
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
	return static_cast<std::size_t>(end_offset() - begin_offset());
}


// calculate_impl.hpp


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


// CalculationState


CalculationState::CalculationState(Algorithm* const /* algorithm */)
{
	// empty
}


ChecksumSet CalculationState::current_subtotal() const
{
	return do_current_subtotal();
}


int32_t CalculationState::samples_processed() const noexcept
{
	return do_samples_processed();
}


std::chrono::milliseconds CalculationState::proc_time_elapsed() const noexcept
{
	return do_proc_time_elapsed();
}


void CalculationState::update(SampleInputIterator start,
		SampleInputIterator stop)
{
	do_update(start, stop);
	do_advance(std::distance(start, stop));
}


void CalculationState::increment_proc_time_elapsed(
		const std::chrono::milliseconds amount)
{
	do_increment_proc_time_elapsed(amount);
}


// CalculationStateImpl


CalculationStateImpl::CalculationStateImpl()
	: CalculationStateImpl(nullptr)
{
	// empty
}


CalculationStateImpl::CalculationStateImpl(Algorithm* const algorithm)
	: sample_offset_           { 0 }
	, proc_time_elapsed_       { std::chrono::duration<int64_t, std::milli>(0) }
	, algorithm_               { algorithm }
{
	// empty
}


ChecksumSet CalculationStateImpl::do_current_subtotal() const
{
	return algorithm_->result();
}


int32_t CalculationStateImpl::do_samples_processed() const noexcept
{
	//return sample_offset_.value() + 1; // +1 since index is 0-based
	return sample_offset_.value();
}


std::chrono::milliseconds CalculationStateImpl::do_proc_time_elapsed() const
	noexcept
{
	return proc_time_elapsed_.value();
}


void CalculationStateImpl::do_update(SampleInputIterator start,
		SampleInputIterator stop)
{
	sample_offset_.increment(std::distance(start, stop));
}


void CalculationStateImpl::do_increment_proc_time_elapsed(
		const std::chrono::milliseconds amount)
{
	proc_time_elapsed_.increment(amount);
}


void CalculationStateImpl::do_advance(const int32_t amount)
{
	sample_offset_.increment(amount);
}


// perform_update


void perform_update(SampleInputIterator start, SampleInputIterator stop,
		const Partitioner& partitioner,
		CalculationState&  state,
		Checksums&         result_buffer)
{
	const auto samples_in_block     { std::distance(start, stop) };
	const auto last_sample_in_block {
		state.samples_processed() + samples_in_block - 1/* stop is behind last*/ };

	ARCS_LOG_DEBUG << "  Offset:  " << state.samples_processed() << " samples";
	ARCS_LOG_DEBUG << "  Size:    " << samples_in_block      << " samples";
	ARCS_LOG_DEBUG << "  Indices: " <<
		state.samples_processed() << " - " << last_sample_in_block;

	// Create a partitioning following the track bounds in this block

	auto partitioning { partitioner.create_partitioning(
			state.samples_processed(), samples_in_block) };

	ARCS_LOG_DEBUG << "  Partitions:  " << partitioning.size();

	const bool is_last_relevant_block {
		Interval<int32_t>(state.samples_processed(), last_sample_in_block)
			.contains(/* last sample */partitioner.total_samples())
	};

	// Update the CalcState with each partition in this partitioning

	auto partition_counter = uint16_t { 0 };
	auto relevant_samples_counter = std::size_t { 0 };

	const auto start_time { std::chrono::steady_clock::now() };
	for (const auto& partition : partitioning)
	{
		++partition_counter;
		relevant_samples_counter += partition.size();

		ARCS_LOG_DEBUG << "  Partition " << partition_counter << "/" <<
			partitioning.size();

		state.update(start + partition.begin_offset(),
				stop + partition.end_offset());

		// If the current partition ends a track, save the ARCSs for this track

		if (partition.ends_track())
		{
			result_buffer.append(state.current_subtotal());

			ARCS_LOG_DEBUG << "    Completed track: "
				<< std::to_string(partition.track());
		}
	}
	const auto block_time_elapsed {
		std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - start_time)
	};
	state.increment_proc_time_elapsed(block_time_elapsed);

	ARCS_LOG_DEBUG << "  Number of relevant samples in this block: "
			<< relevant_samples_counter;

	ARCS_LOG_DEBUG << "  Milliseconds elapsed by processing this block: "
			<<	state.proc_time_elapsed().count();

	if (is_last_relevant_block)
	{
		ARCS_LOG(DEBUG1) << "Calculation complete.";

		ARCS_LOG(DEBUG1) << "Total samples counted:  " <<
			state.samples_processed();
		ARCS_LOG(DEBUG1) << "Total samples declared: " <<
			partitioner.total_samples();
		ARCS_LOG(DEBUG1) << "Milliseconds elapsed by calculating ARCSs: " <<
			state.proc_time_elapsed().count();
	}
}


} // namespace details


// calculate.hpp


// AudioSize


AudioSize::AudioSize() noexcept
	: AudioSize { 0, AudioSize::UNIT::BYTES }
{
	// empty
}


AudioSize::AudioSize(const int32_t value, const AudioSize::UNIT unit) noexcept
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
	if (not details::Interval<int32_t>(0, AudioSize::max(unit)).contains(value))
	{
		using std::to_string;

		if (value > AudioSize::max(unit))
		{
			auto ss = std::stringstream {};
			ss << "Value too big for maximum: " << to_string(value);
			throw std::overflow_error(ss.str());
		} else // value < 0
		{
			auto ss = std::stringstream {};
			ss << "Cannot set AudioSize to negative value: "
				<< to_string(value);
			throw std::underflow_error(ss.str());
		}
	}

	total_pcm_bytes_ = details::to_bytes(value, unit);
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


// SampleInputIterator


inline SampleInputIterator operator + (const int32_t amount,
		SampleInputIterator rhs) noexcept
{
	return rhs + amount;
}


// Algorithm


void Algorithm::update(SampleInputIterator begin, SampleInputIterator end)
{
	this->do_update(begin, end);
}


ChecksumSet Algorithm::result() const
{
	return this->do_result();
}


std::vector<checksum::type> Algorithm::types() const
{
	return this->do_types();
}


// make_calculation


std::unique_ptr<Calculation> make_calculation(
		std::unique_ptr<Algorithm> algorithm, const TOC& toc,
		const AudioSize& size)
{
	if (size.zero())
	{
		return make_calculation(std::move(algorithm), toc);
	}

	// TODO Checks should be already implemented in identifier.cpp

	if (size.total_frames() > CDDA::MAX_OFFSET)
	{
		throw InsufficientCalculationInputException(
				"Cannot build Calculation with max input size exceeded: "
				" should be at most "
				+ std::to_string(CDDA::MAX_OFFSET)
				+ " frames but is "
				+ std::to_string(size.total_frames())
				+ " frames"
		);
	}

	const auto last_track_len {
		size.total_frames() - toc.offset(toc.total_tracks()) };

	if (last_track_len < CDDA::MIN_TRACK_LEN_FRAMES)
	{
		throw InsufficientCalculationInputException(
				"Cannot build Calculation because last track is too short: "
				" should be at least "
				+ std::to_string(CDDA::MIN_TRACK_LEN_FRAMES)
				+ " frames but is only "
				+ std::to_string(last_track_len)
				+ " frames"
		);
	}

	return std::make_unique<Calculation>(std::move(algorithm), toc, size);
}


std::unique_ptr<Calculation> make_calculation(
		std::unique_ptr<Algorithm> algorithm, const TOC& toc)
{
	if (!toc.complete())
	{
		throw InsufficientCalculationInputException(
				"Cannot build a Calculation with an incomplete TOC "
				"and no completing AudioSize"
		);
	}

	return std::make_unique<Calculation>(std::move(algorithm), toc,
			AudioSize { toc.leadout(), AudioSize::UNIT::FRAMES });
}


//


InsufficientCalculationInputException::InsufficientCalculationInputException(
		const std::string &what_arg)
	: std::runtime_error { what_arg }
{
	// empty
}


InsufficientCalculationInputException::InsufficientCalculationInputException(
		const char* what_arg)
	: std::runtime_error { what_arg }
{
	// empty
}


// Calculation


Calculation::Calculation(std::unique_ptr<Algorithm> algorithm, const TOC& toc,
		const AudioSize& size)
	:impl_ { std::make_unique<Impl>(std::move(algorithm), toc, size) }
{
	// empty
}


const Algorithm* Calculation::algorithm() const noexcept
{
	return impl_->algorithm();
}


std::vector<checksum::type> Calculation::types() const noexcept
{
	return algorithm()->types();
}


int32_t Calculation::samples_expected() const noexcept
{
	return impl_->samples_expected();
}


int32_t Calculation::samples_processed() const noexcept
{
	return impl_->samples_processed();
}


int32_t Calculation::samples_todo() const noexcept
{
	return samples_expected() - samples_processed();
}


std::chrono::milliseconds Calculation::proc_time_elapsed() const noexcept
{
	return impl_->proc_time_elapsed();
}


bool Calculation::complete() const noexcept
{
	return impl_->complete();
}


void Calculation::update(SampleInputIterator start, SampleInputIterator stop)
{
	impl_->update(start, stop);
}


void Calculation::update(const AudioSize &audiosize)
{
	impl_->update(audiosize);
}


Checksums Calculation::result() const noexcept
{
	return impl_->result();
}


// Calculation::Impl


Calculation::Impl::Impl(std::unique_ptr<Algorithm> algorithm, const TOC& toc,
		const AudioSize& size)
	: algorithm_     { std::move(algorithm) }
	, partitioner_   { details::make_partitioner(toc, size.total_frames()) }
	                   // TODO make_partitioner throws!
	, state_         {
		std::make_unique<details::CalculationStateImpl>(algorithm_.get()) }
	, result_buffer_ { std::make_unique<Checksums>() }
{
	// empty
}


const Algorithm* Calculation::Impl::algorithm() const noexcept
{
	return algorithm_.get();
}


int32_t Calculation::Impl::samples_expected() const noexcept
{
	return partitioner_->total_samples();
}


int32_t Calculation::Impl::samples_processed() const noexcept
{
	return state_->samples_processed();
}


std::chrono::milliseconds Calculation::Impl::proc_time_elapsed() const noexcept
{
	return state_->proc_time_elapsed();
}


bool Calculation::Impl::complete() const noexcept
{
	return this->samples_processed() >= this->samples_expected();
}


void Calculation::Impl::update(SampleInputIterator start,
		SampleInputIterator stop)
{
	perform_update(start, stop, *partitioner_, *state_, *result_buffer_);
}


void Calculation::Impl::update(const AudioSize &audiosize)
{
	partitioner_->set_total_samples(audiosize.total_samples());
}


Checksums Calculation::Impl::result() const noexcept
{
	return *result_buffer_;
}


} // namespace v_1_0_0
} // namespace arcstk

