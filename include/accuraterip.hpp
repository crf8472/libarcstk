#ifndef __LIBARCSTK_ACCURATERIP_HPP__
#define __LIBARCSTK_ACCURATERIP_HPP__
/**
 * \file
 *
 * \brief AccurateRip Checksum specific implementations.
 */

#include <cstdint>  // for uint_fast32_t, uint_fast64_t, int32_t

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"                // for Identifier
#endif
#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"                  // for checksum::type, ChecksumSet
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"                 // for Algorithm
#endif


namespace arcstk
{
inline namespace v_1_0_0
{
/** \addtogroup calc */
/** @{ */

/**
 * \brief Calculating AccurateRip checksums.
 */
namespace accuraterip
{

/**
 * \internal
 * Implementation details of libarcstk API v1.0.0.
 */
namespace details
{

// Id calculation

// TODO

// Checksum calculation

/**
 * \internal
 * \brief Number of samples to be skipped before the end of the last track.
 *
 * There are 5 frames to be skipped, i.e. 5 frames * 588 samples/frame
 * = 2940 samples. We derive the number of samples to be skipped at the
 * start of the first track by just subtracting 1 from this constant.
 */
constexpr int32_t NUM_SKIP_SAMPLES_BACK  = 5/*frames*/ * 588/*samples/frame*/;

/**
 * \internal
 * \brief Number of samples to be skipped after the start of the first track.
 *
 * There are 5 frames - 1 sample to be skipped, i.e.
 * 5 frames * 588 samples/frame - 1 sample = 2939 samples.
 */
constexpr int32_t NUM_SKIP_SAMPLES_FRONT = NUM_SKIP_SAMPLES_BACK - 1;


/**
 * \brief Interface for updating a calculation state holding subtotals.
 */
template<enum checksum::type T1, enum checksum::type... T2>
class UpdatableBase
{
	/**
	 * \brief Values of a calculation state.
	 */
	struct Subtotals
	{
		uint_fast64_t multiplier  = 1;  // multiplier: sample index
		uint_fast64_t update      = 0;  // update factor
		uint_fast32_t subtotal_v1 = 0;  // subtotal for ARCSv1
		uint_fast32_t subtotal_v2 = 0;  // subtotal for ARCSv2

		friend void swap(Subtotals& lhs, Subtotals& rhs) noexcept
		{
			using std::swap;
			swap(lhs.multiplier,  rhs.multiplier);
			swap(lhs.update,      rhs.update);
			swap(lhs.subtotal_v1, rhs.subtotal_v1);
			swap(lhs.subtotal_v2, rhs.subtotal_v2);
		}
	};

protected:

	/**
	 * \brief Protected virtual default destructor.
	 */
	~UpdatableBase() noexcept;

	/**
	 * \brief Internal subtotals.
	 */
	Subtotals st_;

	/**
	 * \brief Helper for masking the lower 32 bits of a sample.
	 */
	static constexpr uint_fast32_t LOWER_32_BITS_ { 0xFFFFFFFF };

public:

	/**
	 * \brief Default constructor.
	 */
	UpdatableBase();

	/**
	 * \brief Reset the instance to its initial state.
	 */
	void reset();

	/**
	 * \brief Current Multiplier of this instance.
	 *
	 * \return Current multiplier
	 */
	uint_fast64_t multiplier() const;

	/**
	 * \brief Set multiplier to a new value.
	 *
	 * \param[in] m New value for multiplier
	 */
	void set_multiplier(const uint_fast64_t m);

	/**
	 * \brief Return the checksum types this instance calculates.
	 *
	 * \return Set of types calculated by this instance
	 */
	std::unordered_set<checksum::type> types() const;
};


/**
 * \brief Abstract base class for public member functions for Updatables.
 */
class UpdatableAPI
{
	virtual ChecksumSet do_value() const
	= 0;

	virtual std::string do_id_string() const
	= 0;

public:

	/**
	 * \brief Virtual public destructor.
	 */
	virtual ~UpdatableAPI() noexcept = default;

	/**
	 * \brief Get the current updated value from the Updatable.
	 */
	ChecksumSet value() const;

	/**
	 * \brief Get the ID string from the Updatable.
	 */
	std::string id_string() const;
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


// full specialization for ARCS1
template <>
class Updatable<checksum::type::ARCS1> final
								: public UpdatableAPI
								, public UpdatableBase<checksum::type::ARCS1>
{
public:

	template <class B, class E>
	void update(const B& start, const E& stop)
	{
		for (auto pos = start; pos != stop; ++pos, ++st_.multiplier)
		{
			st_.subtotal_v1 += st_.multiplier * (*pos) & LOWER_32_BITS_;
		}
	}

	// UpdatableAPI
	ChecksumSet do_value()     const final;
	std::string do_id_string() const final;
};


// full specialization for ARCS2
template <>
class Updatable<checksum::type::ARCS2> final
								: public UpdatableAPI
								, public UpdatableBase<checksum::type::ARCS2>
{
public:

	template<class B, class E>
	void update(const B& start, const E& stop)
	{
		for (auto pos = start; pos != stop; ++pos, ++st_.multiplier)
		{
			st_.update = st_.multiplier * (*pos);

			st_.subtotal_v2 +=
				(st_.update & LOWER_32_BITS_) + (st_.update >> 32u);
		}
	}

	// UpdatableAPI
	ChecksumSet do_value()     const final;
	std::string do_id_string() const final;
};


// full specialization for ARCS1+ARCS2
template <>
class Updatable<checksum::type::ARCS1,checksum::type::ARCS2> final
			: public UpdatableAPI
			, public UpdatableBase<checksum::type::ARCS1,checksum::type::ARCS2>
{ // TODO Order of args in parameter pack??
public:

	template<class B, class E>
	void update(const B& start, const E& stop)
	{
		for (auto pos = start; pos != stop; ++pos, ++st_.multiplier)
		{
			st_.update       = st_.multiplier * (*pos);
			st_.subtotal_v1 += st_.update & LOWER_32_BITS_;
			st_.subtotal_v2 += (st_.update >> 32u);
		}
	}

	// UpdatableAPI
	ChecksumSet do_value()     const final;
	std::string do_id_string() const final;
};


/**
 * \brief Implement AccurateRip algorithm variants.
 */
template<enum checksum::type T1, enum checksum::type... T2>
class ARCSAlgorithm final : public Algorithm
{
	/**
	 * \brief Internal updatable state.
	 */
	Updatable<T1, T2...> state_;

	/**
	 * \brief Current result of performing the algorithm.
	 */
	ChecksumSet current_result_;


	/**
	 * \brief Set multiplier to a new value.
	 *
	 * \param[in] m New value for multiplier
	 */
	void set_multiplier(const uint_fast64_t m);

	/**
	 * \brief Save the current subtotal as result.
	 */
	void save_current_subtotal();


	// Algorithm

	void do_setup(const Settings* s) final;

	std::pair<int32_t, int32_t> do_range(const AudioSize& size,
			const Points& points) const final;

	void do_update(SampleInputIterator start, SampleInputIterator stop) final;

	void do_track_finished(const int t, const AudioSize& length) final;

	ChecksumSet do_result() const final;

	std::unordered_set<checksum::type> do_types() const final;

	std::unique_ptr<Algorithm> do_clone() const final;

public:

	/**
	 * \brief Default constructor.
	 */
	ARCSAlgorithm();

	/**
	 * \brief Current multiplier.
	 *
	 * \return Current multiplier
	 */
	uint_fast64_t multiplier() const;
};

} // namespace details


/**
 * \brief AccurateRip v1 algorithm.
 */
using V1 = details::ARCSAlgorithm<checksum::type::ARCS1>;

/**
 * \brief AccurateRip v2 algorithm.
 */
using V2 = details::ARCSAlgorithm<checksum::type::ARCS2>;

/**
 * \brief AccurateRip v2 algorithm implementation that also provides v1.
 */
using V1and2 =
	details::ARCSAlgorithm<checksum::type::ARCS1,checksum::type::ARCS2>;

} // namespace accuraterip

/** @} */ // group calc

} // namespace v_1_0_0
} // namespace arcstk

#endif

