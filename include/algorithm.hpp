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

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
// avoid includes
class AudioSize;

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
void swap(Context& lhs, Context& rhs) noexcept;

/**
 * \brief Name of the specified Context.
 *
 * \param[in] c Context to provide name of
 *
 * \return Name of context \c
 */
std::string name(const Context& c) noexcept;

/**
 * \brief String representation of a Context.
 *
 * This will return the name of the context. It is equivalent to name().
 *
 * \param[in] c Context to transform to a string
 *
 * \return String representation of context \c
 */
std::string to_string(const Context& c) noexcept;

/**
 * \brief Returns TRUE iff \c c is not equivalent to Context::TRACK.
 *
 * Equivalent to <code>c != Context::TRACK</code>.
 *
 * \param[in] c Context to evaluate
 *
 * \return TRUE iff \c c is not equivalent to Context::TRACK
 */
bool any(const Context& c) noexcept;


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
	 * \brief Constructor.
	 *
	 * \param[in] c Context for this instance
	 */
	explicit Algorithm(const Context c);

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
	Context context() const noexcept;

	/**
	 * \brief Setup the algorithm with a Context.
	 *
	 * \param[in] c Context for this instance
	 */
	void set_context(const Context c) noexcept;

	/**
	 * \brief Name of this algorithm.
	 *
	 * \return Name of algorithm
	 */
	std::string name() const;

	/**
	 * \brief Types of checksums the algorithm calculates.
	 *
	 * \return Checksum types calculated by this algorithm
	 */
	ChecksumtypeSet types() const;

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
			const Points& points) const;

	/**
	 * \brief Mark current track as finished.
	 *
	 * What the instance has to do whenever a track is finished can be
	 * implemented in this hook.
	 *
	 * \param[in] trackno Track number
	 * \param[in] length  Track length as calculated
	 */
	void track_finished(const int trackno, const AudioSize& length);

	/**
	 * \brief Return the result of the algorithm.
	 *
	 * \return Calculation result
	 */
	ChecksumSet result() const;

	/**
	 * \copydoc SNPT_mf_clone
	 */
	std::unique_ptr<Algorithm> clone() const;

protected:

	/**
	 * \brief Implementation of swap for the base class.
	 *
	 * This is to be called by swap() implementations for subclasses.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap_base(Algorithm& rhs);

private:

	virtual void do_setup(const Context c)
	= 0;

	virtual std::pair<int32_t,int32_t> do_range(const AudioSize& size,
			const Points& points) const
	= 0;

	virtual void do_track_finished(const int t, const AudioSize& length)
	= 0;

	virtual ChecksumSet do_result() const
	= 0;

	virtual ChecksumtypeSet do_types() const
	= 0;

	virtual std::unique_ptr<Algorithm> do_clone() const
	= 0;

	virtual std::string do_name() const
	= 0;
};


/**
 * \brief CRTP to add updateing capability to a concrete Algorithm.
 *
 * \tparam A Algorithm type
 *
 * An Updateable is a base class of an Algorithm that can be updated with new
 * input by the caller.
 *
 * The caller is required to instantiate and setup an Algorithm. However, it
 * should usually not be required to update the Algorithm instance directly.
 * This is performed via a Calculation.
 *
 * The calculation of a track is to be finished manually by calling
 * track_finished(). Algorithm instances hold the concrete subtotals.
 */
template <typename A>
class Updateable : public Algorithm
{
public:

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~Updateable() override = default;

	/**
	 * \brief Typedef to \c A.
	 */
	using algorithm_type = A;

	/**
	 * \brief Get a pointer to this instance typed by its concrete type.
	 *
	 * \return Pointer of type A* to this instance
	 */
	algorithm_type* as_algorithm_type()
	{
		return static_cast<algorithm_type*>(this);
	}

	/**
	 * \brief Return the name of the wrapped algorithm.
	 *
	 * \return Name of the algorithm wrapped by this instance
	 */
	std::string algorithm_name()
	{
		return as_algorithm_type()->name();
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
		as_algorithm_type()->perform_update(start, stop);
	}

protected:

	// NOLINTNEXTLINE(bugprone-crtp-constructor-accessibility)
	Updateable() = default;
};

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

