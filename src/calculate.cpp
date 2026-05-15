/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of the new checksum calculation API
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"
#endif

#include <algorithm>   // for min, max
#include <cstdint>     // for int32_t, uint16_t
#include <iomanip>     // for setw, right

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"        // for AudioSize, ToC, CDDA
#endif
#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"      // for AccurateRip::V1, V2 and V1andV2
#endif


namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

// get_partitioning


Partitioning get_partitioning(const SampleRange& interval,
		const SampleRange& legal,
		const Points&      opoints)
{
	if (opoints.empty())
	{
		return get_partitioning(interval, legal);
	}

	const auto real_lower { std::max(legal.lower(), interval.lower()) };
	const auto real_upper { std::min(legal.upper(), interval.upper()) };
	const auto points     { convert<UNIT::SAMPLES>(opoints)  };

	// Both, real_lower and real_upper lie in segments between two of points[].
	// Identify those segments.

	auto b = std::size_t { 0 };
	auto e = std::size_t { 0 };
	for (const auto& p : points)
	{
		if (real_lower >= p) { ++b; };
		if (real_upper >= p) { ++e; } else { break; }
	}

	// Now, b-1 and e-1 are the indices of the tracks/segments in which the
	// bounds lie. All segments between these two, i.e. in the interval
	// [b+1,e-2] can be just read off of points[].

	// Add first partition: from real lower to the start of the subsequent
	// track or the upper bound. May or may not end the first track.

	// Note: The interval may be smaller than a track. In this case, only one
	// partition will be returned and it may be the partition that ends the last
	// track. In this case, b will be bigger than the index.

	auto partitions = std::vector<Partition>{};

	// front

	auto track { b };
	{
		const auto start_of_track = track == 0 ? 0 : points[track - 1];
		const auto end_of_track   = points[track] - 1;

		// start of the first (and maybe only) partition
		const auto p0_lower { real_lower };

		// end of the first (and maybe only) partition
		const auto p0_upper { track < points.size()  // if not last track
			? std::min(end_of_track, real_upper)
			: real_upper
		};

		ARCS_LOG(DEBUG3) << "Create front partition, "
			<< "track " << std::setw(2) << std::right << track << ": "
			<< std::setw(9) << std::right << p0_lower
			<< " - "
			<< std::setw(9) << std::right << p0_upper;

		partitions.emplace_back(
			p0_lower, p0_upper,
			p0_lower == start_of_track || p0_lower == legal.lower(),
			p0_upper == end_of_track   || p0_upper == legal.upper(),
			static_cast<TrackNo>(track)
		);
	} // front

	// If the interval does not span over multiple tracks, we are done now.
	if (b == e) { return partitions; }

	// mid (if any)

	// Add further partitions, this is just from point i to point i + 1.
	// Will be entirely skipped if the partition does span 2 tracks or less.
	for (auto i { b }; i < e - 1; ++i)
	{
		track = i + 1;

		ARCS_LOG(DEBUG3) << "Create mid. partition,  "
			<< "track " << std::setw(2) << std::right << track << ": "
			<< std::setw(9) << std::right << points[i]
			<< " - "
			<< std::setw(9) << std::right << points[track];

		partitions.emplace_back(
				points[i], points[track] - 1, true, true,
				static_cast<TrackNo>(track));
	}

	// back

	track = e;
	{
		const auto pN_lower { points[track - 1] };

		const auto pN_upper { track < points.size()
			? std::min(points[track] - 1, real_upper)
			: real_upper
		};

		ARCS_LOG(DEBUG3) << "Create back partition,  "
			<< "track " << std::setw(2) << std::right << e << ": "
			<< std::setw(9) << std::right << pN_lower
			<< " - "
			<< std::setw(9) << std::right << pN_upper;

		// Add last partition: from the beginning of the track that contains the
		// upper bound to the real upper bound.
		partitions.emplace_back(pN_lower, pN_upper,
			true,/*a previous partition is guaranteed that ends on track end*/
			pN_upper == points[track] - 1 || pN_upper == legal.upper(),
			static_cast<TrackNo>(track)
		);
	} // back

	return partitions;
}


Partitioning get_partitioning(const SampleRange& interval,
		const SampleRange& legal)
{
	// Create a single partition spanning the entire block of samples.
	// Respect legal range.

	const auto partition_start { interval.contains(legal.lower())
		? legal.lower()
		: interval.lower()
	};

	const auto partition_end { interval.contains(legal.upper())
		? legal.upper()
		: interval.upper()
	};

	ARCS_LOG(DEBUG3) << "Create partition from interval: " << partition_start
		<< " - " << partition_end;

	return { Partition { partition_start, partition_end,
		( partition_start == legal.lower() )/* starts track ? */,
		( partition_end   == legal.upper() )/* ends track ? */,
		0/* invalid track */
	}};
}


// Partitioner


Partitioner::Partitioner(const Points& points, const AudioSize& total_samples,
		const SampleRange& legal)
	: points_        { points        }
	, total_samples_ { total_samples }
	, legal_         { legal         }
{
	// empty
}


Partitioning Partitioner::create_partitioning(
		const int32_t offset,
		const int32_t total_samples_in_block) const
{
	const SampleRange current_interval {
		offset, offset + am2ind(total_samples_in_block)
	};

	// If the sample block does not contain any relevant samples,
	// just return an empty partitioning.

	if (current_interval.upper() < legal_range().lower() ||
			current_interval.lower() > legal_range().upper())
	{
		ARCS_LOG(DEBUG2) <<
			"No relevant samples in interval, provide no partitions";
		return Partitioning {};
	}

	return do_create_partitioning(current_interval, legal_range(), points_);
}


AudioSize Partitioner::total_samples() const noexcept
{
	return total_samples_;
}


void Partitioner::set_total_samples(const AudioSize& total_samples) noexcept
{
	total_samples_ = total_samples;
}


SampleRange Partitioner::legal_range() const noexcept
{
	return legal_;
}


Points Partitioner::points() const noexcept
{
	return points_;
}


std::unique_ptr<Partitioner> Partitioner::clone() const
{
	return do_clone();
}


// TrackPartitioner


TrackPartitioner::TrackPartitioner(const Points& points,
		const AudioSize&   total_samples,
		const SampleRange& legal)
	: Partitioner(points, total_samples, legal)
{
	// empty
}


Partitioning TrackPartitioner::do_create_partitioning(
		const SampleRange& interval,     /* block of samples */
		const SampleRange& legal,        /* legal range of samples */
		const Points&      points) const /* track points */
{
	return get_partitioning(interval, legal, points);
}


std::unique_ptr<Partitioner> TrackPartitioner::do_clone() const
{
	return std::make_unique<TrackPartitioner>(*this);
}


// Partition


Partition::Partition(
		const int32_t begin_offset,
		const int32_t end_offset,
		const bool    starts_track,
		const bool    ends_track,
		const TrackNo track
	)
	: begin_offset_ { begin_offset }
	, end_offset_   { end_offset   }
	, starts_track_ { starts_track }
	, ends_track_   { ends_track   }
	, track_        { track        }
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


// ind2am()


int32_t ind2am(const int32_t index)
{
	return index + 1;
}


// am2ind()


int32_t am2ind(const int32_t amount)
{
	return amount - 1;
}


// CalculationState


CalculationState::CalculationState()
	: current_offset_          { 0 }
	, samples_processed_       { 0 }
	, track_samples_processed_ { 0 }
	, tracks_processed_        { 0 }
	, sequences_processed_     { 0 }
	, algo_time_elapsed_       { 0 }
	, update_time_elapsed_     { 0 }
{
	// empty
}


int32_t CalculationState::current_offset() const noexcept
{
	return current_offset_.value();
}


int32_t CalculationState::samples_processed() const noexcept
{
	return samples_processed_.value();
}


int32_t CalculationState::track_samples_processed() const noexcept
{
	return track_samples_processed_.value();
}


int32_t CalculationState::tracks_processed() const noexcept
{
	return tracks_processed_.value();
}


int32_t CalculationState::sequences_processed() const noexcept
{
	return sequences_processed_.value();
}


std::chrono::duration<float> CalculationState::algo_time_elapsed() const
	noexcept
{
	return algo_time_elapsed_;
}


void CalculationState::increment_algo_time_elapsed(
			const std::chrono::duration<float>& duration)
{
	algo_time_elapsed_ += duration;
}


std::chrono::duration<float> CalculationState::update_time_elapsed() const
	noexcept
{
	return update_time_elapsed_;
}


void CalculationState::increment_update_time_elapsed(
			const std::chrono::duration<float>& duration)
{
	update_time_elapsed_ += duration;
}


void CalculationState::advance(const int32_t amount)
{
	current_offset_.increment(amount);
}


void CalculationState::update(const int32_t samples_amount,
		const std::chrono::duration<float>& algo_duration)
{
	samples_processed_.increment(samples_amount);
	track_samples_processed_.increment(samples_amount);
	advance(samples_amount);

	sequences_processed_.increment(1);

	increment_algo_time_elapsed(algo_duration);
}


int32_t CalculationState::track_finished()
{
	const auto track_samples = track_samples_processed();

	track_samples_processed_.reset();
	tracks_processed_.increment(1);

	return track_samples;
}


void CalculationState::swap(CalculationState& rhs) noexcept
{
	using std::swap;

	swap(this->current_offset_,          rhs.current_offset_          );
	swap(this->samples_processed_,       rhs.samples_processed_       );
	swap(this->track_samples_processed_, rhs.track_samples_processed_ );
	swap(this->tracks_processed_,        rhs.tracks_processed_        );
	swap(this->algo_time_elapsed_,       rhs.algo_time_elapsed_       );
	swap(this->update_time_elapsed_,     rhs.update_time_elapsed_     );
}


std::unique_ptr<details::Partitioner> make_partitioner(
		//const Algorithm& algorithm,
		const Points& offsets, const AudioSize& leadout,
		const SampleRange& interval)
{
	//using details::SampleRange;
	//using details::TrackPartitioner;

	//const auto interval {
	//	SampleRange { algorithm.range(leadout, offsets) }};

	ARCS_LOG(DEBUG1) << "Calculation interval is " << interval.to_string();

	return std::make_unique<details::TrackPartitioner>(offsets, leadout,
			interval);
}


void log_sample_stats(const Partition& partition,
		const int32_t from, const int32_t to, const int32_t total)
{
	ARCS_LOG(DEBUG2) << "Samples "
		<< std::setw(9) << std::right << from
		<< " - "
		<< std::setw(9) << std::right << to
		<< " (Track " << partition.track() << ", "
		<< (partition.starts_track()
				? (partition.ends_track() ? "complete"  : "first part")
				: (partition.ends_track() ? "last part" : "mid part"))
		<< ")";

	ARCS_LOG(DEBUG2) << "Samples total: " << total;
}


void log_processing_stats(const Partitioner& partitioner,
		const CalculationState& state)
{
	ARCS_LOG(DEBUG1) << "Total samples declared:  "
		<< partitioner.total_samples().samples();

	ARCS_LOG(DEBUG1) << "Total samples processed: "
		<< state.samples_processed()
		<< " (== " << partitioner.legal_range().to_string() << ")";

	using ms = std::chrono::milliseconds;

	const ms update_time =
		std::chrono::duration_cast<ms>(state.update_time_elapsed());

	ARCS_LOG(DEBUG1) << "Milliseconds elapsed by calculating ARCSs: "
		<< update_time.count();

	const ms algo_time =
		std::chrono::duration_cast<ms>(state.algo_time_elapsed());

	ARCS_LOG(DEBUG1) << "Milliseconds elapsed by Algorithm: "
		<< algo_time.count();
}


namespace update
{


std::pair<int32_t, int32_t> positions(const int32_t& samples_in_block,
		CalculationState& state)
{
	const auto start_pos { state.current_offset() };
	const auto last_pos  { start_pos + am2ind(samples_in_block) };

	ARCS_LOG(DEBUG1) << "Offsets: " << start_pos << " - " << last_pos;
	ARCS_LOG(DEBUG1) << "Size:    " << samples_in_block   << " samples";

	return { start_pos, last_pos };
}


bool complete_after_skip_block(const int32_t& samples_in_block,
		const Partitioner& partitioner,
		CalculationState& state)
{
	ARCS_LOG_DEBUG << "Skip block, advance";

	state.advance(samples_in_block);
	return state.current_offset() >= partitioner.legal_range().upper();
}


void skip_amount(const int32_t& start_pos, const Partitioning& partitioning,
		CalculationState& state)
{
	const auto diff { partitioning.front().begin_offset() - start_pos };

	if (diff > 0)
	{
		ARCS_LOG(DEBUG1) << "Skipped " << diff << " samples, advance";
		state.advance(diff);
	}
}


void complete_track(Algorithm& algorithm,
		CalculationResultBuffer& result_buffer, CalculationState& state)
{
	// Updates + resets state as a side effect: order matters, since
	// CalculationState::track_finished() will update tracks_processed_.
	const auto track_length = AudioSize { state.track_finished(),
				UNIT::SAMPLES };

	// tracks_processed() reflects previous track_finished()
	const auto track_number = state.tracks_processed();

	algorithm.track_finished(track_number, track_length);

	result_buffer.put_value(static_cast<std::size_t>(track_number),
					algorithm.result());
}

} // namespace update
} // namespace details


// Settings


Settings::Settings()
	: context_ { Context::ALBUM }
{
	// empty
}


Settings::Settings(const Context& c)
	: context_ { c }
{
	// empty
}


void Settings::set_context(const Context c)
{
	context_ = c;
}


Context Settings::context() const
{
	return context_;
}


void Settings::swap(Settings& rhs) noexcept
{
	using std::swap;

	swap(this->context_, rhs.context_);
}


bool Settings::equals(const Settings& rhs) const noexcept
{
	return this->context_ == rhs.context_;
}


std::string Settings::to_string() const
{
	using std::to_string;

	return "Context: " + to_string(this->context_);
}


// Stateful


std::string name(const State s)
{
	// The order of names in this aggregate must match the order of types in
	// enum class checksum::type, otherwise function type_name() will fail.
	static const std::array<std::string, 5> names {
		"INSTANTIATED",
		"INITIALIZED",
		"UPDATED",
		"COMPLETED",
		"INVALID"
	};

	return names.at(static_cast<decltype( names )::size_type>(s));
}


// CalculationBase


void Calculation::log_completion() noexcept
{
	ARCS_LOG(DEBUG1) << "Last block completed, calculation finished";

	if constexpr (LOGLEVEL::DEBUG1 <= CLIP_LOGGING_LEVEL)
	{
		log_processing_stats(*partitioner_, state());
	}
}


void Calculation::base_swap(Calculation& rhs) noexcept
{
	Stateful::base_swap(rhs);

	using std::swap;

	swap(settings_,      rhs.settings_      );
	swap(state_,         rhs.state_         );
	swap(partitioner_ ,  rhs.partitioner_   );
	swap(result_buffer_, rhs.result_buffer_ );
}


void Calculation::set_settings(const Settings& s)
{
	allowed_only_before(State::UPDATED,
				"Cannot change settings after first update");

	settings_ = s;
	on_settings_changed();
}


bool Calculation::complete() const noexcept
{
	return state().current_offset() >= partitioner_->legal_range().upper();
}


Checksums Calculation::result() const noexcept
{
	if (current_state() != State::COMPLETED)
	{
		ARCS_LOG_WARNING << "Calculation result accessed before completion";
	}

	return result_buffer_.result();
}


// explicit instantiations

template class Updater<AccurateRip::V1>;
template class Updater<AccurateRip::V2>;
template class Updater<AccurateRip::V1andV2>;

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// instantiate the 24 variants of Updater that are expected
#define INSTANTIATE_UPDATE_FUNCTION(Algorithm, Type, IsPlanar) \
template void Updater<Algorithm>::update<Type, IsPlanar>( \
			const details::SampleSequence<Type, IsPlanar>&);

INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1andV2,  int16_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1andV2,  int16_t, false);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1andV2,  int32_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1andV2,  int32_t, false);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1andV2, uint16_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1andV2, uint16_t, false);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1andV2, uint32_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1andV2, uint32_t, false);

INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V2,       int16_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V2,       int16_t, false);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V2,       int32_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V2,       int32_t, false);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V2,      uint16_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V2,      uint16_t, false);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V2,      uint32_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V2,      uint32_t, false);

INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1,       int16_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1,       int16_t, false);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1,       int32_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1,       int32_t, false);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1,      uint16_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1,      uint16_t, false);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1,      uint32_t, true);
INSTANTIATE_UPDATE_FUNCTION(AccurateRip::V1,      uint32_t, false);

// NOLINTBEGIN(bugprone-macro-parentheses)
#define INSTANTIATE_UPDATE_FUNCTION_IT(Algorithm, Type, IsPlanar) \
template void Updater<Algorithm>::update \
	<details::SampleIterator<Type, IsPlanar>>( \
			details::SampleIterator<Type, IsPlanar>, \
			details::SampleIterator<Type, IsPlanar> );
// NOLINTEND(bugprone-macro-parentheses)

INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1andV2,  int16_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1andV2,  int16_t, false);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1andV2,  int32_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1andV2,  int32_t, false);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1andV2, uint16_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1andV2, uint16_t, false);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1andV2, uint32_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1andV2, uint32_t, false);

INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V2,       int16_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V2,       int16_t, false);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V2,       int32_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V2,       int32_t, false);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V2,      uint16_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V2,      uint16_t, false);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V2,      uint32_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V2,      uint32_t, false);

INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1,       int16_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1,       int16_t, false);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1,       int32_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1,       int32_t, false);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1,      uint16_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1,      uint16_t, false);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1,      uint32_t, true);
INSTANTIATE_UPDATE_FUNCTION_IT(AccurateRip::V1,      uint32_t, false);
// NOLINTEND(cppcoreguidelines-macro-usage)

} // namespace v_1_0_0
} // namespace arcstk

