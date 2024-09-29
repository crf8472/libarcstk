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
 * \brief Updates a calculation process by a sample block.
 *
 * \tparam B Type of the iterator pointing to the start position
 * \tparam E Type of the iterator pointing to the stop position
 *
 * \param[in]     start         Iterator pointing to first sample in block
 * \param[in]     stop          Iterator pointing to last sample in block
 * \param[in]     last_sample   Index of the leadout or last sample expected
 * \param[in]     partitioner   Partition provider
 * \param[in,out] state         Current calculation state
 * \param[in,out] result_buffer Collect the results
 */
void calc_update(SampleInputIterator start, SampleInputIterator stop,
		//const int32_t last_sample,
		const Partitioner& partitioner,
		CalculationState& state,
		ChecksumBuffer& result_buffer);


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
	 * \brief Internal total samples expected.
	 */
	int32_t total_samples_expected_;

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
	void        do_set_samples_expected(const int32_t total_expected) final;
	int32_t     do_samples_expected()  const noexcept final;
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

} // namespace details


/**
 * \brief Private implementation of a Calculation.
 */
class Calculation::Impl
{
	// Public input for construction:
	const Algorithm*                   algorithm_;

	// Internal input for construction:
	const std::unique_ptr<details::Partitioner> partitioner_;

	// constructed, controlled and destroyed by calculation:
	std::unique_ptr<ChecksumBuffer>    result_buffer_;
	std::unique_ptr<CalculationState>  state_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] algorithm The algorithm to use for calculating
	 * \param[in] toc       TOC to perform calculation for
	 * \param[in] size      Total size of the audio input
	 */
	Impl(const Algorithm* algorithm, const TOC& toc, const AudioSize& size);

	const Algorithm& algorithm() const noexcept;

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

