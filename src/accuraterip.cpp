/**
 * \file
 *
 * \brief Implementation of the AccurateRip checksum algorithm.
 */

#ifndef __LIBARCSTK_ACCURATERIP_HPP__
#include "accuraterip.hpp"
#endif

#include <cstdint>     // for uint_fast32_t, uint_fast64_t, int32_t
#include <vector>      // for string

namespace arcstk
{
inline namespace v_1_0_0
{
namespace accuraterip
{
namespace details
{


// UpdatableAPI


ChecksumSet UpdatableAPI::value() const
{
	return do_value();
}


std::string UpdatableAPI::id_string() const
{
	return do_id_string();
}


// UpdatableBase


template <enum checksum::type T1, enum checksum::type... T2>
UpdatableBase<T1, T2...>::~UpdatableBase() noexcept = default;


template <enum checksum::type T1, enum checksum::type... T2>
UpdatableBase<T1, T2...>::UpdatableBase()
	: st_ { /* default */ }
{
	// empty
}


template <enum checksum::type T1, enum checksum::type... T2>
void UpdatableBase<T1, T2...>::reset()
{
	st_.update      = 0;
	st_.subtotal_v1 = 0;
	st_.subtotal_v2 = 0;
}


template <enum checksum::type T1, enum checksum::type... T2>
uint_fast64_t UpdatableBase<T1, T2...>::multiplier() const
{
	return st_.multiplier;
}


template <enum checksum::type T1, enum checksum::type... T2>
void UpdatableBase<T1, T2...>::set_multiplier(const uint_fast64_t m)
{
	st_.multiplier = m;
}


template <enum checksum::type T1, enum checksum::type... T2>
std::unordered_set<checksum::type> UpdatableBase<T1, T2...>::types() const
{
	return { T1, T2... };
}


// Updatable <ARCS1>


ChecksumSet Updatable<checksum::type::ARCS1>::do_value() const
{
	return { 0, {{ checksum::type::ARCS1, st_.subtotal_v1 }} };
}


std::string Updatable<checksum::type::ARCS1>::do_id_string() const
{
	return "v1";
}


// Updatable <ARCS2>


ChecksumSet Updatable<checksum::type::ARCS2>::do_value() const
{
	return { 0, {{ checksum::type::ARCS2, st_.subtotal_v2 }} };
}


std::string Updatable<checksum::type::ARCS2>::do_id_string() const
{
	return "v2";
}


// Updatable <ARCS1, ARCS2>


ChecksumSet Updatable<checksum::type::ARCS1, checksum::type::ARCS2>::do_value()
	const
{
	return { 0, {
		{ checksum::type::ARCS1, st_.subtotal_v1 },
		{ checksum::type::ARCS2, st_.subtotal_v1 + st_.subtotal_v2 },
	} };
}


std::string Updatable<checksum::type::ARCS1, checksum::type::ARCS2>::do_id_string()
	const
{
	return "v1+2";
}


// ARCSAlgorithm


template <enum checksum::type T1, enum checksum::type... T2>
ARCSAlgorithm<T1, T2...>::ARCSAlgorithm()
	: state_ { /* default */ }
{
	// empty
	ARCS_LOG_DEBUG << "Algorithm is AccurateRip " << state_.id_string();
}


template <enum checksum::type T1, enum checksum::type... T2>
uint_fast64_t ARCSAlgorithm<T1, T2...>::multiplier() const
{
	return state_.multiplier();
}


template <enum checksum::type T1, enum checksum::type... T2>
void ARCSAlgorithm<T1, T2...>::set_multiplier(const uint_fast64_t m)
{
	state_.set_multiplier(m);
}


template <enum checksum::type T1, enum checksum::type... T2>
void ARCSAlgorithm<T1, T2...>::do_setup(const Settings* s)
{
	if (any(Context::FIRST_TRACK | s->context()))
	{
		this->set_multiplier(NUM_SKIP_SAMPLES_FRONT + 1);

		ARCS_LOG_DEBUG << "  Initialize multiplier: " << this->multiplier();
	}
}


template <enum checksum::type T1, enum checksum::type... T2>
std::pair<int32_t, int32_t> ARCSAlgorithm<T1, T2...>::do_range(
		const AudioSize& size,
		const std::vector<int32_t>& points) const
{
	int32_t from = 0;
	int32_t to   = size.total_samples() - 1;

	if (!points.empty())
	{
		from += points[0]; // start on first offset
	}

	ARCS_LOG_DEBUG << "  Start on sample offset " << from;

	if (any(Context::FIRST_TRACK | this->settings()->context()))
	{
		from += NUM_SKIP_SAMPLES_FRONT;

		ARCS_LOG_DEBUG << "  Then skip " << NUM_SKIP_SAMPLES_FRONT
			<< " samples";
	}

	if (any(Context::LAST_TRACK  | this->settings()->context()))
	{
		to -= NUM_SKIP_SAMPLES_BACK;

		ARCS_LOG_DEBUG << "  Skip last " << NUM_SKIP_SAMPLES_BACK
			<< " samples";
	}

	ARCS_LOG_DEBUG << "  Interval: " << from << " - " << to;

	return { from, to };
}


template <enum checksum::type T1, enum checksum::type... T2>
void ARCSAlgorithm<T1, T2...>::do_update(SampleInputIterator start,
		SampleInputIterator stop)
{
	ARCS_LOG_DEBUG << "    First multiplier: " << multiplier();

	state_.update(start, stop);

	ARCS_LOG_DEBUG << "    Last multiplier:  " << multiplier() - 1;
}


template <enum checksum::type T1, enum checksum::type... T2>
void ARCSAlgorithm<T1, T2...>::do_track_finished()
{
	state_.reset();
	set_multiplier(1);
}


template <enum checksum::type T1, enum checksum::type... T2>
ChecksumSet ARCSAlgorithm<T1, T2...>::do_result() const
{
	return state_.value();
}


template <enum checksum::type T1, enum checksum::type... T2>
std::unordered_set<checksum::type> ARCSAlgorithm<T1, T2...>::do_types() const
{
	return state_.types();
}


template <enum checksum::type T1, enum checksum::type... T2>
std::unique_ptr<Algorithm> ARCSAlgorithm<T1, T2...>::do_clone() const
{
	return std::make_unique<ARCSAlgorithm>(*this);
}


// Explicit instantiations


template
class UpdatableBase<checksum::type::ARCS1>;

template
class UpdatableBase<checksum::type::ARCS2>;

template
class UpdatableBase<checksum::type::ARCS1, checksum::type::ARCS2>;


template
class Updatable<checksum::type::ARCS1>;

template
class Updatable<checksum::type::ARCS2>;

template
class Updatable<checksum::type::ARCS1, checksum::type::ARCS2>;


template
class ARCSAlgorithm<checksum::type::ARCS1>;

template
class ARCSAlgorithm<checksum::type::ARCS2>;

template
class ARCSAlgorithm<checksum::type::ARCS1, checksum::type::ARCS2>;

} // namespace details

} // namespace accuraterip
} // namespace v_1_0_0
} // namespace arcstk

