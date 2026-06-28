#ifndef LIBARCSTK_CALCULATE_HPP_
#define LIBARCSTK_CALCULATE_HPP_

/**
 * \file
 *
 * \brief Public API for \link calc AccurateRip checksum calculation\endlink.
 *
 * \details
 *
 * Provides the API for \link calc calculating AccurateRip checksums\endlink.
 */

#include <chrono>           // for duration
#include <cstddef>          // for ptrdiff_t
#include <cstdint>          // for int32_t
#include <functional>       // for function
#include <iterator>         // for advance, input_iterator_tag
#include <map>              // for map
#include <memory>           // for make_unique, unique_ptr
#include <sstream>          // for ostream
#include <string>           // for string
#include <type_traits>      // for decay_t, enable_if_t, is_same, decay
#include <unordered_set>    // for unordered_set
#include <utility>          // for declval, move, pair
#include <vector>           // for vector

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"    // for ChecksumtypeSet, Points, Algorithm, Context
#endif
#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"   // for AccurateRip::V1andV2, ::V1, ::V2
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"     // for ChecksumSet, Checksums
#endif
#ifndef LIBARCSTK_MIXINS_HPP_
#include "mixins.hpp"       // for Comparable
#endif
#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples.hpp"      // for SampleSequence
#endif
#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"
#endif

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
// avoid includes
class AudioSize;
class ToC;

// redefined as in metadata.hpp, documented there
using ToCData = std::vector<AudioSize>;


/** \defgroup calc AccurateRip Checksum Calculation
 *
 * \brief Calculate checksums of audio tracks.
 *
 * \details
 *
 * AccurateRip checksums are calculated from a sequence of input sample
 * sequences. The caller is responsible for determining an Algorithm and
 * providing the entire sequence of audio sample sequences to a Calculation
 * instance. The result is provided in a Checksums instance.
 *
 * An Algorithm specifies a method to calculate Checksums over a sequence of
 * audio samples. AccurateRip specifies two distinct algorithms for calculating
 * a checksum, v1 and v2. A v1 checksum can be materialized as a subtotal when
 * calculating a v2 checksum. Therefore a calculation of a v2 value can also
 * provide the v1 value for the input. Hence there are three variants of the
 * Algorithm available: \e V1, \e V2 and \e V1and2. The latter provides v1 as
 * well as v2.
 *
 * Class Settings provides an interface for configuring an Algorithm or the
 * calculation process.
 *
 * As part of the Settings there exists a Context in which the Calculation is
 * performed. The Algorithm is aware of the Context. The Context indicates if
 * either FIRST_TRACK, LAST_TRACK, or both have to be treated specially when
 * calculating.
 *
 * A Calculation represents the technical process of calculating Checksums by an
 * Algorithm. It has to be parametized with an Algorithm, initialized with the
 * offsets and the leadout of the audio image and then subsequently be updated
 * with portions of samples in their correct order. A Calculation works fine
 * with the default settings, but can be finetuned by providing custom Settings.
 *
 * Updating a Calculation with an actual sequence of samples is done by
 * providing a sequence of samples represented by two iterators. Those iterators
 * represent start and stop of the update. Any LegacyInputIterator with a \c
 * value_type of csample_t is allowed. Type csample_t is the declared type for
 * PCM 32 bit samples. Using a SampleSequence may be of convenience for
 * establishing compatibility of the sample input format.
 *
 * As soon as a Calculation is \link arcstk::Calculation::complete() complete()
 * \endlink its result can be provided. The resulting Checksums represent the
 * result for all requested checksum types and all tracks of the audio input. It
 * is an aggregation of the ChecksumSet for each track of an respective audio
 * input. Depending on the input, it can represent either an entire album or a
 * single track.
 *
 * ChecksumSet is a set of \link arcstk::Checksum Checksums \endlink of
 * different \link arcstk::checksum::type checksum::types \endlink of the same
 * track.
 *
 * A Checksum refers to a particular track and a particular checksum::type.
 * Checksums are calculated by updating a Calculation with a sequence of sample
 * sequences.
 *
 * @{
 */

/**
 * \brief Represent a "combined" PCM stereo sample (of two CDDA samples).
 *
 * An unsigned integer of exact 32 bit length.
 *
 * The type is not intended to do arithmetic operations on it.
 *
 * Bitwise operators are guaranteed to work as on unsigned types.
 */
using csample_t = uint32_t; // also defined in samples.hpp


namespace details
{

/**
 * \brief Get value_type of Iterator.
 *
 * \tparam Iterator Iterator type to test
 */
template <typename Iterator>
using it_value_type = std::decay_t<decltype( *std::declval<Iterator>() )>;
// This is SFINAE compatible and respects bare pointers, which would not
// have been respected when using std::iterator_traits<Iterator>::value_type.
// Nonetheless I am not quite sure whether bare pointers indeed should be used
// in this context at the first place.

/**
 * \brief Check a given Iterator whether it iterates over type T.
 *
 * \tparam Iterator Iterator type to test
 * \tparam T        Type to test for
 */
template <typename Iterator, typename T>
using is_iterator_over = std::is_same< it_value_type<Iterator>, T >;

/**
 * \brief Defined iff \c Iterator is an iterator over \c csample_t.
 *
 * \tparam Iterator Iterator type to test
 */
template <typename Iterator>
using is_sample_iterator = std::is_same<it_value_type<Iterator>, csample_t>;

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
	T a_ {};

	/**
	 * \brief Last number in interval.
	 */
	T b_ {};

public:

	/**
	 * \brief Default constructor.
	 */
	Interval() = default;


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
 *
 * \return Partitioning
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
 *
 * \return Partitioning
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
	 * \param[in] points        List of splitting points
	 * \param[in] total_samples Total number of samples expected in input
	 * \param[in] legal         Legal range of calculation
	 */
	Partitioner(const Points& points, const AudioSize& total_samples,
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
	AudioSize total_samples() const noexcept;

	/**
	 * \brief Set total number of samples.
	 *
	 * Maybe necessary when reading the last block reveals a different number of
	 * samples than expected.
	 *
	 * \param[in] total_samples Total number of samples
	 */
	void set_total_samples(const AudioSize& total_samples) noexcept;

	/**
	 * \brief Legal range to occurr in partitions.
	 *
	 * The physical range of input samples may be bigger.
	 *
	 * \return The legal range of samples to be partitioned.
	 */
	SampleRange legal_range() const noexcept;

	/**
	 * \brief Partitioning bounds.
	 *
	 * \return Points to separate partitions.
	 */
	Points points() const noexcept;

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
	 * \brief Internal splitting points.
	 */
	Points points_ {};

	/**
	 * \brief Total number of samples expected.
	 */
	AudioSize total_samples_ {};

	/**
	 * \brief Legal range of partitioning.
	 */
	SampleRange legal_ {};
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
	 * \param[in] points        List of splitting points
	 * \param[in] total_samples Total number of samples expected in input
	 * \param[in] legal         Legal range of calculation
	 */
	TrackPartitioner(const Points& points, const AudioSize& total_samples,
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
	int32_t begin_offset_ {};

	/**
	 * \brief Relative offset of the last sample in this partition + 1
	 */
	int32_t end_offset_ {};

	/**
	 * \brief TRUE iff the first sample in this partition is also the first
	 * sample in the track
	 */
	bool starts_track_ {};

	/**
	 * \brief TRUE iff the last sample in this partition is also the last sample
	 * in the track
	 */
	bool ends_track_ {};

	/**
	 * \brief 1-based number of the track of which the samples in the partition
	 * are part of
	 */
	int track_ {};

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
			const int32_t begin_offset,
			const int32_t end_offset,
			const bool    starts_track,
			const bool    ends_track,
			const TrackNo track);

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
	T value_ {};

public:

	/**
	 * \brief Type of the counter value.
	 */
	using type = T;

	/**
	 * \brief Default constructor.
	 */
	Counter() = default;

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
class CalculationState final
{
	/**
	 * \brief Internal 0-based current sample offset.
	 */
	Counter<int32_t> current_offset_ {};

	/**
	 * \brief Internal 0-based counter for samples processed.
	 */
	Counter<int32_t> samples_processed_ {};

	/**
	 * \brief Internal 0-based counter for track samples processed.
	 */
	Counter<int32_t> track_samples_processed_ {};

	/**
	 * \brief Internal 0-based counter for tracks.
	 */
	Counter<int32_t> tracks_processed_ {};

	/**
	 * \brief Internal 0-based counter for sample sequences.
	 */
	Counter<int32_t> sequences_processed_ {};

	/**
	 * \brief Internal time elapsed by processing.
	 */
	std::chrono::duration<float> algo_time_elapsed_ {};

	/**
	 * \brief Internal time elapsed by updating.
	 */
	std::chrono::duration<float> update_time_elapsed_ {};

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	CalculationState();

	/**
	 * \brief Offset of the current sample.
	 *
	 * This sample is not yet processed but will be the next sample to process.
	 *
	 * \return Current sample
	 */
	int32_t current_offset() const noexcept;

	/**
	 * \brief Return the total number of PCM 32 bit samples yet processed.
	 *
	 * This value is equivalent to samples_expected() - samples_todo().
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	int32_t samples_processed() const noexcept;

	/**
	 * \brief Return the total number of PCM 32 bit samples in current track.
	 *
	 * \return Total number of PCM 32 bit samples processed in current track.
	 */
	int32_t track_samples_processed() const noexcept;

	/**
	 * \brief Return the total number of tracks yet processed.
	 *
	 * This value is incremented by track_finished().
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	int32_t tracks_processed() const noexcept;

	/**
	 * \brief Returns the total number of sample sequences yet processed.
	 *
	 * \return Total number of sample sequences processed.
	 */
	int32_t sequences_processed() const noexcept;

	/**
	 * \brief Amount of milliseconds elapsed so far by Algorithm::update().
	 *
	 * \return Amount of milliseconds elapsed so far by the Algorithm instance.
	 */
	std::chrono::duration<float> algo_time_elapsed() const noexcept;

	/**
	 * \brief Increment the duration for updating.
	 *
	 * \param[in] duration Amount of duration to add
	 */
	void increment_algo_time_elapsed(
			const std::chrono::duration<float>& duration);

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
	 * \brief Advance by some amount to a higher current offset.
	 *
	 * \param[in] amount Amount (in samples) to advance
	 */
	void advance(const int32_t amount);

	/**
	 * \brief Update the calculation state with amounts of samples and time.
	 *
	 * \param[in] amount    Samples processed in current algorithm update
	 * \param[in] algo_time Time consumed by algorithm in current update
	 */
	void update(const int32_t amount,
		const std::chrono::duration<float>& algo_time);

	/**
	 * \brief Mark track as finished.
	 *
	 * \return Samples processed in the course of this track
	 */
	int32_t track_finished();

	/**
	 * \brief Swap abstract part of the concrete subclass..
	 *
	 * \param[in] rhs Other instance to swap
	 */
	void swap(CalculationState& rhs) noexcept;

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(CalculationState& lhs, CalculationState& rhs)
		noexcept
	{
		lhs.swap(rhs);
	}
};


/**
 * \brief Create a partitioner for specific values.
 *
 * \param[in] offsets  Offsets
 * \param[in] leadout  Leadout
 * \param[in] interval Legal interval
 *
 * \return Partitioner
 */
std::unique_ptr<details::Partitioner> make_partitioner(
		const Points& offsets, const AudioSize& leadout,
		const details::SampleRange& interval);

/**
 * \brief Worker: log partition stats.
 *
 * \param[in] partition Partition
 * \param[in] from      From sample
 * \param[in] to        To sample
 * \param[in] total     Total samples
 */
void log_sample_stats(const Partition& partition,
		const int32_t from, const int32_t to, const int32_t total);

/**
 * \brief Worker: log processing stats.
 *
 * \param[in] partitioner Partitioner to log
 * \param[in] state       CalculationState to log
 */
void log_processing_stats(const Partitioner& partitioner,
		const CalculationState& state);

namespace update
{

/**
 * \brief Updates a calculation process by a single partition.
 *
 * \tparam A Algorithm
 * \tparam I Iterator type
 *
 * \param[in]     partition Partition to update Calculation with
 * \param[in]     start     Iterator pointing to first sample
 * \param[in]     start_pos Current offset position
 * \param[in,out] algorithm Algorithm to use for calculation
 * \param[in,out] state     Current calculation state
 */
template <typename A, typename I>
void update_partition(const Partition& partition,
		I start, const int32_t start_pos,
		Updateable<A>& algorithm, CalculationState& state)
{
	const auto offset_first = partition.begin_offset() - start_pos;
	const auto offset_last  = partition.end_offset()   - start_pos;
	const auto total        = offset_last + 1 - offset_first;

	log_sample_stats(partition, start_pos + offset_first,
				start_pos + offset_last, total);

	using clock = std::chrono::steady_clock;

	const auto start_time { clock::now() };

	try
	{
		auto b = start; std::advance(b, offset_first);
		auto e = start; std::advance(e, offset_last + 1);
		// +1 because the stop point has to be shifted _behind_ the last
		// sample. The last sample would not be processed otherwise.

		algorithm.update(b, e);
	} catch (...)
	{
		const auto stop_time { clock::now() };
		state.increment_algo_time_elapsed(stop_time - start_time);

		throw;
	}

	const auto stop_time { clock::now() };

	state.update(total, stop_time - start_time);
}

/**
 * \brief Get current first and last positions.
 *
 * \param[in] samples_in_block Total number of samples in block
 * \param[in] state            Calculation state
 *
 * \return First and last samples
 */
std::pair<int32_t, int32_t> positions(const int32_t& samples_in_block,
		CalculationState& state);

/**
 * \brief Skip block and return whether this completed the Calculation.
 *
 * \param[in] samples_in_block Total number of samples in block
 * \param[in] partitioner      Partitioner
 * \param[in] state            Calculation state
 *
 * \return TRUE if Calculation is complete after skipping the specified amount
 */
bool complete_after_skip_block(const int32_t& samples_in_block,
		const Partitioner& partitioner,
		CalculationState& state);

/**
 * \brief Skip amount of samples.
 *
 * \param[in] start_pos    Start sample
 * \param[in] partitioning Partitioning
 * \param[in] state        Calculation state
 */
void skip_amount(const int32_t& start_pos, const Partitioning& partitioning,
		CalculationState& state);

/**
 * \brief Inform all instances about the completion of a track.
 *
 * \param[in] algorithm     Algorithm
 * \param[in] result_buffer Result buffer
 * \param[in] state         Calculation state
 */
void complete_track(Algorithm& algorithm,
		Checksums& result_buffer, CalculationState& state);

/**
 * \brief Updates a calculation process by a sample block.
 *
 * \tparam A Algorithm
 * \tparam B Type of iterator pointing to the begin of the update sequence
 * \tparam E Type of iterator pointing to the end   of the update sequence
 *
 * \param[in]     start         Iterator pointing to first sample in block
 * \param[in]     stop          Iterator pointing behind last sample in block
 * \param[in]     partitioner   Partition provider
 * \param[in]     algorithm     Algorithm to use for calculation
 * \param[in,out] state         Current calculation state
 * \param[in,out] result_buffer Buffer for collecting results
 *
 * \return FALSE iff more updates are required, otherwise TRUE
 */
template <typename A, typename B, typename E>
bool perform_update(B start, E stop, const Partitioner& partitioner,
		Updateable<A>& algorithm, CalculationState& state,
		Checksums& result_buffer)
{
	const auto samples_in_block {
		static_cast<int32_t>(std::distance(start, stop)) };

	const auto [ start_pos, last_pos ] = positions(samples_in_block, state);

	const auto partitioning { partitioner.create_partitioning(
			start_pos, samples_in_block) };

	if (partitioning.empty())
	{
		return complete_after_skip_block(samples_in_block, partitioner, state);
	} else
	{
		// If we skipped some samples at the beginning of the partition, advance
		// the state by this amount so that current_offset() will be correct on
		// subsequent call.
		skip_amount(start_pos, partitioning, state);
	}

	ARCS_LOG(DEBUG1) << "Partitions: " << partitioning.size();

	// Update the state with each partition in this partitioning

	auto partition_counter = uint16_t { 0 };

	for (const auto& partition : partitioning)
	{
		++partition_counter;

		ARCS_LOG(DEBUG2) << "PARTITION " << partition_counter << "/" <<
			partitioning.size();

		update_partition(partition, start, start_pos, algorithm, state);

		// If the current partition ends a track, save the ARCSs for this track
		if (partition.ends_track())
		{
			ARCS_LOG(DEBUG3) << "Completed track:  " << partition.track();

			complete_track(algorithm, result_buffer, state);
		}
	}

	/* Return TRUE iff the last relevant sample was in the current block. */
	return SampleRange { start_pos, last_pos }.contains(
			partitioner.legal_range().upper());
}

} // namespace update
} // namespace details


/**
 * \brief Settings for a Calculation.
 */
class Settings final : Equality<Settings>, Comparable<Settings>, Swap<Settings>
{
	/**
	 * \brief Internal context.
	 */
	Context context_ {};

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 *
	 * \details Initializes the Context of the Settings instance as ALBUM.
	 */
	Settings();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] c Context for a calculation
	 */
	explicit Settings(const Context& c);

	/**
	 * \brief Set context for this algorithm.
	 *
	 * \param[in] c Context to set on this instance
	 */
	void set_context(const Context c);

	/**
	 * \brief Current context of this algorithm.
	 *
	 * \return Context of this instance
	 */
	Context context() const;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(Settings& rhs) noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const Settings& rhs) const noexcept;

	/**
	 * \copydoc SNPT_mf_to_string
	 */
	std::string to_string() const;
};


/**
 * \brief Calculation phases.
 */
enum class State : uint8_t
{
    INSTANTIATED,
    INITIALIZED,
    UPDATED,
    COMPLETED,
	INVALID
};


/**
 * \brief Obtain the name of a State.
 *
 * \param[in] s State to get name of
 *
 * \return Name of State \c s
 */
std::string name(const State s);


/**
 * \brief Phase management of a Calculation.
 */
class Stateful
{
protected:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	Stateful() = default;

	/**
	 * \brief Transist from current state to target_state.
	 *
	 * \param[in] target_state State to transist to
	 *
	 * \throw logic_error If transition is not legal
	 */
    void transition_to(State target_state)
	{
        if (!is_valid_transition(state_, target_state))
		{
			auto ss = std::ostringstream{};
			ss << "Illegal state transition requested from state "
				<< static_cast<int>(state_)
				<< " to state "
				<< static_cast<int>(target_state);

            throw std::logic_error(ss.str());
        }

        state_ = target_state;

		ARCS_LOG(DEBUG2) << "New state: " << name(target_state);
    }

	/**
	 * \brief Iff state is not before \c s, throw.
	 *
	 * \param[in] s State to compare current state to
	 * \param[in] error_msg Error message
	 *
	 * \throw std::logic_error If current_state() is not before \c s
	 */
	void allowed_only_before(State s, const std::string& error_msg)
	{
		if (!state_earlier_than(s))
		{
			throw std::logic_error(error_msg);
		}
	}

	/**
	 * \brief Iff state is not \c s, throw.
	 *
	 * \param[in] s State to compare current state to
	 * \param[in] error_msg Error message
	 *
	 * \throw std::logic_error If current_state() is not \c s
	 */
	void allowed_only_for(State s, const std::string& error_msg)
	{
		if (current_state() != s)
		{
			throw std::logic_error(error_msg);
		}
	}

	/**
	 * \brief Iff state is not after \c s, throw.
	 *
	 * \param[in] s State to compare current state to
	 * \param[in] error_msg Error message
	 *
	 * \throw std::logic_error If current_state() is not after \c s
	 */
	void allowed_only_after(State s, const std::string& error_msg)
	{
		if (state_earlier_than(s) || current_state() == s)
		{
			throw std::logic_error(error_msg);
		}
	}

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void base_swap(Stateful& rhs) noexcept
	{
		using std::swap;

		swap(state_, rhs.state_);
	}

private:

	/**
	 * \brief Internal state.
	 */
    State state_ = State::INSTANTIATED;

	/**
	 * \brief TRUE iff current state is earlier than \c rhs.
	 *
	 * \param[in] rhs State to compare current state to
	 *
	 * \return TRUE iff current state is earlier than \c rhs
	 */
	bool state_earlier_than(State rhs) const
	{
		return static_cast<int>(current_state()) < static_cast<int>(rhs);
	}

	/**
	 * \brief Check whether transition between states is legal.
	 *
	 * \return TRUE if transition is legal, otherwise FALSE
	 */
    static bool is_valid_transition(State from, State to)
	{
        static const std::map<State, std::set<State>> allowed
		{
            { State::INSTANTIATED,/*->*/{ State::INITIALIZED }},
            { State::INITIALIZED, /*->*/{ State::INITIALIZED, State::UPDATED }},
            { State::UPDATED,     /*->*/{ State::UPDATED, State::COMPLETED }},
            { State::COMPLETED,   /*->*/{} },
            { State::INVALID,     /*->*/{} }
        };

        return allowed.at(from).count(to) > 0;
    }

public:

	/**
	 * \copydoc SNPT_sm_default_dtor.
	 */
	virtual ~Stateful() noexcept = default;

	/**
	 * \brief Current state.
	 *
	 * \return Current state
	 */
    State current_state() const
	{
		return state_;
	}
};


/**
 * \brief Encapsulate the stateful parts of a Calculation.
 *
 * All these parts are independent of the concrete algorithm.
 */
class Calculation : public Stateful
{
	/**
	 * \brief Internal settings for this calculation.
	 */
	Settings settings_ { /* default */ };

	/**
	 * \brief Internal current calculation state.
	 */
	details::CalculationState state_ { /* default */ };

	/**
	 * \brief Partitioner to provide the stop positions.
	 */
	std::unique_ptr<details::Partitioner> partitioner_ {}; /* nullptr */

	/**
	 * \brief Internal result buffer.
	 */
	Checksums result_buffer_ { /* default */ };

	virtual const Algorithm* do_algorithm() const noexcept
	= 0;

	virtual std::size_t do_total_tracks() const
	= 0;

	virtual Points do_offsets() const
	= 0;

	virtual AudioSize do_leadout() const
	= 0;

	virtual void do_init(const Points& offsets, const AudioSize& leadout)
	= 0;

	/**
	 * \brief Hook: called after settings have been set.
	 */
	virtual void on_settings_changed()
	= 0;

	/**
	 * \brief Hook: called after the update sequence is completed.
	 */
	virtual void on_completion()
	= 0;

protected:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] settings  The settings for the calculation
	 */
	explicit Calculation(const Settings& settings)
		: settings_      { settings      }
		, state_         { /* default */ }
		, partitioner_   {}  /* nullptr */
		, result_buffer_ { /* default */ }
	{
		// empty
	}

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	Calculation(Calculation&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	Calculation& operator = (Calculation&& rhs) noexcept = default;

	/**
	 * \brief Initialize internal algorithm.
	 *
	 * \param[in] algorithm Algorithm to initialize
	 */
	void init_algorithm(Algorithm& algorithm)
	{
		allowed_only_before(State::UPDATED,
				"Cannot change context after first update");

		algorithm.set_context(settings_.context());
	}

	/**
	 * \brief Worker: initialize internal partitioner.
	 *
	 * \param[in] offsets Offsets
	 * \param[in] leadout Leadout
	 * \param[in] legal   Legal interval
	 */
	void init_partitioner(const Points& offsets,
			const AudioSize& leadout, const details::SampleRange& legal)
	{
		allowed_only_before(State::UPDATED,
				"Cannot change partitioner after first update");

		ARCS_LOG(DEBUG3) << "Initialize partitioner";

		partitioner_ = details::make_partitioner(offsets, leadout, legal);
	}

	/**
	 * \brief Worker: initialize internal result buffer.
	 *
	 * \param[in] total_elements Total number of elements
	 */
	void init_resultbuffer(const std::size_t total_elements)
	{
		allowed_only_before(State::UPDATED,
				"Cannot change buffer size after first update");

		ARCS_LOG(DEBUG3) << "Initialize result buffer for " << total_elements
			<< " tracks";

		result_buffer_.resize(total_elements);
	}

	/**
	 * \brief Initialize internal data parts.
	 *
	 * \param[in] offsets  Offsets
	 * \param[in] leadout  Leadout
	 * \param[in] interval Legal interval
	 */
	void init_data(const Points& offsets,
			const AudioSize& leadout, const details::SampleRange& interval)
	{
		allowed_only_before(State::UPDATED,
				"Cannot modify offsets, leadout or range after first update");

		init_partitioner(offsets, leadout, interval);
		init_resultbuffer(offsets.size());

		transition_to(State::INITIALIZED);
	}

	/**
	 * \brief Read the internal state.
	 *
	 * \return Internal state
	 */
	const details::CalculationState& state() const noexcept
	{
		return state_;
	}

	/**
	 * \brief Read the internal partitioner.
	 *
	 * \return Internal partitioner
	 */
	const details::Partitioner* partitioner() const noexcept
	{
		return partitioner_.get();
	}

	/**
	 * \brief Provide the internal state for write access.
	 *
	 * \return Writable reference to internal state
	 */
	details::CalculationState& provide_state() noexcept
	{
		return state_;
	}

	/**
	 * \brief Provide the internal result buffer for write access.
	 *
	 * \return Writable reference to internal result buffer
	 */
	Checksums& provide_buffer() noexcept
	{
		return result_buffer_;
	}

	/**
	 * \brief Worker: write log after completion.
	 */
	void log_completion() noexcept;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void base_swap(Calculation& rhs) noexcept;

public:

	Calculation(const Calculation& rhs) = delete;

	Calculation& operator = (const Calculation& rhs) = delete;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~Calculation() override = default;

	/**
	 * \brief Initialize this Upater with input data.
	 *
	 * \param[in] offsets Offsets
	 * \param[in] leadout Leadout
	 */
	void init(const Points& offsets, const AudioSize& leadout)
	{
		allowed_only_before(State::UPDATED,
				"Cannot modify offsets or leadout after first update");

		do_init(offsets, leadout);
	}

	/**
	 * \brief Returns the algorithm instance used by this Calculation.
	 *
	 * \return Algorithm used by this Calculation.
	 */
	const Algorithm* algorithm() const noexcept
	{
		return do_algorithm();
	}

	/**
	 * \brief Returns the types requested to this Calculation.
	 *
	 * Convenience function for <tt>mycalculation.algorithm().types()</tt>.
	 *
	 * \return All requested Checksum types.
	 */
	ChecksumtypeSet types() const noexcept
	{
		return algorithm()->types();
	}

	/**
	 * \brief Return the settings of this instance.
	 *
	 * \return Settings of this instance
	 */
	Settings settings() const noexcept
	{
		return settings_;
	}

	/**
	 * \brief Configure the algorithm with settings.
	 *
	 * \param[in] s Settings to use on this instance
	 */
	void set_settings(const Settings& s);

	/**
	 * \brief Total tracks to be processed by this instance.
	 *
	 * \return Total tracks
	 */
	std::size_t total_tracks() const
	{
		return do_total_tracks();
	}

	/**
	 * \brief Offsets used by this instance.
	 *
	 * \return Offsets
	 */
	Points offsets() const
	{
		return do_offsets();
	}

	/**
	 * \brief Leadout used by this instance.
	 *
	 * \return Leadout
	 */
	AudioSize leadout() const
	{
		return do_leadout();
	}

	/**
	 * \brief Returns the total number for PCM 32 bit samples yet processed.
	 *
	 * This value is equivalent to samples_expected() - samples_todo().
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	int32_t samples_processed() const noexcept
	{
		return state_.samples_processed();
	}

	/**
	 * \brief Returns the total number of initially expected PCM 32 bit samples.
	 *
	 * This value is equivalent to samples_processed() + samples_todo(). It will
	 * always remain constant for the given instance.
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples expected.
	 */
	int32_t samples_expected() const noexcept
	{
		// Expected total number of input samples
		return partitioner_->total_samples().samples();
	}

	/**
	 * \brief Returns the total number of PCM 32 bit samples that is yet to be
	 * processed.
	 *
	 * This value is equivalent to samples_expected() - samples_processed().
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples yet to process.
	 */
	int32_t samples_todo() const noexcept
	{
		return this->samples_expected() - this->samples_processed();
	}

	/**
	 * \brief Returns the total number of sample sequences yet processed.
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of sample sequences processed.
	 */
	int32_t sequences_processed() const noexcept
	{
		return state_.sequences_processed();
	}

	/**
	 * \brief Amount of time elapsed so far by update().
	 *
	 * \return Amount of time elapsed so far by update().
	 */
	std::chrono::duration<float> update_time_elapsed() const noexcept
	{
		return state_.update_time_elapsed();
	}

	/**
	 * \brief Amount of time elapsed so far by the algorithm instance.
	 *
	 * \return Amount of time elapsed so far by the algorithm instance.
	 */
	std::chrono::duration<float> algo_time_elapsed() const noexcept
	{
		return state_.algo_time_elapsed();
	}

	/**
	 * \brief Returns \c TRUE iff this Calculation is completed, otherwise
	 * \c FALSE.
	 *
	 * If the instance returns \c TRUE it is safe to call result(). Value
	 * \c FALSE indicates that the instance expects more updates.
	 *
	 * \return \c TRUE if the Calculation is completed, otherwise \c FALSE
	 */
	bool complete() const noexcept;

	/**
	 * \brief Acquire the resulting Checksums.
	 *
	 * \return The computed Checksums
	 */
	Checksums result() const noexcept;

	/**
	 * \brief Update the instance with a new AudioSize.
	 *
	 * \param[in] audiosize The updated AudioSize
	 */
	void update(const AudioSize& audiosize)
	{
		allowed_only_before(State::UPDATED,
				"Cannot change size after first update");

		if (partitioner_)
		{
			partitioner_->set_total_samples(audiosize);
		}
	}
};


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
// -Wnon-virtual-dtor is deactivated: warns about protected non-virtual dtors in
// the CRTP mixins. This is a false positive since the instances are never
// destroyed by a pointer to Swap<T> or Equality<T> and a CRTP does normally
// not have virtual members.

/**
 * \brief Calculation instance that can be updated by custom types.
 *
 * \tparam A Algorithm to use for Calculation
 *
 * An Updater represents a Calculation for a concrete checksum calculation
 * process. It is manually performed by the caller by calling update().
 *
 * Updater instances must be initialized with the specific size of the input
 * audio file and an Algorithm that defines the type of the checksums. If
 * multiple tracks e.g. an entire disc content is to be processed, the ToC
 * information of the disc is required. Additionally, a Settings instance can be
 * specified. Currently, the only supported Settings attribute is Context.
 *
 * The input of the audio file must be represented as a succession of iterable
 * \link arcstk::SampleSequence SampleSequences \endlink and the
 * Calculation is to be sequentially updated with these sequences in order.
 * After the last update, the Calculation returns the calculation result on
 * request. The calculated Checksums are represented as an iterable aggregate of
 * \link arcstk::ChecksumSet ChecksumSets \endlink.
 *
 * \see make_calculationset
 */
template <class A>
class Updater final : public Calculation,
					  public Swap<Updater<A>>
{
	/**
	 * \brief Algorithm to calculate checksums.
	 *
	 * Internally represented as an Updateable.
	 */
	std::unique_ptr<Updateable<A>> algorithm_ { std::make_unique<A>() };

public:

	/**
	 * \brief Type of the internal algorithm.
	 */
	using algorithm_type = A;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] settings The settings for the calculation
	 */
	explicit Updater(const Settings& settings)
		: Calculation { settings }
	{
		init_algorithm(*algorithm_);
	}

	/**
	 * \brief Constructor.
	 *
	 * Instantiates Updater with default settings.
	 */
	Updater()
		: Updater { Settings {/*default*/} }
	{
		// empty
	}

	/**
	 * \brief Constructor.
	 *
	 * If <tt>leadout.zero()</tt>, the leadout has to be published by
	 * update(const AudioSize&) before transisting to UPDATED.
	 *
	 * \param[in] settings The settings for the calculation
	 * \param[in] offsets  Track offsets (as samples)
	 * \param[in] leadout  Size of the expected input
	 */
	Updater(const Settings& settings,
			const Points& offsets, const AudioSize& leadout)
		: Calculation { settings }
	{
		ARCS_LOG(DEBUG3) << "Initialize Updater for algorithm '"
			<< algorithm_->name() << "' with data for context "
			<< name(settings.context());

		init_algorithm(*algorithm_);
		init_data(offsets, leadout,
			details::SampleRange { algorithm_->range(leadout, offsets) });
	}

	/**
	 * \brief Constructor.
	 *
	 * The ToC is not required to be complete. In this case, the leadout has to
	 * be published by update(const AudioSize&) before transisting to UPDATED.
	 *
	 * \param[in] settings  The settings for the calculation
	 * \param[in] toc       Track offsets and leadout
	 */
	Updater(const Settings& settings, const ToC& toc)
		: Updater { settings, toc.offsets(), toc.leadout() }
	{
		// empty
	}

	/**
	 * \brief Constructor.
	 *
	 * The ToCData is not required to contain a leadout. In this case, the
	 * leadout has to be published by update(const AudioSize&) before
	 * transisting to UPDATED.
	 *
	 * \param[in] settings  The settings for the calculation
	 * \param[in] toc       Track offsets and leadout
	 */
	Updater(const Settings& settings, const ToCData& toc)
		: Updater { settings, toc::offsets(toc), toc::leadout(toc) }
	{
		// empty
	}

	// non-copyable
	Updater(const Updater& rhs)              = delete;
	Updater& operator = (const Updater& rhs) = delete;

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	Updater(Updater&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	Updater& operator = (Updater&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~Updater() noexcept override = default;

	/**
	 * \brief Implements update for sample sequences.
	 *
	 * \tparam T The value_type of the SampleSequence
	 * \tparam is_planar TRUE for planar SampleSequences, FALSE for interleaved
	 *
	 * \param[in] samples Sample sequence for update
	 */
    template<typename T, bool is_planar>
    void update(const SampleSequence<T, is_planar>& samples)
	{
		using std::cbegin;
		using std::cend;

		this->update(cbegin(samples), cend(samples));
	}

	/**
	 * \brief Update with a planar sequence of samples.
	 *
	 * \tparam B Type of iterator pointing to the begin of the update sequence
	 * \tparam E Type of iterator pointing to the end   of the update sequence
	 *
	 * \param[in] start Iterator pointing to the first sample of the sequence
	 * \param[in] stop  Iterator pointing behind the last sample of the sequence
	 */
	template<typename B, typename E>
    void update(B start, E stop)
	{
		this->update_impl(start, stop);
    }

	/**
	 * \brief Return the name of the wrapped algorithm.
	 *
	 * \return Name of the algorithm wrapped by this instance
	 */
	std::string algorithm_name()
	{
		return this->algorithm_->name();
	}

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(Updater& rhs) noexcept
	{
		Calculation::base_swap(rhs);

		using std::swap;

		swap(algorithm_, rhs.algorithm_);
	}

private:

	// Calculation

	const Algorithm* do_algorithm() const noexcept final
	{
		return algorithm_.get();
	}

	std::size_t do_total_tracks() const final
	{
		return offsets().size();
	}

	Points do_offsets() const final
	{
		return partitioner() ? partitioner()->points() : Points{};
	}

	AudioSize do_leadout() const final
	{
		return partitioner() ? partitioner()->total_samples() : AudioSize{};
	}

	void do_init(const Points& offsets, const AudioSize& leadout) final
	{
		init_data(offsets, leadout,
			details::SampleRange { algorithm_->range(leadout, offsets) });
	}

	void on_settings_changed() final
	{
		init_algorithm(*algorithm_);
	}

	void on_completion() final
	{
		transition_to(State::COMPLETED);
		this->log_completion();
	}

	/**
	 * \brief Implements update for iterators.
	 *
	 * \tparam B Type of iterator pointing to the begin of the update sequence
	 * \tparam E Type of iterator pointing to the end   of the update sequence
	 */
	template <typename B, typename E>
	void update_impl(B start, E stop)
	{
		transition_to(State::UPDATED);

		ARCS_LOG(DEBUG1) << "PROCESS BLOCK: START";

		try
		{
			using std::chrono::steady_clock;

			const auto start_time { steady_clock::now() };

			const auto* part_er = partitioner();

			if (!part_er)
			{
				throw std::runtime_error("No partitioner available");
			}

			const auto is_completed = bool { details::update::perform_update(
					start, stop, *part_er, *algorithm_,
					provide_state(), provide_buffer())
			};

			const auto stop_time  { steady_clock::now() };

			provide_state().increment_update_time_elapsed(
					stop_time - start_time);

			if (is_completed)
			{
				on_completion();
			}
		} catch (...)
		{
			transition_to(State::INVALID);
			throw;
		}

		ARCS_LOG(DEBUG1) << "PROCESS BLOCK: END";
	}
};

#pragma GCC diagnostic pop

/**
 * \brief Interface for a set of Calculations.
 */
class CalculationSet
{
public:

	/**
	 * \brief Size type of this CalculationSet.
	 */
	using size_type = std::size_t;

private:

	virtual void do_init(const Points& offsets, const AudioSize& leadout)
	= 0;

	virtual std::size_t do_total_tracks() const
	= 0;

	virtual Points do_offsets() const
	= 0;

	virtual AudioSize do_leadout() const
	= 0;

	virtual void do_update(const AudioSize& audiosize)
	= 0;

	virtual Checksums do_result() const
	= 0;

	virtual size_type do_size() const
	= 0;

	virtual bool do_empty() const
	= 0;

	virtual bool do_complete() const
	= 0;

public:

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~CalculationSet() noexcept = default;

	/**
	 * \brief Initialize with data.
	 *
	 * \param[in] offsets Offsets
	 * \param[in] leadout Leadout
	 */
	void init(const Points& offsets, const AudioSize& leadout)
	{
		do_init(offsets, leadout);
	}

	/**
	 * \brief Initialize with data.
	 *
	 * \param[in] toc ToC
	 */
	void init(const ToC& toc)
	{
		this->init(toc.offsets(), toc.leadout());
	}

	/**
	 * \brief Initialize with data.
	 *
	 * \param[in] toc_data ToCData
	 */
	void init(const ToCData& toc_data)
	{
		this->init(toc::offsets(toc_data), toc::leadout(toc_data));
	}

	/**
	 * \brief Total number of tracks.
	 *
	 * \return Total tracks
	 */
	std::size_t total_tracks() const
	{
		return do_total_tracks();
	}

	/**
	 * \brief Offsets used by this instance.
	 *
	 * \return Offsets
	 */
	Points offsets() const
	{
		return do_offsets();
	}

	/**
	 * \brief Leadout used by this instance.
	 *
	 * \return Leadout
	 */
	AudioSize leadout() const
	{
		return do_leadout();
	}

	/**
	 * \brief Update the instance with a new AudioSize.
	 *
	 * \param[in] audiosize The updated AudioSize
	 */
	void update(const AudioSize& audiosize)
	{
		do_update(audiosize);
	}

	/**
	 * \brief Return calculation result.
	 *
	 * \return Resulting Checksums
	 */
	Checksums result() const
	{
		return do_result();
	}

	/**
	 * \brief Size of this instance, i.e. total number of Calculation instances.
	 *
	 * \return Size of this instance
	 */
	size_type size() const
	{
		return do_size();
	}

	/**
	 * \brief Return TRUE if this instance is empty, i.e. size() is 0
	 *
	 * \return TRUE iff instance is empty, otherwise FALSE
	 */
	bool empty() const
	{
		return do_empty();
	}

	/**
	 * \brief TRUE if each Calculation is complete, otherwise FALSE.
	 *
	 * \return Completion status
	 */
	bool complete() const
	{
		return do_complete();
	}

	// TODO ChecksumtypeSet types() const noexcept
};

/**
 * \brief A set of Calculations.
 *
 * \tparam B Type of begin iterator
 * \tparam E Type of end   iterator
 *
 * Specifiy a set of calculations updateable by types B and E.
 *
 * \see make_calculationset
 */
template<typename B, typename E>
class UpdateableCalculationSet final : public CalculationSet
{
	/**
	 * \brief Internal Updater instances for each algorithm.
	 */
    std::vector<std::unique_ptr<Calculation>> updaters_ {};

	/**
	 * \brief Internal callers for update<B, E>().
	 */
    std::vector<std::function<void(B, E)>> handlers_ {};

public:

	/**
	 * \brief Function for registering the concrete algorithms.
	 */
    using RegistrationFunc_t = std::function<void(
			const Settings& settings, UpdateableCalculationSet&)>;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] settings Settings to apply to each algorithm
	 * \param[in] register_algorithms Register function for algorithms
	 */
    UpdateableCalculationSet(const Settings& settings,
			RegistrationFunc_t register_algorithms)
	{
        register_algorithms(settings, *this);
    }

	/**
	 * \brief Constructor.
	 *
	 * Settings for ALBUM, configurable algorithms.
	 *
	 * \param[in] register_algorithms Register function for algorithms
	 */
    explicit UpdateableCalculationSet(RegistrationFunc_t register_algorithms)
		: UpdateableCalculationSet { Settings{}, register_algorithms }
	{
		// empty
	}

	/**
	 * \brief Constructor.
	 *
	 * Configurable settings, AccurateRip v1 and v2.
	 *
	 * \param[in] settings Settings to apply to each algorithm
	 */
	explicit UpdateableCalculationSet(const Settings& settings)
	{
		// Define default algorithms to register HERE:
        this->add<AccurateRip::V1andV2>(settings);
	}

	/**
	 * \brief Constructor.
	 *
	 * Settings for ALBUM, AccurateRip v1 and v2.
	 */
	UpdateableCalculationSet()
		: UpdateableCalculationSet { Settings{} }
	{
		// empty
	}

	/**
	 * \brief Register an Algorithm.
	 *
	 * \tparam A Type of the algorithm to add
	 *
	 * \param[in] settings Settings to set for this algorithm
	 */
    template<class A>
    void add(const Settings& settings)
	{
        auto updater = std::make_unique<Updater<A>>(settings);

		ARCS_LOG(DEBUG3) << "Add Updater for algorithm '"
			<< updater->algorithm_name()
			<< " with context " << name(settings.context());

        Updater<A>* upd_ptr = updater.get();
        updaters_.push_back(std::move(updater));

        // Connect algorithm A and the iterators set (B, E) at compile-time
        handlers_.push_back(
			[upd_ptr](B start, E stop)
			{
				upd_ptr->template update<B, E>(start, stop);
			}
		);
    }

	/**
	 * \brief Update all Updater-instances in the set.
	 *
	 * \param[in] start Start iterator
	 * \param[in] stop  Stop iterator
	 */
    void update(B start, E stop)
	{
        for (auto& call_update : handlers_)
		{
            call_update(start, stop);
        }
    }

private:

	// CalculationSet

	void do_init(const Points& offsets, const AudioSize& leadout) final
	{
		for (auto& calculation : updaters_)
		{
			calculation->init(offsets, leadout);
		}
	}

	std::size_t do_total_tracks() const final
	{
		return offsets().size();
	}

	Points do_offsets() const final
	{
		if (const auto* calc = updaters_[0].get(); calc)
		{
			return calc->offsets();
		}

		return {};
	}

	AudioSize do_leadout() const final
	{
		if (const auto* calc = updaters_[0].get(); calc)
		{
			return calc->leadout();
		}

		return {};
	}

	void do_update(const AudioSize& audiosize) final
	{
		for (auto& calculation : updaters_)
		{
			calculation->update(audiosize);
		}
	}

	Checksums do_result() const final
	{
		return this->merge_results(updaters_);
	}

	size_type do_size() const final
	{
		return updaters_.size();
	}

	bool do_empty() const final
	{
		return updaters_.empty();
	}

	bool do_complete() const final
	{
		if (empty())
		{
			return false;
		}

		for (auto& calculation : updaters_)
		{
			if (calculation && !calculation->complete())
			{
				return false;
			}
		}

		return true;
	}

	//

	Checksums merge_results(const std::vector<std::unique_ptr<Calculation>>&
			calculations) const
	{
		const auto total_tracks =
			updaters_[0] ? updaters_[0]->result().size() : 0;

		ARCS_LOG(DEBUG3) << "Allocate result for " << total_tracks << " tracks";

		//auto tracks { Checksums(total_tracks, ChecksumSet { {/*0*/} }) };
		auto tracks = Checksums{};
		tracks.resize(total_tracks);

		using std::begin;
		using std::cbegin;
		using std::cend;

		std::for_each(cbegin(calculations), cend(calculations),
			[&tracks](const std::unique_ptr<Calculation>& c)
			{
				auto checksums { c->result() };

				std::transform(cbegin(checksums), cend(checksums), // input
					begin(tracks), // input (but has to be non-const)
					begin(tracks), // output
					[](const ChecksumSet& s, ChecksumSet& t) -> ChecksumSet
					{
						auto set { s };
						t.merge(set);

						// FIXME Supposing lengths all-equal is an error
						t.set_length(set.length());

						return t;
					}
				);
			});

		return tracks;
	}
};


/**
 * \brief Specification of a set of algorithms.
 *
 * A specification of a set of algorithms. Represents job and target for one or
 * more Calculation instances.
 *
 * \tparam A1   Algorithm type
 * \tparam Args 0 or more Algorithm types
 */
template <typename A1, typename... Args>
struct AlgorithmTypes final
{
	/**
	 * \brief Total number of algorithm types.
	 */
	static constexpr std::size_t count { 1 + sizeof...(Args) };

	/**
	 * \brief Configure an existing Updater by this set of algorithms.
	 *
	 * \tparam B Type of begin iterator
	 * \tparam E Type of end   iterator
	 *
	 * \param[in] settings Settings for Updater
	 * \param[in] set      CalculationSet to configure
	 */
	template<typename B, typename E>
	static void configure(const Settings& settings,
			UpdateableCalculationSet<B, E>& set)
	{
		set.template add<A1>(settings);
		(set.template add<Args>(settings), ...);
	}

	/**
	 * \brief Create and configure an UpdateableCalculationSet for two types
	 *
	 * \tparam B Type of begin iterator
	 * \tparam E Type of end   iterator
	 *
	 * \param[in] settings Settings to use for configuration
	 *
	 * \return Configured UpdateableCalculationSet
	 */
	template<typename B, typename E>
	static UpdateableCalculationSet<B, E> typed_calculationset_for(
			const Settings& settings)
	{
		return UpdateableCalculationSet<B, E> { settings,
			&AlgorithmTypes<A1, Args...>::configure<B,E> };
	}

	/**
	 * \brief Create and configure a CalculationSet for two types
	 *
	 * \tparam B Type of begin iterator
	 * \tparam E Type of end   iterator
	 *
	 * \param[in] settings Settings to use for configuration
	 *
	 * \return Configured CalculationSet
	 */
	template<typename B, typename E>
	static std::unique_ptr<CalculationSet> calculationset_for(
			const Settings& settings)
	{
		return std::make_unique<UpdateableCalculationSet<B, E>>(settings,
			&AlgorithmTypes<A1, Args...>::configure<B,E>);
	}
};


/**
 * \brief Create a CalculationSet for specified checksum types and Settings.
 *
 * \tparam B Type of begin iterator
 * \tparam E Type of end   iterator
 *
 * \param[in] types Set of checksum types to calculate
 * \param[in] s     Settings for Calculation
 *
 * \return CalculationSet for checksum types \c types, configured by \c s
 */
template<typename B, typename E>
inline auto make_calculationset(const ChecksumtypeSet& types, const Settings& s)
{
	if (types.size() == 1) // either V1-only or V2-only requested
	{
		using V1_only = AlgorithmTypes<AccurateRip::V1>;
		using V2_only = AlgorithmTypes<AccurateRip::V1>;

		using std::cbegin;

		if (checksum::type::ARCS1 == *cbegin(types))
		{
			ARCS_LOG(DEBUG3) << "Create CalculationSet for AccurateRip v1";
			return V1_only::calculationset_for<B,E>(s);
		} else
		{
			ARCS_LOG(DEBUG3) << "Create CalculationSet for AccurateRip v2";
			return V2_only::calculationset_for<B,E>(s);
		}

	} else
	{
		// V1andV2 is correct for case 0 (default) and case 2 (all)

		using V1_and_V2 = AlgorithmTypes<AccurateRip::V1andV2>;
		ARCS_LOG(DEBUG3) << "Create CalculationSet for AccurateRip v1 and v2";
		return V1_and_V2::calculationset_for<B,E>(s);
	}
}

/** @} */

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

