/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of the new checksum calculation API.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"
#endif

#include <algorithm>   // for min, max
#include <array>       // for array
#include <chrono>      // for duration, duration_cast, milliseconds
#include <cstddef>     // for size_t
#include <cstdint>     // for int32_t, uint16_t
#include <iomanip>     // for setw
#include <ios>         // for right
#include <memory>      // for make_unique, unique_ptr
#include <string>      // for string
#include <utility>     // for move, pair
#include <vector>      // for vector

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"
#endif
#ifndef LIBARCSTK_LOGLEVEL_HPP_
#include "loglevel.hpp"
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"        // for AudioSize, ToC, CDDA
#endif
#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"       // for Algorithm
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

// CalculationState


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


int32_t CalculationState::track_finalized()
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
	const auto first_pos { state.current_offset() };
	const auto last_pos  { first_pos + am2ind(samples_in_block) };

	ARCS_LOG(DEBUG1) << "Offsets: " << first_pos << " - " << last_pos;
	ARCS_LOG(DEBUG1) << "Size:    " << samples_in_block   << " samples";

	return { first_pos, last_pos };
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


// Calculation


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
	return partitioner_ &&
		state().current_offset() >= partitioner_->legal_range().upper();
}


Checksums Calculation::result(const int k) const
{
	if (current_state() != State::COMPLETED)
	{
		ARCS_LOG_WARNING << "Calculation result accessed before completion";
	}

	if (0 == k)
	{
		return result_buffer_;
	}

	// TODO get algorithm.track(t, k) for every t
	ARCS_LOG_ERROR << "Drive offsets not fully implemented";

	return result_buffer_;
}


// explicit instantiations

template class CalculationUpdater<AccurateRip::V1>;
template class CalculationUpdater<AccurateRip::V2>;
template class CalculationUpdater<AccurateRip::V1andV2>;

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// instantiate the 24 variants of CalculationUpdater that are expected
#define INSTANTIATE_UPDATE_FUNCTION(Algorithm, Type, IsPlanar) \
template void CalculationUpdater<Algorithm>::update<Type, IsPlanar>( \
			const SampleSequence<Type, IsPlanar>&);

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
template void CalculationUpdater<Algorithm>::update \
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

