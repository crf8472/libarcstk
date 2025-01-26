#ifndef __LIBARCSTK_CALCULATE_HPP__
#error "Do not include calculate_details.hpp, include calculate.hpp instead"
#endif

#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#define __LIBARCSTK_CALCULATE_DETAILS_HPP__
/**
 * \file
 *
 * \brief Implementation details of the calculation interface.
 */
#include <chrono>        // for milliseconds, duration-cast, operator-
#include <cstdint>       // for int32_t
#include <memory>        // for memory
#include <string>        // for string
#include <vector>        // for vector

#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

// avoid includes
class AudioSize;
class ToC;
class ChecksumSet;

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
	const T a_;

	/**
	 * \brief Last number in interval.
	 */
	const T b_;

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
 */
Partitioning get_partitioning(
		const SampleRange& interval,
		const SampleRange& legal,
		const Points& points);

/**
 * \brief Create a single partition for an interval in a legal range.
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
	Partitioner(const int32_t total_samples, const Points& points,
			const SampleRange& legal);

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~Partitioner() noexcept;

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
	int32_t total_samples() const;

	/**
	 * \brief Set total number of samples.
	 *
	 * Maybe necessary when reading the last block reveals a different number of
	 * samples than expected.
	 *
	 * \param[in] total_samples Total number of samples
	 */
	void set_total_samples(const int32_t total_samples);

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
	 * \brief Deep copy of this instance.
	 *
	 * \return A deep copy of this instance
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

	/**
	 * \brief Implements Partitioner::create_partitioning() without a ToC.
	 *
	 * \param[in] current_interval Interval to build partitions from
	 * \param[in] legal_range      Legal interval to process
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	virtual Partitioning do_create_partitioning(
		const SampleRange& current_interval,
		const SampleRange& legal_range) const
	= 0;

	virtual std::unique_ptr<Partitioner> do_clone() const
	= 0;

	/**
	 * \brief Total number of samples expected.
	 */
	int32_t total_samples_;

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
 * \brief Create a partitioner for a closed input interval.
 *
 * The concrete interval is [1;size.total_samples()].
 *
 * \param[in] size       Upper bound for the closed input interval.
 * \param[in] calc_range The legal range for partitioning.
 *
 * \return Partitioner for the specified interval.
 */
std::unique_ptr<Partitioner> make_partitioner(const AudioSize& size,
		const SampleRange& calc_range) noexcept;


/**
 * \brief Create a partitioner for a closed input interval.
 *
 * \param[in] size       Upper bound for the closed input interval.
 * \param[in] calc_range The legal range for partitioning.
 * \param[in] points     The splitting points for the partitions.
 *
 * \return Partitioner for the specified interval.
 */
std::unique_ptr<Partitioner> make_partitioner(const AudioSize& size,
		const Points& points, const SampleRange& calc_range) noexcept;


/**
 * \brief Provides partitions along track bounds.
 */
class TrackPartitioner final : public Partitioner
{
	virtual Partitioning do_create_partitioning(
		const SampleRange& sample_block,
		const SampleRange& relevant_interval,
		const Points& points) const final;

	virtual Partitioning do_create_partitioning(
		const SampleRange& sample_block,
		const SampleRange& relevant_interval) const final;

	virtual std::unique_ptr<Partitioner> do_clone() const final;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] total_samples Total number of samples expected in input
	 * \param[in] points        List of splitting points
	 * \param[in] legal         Legal range of calculation
	 */
	TrackPartitioner(const int32_t total_samples, const Points& points,
			const SampleRange& legal);
};


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
	const int32_t begin_offset_;

	/**
	 * \brief Relative offset of the last sample in this partition + 1
	 */
	const int32_t end_offset_;

	/**
	 * \brief TRUE iff the first sample in this partition is also the first
	 * sample in the track
	 */
	const bool starts_track_;

	/**
	 * \brief TRUE iff the last sample in this partition is also the last sample
	 * in the track
	 */
	const bool ends_track_;

	/**
	 * \brief 1-based number of the track of which the samples in the partition
	 * are part of
	 */
	const int track_;

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
			const int32_t &begin_offset,
			const int32_t &end_offset,
			const bool    &starts_track,
			const bool    &ends_track,
			const int     &track);

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
	 * \brief Converting constructor for a counter.
	 *
	 * \param[in] value Start value
	 */
	Counter(const T& value)
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
};

/**
 * \brief Return the offsets converted to sample indices.
 *
 * \param[in] toc ToC to get offsets from
 *
 * \return List of offsets with each value converted from LBA frames to samples
 */
Points get_offset_sample_indices(const ToC& toc);

} // namespace details
} // namespace v_1_0_0
} // namespace arcstk

#endif

