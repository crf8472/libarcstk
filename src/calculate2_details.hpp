#ifndef __LIBARCSTK_CALCULATE2_DETAILS_HPP__
#define __LIBARCSTK_CALCULATE2_DETAILS_HPP__
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
#ifndef __LIBARCSTK_ACCURATERIP_HPP__
#include "accuraterip.hpp"
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

/**
 * \internal
 * \ingroup calc
 * \brief Default argument for empty strings, avoid creating temporary objects
 */
const auto EmptyString = std::string { };

/**
 * \brief Convert amount of frames to the equivalent amount of samples.
 *
 * \param[in] frames Amount of frames to convert
 *
 * \return Amount of samples equivalent to \c frames
 */
int32_t frames2samples(const int32_t frames);

/**
 * \brief Convert amount of samples to the equivalent amount of frames.
 *
 * \param[in] samples Amount of samples to convert
 *
 * \return Amount of frames equivalent to \c samples
 */
int32_t samples2frames(const int32_t samples);

/**
 * \brief Convert amount of frames to the equivalent amount of bytes.
 *
 * \param[in] frames Amount of frames to convert
 *
 * \return Amount of bytes equivalent to \c frames
 */
int32_t frames2bytes(const int32_t frames);

/**
 * \brief Convert amount of bytes to the equivalent amount of frames.
 *
 * \param[in] bytes Amount of bytes to convert
 *
 * \return Amount of frames equivalent to \c bytes
 */
int32_t bytes2frames(const int32_t bytes);

/**
 * \brief Convert amount of samples to the equivalent amount of bytes.
 *
 * \param[in] samples Amount of samples to convert
 *
 * \return Amount of bytes equivalent to \c samples
 */
int32_t samples2bytes(const int32_t samples);

/**
 * \brief Convert amount of bytes to the equivalent amount of samples.
 *
 * \param[in] bytes Amount of bytes to convert
 *
 * \return Amount of samples equivalent to \c bytes
 */
int32_t bytes2samples(const int32_t bytes);

/**
 * \brief Check wheter \c t is a valid track number.
 *
 * A valid track number is an integer greater or equal to 1 and
 * less or equal to 99.
 *
 * \param[in] t The number to validate
 *
 * \return TRUE iff \c t is a valid track number, otherwise FALSE.
 */
bool is_valid_track_number(const TrackNo t);

/**
 * \brief Check whether the specified TOC has the specified track.
 *
 * \param[in] track The track number to validate
 * \param[in] toc   The TOC to validate the track against
 *
 * \return TRUE iff \c toc contains \c track, otherwise FALSE.
 */
bool is_valid_track(const TrackNo track, const TOC& toc);

/**
 * \brief Check whether the specified TOC has the specified track.
 *
 * If the TOC has no leadout, samples with indices greater than the offset of
 * the last track will always be verified as part of the last track.
 *
 * If the specified sample index is greater than \c s_total or the leadout of
 * the TOC, the resulting track number will not be valid.
 *
 * \param[in] track   The track number to validate
 * \param[in] toc     The TOC to validate the track against
 * \param[in] s_total Total number of samples
 *
 * \return TRUE iff \c toc contains \c track, otherwise FALSE.
 */
TrackNo track(const int32_t sample, const TOC& toc, const int32_t s_total);


/**
 * \internal
 * \ingroup calc
 *
 * \brief A closed interval of non-negative 32 bit sized integers.
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


int32_t first_relevant_sample(const TrackNo track, const TOC& toc,
		const Interval<int32_t> bounds);

int32_t last_relevant_sample(const TrackNo track, const TOC& toc,
		const Interval<int32_t> bounds);
int32_t last_relevant_sample(const TrackNo track, const TOC& toc,
		const Interval<int32_t> bounds, const int32_t total_frames);
int32_t last_relevant_sample(const Interval<int32_t> bounds,
		const int32_t total_frames);


/**
 * \brief Class template for an incrementable and readable counter..
 *
 * \tparam T Type with definition of +=
 */
template<typename T>
class Counter final
{
	T value_;

public:

	using type = T;

	T value() const
	{
		return value_;
	}

	void increment(T amount)
	{
		value_ += amount;
	}
};


// Forward Declaration Required for Partitioner
class Partition;


/**
 * \internal
 * \brief Partitioning of a range of samples.
 */
using Partitioning = std::vector<Partition>;


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
	 * \brief Generates partitioning of the range of samples.
	 *
	 * \param[in] offset  Offset of the first sample
	 * \param[in] total_samples_in_block Number of samples in the block
	 * \param[in] bounds  Index of first and last sample of all blocks
	 * \param[in] toc     TOC
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	Partitioning create_partitioning(
			const int32_t offset,
			const int32_t total_samples_in_block,
			const Interval<int32_t>& bounds,
			const TOC* toc = nullptr) const;

	/**
	 * \brief Deep copy of this instance.
	 *
	 * \return A deep copy of this instance
	 */
	std::unique_ptr<Partitioner> clone() const;

protected:

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
			const int32_t &begin_offset,
			const int32_t &end_offset,
			const int32_t &first,
			const int32_t &last,
			const bool    &starts_track,
			const bool    &ends_track,
			const TrackNo &track) const;

private:

	/**
	 * \brief Implements Partitioner::create_partitioning() with a TOC.
	 *
	 * \param[in] offset  Offset of the first sample
	 * \param[in] total_samples_in_block Number of samples in the block
	 * \param[in] bounds  Index of first and last sample of all blocks
	 * \param[in] toc     TOC
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	virtual Partitioning do_create_partitioning(
			const Interval<int32_t>& block,
			const Interval<int32_t>& total_bounds,
			const TOC& toc) const
	= 0;

	/**
	 * \brief Implements Partitioner::create_partitioning() without a TOC.
	 *
	 * \param[in] offset  Offset of the first sample
	 * \param[in] total_samples_in_block Number of samples in the block
	 * \param[in] bounds  Index of first and last sample of all blocks
	 *
	 * \return Partitioning of \c samples as a sequence of partitions.
	 */
	virtual Partitioning do_create_partitioning(
			const Interval<int32_t>& block,
			const Interval<int32_t>& total_bounds) const
	= 0;

	virtual std::unique_ptr<Partitioner> do_clone() const
	= 0;
};


/**
 * \brief Provides partitions along track bounds.
 */
class TrackPartitioner final : public Partitioner
{
	virtual Partitioning do_create_partitioning(
			const Interval<int32_t>& block,
			const Interval<int32_t>& bounds,
			const TOC& toc) const final;

	virtual Partitioning do_create_partitioning(
			const Interval<int32_t>& block,
			const Interval<int32_t>& bounds) const final;

	virtual std::unique_ptr<Partitioner> do_clone() const final;
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
			const int32_t &begin_offset,
			const int32_t &end_offset,
			const int32_t &first,
			const int32_t &last,
			const bool    &starts_track,
			const bool    &ends_track,
			const TrackNo &track);

	/**
	 * \brief Relative offset of the first sample in this partition
	 */
	const int32_t begin_offset_;

	/**
	 * \brief Relative offset of the last sample in this partition + 1
	 */
	const int32_t end_offset_;

	/**
	 * \brief Global (absolute) index of the first sample in this partition
	 */
	const int32_t first_sample_idx_;

	/**
	 * \brief Global (absolute) index of the last sample in this partition
	 */
	const int32_t last_sample_idx_;

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
	const TrackNo track_;

public:

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
	 * \brief Returns global index of the first sample in the partition.
	 *
	 * \return Global index of the first sample in this partition
	 */
	int32_t first_sample_idx() const;

	/**
	 * \brief Returns global index of the last sample in the partition.
	 *
	 * \return Global index of the last sample in this partition
	 */
	int32_t last_sample_idx() const;

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
	TrackNo track() const;

	/**
	 * \brief Number of samples in this partition.
	 *
	 * \return Number of samples in this partition
	 */
	std::size_t size() const;
};


/**
 * \brief Calculation progress.
 */
class CalculationState
{
	Counter<int32_t> sample_offset_;
	Counter<TrackNo> current_track_;
	Counter<std::chrono::milliseconds> proc_time_elapsed_;
	calc::Updatable<checksum::type::ARCS1,checksum::type::ARCS2> internal_state_;

public:

	int32_t sample_offset() const;
	void increment_sample_offset(const int32_t amount);

	std::chrono::milliseconds proc_time_elapsed() const;
	void increment_proc_time_elapsed(const std::chrono::milliseconds amount);

	ChecksumSet current_value() const;

	template<class B, class E>
	void update(B& start, E& stop)
	{
		internal_state_.update(start, stop);
	}
};

} // namespace details
} // namespace v_1_0_0
} // namespace arcstk

#endif

