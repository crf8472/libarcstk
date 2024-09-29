#ifndef __LIBARCSTK_ACCURATERIP_HPP__
#define __LIBARCSTK_ACCURATERIP_HPP__
/**
 * \file
 *
 * \brief AccurateRip Checksum specific implementations.
 */

#include <cstdint>  // for uint_fast32_t, uint_fast64_t, int32_t
#include <set>      // for set

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"                  // for Checksum, ChecksumSet, Checksums
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif

#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif


namespace arcstk
{
inline namespace v_1_0_0
{
namespace accuraterip
{

/**
 * \internal
 * \brief Number of samples to be skipped before the end of the last track.
 *
 * There are 5 frames to be skipped, i.e. 5 frames * 588 samples/frame
 * = 2940 samples. We derive the number of samples to be skipped at the
 * start of the first track by just subtracting 1 from this constant.
 */
constexpr int32_t NUM_SKIP_SAMPLES_BACK  = 5/*frames*/ * 588/*samples*/;

/**
 * \internal
 * \brief Number of samples to be skipped after the start of the first track.
 *
 * There are 5 frames - 1 sample to be skipped, i.e.
 * 5 frames * 588 samples/frame - 1 sample = 2939 samples.
 */
constexpr int32_t NUM_SKIP_SAMPLES_FRONT = NUM_SKIP_SAMPLES_BACK - 1;

//} // namespace


/**
 * \brief Interface for updating a calculation state holding subtotals.
 */
template<enum checksum::type T1, enum checksum::type... T2>
class UpdatableBase
{
protected:

	/**
	 * \internal
	 * \brief Values of a calculation state.
	 */
	struct Subtotals
	{
		uint_fast64_t multiplier  = 1;  // multiplier: sample index
		uint_fast64_t update      = 0;  // update factor
		uint_fast32_t subtotal_v1 = 0;  // subtotal for ARCSv1
		uint_fast32_t subtotal_v2 = 0;  // subtotal for ARCSv2

		/**
		 * \internal
		 * \brief Reset all subtotals to their initial state.
		 */
		void reset()
		{
			Subtotals s;
			swap(*this, s);
		}

		friend void swap(Subtotals& lhs, Subtotals& rhs) noexcept
		{
			using std::swap;
			swap(lhs.multiplier,  rhs.multiplier);
			swap(lhs.update,      rhs.update);
			swap(lhs.subtotal_v1, rhs.subtotal_v1);
			swap(lhs.subtotal_v2, rhs.subtotal_v2);
		}
	};

	/**
	 * \internal
	 * \brief Internal subtotals.
	 */
	Subtotals state_;

	/**
	 * \internal
	 * \brief Helper for masking the lower 32 bits of a sample.
	 */
	static constexpr uint_fast32_t LOWER_32_BITS_ { 0xFFFFFFFF };

public:

	/**
	 * \brief Reset the instance to its initial state.
	 */
	void reset()
	{
		state_.reset();
	}

	/**
	 * \brief Return the checksum types this instance calculates.
	 */
	std::vector<checksum::type> types() const
	{
		return { T1, T2... };
	}
};


/**
 * \brief Updatable state of subtotals.
 *
 * Default implementation is empty.
 */
template<enum checksum::type T1, enum checksum::type... T2>
class Updatable final : public UpdatableBase<T1, T2...>
{
	// empty
};


template<>
class Updatable<checksum::type::ARCS1> final
								: public UpdatableBase<checksum::type::ARCS1>
{
public:

	template<class B, class E>
	void update(const B& start, const E& stop)
	{
		for (auto pos = start; pos != stop; ++pos, ++state_.multiplier)
		{
			state_.subtotal_v1 += state_.multiplier * (*pos) & LOWER_32_BITS_;
		}
	}

	ChecksumSet value() const
	{
		return { 0, {{ checksum::type::ARCS1, state_.subtotal_v1 }} };
	}
};


template<>
class Updatable<checksum::type::ARCS2> final
								: public UpdatableBase<checksum::type::ARCS2>
{
public:

	template<class B, class E>
	void update(const B& start, const E& stop)
	{
		for (auto pos = start; pos != stop; ++pos, ++state_.multiplier)
		{
			state_.update = state_.multiplier * (*pos);
			state_.subtotal_v2 +=
				(state_.update & LOWER_32_BITS_) + (state_.update >> 32u);
		}
	}

	ChecksumSet value() const
	{
		return { 0, {{ checksum::type::ARCS2, state_.subtotal_v2 }} };
	}
};


template<>
class Updatable<checksum::type::ARCS1,checksum::type::ARCS2> final
			: public UpdatableBase<checksum::type::ARCS1,checksum::type::ARCS2>
{ // TODO Order of args in parameter pack??
public:

	template<class B, class E>
	void update(const B& start, const E& stop)
	{
		for (auto pos = start; pos != stop; ++pos, ++state_.multiplier)
		{
			state_.update       = state_.multiplier * (*pos);
			state_.subtotal_v1 += state_.update & LOWER_32_BITS_;
			state_.subtotal_v2 += state_.update >> 32u;
		}
	}

	ChecksumSet value() const
	{
		return { 0, {
			{ checksum::type::ARCS1, state_.subtotal_v1 },
			{ checksum::type::ARCS2, state_.subtotal_v1 + state_.subtotal_v2 },
		} };
	}
};


/**
 * \brief Implement AccurateRip algorithm variants.
 */
template<enum checksum::type T1, enum checksum::type... T2>
class AccurateRipAlgorithm final : public Algorithm
{
	/**
	 * \brief Internal updatable state.
	 */
	Updatable<T1, T2...> internal_state_;


	void do_update(SampleInputIterator begin, SampleInputIterator end) final
	{
		return internal_state_.update(begin, end);
	}

	ChecksumSet do_result() const final
	{
		return internal_state_.value();
	}

	std::vector<checksum::type> do_types() const final
	{
		return internal_state_.types();
	}

protected:

	// TODO set requested length and provide ChecksumSet with length
};


// Use concrete AccurateRip algorithms

using AccurateRipV1   = AccurateRipAlgorithm<checksum::type::ARCS1>;
using AccurateRipV2   = AccurateRipAlgorithm<checksum::type::ARCS2>;
using AccurateRipV1V2 =
			AccurateRipAlgorithm<checksum::type::ARCS1,checksum::type::ARCS2>;

} // namespace accuraterip
} // namespace v_1_0_0
} // namespace arcstk

#endif

