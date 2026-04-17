#ifndef LIBARCSTK_CALCULATE_HPP_
#error "Do not include calculate_details.hpp, include calculate.hpp instead"
#endif

#ifndef LIBARCSTK_CALCULATE_DETAILS_HPP_
#define LIBARCSTK_CALCULATE_DETAILS_HPP_

/**
 * \internal
 *
 * \file
 *
 * \brief Implementation details for calculate.hpp.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"
#endif

#include <chrono>        // for duration
#include <cstddef>       // for size_t
#include <cstdint>       // for int32_t
#include <memory>        // for unique_ptr
#include <utility>       // for pair
#include <string>        // for string
#include <vector>        // for vector

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"  // for AudioSize
#endif


namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

// avoid includes
class Checksum;
class ChecksumSet;

// duplicate of checksum.hpp, documented there
using Checksums = std::vector<ChecksumSet>;

// duplicate of calculate.hpp, documented there
using Points  = std::vector<AudioSize>;

namespace details
{

/**
 * \internal
 * \ingroup calc
 *
 * \brief A closed interval <tt>[a,b]</tt>.
 *
 * \tparam T Type with definition of <=
 */
template<typename T>
class Interval final
{
	/**
	 * \brief First number in interval.
	 */
	T a_;

	/**
	 * \brief Last number in interval.
	 */
	T b_;

public:

	/**
	 * \brief Constructor for <tt>[a,b]</tt>.
	 *
	 * \param[in] a First number in closed interval
	 * \param[in] b Last number in closed interval
	 */
	Interval(const T a, const T b)
		: a_ { a }
		, b_ { b }
	{
		// empty
	}

	/**
	 * \brief Constructor for <tt>[a,b]</tt>.
	 *
	 * \param[in] pair Pair of bounds in closed interval
	 */
	explicit Interval(const std::pair<T,T>& pair)
		: Interval { pair.first, pair.second }
	{
		// empty
	}

	/**
	 * \brief Smallest value of the interval.
	 *
	 * \return Smallest value of the interval
	 */
	T lower() const
	{
		return a_ <= b_ ? a_ : b_;
	}

	/**
	 * \brief Greatest value of the interval.
	 *
	 * \return Greates value of the interval
	 */
	T upper() const
	{
		return a_ <= b_ ? b_ : a_;
	}

	/**
	 * \brief Returns TRUE iff the closed interval contains \c i, otherwise
	 * FALSE.
	 *
	 * \param[in] i Number to test for containment in interval
	 *
	 * \return TRUE iff \c i is contained in the Interval, otherwise FALSE
	 */
	bool contains(const T& i) const
	{
		return (a_ <= b_) ? a_ <= i && i <= b_ : b_ <= i && i <= a_;
	}

	/**
	 * \brief Return a string representation of the interval.
	 *
	 * \return Interval as a string
	 */
	std::string to_string() const
	{
		using std::to_string;

		return "[" + to_string(lower()) + "," + to_string(upper()) + "]";
	}
};

/**
 * \brief Range of samples.
 */
using SampleRange = Interval<int32_t>;


// Forward Declaration Required for Partitioner
class Partition;

/**
 * \internal
 * \brief Type of the partitioning of a range of samples.
 */
using Partitioning = std::vector<Partition>;

/**
 * \brief Create a partitioning for an interval in a legal range by a sequence
 * of points.
 *
 * \param[in] interval	Interval to create a partitioning for
 * \param[in] legal		Relevant range within the interval
 * \param[in] points    Points to define partition bounds
 */
Partitioning get_partitioning(
		const SampleRange& interval,
		const SampleRange& legal,
		const Points& points);

/**
 * \brief Create a single partition for an interval in a legal range.
 *
 * \param[in] interval	Interval to create a partitioning for
 * \param[in] legal		Relevant range within the interval
 */
Partitioning get_partitioning(
		const SampleRange& interval,
		const SampleRange& legal);

/**
 * \internal
 * \ingroup calc
 *
 * \brief Interface for generating a partitioning over a sequence of samples.
 *
 * The partitioning is done along the track bounds according to the ToC such
 * that every two partitions adjacent within the same sequence belong to
 * different tracks. This way it is possible to entirely avoid checking for
 * track bounds within the checksum calculation loop.
 */
class Partitioner
{
public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] total_samples Total number of samples expected in input
	 * \param[in] points        List of splitting points
	 * \param[in] legal         Legal range of calculation
	 */
	Partitioner(const AudioSize& total_samples, const Points& points,
			const SampleRange& legal);

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~Partitioner() noexcept = default;

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	Partitioner(const Partitioner& rhs) = default;

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	Partitioner& operator = (const Partitioner& rhs) = delete;

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	Partitioner(Partitioner&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	Partitioner& operator = (Partitioner&& rhs) noexcept = delete;

	/**
	 * \brief Generates partitioning of the range of samples.
	 *
	 * \param[in] offset                 Offset of the first sample
	 * \param[in] total_samples_in_block Number of samples in the block
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	Partitioning create_partitioning(
			const int32_t offset,
			const int32_t total_samples_in_block) const;

	/**
	 * \brief Total number of samples.
	 *
	 * \return Total number of samples
	 */
	AudioSize total_samples() const;

	/**
	 * \brief Set total number of samples.
	 *
	 * Maybe necessary when reading the last block reveals a different number of
	 * samples than expected.
	 *
	 * \param[in] total_samples Total number of samples
	 */
	void set_total_samples(const AudioSize& total_samples);

	/**
	 * \brief Legal range to occurr in partitions.
	 *
	 * The physical range of input samples may be bigger.
	 *
	 * \return The legal range of samples to be partitioned.
	 */
	SampleRange legal_range() const;

	/**
	 * \brief Partitioning bounds.
	 *
	 * \return Points to separate partitions.
	 */
	Points points() const;

	/**
	 * \copydoc SNPT_mf_clone
	 */
	std::unique_ptr<Partitioner> clone() const;

private:

	/**
	 * \brief Implements Partitioner::create_partitioning() with a ToC.
	 *
	 * \param[in] current_interval Interval to build partitions from
	 * \param[in] legal_range      Legal interval to process
	 * \param[in] points           Splitting points
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	virtual Partitioning do_create_partitioning(
		const SampleRange& current_interval,
		const SampleRange& legal_range,
		const Points& points) const
	= 0;

	virtual std::unique_ptr<Partitioner> do_clone() const
	= 0;

	/**
	 * \brief Total number of samples expected.
	 */
	AudioSize total_samples_;

	/**
	 * \brief Internal splitting points.
	 */
	Points points_;

	/**
	 * \brief Legal range of partitioning.
	 */
	SampleRange legal_;
};

/**
 * \brief Provides partitions along track bounds.
 */
class TrackPartitioner final : public Partitioner
{
	Partitioning do_create_partitioning(
		const SampleRange& sample_block,
		const SampleRange& relevant_interval,
		const Points& points) const final;

	std::unique_ptr<Partitioner> do_clone() const final;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] total_samples Total number of samples expected in input
	 * \param[in] points        List of splitting points
	 * \param[in] legal         Legal range of calculation
	 */
	TrackPartitioner(const AudioSize& total_samples, const Points& points,
			const SampleRange& legal);
};

/**
 * \brief Type to represent 1-based track numbers.
 *
 * A signed integer type.
 *
 * Valid track numbers are in the range of 1-99. Note that 0 is not a valid
 * TrackNo. Hence, a TrackNo is not suitable to represent a total number of
 * tracks or a counter for tracks.
 *
 * The intention of this typedef is to provide a marker for parameters that
 * expect 1-based track numbers instead of 0-based track indices. TrackNo will
 * not occurr as a return type in the API.
 *
 * A validation check is not provided, though. Every function that accepts a
 * TrackNo will in fact accept 0 but will then either throw or return a default
 * error value.
 *
 * It is not encouraged to use TrackNo in client code.
 */
using TrackNo = int;

/**
 * \ingroup calc
 *
 * \brief A contigous part of a sequence of samples.
 *
 * A partition does not hold any samples but provides access to a slice of the
 * underlying sequence of samples.
 */
class Partition final
{
	// Partitioners are friends of Partition since they construct
	// Partitions exclusively

	friend Partitioner;

	// NOTE: There is no default constructor since Partition have constant
	// elements that cannot be default initialized

	/**
	 * \brief Relative offset of the first sample in this partition
	 */
	int32_t begin_offset_;

	/**
	 * \brief Relative offset of the last sample in this partition + 1
	 */
	int32_t end_offset_;

	/**
	 * \brief TRUE iff the first sample in this partition is also the first
	 * sample in the track
	 */
	bool starts_track_;

	/**
	 * \brief TRUE iff the last sample in this partition is also the last sample
	 * in the track
	 */
	bool ends_track_;

	/**
	 * \brief 1-based number of the track of which the samples in the partition
	 * are part of
	 */
	int track_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] begin_offset Local index of the first sample in the partition
	 * \param[in] end_offset   Local index of the last sample in the partition
	 * \param[in] starts_track TRUE iff this partition starts its track
	 * \param[in] ends_track   TRUE iff this partition ends its track
	 * \param[in] track        Number of the track that contains the partition
	 */
	Partition(
			const int32_t& begin_offset,
			const int32_t& end_offset,
			const bool&    starts_track,
			const bool&    ends_track,
			const TrackNo& track);

	/**
	 * \brief Relative offset of the first sample in the partition.
	 *
	 * \return Relative offset of the first sample in the partition.
	 */
	int32_t begin_offset() const;

	/**
	 * \brief Relative offset of the last sample in the partition + 1.
	 *
	 * \return Relative offset of the last sample in the partition + 1.
	 */
	int32_t end_offset() const;

	/**
	 * \brief Returns TRUE iff the first sample of this partition is also the
	 * first sample of the track which the partition is part of.
	 *
	 * \return TRUE iff this is partition starts a track
	 */
	bool starts_track() const;

	/**
	 * \brief Returns TRUE if the last sample of this partition is also the last
	 * sample of the track which the partition is part of.
	 *
	 * \return TRUE iff this is partition ends a track
	 */
	bool ends_track() const;

	/**
	 * \brief The track of which the samples in the partition are part of.
	 *
	 * \return The track that contains this partition
	 */
	int track() const;

	/**
	 * \brief Number of samples in this partition.
	 *
	 * \return Number of samples in this partition
	 */
	std::size_t size() const;
};

/**
 * \brief Class template for an incrementable and readable counter.
 *
 * \tparam T Type with definition of +=
 */
template<typename T>
class Counter final
{
	/**
	 * \brief Internal counter value.
	 */
	T value_;

public:

	/**
	 * \brief Type of the counter value.
	 */
	using type = T;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] value Start value
	 */
	explicit Counter(const T& value)
		: value_ { value }
	{
		// empty
	}

	/**
	 * \brief Counter value.
	 *
	 * \return Current value of the Counter
	 */
	T value() const noexcept
	{
		return value_;
	}

	/**
	 * \brief Increment the counter by the specified amount.
	 *
	 * \param[in] amount Amount to increment the Counter.
	 */
	void increment(T amount)
	{
		value_ += amount;
	}

	/**
	 * \brief Reset the counted value to the default initializer of T..
	 */
	void reset()
	{
		value_ = T{};
	}
};

/**
 * \brief Convert a 0-based sample index to an equivalent amount of samples.
 *
 * \param[in] index The index to convert to an amount
 *
 * \return Amount of samples equivalent to the index passed
 */
int32_t ind2am(const int32_t index);

/**
 * \brief Convert a 1-based amount of samples to an equivalent index.
 *
 * \param[in] amount The amount to convert to an index
 *
 * \return Sample index equivalent to the amount passed
 */
int32_t am2ind(const int32_t amount);

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
	 * \brief Internal 0-based counter for track samples processed.
	 */
	Counter<int32_t> track_samples_processed_;

	/**
	 * \brief Internal 0-based counter for tracks..
	 */
	Counter<int32_t> tracks_processed_;

	/**
	 * \brief Internal time elapsed by processing.
	 */
	std::chrono::duration<float> algo_time_elapsed_;

	/**
	 * \brief Internal time elapsed by updating.
	 */
	std::chrono::duration<float> update_time_elapsed_;

	/**
	 * \brief Internal Algorithm to caculate updates.
	 */
	Algorithm* algorithm_;


	virtual void do_advance(const int32_t amount);

	virtual void do_update(SampleInputIterator start, SampleInputIterator stop);

	virtual ChecksumSet do_current_subtotal() const;

	virtual void do_track_finished();

	virtual std::unique_ptr<CalculationState> do_clone() const
	= 0;

	virtual std::unique_ptr<CalculationState> do_clone_to(Algorithm* a) const
	= 0;

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	CalculationState();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] algorithm The update algorithm to be used.
	 */
	explicit CalculationState(Algorithm* const algorithm);

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~CalculationState() noexcept = default;

	/**
	 * \brief Offset of the current sample.
	 *
	 * This sample is not yet processed but will be the next sample to process.
	 *
	 * \return Current sample
	 */
	int32_t current_offset() const noexcept;

	/**
	 * \brief Advance by some amount to a higher current offset.
	 *
	 * \param[in] amount Amount (in samples) to advance
	 */
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
	 * \brief Amount of milliseconds elapsed so far by Algorithm::update().
	 *
	 * \return Amount of milliseconds elapsed so far by the Algorithm instance.
	 */
	std::chrono::duration<float> algo_time_elapsed() const noexcept;

	/**
	 * \brief Amount of milliseconds elapsed so far by updating this instance.
	 *
	 * \return Amount of milliseconds elapsed so far by updating.
	 */
	std::chrono::duration<float> update_time_elapsed() const noexcept;

	/**
	 * \brief Increment the duration for updating.
	 *
	 * \param[in] duration Amount of duration to add
	 */
	void increment_update_time_elapsed(
			const std::chrono::duration<float>& duration);

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

	/**
	 * \brief Mark track as finished.
	 */
	void track_finished();

	/**
	 * \copydoc SNPT_mf_clone
	 */
	std::unique_ptr<CalculationState> clone() const;

	/**
	 * \brief Clone this instance but use another Algorithm instance.
	 *
	 * \return Deep copy of this instance with another Algorithm instance.
	 */
	std::unique_ptr<CalculationState> clone_to(Algorithm* a) const;

protected:

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	CalculationState(const CalculationState& rhs) = default;

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	CalculationState& operator = (const CalculationState& rhs) = default;

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	CalculationState(CalculationState&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	CalculationState& operator = (CalculationState&& rhs) noexcept = default;

	/**
	 * \brief Swap abstract part of the concrete subclass..
	 *
	 * \param[in] rhs Other instance to swap
	 */
	void swap_base(CalculationState& rhs);

	/**
	 * \brief Set the Algorithm of this instance.
	 *
	 * \param[in] algorithm Algorithm instance to set
	 */
	void set_algorithm(Algorithm* const algorithm) noexcept;
};


// CalculationStateImpl


/**
 * \brief Default implementation of a CalculationState.
 */
class CalculationStateImpl final : public CalculationState
{
	/**
	 * \brief Clone this instance.
	 *
	 * \return Deep copy of this instance
	 */
	std::unique_ptr<CalculationState> do_clone() const final;

	/**
	 * \brief Clone this instance and use another Algorithm instance.
	 *
	 * \param[in] a Algorithm instance to set to the deep copy
	 *
	 * \return Deep copy of this instance
	 */
	std::unique_ptr<CalculationState> do_clone_to(Algorithm* a) const final;

	/**
	 * \brief Only clone the base class part.
	 */
	std::unique_ptr<CalculationStateImpl> base_clone() const;

public:

	/**
	 * \brief Constructor with Algorithm.
	 *
	 * \param[in] algorithm Algorithm instance to set
	 */
	explicit CalculationStateImpl(Algorithm* const algorithm);

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~CalculationStateImpl() noexcept final = default;

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(CalculationStateImpl& lhs, CalculationStateImpl& rhs)
		noexcept
	{
		lhs.swap_base(rhs);
	}
};


/**
 * \brief Updates a calculation process by a sample block.
 *
 * \param[in]     start         Iterator pointing to first sample in block
 * \param[in]     stop          Iterator pointing behind last sample in block
 * \param[in]     partitioner   Partition provider
 * \param[in,out] state         Current calculation state
 * \param[in,out] result_buffer Buffer for collecting results
 *
 * \return FALSE iff more updates are required, otherwise TRUE
 */
bool perform_update(const SampleInputIterator& start,
		const SampleInputIterator& stop,
		const Partitioner& partitioner,
		CalculationState&  state,
		Checksums&         result_buffer);


/**
 * \brief Wrapper for perform_update that adds time measuring.
 *
 * The state is incremented by the update time elapsed.
 *
 * \param[in]     start         Iterator pointing to first sample in block
 * \param[in]     stop          Iterator pointing behind last sample in block
 * \param[in]     partitioner   Partition provider
 * \param[in,out] state         Current calculation state
 * \param[in,out] result_buffer Buffer for collecting results
 *
 * \return FALSE iff more updates are required, otherwise TRUE
 */
bool perform_update_profiled(const SampleInputIterator& start,
		const SampleInputIterator& stop,
		const Partitioner& partitioner,
		CalculationState&  state,
		Checksums&         result_buffer);

} // namespace details

/**
 * \brief Private implementation of a Calculation.
 */
class Calculation::Impl final
{
	/**
	 * \brief Internal settings for this calculation.
	 */
	Settings settings_;

	/**
	 * \brief Partitioner to provide the stop positions.
	 */
	std::unique_ptr<details::Partitioner> partitioner_;

	/**
	 * \brief Collect calculated checksums.
	 */
	std::unique_ptr<Checksums> result_buffer_; // TODO Why pointer?

	/**
	 * \brief Algorithm to calculate checksums.
	 */
	std::unique_ptr<Algorithm> algorithm_;

	/**
	 * \brief Internal current calculation state.
	 */
	std::unique_ptr<details::CalculationState> state_;

	/**
	 * \brief Hook: called after the update sequence is completed.
	 */
	void completed();

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] algorithm The algorithm to use in update()
	 */
	explicit Impl(std::unique_ptr<Algorithm> algorithm);

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	Impl(const Impl& rhs);

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	Impl& operator = (const Impl& rhs) = delete;

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	Impl(Impl&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	Impl& operator = (Impl&& rhs) noexcept = delete;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~Impl() noexcept = default;


	// Impl specific

	/**
	 * \brief Initialize the instance.
	 *
	 * \param[in] s      Settings for this instance
	 * \param[in] toc    ToC
	 */
	void init(const Settings& s, const ToCData& toc);

	/**
	 * \brief Initialize the instance.
	 *
	 * \param[in] s      Settings for this instance
	 * \param[in] size   Total size of the expected input
	 * \param[in] points Track offsets (as sample indices)
	 */
	void init(const Settings& s, const AudioSize& size, const Points& points);

	/**
	 * \brief Initializing worker to create the internal state.
	 *
	 * \param[in] algorithm Algorithm instance to initialize the state with
	 *
	 * \return Initialized CalculationState instance
	 */
	std::unique_ptr<details::CalculationStateImpl> init_state(
		Algorithm* const algorithm);

	/**
	 * \brief Initializing worker to create the internal result buffer.
	 *
	 * \return Initialized Checksums instance
	 */
	std::unique_ptr<Checksums> init_buffer();


	// Calculation

	void set_settings(const Settings& s) noexcept;

	const Settings& settings() const noexcept;

	void set_algorithm(std::unique_ptr<Algorithm> algorithm) noexcept;

	const Algorithm* algorithm() const noexcept;

	int32_t samples_expected() const noexcept;

	int32_t samples_processed() const noexcept;

	std::chrono::duration<float> update_time_elapsed() const noexcept;

	std::chrono::duration<float> algo_time_elapsed() const noexcept;

	bool complete() const noexcept;

	void update(SampleInputIterator& begin, SampleInputIterator& end);

	void update(const AudioSize& audiosize);

	Checksums result() const noexcept;
};

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

