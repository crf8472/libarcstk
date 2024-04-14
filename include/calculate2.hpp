#ifndef __LIBARCSTK_CALCULATE2_HPP___
#define __LIBARCSTK_CALCULATE2_HPP___
/**
 * \file
 *
 * \brief Calculation interface.
 */
#include <chrono>        // for milliseconds, duration-cast, operator-
#include <cstdint>       // for uint32_t, int32_t
#include <unordered_map> // for unordered_map

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"                  // for Checksum, ChecksumSet, Checksums
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"                 // for AudioSize
#endif
#ifndef __LIBARCSTK_CALC_PARTITION_HPP__
//#include "calc_partition.hpp"            // for Partiioner
#endif
#ifndef __LIBARCSTK_CALC_CONTEXT_HPP__
//#include "calc_context.hpp"              // for CalcContext
#endif
#ifndef __LIBARCSTK_CALC_STATE_HPP__
//#include "calc_state.hpp"                // for CalcState
#endif

#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

template<typename T>
class Counter final
{
	T value_;

public:

	T value() const
	{
		return value_;
	}

	void increment(const T& amount)
	{
		value_ += amount;
	}
};


static constexpr uint_fast32_t LOWER_32_BITS_ { 0xFFFFFFFF };


/**
 * \brief Interface for updating a calculation state.
 */
template<enum checksum::type T1, enum checksum::type... T2>
class Updatable
{
public:

	template<class B, class E>
	void update(const B& start, const E& stop);
	ChecksumSet value();
	void reset();
};

template<>
class Updatable<checksum::type::ARCS1>
{
	uint_fast64_t multiplier_ = 1;
	uint_fast32_t subtotal_   = 0;

public:

	template<class B, class E>
	void update(const B& start, const E& stop)
	{
		for (auto pos = start; pos != stop; ++pos, ++multiplier_)
		{
			subtotal_ += (multiplier_ * (*pos)) & LOWER_32_BITS_;
		}
	}

	ChecksumSet value()
	{
		return { 0, {{ checksum::type::ARCS1, subtotal_ }} };
	}

	void reset()
	{
		multiplier_  = 1;
		subtotal_    = 0;
	}
};

template<>
class Updatable<checksum::type::ARCS2>
{
	uint_fast64_t multiplier_ = 1;
	uint_fast64_t update64_   = 0;
	uint_fast32_t subtotal_   = 0;

public:

	template<class B, class E>
	void update(const B& start, const E& stop)
	{
		for (auto pos = start; pos != stop; ++pos, ++multiplier_)
		{
			update64_ = multiplier_ * (*pos);
			subtotal_ += (update64_ & LOWER_32_BITS_) + (update64_ >> 32u);
		}
	}

	ChecksumSet value()
	{
		return { 0, {{ checksum::type::ARCS2, subtotal_ }} };
	}

	void reset()
	{
		multiplier_  = 1;
		subtotal_    = 0;
		update64_    = 0;
	}
};

template<>
class Updatable<checksum::type::ARCS1,checksum::type::ARCS2>
{
	uint_fast64_t multiplier_  = 1;
	uint_fast64_t update64_    = 0;
	uint_fast32_t subtotal_v1_ = 0;
	uint_fast32_t subtotal_v2_ = 0;

public:

	template<class B, class E>
	void update(const B& start, const E& stop)
	{
		for (auto pos = start; pos != stop; ++pos, ++multiplier_)
		{
			update64_ = multiplier_ * (*pos);
			subtotal_v1_ += update64_ & LOWER_32_BITS_;
			subtotal_v2_ += update64_ >> 32u;
		}
	}

	ChecksumSet value()
	{
		return { 0, {
			{ checksum::type::ARCS1, subtotal_v1_ },
			{ checksum::type::ARCS2, subtotal_v1_ + subtotal_v2_ },
		} };
	}

	void reset()
	{
		multiplier_  = 1;
		subtotal_v1_ = 0;
		subtotal_v2_ = 0;
		update64_    = 0;
	}
};


/**
 * \brief Calculation progress.
 */
class CalcCounters
{
	Counter<sample_count_t> smpl_offset_;
	Counter<std::chrono::milliseconds> proc_time_elapsed_;

public:

	sample_count_t sample_offset() const;
	void increment_sample_offset(const sample_count_t amount);

	std::chrono::milliseconds proc_time_elapsed() const;
	void increment_proc_time_elapsed(const std::chrono::milliseconds amount);
};


/*
template<typename T>
class ChecksumBuffer
{
	std::unordered_map<int, T> track_checksums_;
};

template<class B, class E>
class State : public CalculationState
{
	sample_count_t actual_skip_front_;
	sample_count_t actual_skip_back_;

};
*/

/**
 *
 * \tparam B Type of the iterator pointing to the start position
 * \tparam E Type of the iterator pointing to the stop position
 *
 * \param[in] begin
 * \param[in] end
 * \param[in] context
 * \param[in] partitioner
 * \param[in,out] state
 * \param[in,out] smpl_offset
 */
/*
template<class B, class E>
void calc_update(B& begin, E& end, const CalcContext& context,
		const Partitioner& partitioner, CalcState& state, CalcCounters& p)
{
	const auto samples_in_block     { std::distance(begin, end) };
	const auto last_sample_in_block { p.sample_offset() + samples_in_block - 1 };

	ARCS_LOG_DEBUG << "  Offset:  " << p.sample_offset() << " samples";
	ARCS_LOG_DEBUG << "  Size:    " << samples_in_block << " samples";
	ARCS_LOG_DEBUG << "  Indices: " <<
		p.sample_offset() << " - " << last_sample_in_block;

	// Create a partitioning following the track bounds in this block

	auto partitioning {
		partitioner.create_partitioning(p.sample_offset(),
				samples_in_block, context)
	};

	ARCS_LOG_DEBUG << "  Partitions:  " << partitioning.size();

	const bool is_last_relevant_block {
		Interval(p.sample_offset(), last_sample_in_block).contains(
			context.last_relevant_sample())
	};


	// Update the CalcState with each partition in this partitioning

	auto partition_counter = uint16_t { 0 };
	auto relevant_samples_counter = sample_count_t { 0 };

	const auto start_time { std::chrono::steady_clock::now() };
	for (const auto& partition : partitioning)
	{
		++partition_counter;
		relevant_samples_counter += partition.size();

		//this->log_partition(partition_counter, partitioning.size(), partition);

		// Update the calculation state with the current partition/chunk

		state->update(SampleInputIterator { begin + partition.begin_offset() },
				SampleInputIterator { begin + partition.end_offset() });

		// If the current partition ends a track, save the ARCSs for this track

		if (partition.ends_track())
		{
			state->save(partition.track());

			ARCS_LOG_DEBUG << "    Completed track: "
				<< std::to_string(partition.track());
		}
	}
	//smpl_offset += samples_in_block;
	p.increment_sample_offset(samples_in_block);
	const auto end_time { std::chrono::steady_clock::now() };


	// Do the logging

	ARCS_LOG_DEBUG << "  Number of relevant samples in this block: "
			<< relevant_samples_counter;

	{
		const auto block_time_elapsed {
			std::chrono::duration_cast<std::chrono::milliseconds>
				(end_time - start_time)
		};

		p.increment_proc_time_elapsed(block_time_elapsed);

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
			<< p.proc_time_elapsed().count();
	}
}
*/
} // namespace details
} // namespace v_1_0_0
} // namespace arcstk

#endif

