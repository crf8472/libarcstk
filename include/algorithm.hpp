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
 * \brief Set of \link arcstk::checksum::type Checksum types \endlink.
 *
 * Guaranteed to be iterable and duplicate-free.
 */
using ChecksumtypeSet = std::unordered_set<checksum::type>;

/**
 * \brief List of split points within a range of samples.
 *
 * Guaranteed to be forward iterable and have operator [].
 */
using Points = std::vector<AudioSize>;


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
 * \brief Returns TRUE iff \c c is not equivalent to Context::TRACK.
 *
 * Equivalent to <code>c != Context::TRACK</code>.
 *
 * \param[in] c Context to evaluate
 *
 * \return TRUE iff \c c is not equivalent to Context::TRACK
 */
inline bool any(const Context& rhs) noexcept
{
	return static_cast<unsigned>(rhs) != 0;
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
	std::pair<int32_t,int32_t> range(const AudioSize& size,
			const Points& points) const
	{
		return this->do_range(size, points);
	}

	/**
	 * \brief Return the result of the algorithm.
	 *
	 * \return Calculation result
	 */
	ChecksumSet result() const
	{
		return this->do_result();
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

	virtual std::pair<int32_t,int32_t> do_range(const AudioSize& size,
			const Points& points) const
	= 0;

	virtual ChecksumSet do_result() const
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
 * finished manually by calling finish_track(). Algorithm instances hold the
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
	 * \param[in] Args Arguments for this Algorithm
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
	 * \brief Update the instance.
	 *
	 * \tparam B Type of iterator pointing to the begin of the update sequence
	 * \tparam E Type of iterator pointing to the end   of the update sequence
	 *
	 * \param[in] start Iterator pointing to the begin of the update sequence
	 * \param[in] stop  Iterator pointing to the end   of the update sequence
	 */
	template <typename B, typename E>
	void update(B start, E stop)
	{
		// An Algorithm must implement template<> perform_update() to be
		// coverable as an Updateable.
		algorithm_->perform_update(start, stop);
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
	 * \brief Mark current track as finished.
	 *
	 * What the instance has to do whenever a track is finished can be
	 * implemented in this hook.
	 *
	 * \param[in] trackno Track number
	 * \param[in] length  Track length as calculated
	 */
	void finish_track(const int trackno, const AudioSize& length)
	{
		return algorithm_->perform_finish_track(trackno, length);
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

	ChecksumSet result() const
	{
		return algorithm_->result();
	}
};

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

