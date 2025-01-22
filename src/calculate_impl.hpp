#ifndef __LIBARCSTK_CALCULATE_IMPL_HPP__
#define __LIBARCSTK_CALCULATE_IMPL_HPP__
/**
 * \file
 *
 * \brief Implementations of calculate.hpp that depend on the public header.
 */

#include <cstdint>       // for int32_t
#include <memory>        // for unique_ptr

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


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

/**
 * \brief Current state of a Calculation.
 *
 * A CalculationState provides the relevant counters for samples and time. It
 * updates the algorithm and provides the current subtotal.
 */
class CalculationState
{
	/**
	 * \brief Internal 0-based current sample offset.
	 */
	Counter<int32_t> current_offset_;

	/**
	 * \brief Internal 0-based counter for samples processed.
	 */
	Counter<int32_t> samples_processed_;

	/**
	 * \brief Internal time elapsed by updating.
	 */
	Counter<std::chrono::milliseconds> proc_time_elapsed_;

	/**
	 * \brief Internal Algorithm to caculate updates.
	 */
	Algorithm* algorithm_;


	virtual int32_t do_current_offset() const noexcept;

	virtual int32_t do_samples_processed() const noexcept;

	/**
	 * \brief Advance calculation state by the specified amount of samples.
	 *
	 * \param[in] amount Amount of samples to advance
	 */
	virtual void do_advance(const int32_t amount);

	virtual std::chrono::milliseconds do_proc_time_elapsed() const noexcept;

	virtual void do_increment_proc_time_elapsed(
			const std::chrono::milliseconds amount);

	virtual void do_update(SampleInputIterator start, SampleInputIterator stop);

	virtual ChecksumSet do_current_subtotal() const;

	virtual std::unique_ptr<CalculationState> do_clone() const
	= 0;

	virtual std::unique_ptr<CalculationState> do_clone_to(Algorithm* a) const
	= 0;

public:

	/**
	 * \brief Default constructor.
	 */
	CalculationState();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] algorithm The update algorithm to be used.
	 */
	explicit CalculationState(Algorithm* const algorithm);

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~CalculationState() noexcept;

	int32_t current_offset() const noexcept;

	void advance(const int32_t amount);

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
	 * \brief Returns the algorithm instance used by the state.
	 *
	 * \return Algorithm instance used by thi state
	 */
	const Algorithm* algorithm() const noexcept;

	/**
	 * \brief Amount of milliseconds elapsed so far by processing.
	 *
	 * This includes the time of reading as well as of calculation.
	 *
	 * \return Amount of milliseconds elapsed so far by processing.
	 */
	std::chrono::milliseconds proc_time_elapsed() const noexcept;

	/**
	 * \brief Increment the amount of time elapsed.
	 *
	 * \param[in] amount Amount of milliseconds to advance
	 */
	void increment_proc_time_elapsed(const std::chrono::milliseconds amount);

	/**
	 * \brief Update the calculation state with an contigous amount of samples.
	 *
	 * \param[in] start First sample of update
	 * \param[in] stop  Sample behind last sample of update
	 */
	void update(SampleInputIterator start, SampleInputIterator stop);

	/**
	 * \brief Current subtotal as provided by the Algorithm.
	 *
	 * \return Current subtotal.
	 */
	ChecksumSet current_subtotal() const;

	std::unique_ptr<CalculationState> clone() const;
	std::unique_ptr<CalculationState> clone_to(Algorithm* a) const;

	friend void swap(CalculationState& lhs, CalculationState& rhs) noexcept;

protected:

	explicit CalculationState(const CalculationState& rhs);

	explicit CalculationState(CalculationState&& rhs) noexcept;

	void set_algorithm(Algorithm* const algorithm) noexcept;
};

#pragma GCC diagnostic pop


// CalculationStateImpl


/**
 * \brief Default implementation of a CalculationState.
 */
class CalculationStateImpl final : public CalculationState
{
	std::unique_ptr<CalculationState> do_clone() const final;
	std::unique_ptr<CalculationState> do_clone_to(Algorithm* a) const final;

	std::unique_ptr<CalculationStateImpl> raw_clone() const;

public:

	/**
	 * \brief Constructor with Algorithm.
	 */
	explicit CalculationStateImpl(Algorithm* const algorithm);

	explicit CalculationStateImpl(const CalculationStateImpl& rhs);

	CalculationStateImpl& operator = (const CalculationStateImpl& rhs);

	explicit CalculationStateImpl(CalculationStateImpl&& rhs) noexcept;

	CalculationStateImpl& operator = (CalculationStateImpl&& rhs) noexcept;

	/**
	 * \brief Destructor.
	 */
	~CalculationStateImpl() noexcept final;

	friend void swap(CalculationStateImpl& lhs, CalculationStateImpl& rhs)
		noexcept;
};


/**
 * \brief Updates a calculation process by a sample block.
 *
 * \param[in]     start         Iterator pointing to first sample in block
 * \param[in]     stop          Iterator pointing to last sample in block
 * \param[in]     partitioner   Partition provider
 * \param[in,out] state         Current calculation state
 * \param[in,out] result_buffer Collect the results
 */
void perform_update(SampleInputIterator start, SampleInputIterator stop,
		const Partitioner& partitioner,
		CalculationState&  state,
		Checksums&         result_buffer);

} // namespace details


/**
 * \brief Private implementation of a Calculation.
 */
class Calculation::Impl final
{
	Settings                                    settings_;
	std::unique_ptr<details::Partitioner>       partitioner_;
	std::unique_ptr<Checksums>                  result_buffer_;
	std::unique_ptr<Algorithm>                  algorithm_;
	std::unique_ptr<details::CalculationState>  state_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] algorithm The algorithm to use in update()
	 */
	explicit Impl(std::unique_ptr<Algorithm> algorithm);

	Impl(const Impl& rhs);
	Impl& operator=(const Impl& rhs);

	Impl(Impl&& rhs) noexcept;
	Impl& operator=(Impl&& rhs) noexcept;

	~Impl() noexcept;

	// Impl specific

	void init(const Settings& s, const AudioSize& size,
		const std::vector<int32_t>& points);

	std::unique_ptr<details::CalculationStateImpl> init_state(
		Algorithm* const algorithm);

	std::unique_ptr<Checksums> init_buffer();

	// Calculation

	void set_settings(const Settings& s) noexcept;

	const Settings& settings() const noexcept;

	void set_algorithm(std::unique_ptr<Algorithm> algorithm) noexcept;

	const Algorithm* algorithm() const noexcept;

	int32_t samples_expected() const noexcept;

	int32_t samples_processed() const noexcept;

	std::chrono::milliseconds proc_time_elapsed() const noexcept;

	bool complete() const noexcept;

	void update(SampleInputIterator begin, SampleInputIterator end);

	void update(const AudioSize &audiosize);

	Checksums result() const noexcept;
};

} // namespace v_1_0_0
} // namespace arcstk

#endif

