#ifndef __LIBARCSTK_CALC_PARTITION_HPP__
#define __LIBARCSTK_CALC_PARTITION_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Internal API for partitioning sample sequences in tracks
 */

#include <vector>   // for vector
#include <memory>   // for unique_ptr


namespace arcstk
{
inline namespace v_1_0_0
{

// Forward Declaration Required for Partitioners
class CalcContext;

namespace details
{

/**
 * \internal
 * \brief Type to represent amounts of PCM 32 bit samples.
 *
 * A signed integer of at least 32 bit length.
 *
 * The type is required to express the maximum sample count in a medium, which
 * is MAX_BLOCK_ADDRESS * SAMPLES_PER_FRAME == 264.599.412 samples.
 *
 * The type is intended to perform arithmetic operations on it.
 *
 * Compatibility with arcstk::sample_count_t is required. Actually, both types
 * are intended to be identical, but redeclaring it here avoids unnecessary
 * includes.
 */
using psample_count_t = int32_t;

/**
 * \internal
 * \brief Type to represent 1-based track numbers.
 *
 * A signed integer type.
 *
 * Compatibility with arcstk::TrackNo is required. Actually, both types
 * are intended to be identical, but redeclaring it here avoids unnecessary
 * includes.
 */
using ptrackno_t = int;


// Forward Declaration Required for Partitioner
class Partition;

/**
 * \internal
 * \brief Partitioning of a range of samples.
 */
using Partitioning = std::vector<Partition>;


// Partitioner


/**
 * \internal
 * \ingroup calc
 *
 * \brief Interface for generating a partitioning over a sequence of samples.
 *
 * The partitioning is done along the track bounds according to the TOC such
 * that every two partitions adjacent within the same sequence belong to
 * different tracks. This way it is possible to entirely avoid checking for
 * track bounds within the checksum calculation loop.
 */
class Partitioner
{
public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~Partitioner() noexcept
	= default;

	/**
	 * \brief Generates partitioning of the range of samples in accordance to a
	 * CalcContext.
	 *
	 * \param[in] offset            Offset of the first sample
	 * \param[in] number_of_samples Number of samples in the block
	 * \param[in] context           The context to derive the partitioning from
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	Partitioning create_partitioning(
			const psample_count_t offset,
			const psample_count_t number_of_samples,
			const CalcContext &context) const;

	/**
	 * \brief Deep copy of this instance.
	 *
	 * \return A deep copy of this instance
	 */
	std::unique_ptr<Partitioner> clone() const;

protected:

	/**
	 * \brief Index of the last sample of the block.
	 *
	 * \param[in] offset          Offset of the sample block
	 * \param[in] psample_count_t Number of samples in the partition
	 *
	 * \return Index of the last physical sample in the block
	 */
	psample_count_t last_sample_idx(const psample_count_t offset,
			const psample_count_t sample_count) const;

	/**
	 * \brief Creates a Partition.
	 *
	 * This method is the exclusive way to create
	 * @link Partition Partitions @endlink. It is provided to all Partitioners.
	 *
	 * \todo begin_offset and last_offset seem redundant to first and last
	 *
	 * \param[in] begin_offset Local index of the first sample in the partition
	 * \param[in] end_offset   Local index of the last sample in the partition
	 * \param[in] first        Global index of the first sample in the partition
	 * \param[in] last         Global index of the last sample in the partition
	 * \param[in] starts_track TRUE iff this partition starts its track
	 * \param[in] ends_track   TRUE iff this partition ends its track
	 * \param[in] track        Number of the track that contains the partition
	 *
	 * \return A Partition as specified
	 */
	Partition create_partition(
			const psample_count_t &begin_offset,
			const psample_count_t &end_offset,
			const psample_count_t &first,
			const psample_count_t &last,
			const bool            &starts_track,
			const bool            &ends_track,
			const ptrackno_t      &track) const;

private:

	/**
	 * \brief Implements Partitioner::create_partitioning().
	 *
	 * \param[in] offset            Offset of the first sample
	 * \param[in] number_of_samples Number of samples in the block
	 * \param[in] context           The context to derive the partitioning from
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	virtual Partitioning do_create_partitioning(
			const psample_count_t offset,
			const psample_count_t number_of_samples,
			const CalcContext &context) const
	= 0;

	virtual std::unique_ptr<Partitioner> do_clone() const
	= 0;
};


/**
 * \brief Partitioner for multitrack partitions.
 */
class MultitrackPartitioner final : public Partitioner
{
	std::unique_ptr<Partitioner> do_clone() const final;

private:

	/**
	 * \brief Create partitioning for multitrack partitions.
	 *
	 * The partitioning is an ordered list of partitions where a track bound
	 * lies between every two adjacent partitions. The first sample of the first
	 * partition and the last sample of the last partition may not lie on
	 * a track bound.
	 *
	 * \param[in] offset            Offset of the first sample
	 * \param[in] number_of_samples Number of samples in the block
	 * \param[in] context           The context to derive the partitioning from
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	Partitioning do_create_partitioning(
			const psample_count_t offset,
			const psample_count_t number_of_samples,
			const CalcContext &context)
			const override;
};


/**
 * \brief Partitioner for singletrack partitions.
 */
class SingletrackPartitioner final : public Partitioner
{
	std::unique_ptr<Partitioner> do_clone() const final;

private:

	/**
	 * \brief Create partitioning for singletrack partitions.
	 *
	 * The partition contains just one partition representing the entire sample
	 * sequence.
	 *
	 * This supports the usecase where a single track is to be processed.
	 *
	 * \param[in] offset            Offset of the first sample
	 * \param[in] number_of_samples Number of samples in the block
	 * \param[in] context           The context to derive the partitioning from
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	Partitioning do_create_partitioning(
			const psample_count_t offset,
			const psample_count_t number_of_samples,
			const CalcContext &context)
			const override;
};


/**
 * \internal
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
	 * \brief Constructor.
	 *
	 * \todo begin_offset and last_offset seem redundant to first and last
	 *
	 * \param[in] begin_offset Local index of the first sample in the partition
	 * \param[in] end_offset   Local index of the last sample in the partition
	 * \param[in] first        Global index of the first sample in the partition
	 * \param[in] last         Global index of the last sample in the partition
	 * \param[in] starts_track TRUE iff this partition starts its track
	 * \param[in] ends_track   TRUE iff this partition ends its track
	 * \param[in] track        Number of the track that contains the partition
	 */
	Partition(
			const psample_count_t &begin_offset,
			const psample_count_t &end_offset,
			const psample_count_t &first,
			const psample_count_t &last,
			const bool            &starts_track,
			const bool            &ends_track,
			const ptrackno_t      &track);

	/**
	 * \brief Relative offset of the first sample in this partition
	 */
	const psample_count_t begin_offset_;

	/**
	 * \brief Relative offset of the last sample in this partition + 1
	 */
	const psample_count_t end_offset_;

	/**
	 * \brief Global (absolute) index of the first sample in this partition
	 */
	const psample_count_t first_sample_idx_;

	/**
	 * \brief Global (absolute) index of the last sample in this partition
	 */
	const psample_count_t last_sample_idx_;

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
	 * \brief Relative offset of the first sample in the partition.
	 *
	 * \return Relative offset of the first sample in the partition.
	 */
	psample_count_t begin_offset() const;

	/**
	 * \brief Relative offset of the last sample in the partition + 1.
	 *
	 * \return Relative offset of the last sample in the partition + 1.
	 */
	psample_count_t end_offset() const;

	/**
	 * \brief Returns global index of the first sample in the partition.
	 *
	 * \return Global index of the first sample in this partition
	 */
	psample_count_t first_sample_idx() const;

	/**
	 * \brief Returns global index of the last sample in the partition.
	 *
	 * \return Global index of the last sample in this partition
	 */
	psample_count_t last_sample_idx() const;

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
	psample_count_t size() const;
};


/**
 * \internal
 * \ingroup calc
 *
 * \brief A closed interval of non-negative integers.
 */
class Interval final
{
	/**
	 * \brief First number in interval
	 */
	const psample_count_t a_;

	/**
	 * \brief Last number in interval
	 */
	const psample_count_t b_;

public:

	/**
	 * \brief Constructor for <tt>[a,b]</tt>.
	 *
	 * \param[in] a First number in closed interval
	 * \param[in] b Last number in closed interval
	 */
	Interval(const psample_count_t a, const psample_count_t b);

	/**
	 * \brief Returns TRUE iff the closed interval contains \c i, otherwise
	 * FALSE.
	 *
	 * \param[in] i Number to test for containment in interval
	 *
	 * \return TRUE iff \c i is contained in the Interval, otherwise FALSE
	 */
	bool contains(const psample_count_t i) const;
};

} // namespace details
} // namespace v_1_0_0
} // namespace arcstk

#endif

