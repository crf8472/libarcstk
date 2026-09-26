#ifndef LIBARCSTK_ALGORITHM_HPP_
#define LIBARCSTK_ALGORITHM_HPP_
/**
 * \file
 *
 * \brief Interface for updateable algorithms.
 */

#include <cstdint>          // for int32_t
#include <memory>           // for unique_ptr
#include <string>           // for string
#include <unordered_set>    // for unordered_set

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"     // for ChecksumSet, Checksums
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"     // for AudioSize
#endif

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

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
 * \brief A closed interval <tt>[a,b]</tt>.
 *
 * \tparam T Type with definition of <=
 */
template <typename T>
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
 * \brief Type of the partitioning of a range of samples.
 */
using Partitioning = std::vector<Partition>;


/**
 * \brief List of split points within a range of samples.
 *
 * Guaranteed to be forward iterable and have operator [].
 */
using Points = std::vector<AudioSize>;


/**
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
	Partitioner(Points points, AudioSize total_samples, SampleRange legal);

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~Partitioner() noexcept = default;

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
	 * \brief Length of track \c t.
	 *
	 * \param[in] t Track number
	 *
	 * \return Length of track \c t
	 */
	AudioSize length(const TrackNo t) const;

	/**
	 * \brief Offset of track \c t.
	 *
	 * \param[in] t Track number
	 *
	 * \return Offset of track \c t
	 */
	AudioSize offset(const TrackNo t) const;

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


namespace details
{

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

	TrackPartitioner(const TrackPartitioner& rhs)                 = default;
	TrackPartitioner& operator= (const TrackPartitioner& rhs)     = default;
	TrackPartitioner(TrackPartitioner&& rhs) noexcept             = default;
	TrackPartitioner& operator= (TrackPartitioner&& rhs) noexcept = default;
	~TrackPartitioner() noexcept final                            = default;
};

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

} // namespace details


/**
 * \brief Set of \link arcstk::checksum::type Checksum types \endlink.
 *
 * Guaranteed to be iterable and duplicate-free.
 */
using ChecksumtypeSet = std::unordered_set<checksum::type>;


/**
 * \brief Indicate the track context.
 *
 * AccurateRip algorithms imply different restrictions for calculating the
 * checksums of the the first and last track of an album. Context represents
 * this information.
 */
enum class Context : uint8_t
{
	/**
	 * \brief Single track that is neither first or last track.
	 */
	TRACK       = 0,

	/**
	 * \brief First track is first track of an album.
	 */
	FIRST_TRACK = 1,

	/**
	 * \brief Last track is last track of an album.
	 */
	LAST_TRACK  = 2,

	/**
	 * \brief Entire album, hence first as well as last track.
	 */
	ALBUM       = 3
};

/**
 * \brief Logical OR for two contexts.
 *
 * \param[in] lhs Left hand side
 * \param[in] rhs Right hand side
 *
 * \return Context that respresents the result of lhs-OR-rhs
 */
inline constexpr Context operator | (const Context lhs, const Context rhs)
{
	return static_cast<Context>(
			static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

/**
 * \brief Logical AND for two contexts.
 *
 * \param[in] lhs Left hand side
 * \param[in] rhs Right hand side
 *
 * \return Context that respresents the result of lhs-AND-rhs
 */
inline constexpr Context operator & (const Context lhs, const Context rhs)
{
	return static_cast<Context>(
			static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
}

/**
 * \brief Equality for two contexts.
 *
 * \param[in] lhs Left hand side
 * \param[in] rhs Right hand side
 *
 * \return TRUE if \c lhs equals \c rhs, otherwise FALSE
 */
inline constexpr bool operator == (const Context lhs, const Context rhs)
{
	return static_cast<unsigned>(lhs) == static_cast<unsigned>(rhs);
}

/**
 * \brief Swap two Context instances.
 *
 * \param[in] lhs Left hand side to swap
 * \param[in] rhs Right hand side to swap
 */
inline void swap(Context& lhs, Context& rhs) noexcept
{
	const Context tmp { lhs };
	lhs = rhs;
	rhs = tmp;
}

/**
 * \brief Name of the specified Context.
 *
 * \param[in] c Context to provide name of
 *
 * \return Name of context \c
 */
inline std::string name(const Context& c) noexcept
{
	switch (c)
	{
		case Context::ALBUM:       return "ALBUM";
		case Context::LAST_TRACK:  return "LAST_TRACK";
		case Context::FIRST_TRACK: return "FIRST_TRACK";
		case Context::TRACK:       return "TRACK";
		default: ;
	}

	return {};
}

/**
 * \brief String representation of a Context.
 *
 * This will return the name of the context. It is equivalent to name().
 *
 * \param[in] c Context to transform to a string
 *
 * \return String representation of context \c
 */
inline std::string to_string(const Context& c) noexcept
{
	return name(c);
}

/**
 * \brief Returns TRUE iff \c rhs is not equivalent to Context::TRACK.
 *
 * Equivalent to <code>c != Context::TRACK</code>.
 *
 * \param[in] c Context to evaluate
 *
 * \return TRUE iff \c c is not equivalent to Context::TRACK
 */
inline bool any(const Context& c) noexcept
{
	return static_cast<unsigned>(c) != 0;
}


/**
 * \brief Interface: An algorithm for Checksum calculation.
 *
 * An Algorithm represents a specific calculation method for checksums.
 */
class Algorithm
{
	/**
	 * \brief Internal context of the algorithm.
	 */
	Context context_ { Context::ALBUM };

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	Algorithm() = default;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~Algorithm() noexcept = default;

	/**
	 * \brief Return the Context of this instance.
	 *
	 * \return Context of this instance
	 */
	Context context() const noexcept
	{
		return context_;
	}

	/**
	 * \brief Setup the algorithm with a Context.
	 *
	 * \param[in] c Context for this instance
	 */
	void set_context(const Context c) noexcept
	{
		context_ = c;

		this->do_setup(c);
	}

	/**
	 * \brief Name of this algorithm.
	 *
	 * \return Name of algorithm
	 */
	std::string name() const
	{
		return this->do_name();
	}

	/**
	 * \brief Types of checksums the algorithm calculates.
	 *
	 * \return Checksum types calculated by this algorithm
	 */
	ChecksumtypeSet types() const
	{
		return this->do_types();
	}

	/**
	 * \brief Determine the legal range of samples for the calculation performed
	 * on the input amount.
	 *
	 * The algorithm may request to process only a part of the input - e.g. it
	 * may skip an amount of samples at the beginning and at the end.
	 *
	 * \param[in] size   The input size of samples to process
	 * \param[in] points The offset points in number of PCM samples
	 *
	 * \return Input range of 1-based sample indices to use for calculation
	 */
	SampleRange range(const AudioSize& size, const Points& points) const
	{
		return this->do_range(size, points);
	}

	/**
	 * \brief Create a partitioner for specific values.
	 *
	 * \param[in] offsets  Offsets
	 * \param[in] leadout  Leadout
	 *
	 * \return Partitioner
	 */
	std::unique_ptr<Partitioner> partitioner(
			const Points& offsets, const AudioSize& leadout) const
	{
		return this->do_partitioner(offsets, leadout);
	}

	/**
	 * \brief Amount of total tracks processed so far.
	 *
	 * \return Total number of tracks processed
	 */
	std::size_t total_tracks() const
	{
		return this->do_total_tracks();
	}

	/**
	 * \brief Checksums for track \c t.
	 *
	 * \param[in] t Track number
	 *
	 * \return Checksums for track \c t
	 */
	ChecksumSet track(const TrackNo t) const
	{
		return this->track(t, 0);
	}

	/**
	 * \brief Checksums for track \c t, thereby applying drive offset \c k.
	 *
	 * \param[in] t Track number
	 * \param[in] k Drive offset to apply
	 *
	 * \return Checksums for track \c t, shifted by \c k
	 */
	ChecksumSet track(const TrackNo t, const int k) const
	{
		return this->do_track(t, k);
	}

	/**
	 * \brief Return all tracks.
	 *
	 * \return Checksums for all tracks
	 */
	Checksums tracks() const
	{
		return this->tracks(0);
	}

	/**
	 * \brief Return tracks shifted by \c k.
	 *
	 * \param[in] k Drive offset to apply
	 *
	 * \return Checksums for all tracks shifted by \c k
	 */
	Checksums tracks(const int k) const
	{
		auto checksums = Checksums(this->total_tracks()); // parentheses

		for (auto i = std::size_t { 1 }; i < checksums.size(); ++i)
		{
			checksums[i] = track(static_cast<int>(i) + 1, k);
		}

		return checksums;
	}

	/**
	 * \copydoc SNPT_mf_clone
	 */
	std::unique_ptr<Algorithm> clone() const
	{
		return this->do_clone();
	}

protected:

	/**
	 * \brief Constructor.
	 *
	 * This skips the call of do_setup() and the caller is responsible for
	 * setting up the instance with the context passed.
	 *
	 * \param[in] c Context for this instance
	 */
	explicit Algorithm(const Context c)
		: context_ { c }
	{
		// empty
	}

	/**
	 * \brief Implementation of swap for the base class.
	 *
	 * This is to be called by swap() implementations for subclasses.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap_base(Algorithm& rhs)
	{
		using std::swap;
		swap(context_, rhs.context_);
	}

private:

	virtual void do_setup(const Context c)
	= 0;

	virtual std::string do_name() const
	= 0;

	virtual ChecksumtypeSet do_types() const
	= 0;

	virtual SampleRange do_range(const AudioSize& size, const Points& points)
		const
	= 0;

	virtual std::unique_ptr<Partitioner> do_partitioner(
			const Points& offsets, const AudioSize& leadout) const
	= 0;

	virtual std::size_t do_total_tracks() const
	= 0;

	virtual ChecksumSet do_track(const TrackNo t, const int k) const
	= 0;

	virtual std::unique_ptr<Algorithm> do_clone() const
	= 0;
};


/**
 * \brief Add updating capability to a concrete Algorithm.
 *
 * \tparam A Algorithm type
 *
 * An Updateable is a wrapper for a concrete Algorithm that bestows updating
 * capabilities upon the Algorithm instance.
 *
 * The interface of Algorithm does not allow to alter the instance except
 * for some configuration. Creating an Updateable of an Algorithm makes it
 * possible to update the instance with new input by the caller.
 *
 * However, it should usually not be required to manipulate the Updateable
 * instance directly. This is usually performed via a Calculation. Updateable
 * is a low level interface intended for testing and for implementations that do
 * not use the Calculation interface. It makes it possible to distinguish
 * contexts of reading an Algorithm from contexts were it is acutally used for
 * calculating.
 *
 * The calculation process is promoted by calling update(). A track is to be
 * finalizeed manually by calling finalize_track(). Algorithm instances hold the
 * concrete subtotals.
 */
template <typename A>
class Updateable final
{
	/**
	 * \brief Internal algorithm instance.
	 */
	std::unique_ptr<A> algorithm_ { std::make_unique<A>() };

public:

	/**
	 * \brief Typedef to \c A.
	 */
	using algorithm_type = A;

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	Updateable() = default; // NOLINT(bugprone-crtp-constructor-accessibility)

	/**
	 * \brief Constructor.
	 *
	 * \tparam Args Constructor arguments
	 *
	 * \param[in] args Arguments for this Algorithm
	 */
	template <typename ...Args>
	explicit Updateable(const Args&... args)
		: algorithm_ { std::make_unique<A>(args...) }
	{
		// empty
	}

	/**
	 * \copydoc SNPT_sm_copy_ctor.
	 */
	Updateable(const Updateable& rhs)
		: algorithm_ { std::make_unique<A>(*rhs.algorithm_) }
	{
		// empty
	}

	/**
	 * \copydoc SNPT_sm_copy_op.
	 */
	Updateable& operator= (const Updateable& rhs)
	{
		auto copy { rhs };

		using std::swap;
		swap (*this, copy);
	}

	Updateable(Updateable&& rhs) noexcept = default;
	Updateable& operator= (Updateable&& rhs) = default;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~Updateable() = default;

	/**
	 * \brief Get a pointer to this instance typed by its concrete type.
	 *
	 * \return Pointer of type A* to this instance
	 */
	algorithm_type* algorithm() const
	{
		return algorithm_.get();
	}

	/**
	 * \brief Pass samples coming before the actual range of the algorithm.
	 *
	 * \tparam B Type of iterator pointing to the begin of the sample sequence
	 * \tparam E Type of iterator pointing to the end   of the sample sequence
	 *
	 * \param[in] start Iterator pointing to the begin of the sample sequence
	 * \param[in] stop  Iterator pointing to the end   of the sample sequence
	 */
	template <typename B, typename E>
	void pre_range(B start, E stop)
	{
		algorithm_->perform_pre_range(start, stop);
	}

	/**
	 * \brief Hook for starting a new track.
	 *
	 * \param[in] trackno Track number
	 * \param[in] length  Track length as calculated
	 */
	void start_track(const int trackno, const AudioSize& length)
	{
		algorithm_->perform_start_track(trackno, length);
	}

	/**
	 * \brief Update the instance.
	 *
	 * \tparam B Type of iterator pointing to the begin of the update sequence
	 * \tparam E Type of iterator pointing to the end   of the update sequence
	 *
	 * \param[in] start Iterator pointing to the begin of the update sequence
	 * \param[in] stop  Iterator pointing to the end   of the update sequence
	 * \param[in] size  Size of the update
	 */
	template <typename B, typename E>
	void update(B start, E stop, const std::size_t size)
	{
		// An Algorithm must implement template<> perform_update() to be
		// coverable as an Updateable.
		algorithm_->perform_update(start, stop, size);
	}

	/**
	 * \brief Pass samples coming after the actual range of the algorithm.
	 *
	 * \tparam B Type of iterator pointing to the begin of the sample sequence
	 * \tparam E Type of iterator pointing to the end   of the sample sequence
	 *
	 * \param[in] start Iterator pointing to the begin of the sample sequence
	 * \param[in] stop  Iterator pointing to the end   of the sample sequence
	 */
	template <typename B, typename E>
	void post_range(B start, E stop)
	{
		algorithm_->perform_post_range(start, stop);
	}

	/**
	 * \brief Mark current track as finalizeed.
	 *
	 * What the instance has to do whenever a track is finalizeed can be
	 * implemented in this hook.
	 *
	 * \param[in] trackno Track number
	 * \param[in] length  Track length as calculated
	 */
	void finalize_track(const int trackno, const AudioSize& length)
	{
		return algorithm_->perform_finalize_track(trackno, length);
	}


	// Wrapper functions for read-access to the internal algorithm instance


	std::string name() const
	{
		return algorithm_->name();
	}

	ChecksumtypeSet types() const
	{
		return algorithm_->types();
	}

	Context context() const
	{
		return algorithm_->context();
	}

	std::pair<int32_t,int32_t> range(const AudioSize& size,
			const Points& points) const
	{
		return algorithm_->range(size, points);
	}

	std::unique_ptr<Partitioner> partitioner(
			const Points& offsets, const AudioSize& leadout)
	{
		return algorithm_->partitioner(offsets, leadout,
				this->range(leadout, offsets));
	}

	std::size_t total_tracks() const
	{
		return algorithm_->total_tracks();
	}

	ChecksumSet track(const TrackNo t) const
	{
		return algorithm_->track(t);
	}

	ChecksumSet track(const TrackNo t, const int k) const
	{
		return algorithm_->track(t, k);
	}
};

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

