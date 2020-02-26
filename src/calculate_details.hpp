#ifndef __LIBARCSTK_CALCULATE_HPP__
#error "Do not include calculate_details.hpp, include calculate.hpp instead"
#endif

#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#define __LIBARCSTK_CALCULATE_DETAILS_HPP__

// Just for switching off LSP warnings about unresolvable names
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif

/**
 * \internal
 *
 * \file
 *
 * \brief Internal APIs for checksum calculation
 */

#include <cmath>         // for exp2
#include <cstdint>
#include <memory>
#include <stdexcept>     // for logic_error, invalid_argument
#include <tuple>         // for tuple_element_t, tuple_size
                         // C++17: for tuple_size_v instead of tuple_size
#include <type_traits>   // for enable_if_t, is_same, decay_t
                         // C++17: for is_same_v instead of is_same
#include <utility>       // for pair, forward, make_index_sequence
#include <vector>

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
 * \brief Default argument for empty strings, avoid creating temporary objects
 */
const auto empty_string = std::string { std::string() };


// Forward Declaration Required for Partitioner
class Partition;

/**
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
	virtual ~Partitioner() noexcept;

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
			const uint32_t offset,
			const uint32_t number_of_samples,
			const CalcContext &context) const;

	/**
	 * \brief Clone this Partitioner object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return A deep copy of the instance
	 */
	virtual std::unique_ptr<Partitioner> clone() const
	= 0;


protected:

	/**
	 * \brief Index of the last sample of the block.
	 *
	 * \param[in] offset       Offset of the sample block
	 * \param[in] sample_count Number of samples in the partition
	 *
	 * \return Index of the last physical sample in the block
	 */
	uint32_t last_sample_idx(const uint32_t offset,
			const uint32_t sample_count) const;

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
			const uint32_t     &begin_offset,
			const uint32_t     &end_offset,
			const uint32_t     &first,
			const uint32_t     &last,
			const bool         &starts_track,
			const bool         &ends_track,
			const TrackNo      &track) const;


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
			const uint32_t offset,
			const uint32_t number_of_samples,
			const CalcContext &context) const
	= 0;
};


/**
 * \brief Partitioner for multitrack partitions.
 */
class MultitrackPartitioner final : public Partitioner
{

public:

	std::unique_ptr<Partitioner> clone() const override;


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
			const uint32_t offset,
			const uint32_t number_of_samples,
			const CalcContext &context)
			const override;
};


/**
 * \brief Partitioner for singletrack partitions.
 */
class SingletrackPartitioner final : public Partitioner
{

public:

	std::unique_ptr<Partitioner> clone() const override;


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
			const uint32_t offset,
			const uint32_t number_of_samples,
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


public: /* methods */

	/**
	 * \brief Relative offset of the first sample in the partition.
	 *
	 * \return Relative offset of the first sample in the partition.
	 */
	uint32_t begin_offset() const;

	/**
	 * \brief Relative offset of the last sample in the partition + 1.
	 *
	 * \return Relative offset of the last sample in the partition + 1.
	 */
	uint32_t end_offset() const;

	/**
	 * \brief Returns global index of the first sample in the partition.
	 *
	 * \return Global index of the first sample in this partition
	 */
	uint32_t first_sample_idx() const;

	/**
	 * \brief Returns global index of the last sample in the partition.
	 *
	 * \return Global index of the last sample in this partition
	 */
	uint32_t last_sample_idx() const;

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
	uint32_t size() const;


private:

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
			const uint32_t     &begin_offset,
			const uint32_t     &end_offset,
			const uint32_t     &first,
			const uint32_t     &last,
			const bool         &starts_track,
			const bool         &ends_track,
			const TrackNo      &track);

	/**
	 * \brief Relative offset of the first sample in this partition
	 */
	const uint32_t begin_offset_;

	/**
	 * \brief Relative offset of the last sample in this partition + 1
	 */
	const uint32_t end_offset_;

	/**
	 * \brief Global (absolute) index of the first sample in this partition
	 */
	const uint32_t first_sample_idx_;

	/**
	 * \brief Global (absolute) index of the last sample in this partition
	 */
	const uint32_t last_sample_idx_;

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

};


/**
 * \internal
 * \ingroup calc
 *
 * \brief A closed interval of non-negative integers.
 */
class Interval final
{

public:

	/**
	 * \brief Constructor for <tt>[a,b]</tt>.
	 *
	 * \param[in] a First number in closed interval
	 * \param[in] b Last number in closed interval
	 */
	Interval(const uint32_t a, const uint32_t b);

	/**
	 * \brief Returns TRUE iff the closed interval contains \c i, otherwise
	 * FALSE.
	 *
	 * \param[in] i Number to test for containment in interval
	 *
	 * \return TRUE iff \c i is contained in the Interval, otherwise FALSE
	 */
	bool contains(const uint32_t i) const;


private:

	/**
	 * \brief First number in interval
	 */
	const uint32_t a_;

	/**
	 * \brief Last number in interval
	 */
	const uint32_t b_;
};


/// \cond UNDOC_FUNCTION_BODIES


// Partition


Partition::Partition(
		const uint32_t &begin_offset,
		const uint32_t &end_offset,
		const uint32_t &first,
		const uint32_t &last,
		const bool     &starts_track,
		const bool     &ends_track,
		const TrackNo  &track
	)
	: begin_offset_ { begin_offset }
	, end_offset_ { end_offset }
	, first_sample_idx_ { first }
	, last_sample_idx_ { last }
	, starts_track_ { starts_track }
	, ends_track_ { ends_track }
	, track_ { track }
{
	// empty
}


uint32_t Partition::begin_offset() const
{
	return begin_offset_;
}


uint32_t Partition::end_offset() const
{
	return end_offset_;
}


uint32_t Partition::first_sample_idx() const
{
	return first_sample_idx_;
}


uint32_t Partition::last_sample_idx() const
{
	return last_sample_idx_;
}


bool Partition::starts_track() const
{
	return starts_track_;
}


bool Partition::ends_track() const
{
	return ends_track_;
}


TrackNo Partition::track() const
{
	return track_;
}


uint32_t Partition::size() const
{
	return last_sample_idx() - first_sample_idx() + 1;
}


// Interval


Interval::Interval(const uint32_t a, const uint32_t b)
	: a_ { a }
	, b_ { b }
{
	// empty
}


bool Interval::contains(const uint32_t i) const
{
	if (a_ <= b_)
	{
		return a_ <= i and i <= b_;
	}

	return a_ >= i and i >= b_;
}


// Partitioner


Partitioner::~Partitioner() noexcept = default;


Partitioning Partitioner::create_partitioning(
		const uint32_t offset,
		const uint32_t number_of_samples,
		const CalcContext &context) const
{
	// If the sample block does not contain any relevant samples,
	// just return an empty partition list

	auto const block_end { last_sample_idx(offset, number_of_samples) };

	if (block_end < context.first_relevant_sample(1)
		or offset > context.last_relevant_sample())
	{
		ARCS_LOG(DEBUG1) << "  No relevant samples in this block, skip";

		return Partitioning();
	}

	return this->do_create_partitioning(offset, number_of_samples, context);
}


uint32_t Partitioner::last_sample_idx(const uint32_t offset,
		const uint32_t sample_count) const
{
	return offset + sample_count - 1;
}


Partition Partitioner::create_partition(
		const uint32_t     &begin_offset,
		const uint32_t     &end_offset,
		const uint32_t     &first,
		const uint32_t     &last,
		const bool         &starts_track,
		const bool         &ends_track,
		const TrackNo      &track) const
{
	return Partition(begin_offset, end_offset, first, last, starts_track,
			ends_track, track);
}


// MultitrackPartitioner


std::unique_ptr<Partitioner> MultitrackPartitioner::clone() const
{
	return std::make_unique<MultitrackPartitioner>(*this);
}


Partitioning MultitrackPartitioner::do_create_partitioning(
		const uint32_t offset,
		const uint32_t number_of_samples,
		const CalcContext &context) const
{
	const auto sample_count = uint32_t { number_of_samples };

	Interval sample_block {
		offset, this->last_sample_idx(offset, sample_count)
	};

	// If the sample index range of this block contains the last relevant
	// sample, set this as the last sample in block instead of the last
	// physical sample

	auto block_last_smpl = uint32_t {
		this->last_sample_idx(offset, sample_count) };

	if (sample_block.contains(context.last_relevant_sample()))
	{
		block_last_smpl = context.last_relevant_sample();
	}

	// If the sample index range of this block contains the first relevant
	// sample, set this as the first sample of the first partition instead of
	// the first physical sample

	auto chunk_first_smpl = uint32_t { offset };

	if (sample_block.contains(context.first_relevant_sample(1)))
	{
		chunk_first_smpl = context.first_relevant_sample(1);
	}

	// Will be track_count+1 if 1st sample is beyond global last relevant sample
	// This entails that the loop is not entered for irrelevant partitions
	auto track = TrackNo { context.track(chunk_first_smpl) };

	// If track > track_count this is global last sample
	auto chunk_last_smpl = uint32_t { context.last_relevant_sample(track) };

	auto begin_offset = uint32_t { 0 } ;
	auto end_offset   = uint32_t { 0 } ;
	auto starts_track = bool     { false } ;
	auto ends_track   = bool     { false } ;

	const auto last_track = uint8_t { context.track_count() };


	// Now construct all partitions except the last (that needs clipping) in a
	// loop

	Partitioning chunks{};
	chunks.reserve(10);

	while (chunk_last_smpl < block_last_smpl and track <= last_track)
	{
		ends_track   = (chunk_last_smpl == context.last_relevant_sample(track));

		starts_track =
			(chunk_first_smpl == context.first_relevant_sample(track));

		begin_offset = chunk_first_smpl - offset;

		end_offset   = chunk_last_smpl  - offset + 1;

		chunks.push_back(
			this->create_partition(
				begin_offset,
				end_offset,
				chunk_first_smpl,
				chunk_last_smpl,
				starts_track,
				ends_track,
				track
			)
		);

		ARCS_LOG(DEBUG1) << "  Create chunk: " << chunk_first_smpl
				<< " - " << chunk_last_smpl;

		++track;

		chunk_first_smpl = chunk_last_smpl + 1;
		chunk_last_smpl  = context.last_relevant_sample(track);
	} // while


	// If the loop has finished or was never entered, the last partition has to
	// be prepared


	// Clip last partition to block end if necessary

	if (chunk_last_smpl > block_last_smpl)
	{
		chunk_last_smpl = block_last_smpl;

		ARCS_LOG(DEBUG1) << "  Block ends within track "
			<< std::to_string(track)
			<< ", clip last sample to: " << chunk_last_smpl;
	}

	// Prepare last partition

	starts_track = (chunk_first_smpl == context.first_relevant_sample(track));

	ends_track   = (chunk_last_smpl == context.last_relevant_sample(track));

	begin_offset = chunk_first_smpl - offset;

	end_offset   = chunk_last_smpl  - offset + 1;

	ARCS_LOG(DEBUG1) << "  Create last chunk: " << chunk_first_smpl
				<< " - " << chunk_last_smpl;

	chunks.push_back(
		this->create_partition(
			begin_offset,
			end_offset,
			chunk_first_smpl,
			chunk_last_smpl,
			starts_track,
			ends_track,
			track
		)
	);

	chunks.shrink_to_fit();

	return chunks;
}


// SingletrackPartitioner


std::unique_ptr<Partitioner> SingletrackPartitioner::clone() const
{
	return std::make_unique<SingletrackPartitioner>(*this);
}


Partitioning SingletrackPartitioner::do_create_partitioning(
		const uint32_t offset,
		const uint32_t number_of_samples,
		const CalcContext &context) const
{
	const auto sample_count = uint32_t { number_of_samples };

	Interval sample_block {
		offset, this->last_sample_idx(offset, sample_count)
	};

	// If the sample index range of this block contains the last relevant
	// sample, set this as the last sample in block instead of the last
	// physical sample

	auto chunk_last_smpl = uint32_t { this->last_sample_idx(offset, sample_count) };

	if (sample_block.contains(context.last_relevant_sample()))
	{
		chunk_last_smpl = context.last_relevant_sample();
	}

	// If the sample index range of this block contains the first relevant
	// sample, set this as the first sample of the first partition instead of
	// the first physical sample

	auto chunk_first_smpl = uint32_t { offset };

	if (sample_block.contains(context.first_relevant_sample(1)))
	{
		chunk_first_smpl = context.first_relevant_sample(1);
	}

	// Create a single partition spanning the entire sample block, but respect
	// skipping samples at front or back

	// Is this the last partition in the current track?

	const bool ends_track {
		chunk_last_smpl == context.last_relevant_sample()
	};

	// Is this the first partition of the current track in the current block?

	const bool starts_track {
		chunk_first_smpl == context.first_relevant_sample(1)
	};

	// Determine first sample in partition (easy for singletrack: 0)

	const auto begin_offset = uint32_t { chunk_first_smpl - offset };

	// Determine last sample in partition (easy for singletrack: sample_count)

	const auto end_offset = uint32_t { chunk_last_smpl - offset + 1 };

	Partitioning chunks;
	chunks.push_back(
		this->create_partition(
			begin_offset,
			end_offset,
			chunk_first_smpl,
			chunk_last_smpl,
			starts_track,
			ends_track,
			0
		));

	return chunks;
}

/// \endcond

/**
 * \internal
 * \ingroup calc
 *
 * \brief Base class for CalcContext implementations.
 *
 * Provides the properties AudioSize and filename and implements
 * CalcContext::first_relevant_sample() as well as
 * CalcContext::last_relevant_sample().
 */
class CalcContextBase : virtual public arcstk::v_1_0_0::CalcContext
{

public:

	/**
	 * \brief Construct with skip configuration.
	 *
	 * \param[in] filename       The audio file to process
	 * \param[in] num_skip_front Amount of samples to skip at the beginning
	 * \param[in] num_skip_back  Amount of samples to skip at the end
	 */
	CalcContextBase(const std::string &filename, const uint32_t num_skip_front,
			const uint32_t num_skip_back);


private:

	void do_set_audio_size(const AudioSize &audio_size) override;

	const AudioSize& do_audio_size() const override;

	// do_sample_count()

	void do_set_filename(const std::string &filename) override;

	std::string do_filename() const override;

	// do_track_count()
	// do_is_multi_track()

	uint32_t do_first_relevant_sample(const TrackNo) const override;

	uint32_t do_first_relevant_sample_0() const override;

	uint32_t do_last_relevant_sample(const TrackNo track) const override;

	uint32_t do_last_relevant_sample_0() const override;

	// do_track(const uint32_t)
	// do_offset(const uint8_t)
	// do_length(const uint8_t)
	// do_id()
	// do_skips_front()
	// do_skips_back()

	uint32_t do_num_skip_front() const override;

	uint32_t do_num_skip_back() const override;

	void do_notify_skips(const uint32_t num_skip_front,
			const uint32_t num_skip_back) override;

	// do_clone()

	/**
	 * \brief Hook called after set_audio_size() is finished.
	 */
	virtual void do_hook_post_set_audio_size();


protected:

	/**
	 * \brief Default destructor.
	 *
	 * This class is not intended to be used for polymorphical deletion.
	 */
	~CalcContextBase() noexcept override;

	/**
	 * \brief Base implementation for operator == in subclasses.
	 *
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE iff \c *this == \c rhs, otherwise FALSE
	 */
	bool equals(const CalcContextBase &rhs) const noexcept;


private:

	/**
	 * \brief Internal representation of the AudioSize of the current audiofile
	 */
	AudioSize audiosize_;

	/**
	 * \brief Internal representation of the audiofilename
	 */
	std::string filename_;

	/**
	 * \brief Number of samples to skip at beginning of first track if requested
	 */
	uint32_t num_skip_front_;

	/**
	 * \brief Number of samples to skip at end of last track if requested
	 */
	uint32_t num_skip_back_;
};


class SingletrackCalcContext;


/**
 * \brief Equality for SingletrackCalcContext
 *
 * \param[in] lhs Left hand side of the comparison
 * \param[in] rhs Right hand side of the comparison
 */
bool operator == (const SingletrackCalcContext &lhs,
		const SingletrackCalcContext &rhs) noexcept;


/**
 * \internal
 * \ingroup calc
 *
 * \brief CalcContext for singletrack mode.
 *
 * A SingletrackCalcContext is a CalcContext derived from an actual filename
 * representing a single track.
 */
class SingletrackCalcContext final : public CalcContextBase
								   , public Comparable<SingletrackCalcContext>
{

public:

	friend bool operator == (const SingletrackCalcContext &lhs,
		const SingletrackCalcContext &rhs) noexcept;

	/**
	 * \brief Constructor.
	 *
	 * No samples are skipped.
	 *
	 * \param[in] filename Name of the audio file
	 */
	explicit SingletrackCalcContext(const std::string &filename);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] filename   Name of the audio file
	 * \param[in] skip_front Indicate whether to skip samples at the beginning
	 * \param[in] skip_back  Indicate whether to skip samples at the end
	 */
	SingletrackCalcContext(const std::string &filename,
			const bool skip_front, const bool skip_back);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] filename   Name of the audio file
	 * \param[in] skip_front Indicate whether to skip samples at the beginning
	 * \param[in] num_skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Indicate whether to skip samples at the end
	 * \param[in] num_skip_back Amount of samples to skip at the end
	 */
	SingletrackCalcContext(const std::string &filename,
			const bool skip_front, const uint32_t num_skip_front,
			const bool skip_back,  const uint32_t num_skip_back);

	/**
	 * \brief Activate skipping of the first 2939 samples of the first track.
	 *
	 * \param[in] skip TRUE skips the first 2939 samples of the first track
	 */
	void set_skip_front(const bool skip);

	/**
	 * \brief Activate skipping of the last 2940 samples of the last track.
	 *
	 * \param[in] skip TRUE skips the last 2940 samples of the last track
	 */
	void set_skip_back(const bool skip);


private:

	uint8_t do_track_count() const final;

	bool do_is_multi_track() const final;

	uint32_t do_first_relevant_sample(const TrackNo track) const final;

	// do_first_relevant_sample() is generic in CalcContextBase

	uint32_t do_last_relevant_sample(const TrackNo track) const final;

	// do_last_relevant_sample() is generic in CalcContextBase

	TrackNo do_track(const uint32_t smpl) const final;

	lba_count do_offset(const uint8_t track) const final;

	lba_count do_length(const uint8_t track) const final;

	ARId do_id() const final;

	bool do_skips_front() const final;

	bool do_skips_back() const final;

	std::unique_ptr<CalcContext> do_clone() const final;

	/**
	 * \brief State: indicates whether to skip the front samples
	 */
	bool skip_front_;

	/**
	 * \brief State: indicates whether to skip the back samples
	 */
	bool skip_back_;
};


class MultitrackCalcContext;


/**
 * \brief Equality check for MultitrackCalcContext
 *
 * \param[in] lhs Left hand side of the comparison
 * \param[in] rhs Right hand side of the comparison
 */
bool operator == (const MultitrackCalcContext &lhs,
		const MultitrackCalcContext &rhs) noexcept;


/**
 * \internal
 * \ingroup calc
 *
 * \brief CalcContext for multitrack mode.
 *
 * A MultitrackCalcContext is a CalcContext derived from a TOC and an optional
 * actual filename. It always skips the front and back samples.
 */
class MultitrackCalcContext final : public CalcContextBase
								  , public Comparable<MultitrackCalcContext>
{

public:

	friend bool operator == (const MultitrackCalcContext &lhs,
		const MultitrackCalcContext &rhs) noexcept;

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename . No samples
	 * are skipped.
	 *
	 * \param[in] toc      Name of the TOC
	 * \param[in] filename Name of the file
	 */
	MultitrackCalcContext(const TOC &toc,
			const std::string &filename = empty_string);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename . No samples
	 * are skipped.
	 *
	 * \param[in] toc      Name of the TOC
	 * \param[in] filename Name of the file
	 */
	MultitrackCalcContext(const std::unique_ptr<TOC> &toc,
			const std::string &filename = empty_string);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename .
	 *
	 * \param[in] toc        Name of the TOC
	 * \param[in] skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Amount of samples to skip at the end
	 * \param[in] filename   Name of the file
	 */
	MultitrackCalcContext(const TOC &toc, const uint32_t skip_front,
			const uint32_t skip_back,
			const std::string &filename = empty_string);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename .
	 *
	 * \param[in] toc        Name of the TOC
	 * \param[in] skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Amount of samples to skip at the end
	 * \param[in] filename   Name of the file
	 */
	MultitrackCalcContext(const std::unique_ptr<TOC> &toc,
			const uint32_t skip_front, const uint32_t skip_back,
			const std::string &filename = empty_string);

	/**
	 * \brief The TOC of the audio input file.
	 *
	 * \return The TOC information to use for the audio input
	 */
	const TOC& toc() const;

	/**
	 * \brief Set the TOC for the audio input.
	 *
	 * \param[in] toc The TOC information to use for the audio input
	 */
	void set_toc(const TOC &toc);


private:

	void do_hook_post_set_audio_size() final;

	uint8_t do_track_count() const final;

	bool do_is_multi_track() const final;

	uint32_t do_first_relevant_sample(const TrackNo track) const final;

	// do_first_relevant_sample() is generic in CalcContextBase

	uint32_t do_last_relevant_sample(const TrackNo track) const final;

	// do_last_relevant_sample() is generic in CalcContextBase

	TrackNo do_track(const uint32_t smpl) const final;

	lba_count do_offset(const uint8_t track) const final;

	lba_count do_length(const uint8_t track) const final;

	ARId do_id() const final;

	bool do_skips_front() const final;

	bool do_skips_back() const final;

	std::unique_ptr<CalcContext> do_clone() const final;

	/**
	 * \brief TOC representation
	 */
	TOC toc_;
};


/**
 * \internal
 * \ingroup calc
 *
 * \brief Interface to the Calculation state.
 *
 * A calculation state is initialized with a multiplier. It is subsequently
 * updated with new samples. After a track is completed, the calculated
 * checksums for a specified track must be saved and can thereafter be accessed
 * via the appropriate accessors.
 *
 * The calculation state determines which checksums a Calculation actually
 * calculates.
 */
class CalcState
{

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~CalcState() noexcept;

	/**
	 * \brief Initializes the instance for calculating a new track and skip the
	 * amount of samples specific for this state at the beginning.
	 *
	 * Initializing calles <tt>wipe()</tt> before doing anything.
	 */
	virtual void init_with_skip()
	= 0;

	/**
	 * \brief Initializes the instance for calculating a new track.
	 *
	 * Initializing calles <tt>wipe()</tt> before doing anything.
	 */
	virtual void init_without_skip()
	= 0;

	/**
	 * \brief Amount of samples to be skipped at the beginning.
	 *
	 * \return Amount of samples to be skipped at the beginning
	 */
	virtual uint32_t num_skip_front() const
	= 0;

	/**
	 * \brief Amount of samples to be skipped at the end.
	 *
	 * \return Amount of samples to be skipped at the end
	 */
	virtual uint32_t num_skip_back() const
	= 0;

	/**
	 * \brief Update the calculation state with a sequence of samples.
	 *
	 * \param[in] begin Iterator pointing to the beginning of the sequence
	 * \param[in] end   Iterator pointing to the end of the sequence
	 */
	virtual void update(SampleInputIterator &begin, SampleInputIterator &end)
	= 0;

	/**
	 * \brief Saves the current subtotals as ARCSs for the specified track and
	 * resets the instance.
	 *
	 * Saving the ARCSs is necessary whenever the calculation for a track is
	 * finished.
	 *
	 * \param[in] track The 0-based track number to save the ARCSs for
	 */
	virtual void save(const TrackNo track)
	= 0;

	/**
	 * \brief Returns the number of currently saved tracks.
	 *
	 * \return Number of currently saved tracks
	 */
	virtual int track_count() const
	= 0;

	/**
	 * \brief Returns current type.
	 *
	 * \return A disjunction of all requested types.
	 */
	virtual checksum::type type() const
	= 0;

	/**
	 * \brief Returns the result for track \c track in a multitrack calculation.
	 *
	 * The result will be empty in singletrack calculation.
	 *
	 * Note that the state is allowed to return more than one type of
	 * @link Checksum Checksums @endlink, but the type requested from
	 * Calculation is guaranteed to be included.
	 *
	 * \param[in] track Track number to get the @link Checksum Checksums @endlink for.
	 *
	 * \return The @link Checksum Checksums @endlink calculated
	 */
	virtual ChecksumSet result(const TrackNo track) const
	= 0;

	/**
	 * \brief Returns the result of a singletrack calculation.
	 *
	 * The result will be empty for a multitrack calculation.
	 *
	 * Note that the state is allowed to return more than one type of
	 * @link Checksum Checksums @endlink, but the type requested from
	 * Calculation is guaranteed to be included.
	 *
	 * \return The @link Checksum Checksums @endlink calculated
	 */
	virtual ChecksumSet result() const
	= 0;

	/**
	 * \brief Resets the internal subtotals and the multiplier.
	 *
	 * Computation results that have already been <tt>save()</tt>d are kept.
	 * Calling <tt>reset()</tt> does therefore not change the output of
	 * subsequent calls of <tt>arcs1()</tt> or <tt>arcs2()</tt>.
	 *
	 * Resetting the instance is necessary before starting the calculation for a
	 * new track. However, it is not necessary to <tt>reset()</tt> an instance
	 * that was already <tt>init()</tt>ed.
	 */
	virtual void reset()
	= 0;

	/**
	 * \brief Resets the internal subtotals and the multiplier and deletes all
	 * previously saved computation results.
	 */
	virtual void wipe()
	= 0;

	/**
	 * \brief Returns the current multiplier.
	 *
	 * The current multiplier will be applied on the <i>next</i> multiplication
	 * operation. The <i>last</i> multiplier that was actually applied is
	 * <tt>mult() - 1</tt>.
	 *
	 * \return Multiplier for next multiplication operation
	 */
	virtual uint32_t mult() const
	= 0;

	/**
	 * \brief Clone this CalcState object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return A clone of this instance
	 */
	virtual std::unique_ptr<CalcState> clone() const
	= 0;
};


/**
 * \internal
 * \ingroup calc
 *
 * \brief Abstract base for ARCS calculating CalcStates.
 *
 * \see CalcState
 */
class CalcStateARCSBase : public CalcState
{
	// Note: This could be a template, e.g. CalcStateARCS<bool both = true> and
	// CalcStateARCS<true> computes v2 and v1 and CalcStateARCS<false> only v1.
	// The annoying code duplication in CalcStateV1 and CalcStateV1andV2 is
	// currently motivated by avoiding calls to virtual methods in update().
	// (E.g. the multiplier_ would be in the base class, accessing it would
	// require a method call. Of course this might come without real extra cost
	// and may be optimized away but still feels not nice.)

public:

	/**
	 * \brief Constructor
	 */
	CalcStateARCSBase();

	/**
	 * \brief Implements CalcState::init_with_skip()
	 *
	 * Initializes the multiplier with 2941.
	 *
	 * The initial value of the multiplier has to reflect the amount of leading
	 * samples that have been skipped. The multiplier is 1-based, so
	 * <tt>init(1)</tt> means that no samples are skipped at all, and
	 * <tt>init(2941)</tt> means that the first <tt>2939</tt> samples are
	 * skipped and the (0-based) sample <tt>2940</tt> will be the first sample
	 * to actually use.
	 *
	 * Initializing calls <tt>wipe()</tt> before doing anything.
	 */
	void init_with_skip() override;

	/**
	 * \brief Implements CalcState::init_without_skip()
	 *
	 * Initializes the multiplier with 1 for no samples are skipped.
	 *
	 * Initializing calls <tt>wipe()</tt> before doing anything.
	 */
	void init_without_skip() override;

	uint32_t num_skip_front() const override;

	uint32_t num_skip_back() const override;

	void update(SampleInputIterator &begin, SampleInputIterator &end) final;


protected:

	/**
	 * \brief Default destructor.
	 *
	 * This class is not intended to be used for polymorphical deletion.
	 */
	~CalcStateARCSBase() noexcept override;

	/**
	 * \brief Bitmask for getting the lower 32 bits of a 64 bit unsigned
	 * integer.
	 */
	static constexpr uint_fast32_t LOWER_32_BITS_ { 0xFFFFFFFF };


private:

	/**
	 * \brief Worker: initialize state with specified multiplier.
	 */
	virtual void init(const uint32_t mult)
	= 0;

	/**
	 * \brief Worker: implement update()
	 */
	virtual void do_update(SampleInputIterator &begin, SampleInputIterator &end)
	= 0;

	/**
	 * \brief Actual amount of skipped samples at front
	 */
	uint32_t actual_skip_front_;

	/**
	 * \brief Actual amount of skipped samples at back
	 */
	uint32_t actual_skip_back_;
};


class CalcStateV1;
class CalcStateV1andV2;


/**
 * \internal
 * \brief CalcState related tools.
 */
namespace state
{


/**
 * \brief An aggregate of all predefined CalcState implementations.
 */
using types = std::tuple<
	CalcStateV1,       // type::ARCS1
	CalcStateV1andV2   // type::ARCS2
	>;


/**
 * \internal
 * \brief Implementation details of namespace state.
 */
namespace details
{


// The set of the following five template functions along with state::make is
// probably the most sophisticated solution in the entire lib. It enables
// to load a concrete subclass of CalcState by just passing a checksum::type.
// This behaviour could also have been implemented by a bare
// switch-case-statement but this method is completely generic and so much
// cooler!


/**
 * \internal
 * \brief Invoke callable \c F with a nullptr of type \c T* and \c i as
 * arguments iff \c T is not void.
 *
 * \tparam T Base type to invoke \c F on
 * \tparam F Callable type with params \c T* and \c i
 *
 * \param[in] func The callable F to invoke on T*
 * \param[in] i    The size to pass to func
 */
template<typename T, typename F>
auto invoke(F&& func, std::size_t i)
#if __cplusplus >= 201703L
	-> std::enable_if_t<!std::is_same_v<T, void>>
#else
	-> std::enable_if_t<!std::is_same<T, void>::value>
#endif
{
	func(static_cast<T*>(nullptr), i);
}


/**
 * \internal
 * \brief In case \c T is void just do nothing.
 *
 * \tparam T Base type to invoke \c F on
 * \tparam F Callable type with params \c T* and \c i
 */
template<typename T, typename F>
auto invoke(F&& /* func */, std::size_t /* i */)
#if __cplusplus >= 201703L
	-> std::enable_if_t<std::is_same_v<T, void>>
#else
	-> std::enable_if_t<std::is_same<T, void>::value>
#endif
{
	// empty
}


/**
 * \internal
 * \brief Implementation of for_all_types
 *
 * \tparam TUPLE Some iterable tuple type
 * \tparam F     Callable type
 * \tparam I     Parameter pack of sizes
 *
 * \param[in] func To be invoked on each combination of an element and a size
 */
template <typename TUPLE, typename F, std::size_t... I>
void for_all_types_impl(F&& func, std::index_sequence<I...>)
{
	int x[] = { 0, (invoke<std::tuple_element_t<I, TUPLE>>(func, I), 0)... };
	static_cast<void>(x); // to avoid warning for unused x
}


/**
 * \internal
 * \brief Invoke \c func on each type in tuple \c TUPLE
 *
 * \tparam TUPLE Some iterable tuple type
 * \tparam F     Callable type
 *
 * \param[in] func To be invoked on each combination of an element and a size
 */
template <typename TUPLE, typename F>
void for_all_types(F&& func)
{
	for_all_types_impl<TUPLE>(func,
#if __cplusplus >= 201703L
			std::make_index_sequence<std::tuple_size_v<TUPLE>>()
#else
			std::make_index_sequence<std::tuple_size<TUPLE>::value>()
#endif
	);
}


/**
 * \brief Instantiate one of the types in \c TUPLE as \c R by callable \c F .
 *
 * \c R corresponds to the type on index position \c i in \c TUPLE .
 *
 * Argument \c i is of type std::size_t since it is intended to use this
 * function to load a class by specifying an enum value.
 *
 * \tparam T     Type on index position \c i in \c TUPLE
 * \tparam TUPLE Some iterable tuple type
 * \tparam F     Callable type
 *
 * \param[in] func The callable to instantiate the corresponding type
 * \param[in] i    The size to compare
 *
 * \return An instance of type \c R
 */
template <typename T, typename TUPLE, typename F>
T instantiate(F&& func, std::size_t i)
{
	T instance;
	auto found = bool { false };

	// find the enum value whose size corresponds to the index position of the
	// type in TUPLE and invoke func on it
	for_all_types<TUPLE>(
		[&](auto p, std::size_t j) // this lambda becomes 'func' in invoke()
		{
			// This requires the enum to be defined in powers of 2, i.e.:
			// 1, 2, 4, 16, 32 ...
			const auto enum_val { std::exp2(j) };

			if (i == enum_val)
			{
				instance = func(p);
				found    = true;
			}
		}
	);

	if (not found)
	{
		auto msg = std::stringstream {};
		msg << "No type found with id " << i;

		throw std::invalid_argument(msg.str());
	}

	return instance;
}

} // namespace state::details


/**
 * \brief Instantiate the CalcState for a checksum::type.
 *
 * \tparam X Types to be instantiated
 * \tparam T checksum::type to instantiate a CalcState for
 *
 * \param[in] state_type The state type to instantiate
 * \param[in] x          Constructor arguments for constructing CalcState
 *
 * \return The CalcState for \c state_type
 */
template<typename... X, typename T>
auto make(const T state_type, X&&... x) -> std::unique_ptr<CalcState>
{
	return details::instantiate<std::unique_ptr<CalcState>, state::types> (
		[&](auto ptr)
		{
			return std::make_unique<std::decay_t<decltype(*ptr)>>(
					std::forward<X>(x)...
			);
		},
		static_cast<std::size_t>(state_type)
	);
}

} // namespace state

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif

