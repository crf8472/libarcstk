#ifndef __LIBARCSTK_CALCULATE_IMPL_HPP__
#define __LIBARCSTK_CALCULATE_IMPL_HPP__
/**
 * \file
 *
 * \brief Implementations of calculate.hpp that depend on the public header.
 */

#include <cstdint>       // for uint32_t, int32_t
#include <memory>        // for unique_ptr
#include <unordered_map> // for unordered_map
#include <string>        // for string

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"  // for TOC
#endif

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

/**
 * \brief Convert \c value to the corrsponding number of bytes.
 *
 * \param[in] value Value to convert
 * \param[in] unit  Unit of the value
 *
 * \return The equivalent number of bytes.
 */
int32_t to_bytes(const int32_t value, const AudioSize::UNIT unit) noexcept;

/**
 * \brief Convert \c bytes to the specified \c unit.
 *
 * \param[in] bytes Amount of bytes
 * \param[in] unit  Unit to convert to
 *
 * \return The equivalent value in the specified unit
 */
int32_t from_bytes(const int32_t bytes, const AudioSize::UNIT unit) noexcept;


/**
 * \brief Current state of a Calculation.
 */
class CalculationState
{
	virtual ChecksumSet do_current_subtotal() const
	= 0;

	virtual int32_t do_samples_processed() const noexcept
	= 0;

	virtual std::chrono::milliseconds do_proc_time_elapsed() const noexcept
	= 0;

	virtual void do_update(SampleInputIterator start, SampleInputIterator stop)
	= 0;

	virtual void do_increment_proc_time_elapsed(
			const std::chrono::milliseconds amount)
	= 0;

	/**
	 * \brief Advance calculation state by the specified amount of samples.
	 *
	 * \param[in] amount Amount of samples to advance
	 */
	virtual void do_advance(const int32_t amount)
	= 0;

public:

	CalculationState() = default;

	explicit CalculationState(Algorithm* const algorithm);

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~CalculationState() noexcept = default;

	ChecksumSet current_subtotal() const;

	/**
	 * \brief Returns the total number for PCM 32 bit samples yet processed.
	 *
	 * This value is equivalent to samples_expected() - samples_todo().
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	int32_t samples_processed() const noexcept;

	/**
	 * \brief Amount of milliseconds elapsed so far by processing.
	 *
	 * This includes the time of reading as well as of calculation.
	 *
	 * \return Amount of milliseconds elapsed so far by processing.
	 */
	std::chrono::milliseconds proc_time_elapsed() const noexcept;

	/**
	 * \brief Update the calculation state with an contigous amount of samples.
	 *
	 * \param[in] start First sample of update
	 * \param[in] stop  Sample behind last sample of update
	 */
	void update(SampleInputIterator start, SampleInputIterator stop);

	/**
	 * \brief Increment the amount of time elapsed.
	 *
	 * \param[in] amount Amount of milliseconds to advance
	 */
	void increment_proc_time_elapsed(const std::chrono::milliseconds amount);
};


/**
 * \brief Updates a calculation process by a sample block.
 *
 * \tparam B Type of the iterator pointing to the start position
 * \tparam E Type of the iterator pointing to the stop position
 *
 * \param[in]     start         Iterator pointing to first sample in block
 * \param[in]     stop          Iterator pointing to last sample in block
 * \param[in]     partitioner   Partition provider
 * \param[in,out] state         Current calculation state
 * \param[in,out] result_buffer Collect the results
 */
void calc_update(SampleInputIterator start, SampleInputIterator stop,
		const Partitioner& partitioner,
		CalculationState&  state,
		Checksums&         result_buffer);


//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

/**
 * \brief Calculation state.
 *
 * \details
 *
 * The calculation state is a storage wrapper for the current calculation state.
 */
class CalculationStateImpl final : public CalculationState
{
	/**
	 * \internal
	 * \brief Internal 0-based sample offset.
	 */
	Counter<int32_t> sample_offset_;

	/**
	 * \internal
	 * \brief Time elapsed by updating.
	 */
	Counter<std::chrono::milliseconds> proc_time_elapsed_;

	/**
	 * \internal
	 * \brief Algorithm to caculate updates.
	 */
	Algorithm* algorithm_;


	ChecksumSet do_current_subtotal()  const final;
	int32_t     do_samples_processed() const noexcept final;
	std::chrono::milliseconds do_proc_time_elapsed() const noexcept final;
	void do_update(SampleInputIterator start, SampleInputIterator stop) final;
	void do_increment_proc_time_elapsed(const std::chrono::milliseconds amount)
		final;
	void do_advance(const int32_t amount) final;

public:

	CalculationStateImpl();
	explicit CalculationStateImpl(Algorithm* const algorithm);
	~CalculationStateImpl() noexcept final = default;
};

#pragma GCC diagnostic pop


} // namespace details


/**
 * \brief Private implementation of a Calculation.
 */
class Calculation::Impl
{
	// Public input for construction:
	std::unique_ptr<Algorithm>                  algorithm_;

	// Internal input for construction:
	// constructed, controlled and destroyed by calculation:
	const std::unique_ptr<details::Partitioner> partitioner_;
	std::unique_ptr<details::CalculationState>  state_;
	std::unique_ptr<Checksums>                  result_buffer_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] algorithm The algorithm to use for calculating
	 * \param[in] toc       TOC to perform calculation for
	 * \param[in] size      Total size of the audio input
	 */
	Impl(std::unique_ptr<Algorithm> algorithm, const TOC& toc,
			const AudioSize& size);

	const Algorithm* algorithm() const noexcept;

	int64_t samples_expected() const noexcept;

	int64_t samples_processed() const noexcept;

	int64_t samples_todo() const noexcept;

	std::chrono::milliseconds proc_time_elapsed() const noexcept;

	bool complete() const noexcept;

	void update(SampleInputIterator begin, SampleInputIterator end);

	void update_audiosize(const AudioSize &audiosize);

	Checksums result() const noexcept;
};

} // namespace v_1_0_0
} // namespace arcstk

#endif

