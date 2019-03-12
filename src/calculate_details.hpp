#ifndef __LIBARCS_CALCULATE_HPP__
#error "Do not include calculate_details.hpp, include calculate.hpp instead"
#endif

#ifndef __LIBARCS_CALCULATE_DETAILS_HPP__
#define __LIBARCS_CALCULATE_DETAILS_HPP__

/**
 * \file calculate_details.hpp Internal APIs and implementations for calculation
 */


#include <cstdint>
#include <memory>
#include <vector>

#ifndef __LIBARCS_LOGGING_HPP__
#include "logging.hpp"
#endif


namespace arcs
{
/// \internal \addtogroup calcImpl
/// @{
inline namespace v_1_0_0
{

// Forward Declaration Required for Partitioner
class Partition;

/**
 * Partitioning of a range of samples
 */
using Partitioning = std::vector<Partition>;


// Partitioner


/**
 * Generates a partitioning over a block of samples using the track bound
 * information.
 *
 * Partitioning provides a way to avoid checking for track bounds in every
 * run of the checksum calculation loop.
 *
 * There are separate partitioners for singletrack and multitrack partitioning.
 */
class Partitioner
{

public:

	/**
	 * Virtual default destructor.
	 */
	virtual ~Partitioner() noexcept;

	/**
	 * Generates partitioning of the range of samples in accordance to a
	 * CalcContext.
	 *
	 * For multitrack contexts, a multitrack partition is provided, otherwise
	 * the partition will only contain a single chunk.
	 *
	 * \param[in] offset  Offset of the first sample
	 * \param[in] number_of_samples Number of samples in the block
	 * \param[in] context The context to derive the partitioning from
	 *
	 * \return Partitioning of \c samples as a sequence of sample chunks.
	 */
	Partitioning create_partitioning(
			const uint32_t offset,
			const uint32_t number_of_samples,
			const CalcContext &context) const;

	/**
	 * Clone this Partitioner object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 */
	virtual std::unique_ptr<Partitioner> clone() const
	= 0;


protected:

	/**
	 * Index of the last sample of the block.
	 *
	 * \param[in] offset       Offset of the sample block
	 * \param[in] sample_count Number of samples in the partition
	 *
	 * \return Index of the last physical sample in the block
	 */
	uint32_t last_sample_idx(const uint32_t offset,
			const uint32_t sample_count) const;

	/**
	 * Creates a Partition.
	 *
	 * This method is the exclusive way to create <tt>Partition</tt>s. It is
	 * provided to all Partitioners.
	 *
	 * \param[in] first        Global index of the first sample in this chunk
	 * \param[in] last         Global index of the last sample in this chunk
	 * \param[in] starts_track TRUE iff this chunk starts its track
	 * \param[in] ends_track   TRUE iff this chunk ends its track
	 * \param[in] track        Number of the track that contains this chunk
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
	 * Implements Partitioner::create_partitioning().
	 */
	virtual Partitioning do_create_partitioning(
			const uint32_t offset,
			const uint32_t number_of_samples,
			const CalcContext &context) const
	= 0;
};


/**
 * Partitioner for multitrack partitions.
 *
 * The partition is an ordered list of chunks where a track bound lies between
 * every two adjacent chunks. The first sample of the first chunk and the last
 * sample of the last chunk may not lie on a track bound.
 */
class MultitrackPartitioner final : public Partitioner
{

public:

	std::unique_ptr<Partitioner> clone() const override;


private:

	Partitioning do_create_partitioning(
			const uint32_t offset,
			const uint32_t number_of_samples,
			const CalcContext &context)
			const override;
};


/**
 * Partitioner for singletrack partitions.
 *
 * The partition contains just one chunk representing the entire block.
 *
 * This supports the usecase where a single track is to be processed.
 */
class SingletrackPartitioner final : public Partitioner
{

public:

	std::unique_ptr<Partitioner> clone() const override;


private:

	Partitioning do_create_partitioning(
			const uint32_t offset,
			const uint32_t number_of_samples,
			const CalcContext &context)
			const override;
};


// Partition


/**
 * Represents a slice of a sequence of samples.
 *
 * A Partition is a contigous subset of a block of samples. A chunk does not
 * hold any samples but provides access to a slice of the underlying block of
 * samples.
 *
 * This class remains opaque. It just represents the input to
 * CalcState::update().
 */
class Partition final
{
	// Partitioners are friends of Partition since they construct
	// Partitions exclusively

	friend Partitioner;


public: /* methods */

	/**
	 * Relative offset of the first sample in the chunk.
	 *
	 * \return Relative offset of the first sample in the chunk.
	 */
	uint32_t begin_offset() const;

	/**
	 * Relative offset of the last sample in the chunk + 1.
	 *
	 * \return Relative offset of the last sample in the chunk + 1.
	 */
	uint32_t end_offset() const;

	/**
	 * Returns global index of the first sample in the chunk.
	 *
	 * \return Global index of the first sample in this chunk
	 */
	uint32_t first_sample_idx() const;

	/**
	 * Returns global index of the last sample in the chunk.
	 *
	 * \return Global index of the last sample in this chunk
	 */
	uint32_t last_sample_idx() const;

	/**
	 * Returns TRUE iff the first sample of this chunk is also the first sample
	 * of the track which the chunk is part of.
	 *
	 * \return TRUE iff this is chunk starts a track
	 */
	bool starts_track() const;

	/**
	 * Returns TRUE if the last sample of this chunk is also the last sample
	 * of the track which the chunk is part of.
	 *
	 * \return TRUE iff this is chunk ends a track
	 */
	bool ends_track() const;

	/**
	 * The track of which the samples in the chunk are part of.
	 *
	 * \return The track that contains this chunk
	 */
	TrackNo track() const;

	/**
	 * Number of samples in this chunk.
	 *
	 * \return Number of samples in this chunk
	 */
	uint32_t size() const;


private:

	// NOTE: There is no default constructor since Partition have constant
	// elements that cannot be default initialized


	/**
	 * Constructor.
	 *
	 * \param[in] first        Global index of the first sample in this chunk
	 * \param[in] last         Global index of the last sample in this chunk
	 * \param[in] starts_track TRUE iff this chunk starts its track
	 * \param[in] ends_track   TRUE iff this chunk ends its track
	 * \param[in] track        Number of the track that contains this chunk
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
	 * Relative offset of the first sample in this chunk
	 */
	const uint32_t begin_offset_;

	/**
	 * Relative offset of the last sample in this chunk + 1
	 */
	const uint32_t end_offset_;

	/**
	 * Global (absolute) index of the first sample in this chunk
	 */
	const uint32_t first_sample_idx_;

	/**
	 * Global (absolute) index of the last sample in this chunk
	 */
	const uint32_t last_sample_idx_;

	/**
	 * TRUE iff the first sample in this chunk is also the first sample in the
	 * track
	 */
	const bool starts_track_;

	/**
	 * TRUE iff the last sample in this chunk is also the last sample in the
	 * track
	 */
	const bool ends_track_;

	/**
	 * 1-based number of the track of which the samples in the chunk are part
	 * of
	 */
	const TrackNo track_;
};


/**
 * A closed interval of non-negative integers.
 */
class Interval final
{

public:

	/**
	 * Constructor for <tt>[a,b]</tt>.
	 *
	 * \param[in] a First number in interval
	 * \param[in] b Last number in interval
	 */
	Interval(const uint32_t a, const uint32_t b);

	/**
	 * Returns TRUE iff the closed interval contains \c i, otherwise FALSE.
	 *
	 * \param[in] i Number to test for containment in interval
	 *
	 * \return TRUE iff \c i is contained in the closed interval, otherwise FALSE
	 */
	bool contains(const uint32_t i) const;


private:

	/**
	 * First number in interval
	 */
	const uint32_t a_;

	/**
	 * Last number in interval
	 */
	const uint32_t b_;
};


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
	: begin_offset_(begin_offset)
	, end_offset_(end_offset)
	, first_sample_idx_(first)
	, last_sample_idx_(last)
	, starts_track_(starts_track)
	, ends_track_(ends_track)
	, track_(track)
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
	: a_(a)
	, b_(b)
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
	// just return an empty chunk list

	auto const block_end = last_sample_idx(offset, number_of_samples);//samples.size());

	if (block_end < context.first_relevant_sample(1)
		or offset > context.last_relevant_sample())
	{
		ARCS_LOG(LOG_DEBUG1) << "  No relevant samples in this block, skip";

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
	const uint32_t sample_count = number_of_samples;//samples.size();

	Interval sample_block {
		offset, this->last_sample_idx(offset, sample_count)
	};

	// If the sample index range of this block contains the last relevant
	// sample, set this as the last sample in block instead of the last
	// physical sample

	uint32_t block_last_smpl = this->last_sample_idx(offset, sample_count);

	if (sample_block.contains(context.last_relevant_sample()))
	{
		block_last_smpl = context.last_relevant_sample();
	}

	// If the sample index range of this block contains the first relevant
	// sample, set this as the first sample of the first chunk instead of the
	// first physical sample

	uint32_t chunk_first_smpl = offset;

	if (sample_block.contains(context.first_relevant_sample(1)))
	{
		chunk_first_smpl = context.first_relevant_sample(1);
	}

	// Will be track_count+1 if 1st sample is beyond global last relevant sample
	// This entails that the loop is not entered for irrelevant chunks
	TrackNo   track             = context.track(chunk_first_smpl);

	// If track > track_count this is global last sample
	uint32_t  chunk_last_smpl   = context.last_relevant_sample(track);

	uint32_t  begin_offset = 0;
	uint32_t  end_offset   = 0;
	bool      starts_track = false;
	bool      ends_track   = false;

	const uint8_t last_track    = context.track_count();


	// Now construct all chunks except the last (that needs clipping) in a loop

	Partitioning chunks{};
	chunks.reserve(10);

	while (chunk_last_smpl < block_last_smpl and track <= last_track)
	{
		ends_track  =
			(chunk_last_smpl == context.last_relevant_sample(track));

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

		ARCS_LOG(LOG_DEBUG1) << "  Create chunk: " << chunk_first_smpl
				<< " - " << chunk_last_smpl;

		++track;

		chunk_first_smpl = chunk_last_smpl + 1;
		chunk_last_smpl  = context.last_relevant_sample(track);
	} // while


	// If the loop has finished or was never entered, the last chunk has to be
	// prepared


	// Clip last chunk to block end if necessary

	if (chunk_last_smpl > block_last_smpl)
	{
		chunk_last_smpl = block_last_smpl;

		ARCS_LOG(LOG_DEBUG1) << "  Block ends within track "
			<< std::to_string(track)
			<< ", clip last sample to: " << chunk_last_smpl;
	}

	// Prepare last chunk

	starts_track =
			(chunk_first_smpl == context.first_relevant_sample(track));

	ends_track  =
			(chunk_last_smpl == context.last_relevant_sample(track));

	begin_offset = chunk_first_smpl - offset;

	end_offset   = chunk_last_smpl  - offset + 1;

	ARCS_LOG(LOG_DEBUG1) << "  Create last chunk: " << chunk_first_smpl
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
	const uint32_t sample_count = number_of_samples;//samples.size();

	Interval sample_block {
		offset, this->last_sample_idx(offset, sample_count)
	};

	// If the sample index range of this block contains the last relevant
	// sample, set this as the last sample in block instead of the last
	// physical sample

	uint32_t chunk_last_smpl { this->last_sample_idx(offset, sample_count) };

	if (sample_block.contains(context.last_relevant_sample()))
	{
		chunk_last_smpl = context.last_relevant_sample();
	}

	// If the sample index range of this block contains the first relevant
	// sample, set this as the first sample of the first chunk instead of the
	// first physical sample

	uint32_t chunk_first_smpl { offset };

	if (sample_block.contains(context.first_relevant_sample(1)))
	{
		chunk_first_smpl = context.first_relevant_sample(1);
	}

	// Create a single chunk spanning the entire sample block, but respect
	// skipping samples at front or back

	// Is this the last chunk in the current track?

	const bool ends_track {
		chunk_last_smpl == context.last_relevant_sample()
	};

	// Is this the first chunk of the current track in the current block?

	const bool starts_track {
		chunk_first_smpl == context.first_relevant_sample(1)
	};

	// Determine first sample in chunk (easy for singletrack: 0)

	const uint32_t begin_offset { chunk_first_smpl - offset };

	// Determine last sample in chunk (easy for singletrack: sample_count)

	const uint32_t end_offset { chunk_last_smpl - offset + 1 };

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


} // namespace v_1_0_0

/// @}

} // namespace arcs

#endif

