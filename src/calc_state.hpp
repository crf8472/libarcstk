#ifndef __LIBARCSTK_CALC_STATE_HPP__
#define __LIBARCSTK_CALC_STATE_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Internal API for calculation states
 */

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif

#include <cmath>         // for exp2
#include <cstddef>       // for size_t
#include <memory>        // for make_unique
#include <tuple>         // for tuple_element_t, tuple_size
                         // C++17: for tuple_size_v instead of tuple_size
#include <stdexcept>     // for invalid_argument
#include <sstream>       // for stringstream
#include <type_traits>   // for enable_if
#include <utility>       // for make_index_sequence

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

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
	virtual void init_with_skip() noexcept
	= 0;

	/**
	 * \brief Initializes the instance for calculating a new track.
	 *
	 * Initializing calles <tt>wipe()</tt> before doing anything.
	 */
	virtual void init_without_skip() noexcept
	= 0;

	/**
	 * \brief Amount of samples to be skipped at the beginning.
	 *
	 * \return Amount of samples to be skipped at the beginning
	 */
	virtual sample_count_t num_skip_front() const noexcept
	= 0;

	/**
	 * \brief Amount of samples to be skipped at the end.
	 *
	 * \return Amount of samples to be skipped at the end
	 */
	virtual sample_count_t num_skip_back() const noexcept
	= 0;

	/**
	 * \brief Update the calculation state with a sequence of samples.
	 *
	 * \param[in] begin Iterator pointing to the beginning of the sequence
	 * \param[in] end   Iterator pointing to the end of the sequence
	 */
	virtual void update(SampleInputIterator begin, SampleInputIterator end)
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
	virtual void save(const TrackNo track) noexcept
	= 0;

	/**
	 * \brief Returns the number of currently saved tracks.
	 *
	 * \return Number of currently saved tracks
	 */
	virtual int total_tracks() const noexcept
	= 0;

	/**
	 * \brief Returns current type.
	 *
	 * \return A disjunction of all requested types.
	 */
	virtual checksum::type type() const noexcept
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
	virtual ChecksumSet result(const TrackNo track) const noexcept
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
	virtual ChecksumSet result() const noexcept
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
	virtual void reset() noexcept
	= 0;

	/**
	 * \brief Resets the internal subtotals and the multiplier and deletes all
	 * previously saved computation results.
	 */
	virtual void wipe() noexcept
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
	virtual uint32_t mult() const noexcept
	= 0;

	/**
	 * \brief Clone this CalcState object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return A clone of this instance
	 */
	virtual std::unique_ptr<CalcState> clone() const noexcept
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
	void init_with_skip() noexcept override;

	/**
	 * \brief Implements CalcState::init_without_skip()
	 *
	 * Initializes the multiplier with 1 for no samples are skipped.
	 *
	 * Initializing calls <tt>wipe()</tt> before doing anything.
	 */
	void init_without_skip() noexcept override;

	sample_count_t num_skip_front() const noexcept override;

	sample_count_t num_skip_back() const noexcept override;

	void update(SampleInputIterator begin, SampleInputIterator end) final;


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
	virtual void init(const uint32_t mult) noexcept
	= 0;

	/**
	 * \brief Worker: implement update()
	 */
	virtual void do_update(SampleInputIterator &begin, SampleInputIterator &end)
	= 0;

	/**
	 * \brief Actual amount of skipped samples at front
	 */
	sample_count_t actual_skip_front_;

	/**
	 * \brief Actual amount of skipped samples at back
	 */
	sample_count_t actual_skip_back_;
};


// forward declaration for state::types
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
template <typename T, typename F>
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
template <typename T, typename F>
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
 * \internal
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
 * \internal
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
template <typename... X, typename T>
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

