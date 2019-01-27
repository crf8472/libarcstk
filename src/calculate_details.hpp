#ifndef __LIBARCS_CALCULATE_HPP__
#error "Do not include calculate_details.hpp directly, include calculate.hpp instead"
#endif

#ifndef __LIBARCS_CALCULATE_DETAILS_HPP__
#define __LIBARCS_CALCULATE_DETAILS_HPP__

/**
 * \file calculate_details.hpp Internal APIs and implementations for calculation
 */


// Forward Declaration Required for Partitioner
class SampleChunk;


/**
 * Partitioning of a SampleBlock.
 */
using Partitioning = std::vector<SampleChunk>;


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
	 * Generates partitioning of the SampleBlock in accordance to a
	 * CalcContext.
	 *
	 * For multitrack contexts, a multitrack partition is provided, otherwise
	 * the partition will only contain a single chunk.
	 *
	 * \param[in] offset  Offset of the first sample
	 * \param[in] samples Samples in the block
	 * \param[in] context The context to derive the partitioning from
	 *
	 * \return Partitioning of \c samples as a sequence of sample chunks.
	 */
	Partitioning create_partition(
		const uint32_t offset,
		const SampleBlock *samples,
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
	 * Creates a SampleChunk.
	 *
	 * This method is the exclusive way to create <tt>SampleChunk</tt>s. It is
	 * provided to all Partitioners.
	 *
	 * \param[in] begin_it        Iterator to first sample in this chunk
	 * \param[in] end_it          Iterator to last sample in this chunk
	 * \param[in] first           Global index of the first sample in this chunk
	 * \param[in] last            Global index of the last sample in this chunk
	 * \param[in] first_in_track  TRUE iff this chunk starts its track
	 * \param[in] last_in_track   TRUE iff this chunk ends its track
	 * \param[in] track           Number of the track that contains this chunk
	 *
	 * \return A SampleChunk as specified
	 */
	SampleChunk create_chunk(
		const SampleBlock::const_iterator &begin_it,
		const SampleBlock::const_iterator &end_it,
		const uint32_t     &first,
		const uint32_t     &last,
		const bool         &first_in_track,
		const bool         &last_in_track,
		const TrackNo      &track) const;


private:

	/**
	 * Implements Partitioner::create_partition().
	 */
	virtual Partitioning do_create_partition(
		const uint32_t offset,
		const SampleBlock *samples,
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


protected:

	Partitioning do_create_partition(
		const uint32_t offset,
		const SampleBlock *samples,
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


protected:

	Partitioning do_create_partition(
		const uint32_t offset,
		const SampleBlock *samples,
		const CalcContext &context)
		const override;
};


// SampleChunk


/**
 * Represents a slice of a sequence of samples.
 *
 * A SampleChunk is a contigous subset of a block of samples. A chunk does not
 * hold any samples but provides access to a slice of the underlying block of
 * samples.
 *
 * This class remains opaque. It just represents the input to
 * CalcState::update().
 */
class SampleChunk final
{
	// Partitioners are friends of SampleChunk since they construct
	// SampleChunks exclusively

	friend Partitioner;


public: /* types */

	//using iterator = SampleBlock::iterator;

	using const_iterator = SampleBlock::const_iterator;


public: /* methods */

	/**
	 * Iterator pointing to first_sample_idx().
	 *
	 * \return Iterator pointing to the first sample
	 */
	const_iterator begin() const;

	/**
	 * Iterator pointing behind last_sample_idx().
	 *
	 * \return Iterator pointing behind the last sample
	 */
	const_iterator end() const;

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
	bool first_in_track() const;

	/**
	 * Returns TRUE if the last sample of this chunk is also the last sample
	 * of the track which the chunk is part of.
	 *
	 * \return TRUE iff this is chunk ends a track
	 */
	bool last_in_track() const;

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

	// NOTE: There is no default constructor since SampleChunk have constant
	// elements that cannot be default initialized


	/**
	 * Constructor.
	 *
	 * \param[in] begin_it       Iterator to first sample in this chunk
	 * \param[in] end_it         Iterator to last sample in this chunk
	 * \param[in] first          Global index of the first sample in this chunk
	 * \param[in] last           Global index of the last sample in this chunk
	 * \param[in] first_in_track TRUE iff this chunk starts its track
	 * \param[in] last_in_track  TRUE iff this chunk ends its track
	 * \param[in] track          Number of the track that contains this chunk
	 */
	SampleChunk(
		const const_iterator &begin_it,
		const const_iterator &end_it,
		const uint32_t     &first,
		const uint32_t     &last,
		const bool         &first_in_track,
		const bool         &last_in_track,
		const TrackNo      &track);

	/**
	 * Iterator to start of the chunk
	 */
	const_iterator begin_it_;

	/**
	 * Iterator to end of the chunk
	 */
	const_iterator end_it_;

	/**
	 * Global index of the first sample in this chunk
	 */
	const uint32_t first_sample_idx_;

	/**
	 * Global index of the last sample in this chunk
	 */
	const uint32_t last_sample_idx_;

	/**
	 * TRUE iff the first sample in this chunk is also the first sample in the
	 * track
	 */
	const bool first_in_track_;

	/**
	 * TRUE iff the last sample in this chunk is also the last sample in the
	 * track
	 */
	const bool last_in_track_;

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


/**
 * Interface to the Calculation state.
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
	 * Virtual default destructor.
	 */
	virtual ~CalcState() noexcept;

	/**
	 * Initializes the instance for calculating a new track and skip the
	 * amount of samples specific for this state at the beginning.
	 *
	 * Initializing calles <tt>wipe()</tt> before doing anything.
	 */
	virtual void init_with_skip()
	= 0;

	/**
	 * Initializes the instance for calculating a new track.
	 *
	 * Initializing calles <tt>wipe()</tt> before doing anything.
	 */
	virtual void init_without_skip()
	= 0;

	/**
	 * Amount of samples to be skipped at the beginning.
	 *
	 * \return Amount of samples to be skipped at the beginning
	 */
	virtual uint32_t num_skip_front() const
	= 0;

	/**
	 * Amount of samples to be skipped at the end.
	 *
	 * \return Amount of samples to be skipped at the end
	 */
	virtual uint32_t num_skip_back() const
	= 0;

	/**
	 * Update the calculation state with the samples in the chunk.
	 *
	 * \param[in] chunk The SampleChunk to update the calculation state
	 */
	virtual void update(const SampleChunk &chunk)
	= 0;

	/**
	 * Saves the current subtotals as ARCSs for the specified track and resets
	 * the instance.
	 *
	 * Saving the ARCSs is necessary whenever the calculation for a track is
	 * finished.
	 *
	 * \param[in] track The 0-based track number to save the ARCSs for
	 */
	virtual void save(const TrackNo track)
	= 0;

	/**
	 * Returns the number of currently saved tracks.
	 *
	 * \return Number of currently saved tracks
	 */
	virtual int track_count() const
	= 0;

	/**
	 * Returns current type.
	 *
	 * \return A disjunction of all requested types.
	 */
	virtual checksum::type type() const
	= 0;

	/**
	 * Returns the result for track \c track in a multitrack calculation.
	 *
	 * The result will be empty in singletrack calculation.
	 *
	 * Note that the state is allowed to return more than one type of
	 * <tt>Checksum</tt>s, but the type requested from Calculation is
	 * guaranteed to be included.
	 *
	 * \param[in] track Track number to get the <tt>Checksum</tt>s for.
	 *
	 * \return The <tt>Checksum</tt>s calculated
	 */
	virtual ChecksumSet result(const TrackNo track) const
	= 0;

	/**
	 * Returns the result of a singletrack calculation.
	 *
	 * The result will be empty for a multitrack calculation.
	 *
	 * Note that the state is allowed to return more than one type of
	 * <tt>Checksum</tt>s, but the type requested from Calculation is
	 * guaranteed to be included.
	 *
	 * \return The <tt>Checksum</tt>s calculated
	 */
	virtual ChecksumSet result() const
	= 0;

	/**
	 * Resets the internal subtotals and the multiplier.
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
	 * Resets the internal subtotals and the multiplier and deletes all
	 * previously saved computation results.
	 */
	virtual void wipe()
	= 0;

	/**
	 * Returns the current multiplier.
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
	 * Clone this CalcState object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return A clone of this instance
	 */
	virtual std::unique_ptr<CalcState> clone() const
	= 0;
};


// SampleChunk


SampleChunk::SampleChunk(
		const SampleBlock::const_iterator &begin_it,
		const SampleBlock::const_iterator &end_it,
		const uint32_t    &first,
		const uint32_t    &last,
		const bool        &first_in_track,
		const bool        &last_in_track,
		const TrackNo     &track
	)
	: begin_it_(begin_it)
	, end_it_(end_it)
	, first_sample_idx_(first)
	, last_sample_idx_(last)
	, first_in_track_(first_in_track)
	, last_in_track_(last_in_track)
	, track_(track)
{
	// empty
}


SampleChunk::const_iterator SampleChunk::begin() const
{
	return begin_it_;
}


SampleChunk::const_iterator SampleChunk::end() const
{
	return end_it_;
}


uint32_t SampleChunk::first_sample_idx() const
{
	return first_sample_idx_;
}


uint32_t SampleChunk::last_sample_idx() const
{
	return last_sample_idx_;
}


bool SampleChunk::first_in_track() const
{
	return first_in_track_;
}


bool SampleChunk::last_in_track() const
{
	return last_in_track_;
}


TrackNo SampleChunk::track() const
{
	return track_;
}


uint32_t SampleChunk::size() const
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


Partitioning Partitioner::create_partition(
		const uint32_t offset,
		const SampleBlock *samples,
		const CalcContext &context) const
{
	// If the sample block does not contain any relevant samples,
	// just return an empty chunk list

	auto const block_end = last_sample_idx(offset, samples->size());

	if (block_end < context.first_relevant_sample(1)
		or offset > context.last_relevant_sample())
	{
		ARCS_LOG(LOG_DEBUG1) << "  No relevant samples in this block, skip";

		return Partitioning();
	}

	return this->do_create_partition(offset, samples, context);
}


uint32_t Partitioner::last_sample_idx(const uint32_t offset,
			const uint32_t sample_count) const
{
	return offset + sample_count - 1;
}


SampleChunk Partitioner::create_chunk(
			const SampleBlock::const_iterator &begin_it,
			const SampleBlock::const_iterator &end_it,
			const uint32_t     &first,
			const uint32_t     &last,
			const bool         &first_in_track,
			const bool         &last_in_track,
			const TrackNo      &track) const
{
	return SampleChunk(begin_it, end_it, first, last, first_in_track,
			last_in_track, track);
}


// MultitrackPartitioner


std::unique_ptr<Partitioner> MultitrackPartitioner::clone() const
{
	return std::make_unique<MultitrackPartitioner>(*this);
}


Partitioning MultitrackPartitioner::do_create_partition(
		const uint32_t offset,
		const SampleBlock *samples,
		const CalcContext &context) const
{
	const uint32_t sample_count = samples->size();

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

	uint32_t  begin_offset      = 0;
	uint32_t  end_offset        = 0;
	bool      is_first_in_track = false;
	bool      is_last_in_track  = false;

	const uint8_t last_track    = context.track_count();


	// Now construct all chunks except the last (that needs clipping) in a loop

	Partitioning chunks{};
	chunks.reserve(10);

	while (chunk_last_smpl < block_last_smpl and track <= last_track)
	{
		is_last_in_track  =
			(chunk_last_smpl == context.last_relevant_sample(track));

		is_first_in_track =
			(chunk_first_smpl == context.first_relevant_sample(track));

		begin_offset = chunk_first_smpl - offset;

		end_offset   = chunk_last_smpl  - offset + 1;

		chunks.push_back(
			this->create_chunk(
				samples->cbegin() + begin_offset,
				samples->cbegin() + end_offset,
				chunk_first_smpl,
				chunk_last_smpl,
				is_first_in_track,
				is_last_in_track,
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

	is_first_in_track =
			(chunk_first_smpl == context.first_relevant_sample(track));

	is_last_in_track  =
			(chunk_last_smpl == context.last_relevant_sample(track));

	begin_offset = chunk_first_smpl - offset;

	end_offset   = chunk_last_smpl  - offset + 1;

	ARCS_LOG(LOG_DEBUG1) << "  Create last chunk: " << chunk_first_smpl
				<< " - " << chunk_last_smpl;

	chunks.push_back(
		this->create_chunk(
			samples->cbegin() + begin_offset,
			samples->cbegin() + end_offset,
			chunk_first_smpl,
			chunk_last_smpl,
			is_first_in_track,
			is_last_in_track,
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


Partitioning SingletrackPartitioner::do_create_partition(
		const uint32_t offset,
		const SampleBlock *samples,
		const CalcContext &context) const
{
	const uint32_t sample_count = samples->size();

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

	const bool is_last_in_track {
		chunk_last_smpl == context.last_relevant_sample()
	};

	// Is this the first chunk of the current track in the current block?

	const bool is_first_in_track {
		chunk_first_smpl == context.first_relevant_sample(1)
	};

	// Determine first sample in chunk (easy for singletrack: 0)

	const uint32_t begin_offset { chunk_first_smpl - offset };

	// Determine last sample in chunk (easy for singletrack: sample_count)

	const uint32_t end_offset { chunk_last_smpl - offset + 1 };

	Partitioning chunks;
	chunks.push_back(
		this->create_chunk(
			samples->cbegin() + begin_offset,
			samples->cbegin() + end_offset,
			chunk_first_smpl,
			chunk_last_smpl,
			is_first_in_track,
			is_last_in_track,
			0
		));

	return chunks;
}


// CalcState


CalcState::~CalcState() noexcept = default;

#endif

