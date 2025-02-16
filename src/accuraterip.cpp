/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of the AccurateRip checksum algorithm.
 */

#ifndef __LIBARCSTK_ACCURATERIP_HPP__
#include "accuraterip.hpp"
#endif

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"              // for type, ChecksumSet
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif
#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"              // for AudioSize
#endif

#include <cstdint>       // for int32_t, uint_fast64_t
#include <memory>        // for make_unique, unique_ptr
#include <utility>       // for pair

namespace arcstk
{
inline namespace v_1_0_0
{
namespace accuraterip
{
namespace details
{

using cstype = checksum::type; // local, for Readability


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


template <cstype T1, cstype... T2>
UpdatableBase<T1, T2...>::~UpdatableBase() noexcept = default;


template <cstype T1, cstype... T2>
UpdatableBase<T1, T2...>::UpdatableBase()
	: st_ { /* default */ }
{
	// empty
}


template <cstype T1, cstype... T2>
void UpdatableBase<T1, T2...>::reset()
{
	st_.update      = 0;
	st_.subtotal_v1 = 0;
	st_.subtotal_v2 = 0;
}


template <cstype T1, cstype... T2>
uint_fast64_t UpdatableBase<T1, T2...>::multiplier() const
{
	return st_.multiplier;
}


template <cstype T1, cstype... T2>
void UpdatableBase<T1, T2...>::set_multiplier(const uint_fast64_t m)
{
	st_.multiplier = m;
}


template <cstype T1, cstype... T2>
std::unordered_set<cstype> UpdatableBase<T1, T2...>::types() const
{
	return { T1, T2... };
}


// Updatable <ARCS1>


ChecksumSet Updatable<cstype::ARCS1>::do_value() const
{
	return { 0, {{ cstype::ARCS1, st_.subtotal_v1 }} };
}


std::string Updatable<cstype::ARCS1>::do_id_string() const
{
	return "v1";
}


// Updatable <ARCS2>


ChecksumSet Updatable<cstype::ARCS2>::do_value() const
{
	return { 0, {{ cstype::ARCS2, st_.subtotal_v2 }} };
}


std::string Updatable<cstype::ARCS2>::do_id_string() const
{
	return "v2";
}


// Updatable <ARCS1, ARCS2>


ChecksumSet Updatable<cstype::ARCS1, cstype::ARCS2>::do_value()
	const
{
	return { 0, {
		{ cstype::ARCS1, st_.subtotal_v1 },
		{ cstype::ARCS2, st_.subtotal_v1 + st_.subtotal_v2 },
	} };
}


std::string Updatable<cstype::ARCS1, cstype::ARCS2>::do_id_string()
	const
{
	return "v1+2";
}


// ARCSAlgorithm


template <cstype T1, cstype... T2>
ARCSAlgorithm<T1, T2...>::ARCSAlgorithm()
	: state_          { /* default */ }
	, current_result_ { /* default */ }
{
	// empty
	ARCS_LOG_DEBUG << "Use algorithm: AccurateRip " << state_.id_string();
}


template <cstype T1, cstype... T2>
uint_fast64_t ARCSAlgorithm<T1, T2...>::multiplier() const
{
	return state_.multiplier();
}


template <cstype T1, cstype... T2>
void ARCSAlgorithm<T1, T2...>::set_multiplier(const uint_fast64_t m)
{
	state_.set_multiplier(m);
}


template <cstype T1, cstype... T2>
void ARCSAlgorithm<T1, T2...>::save_current_subtotal()
{
	current_result_ = state_.value();
}


template <cstype T1, cstype... T2>
void ARCSAlgorithm<T1, T2...>::do_setup(const Settings* s)
{
	ARCS_LOG(DEBUG1) << "Context for Algorithm: " << to_string(s->context());

	if (any(Context::FIRST_TRACK & s->context()))
	{
		this->set_multiplier(NUM_SKIP_SAMPLES_FRONT + 1);
	}

	ARCS_LOG(DEBUG1) << "Initialize multiplier to: " << this->multiplier();
}


template <cstype T1, cstype... T2>
std::pair<int32_t, int32_t> ARCSAlgorithm<T1, T2...>::do_range(
		const AudioSize& size, const Points& points) const
{
	int32_t from = 0;
	int32_t to   = size.samples() - 1;

	if (!points.empty())
	{
		from += points[0].samples(); // start on first offset
	}

	ARCS_LOG(DEBUG2) << "Legal range for context "
		<< to_string(this->settings()->context());

	if (any(Context::FIRST_TRACK & this->settings()->context()))
	{
		from += NUM_SKIP_SAMPLES_FRONT;

		ARCS_LOG(DEBUG2) << "Start on sample offset " << from;
		ARCS_LOG(DEBUG2) << "Then skip " << NUM_SKIP_SAMPLES_FRONT
			<< " samples";
	}

	if (any(Context::LAST_TRACK  & this->settings()->context()))
	{
		to -= NUM_SKIP_SAMPLES_BACK;

		ARCS_LOG(DEBUG2) << "Skip last " << NUM_SKIP_SAMPLES_BACK
			<< " samples";
	}

	ARCS_LOG(DEBUG2) << "Legal range is: " << from << " - " << to;

	return { from, to };
}


template <cstype T1, cstype... T2>
void ARCSAlgorithm<T1, T2...>::do_update(SampleInputIterator start,
		SampleInputIterator stop)
{
	ARCS_LOG(DEBUG3) << "First multiplier: " << multiplier();

	state_.update(start, stop);

	ARCS_LOG(DEBUG3) << "Last multiplier:  " << multiplier() - 1;
	// -1 because multiplier_ has already been updated to next input
}


template <cstype T1, cstype... T2>
void ARCSAlgorithm<T1, T2...>::do_track_finished(const int /*t*/,
		const AudioSize& s)
{
	this->save_current_subtotal();
	current_result_.set_length(s.frames());

	state_.reset();
	set_multiplier(1);
}


template <cstype T1, cstype... T2>
ChecksumSet ARCSAlgorithm<T1, T2...>::do_result() const
{
	return current_result_;
}


template <cstype T1, cstype... T2>
std::unordered_set<cstype> ARCSAlgorithm<T1, T2...>::do_types() const
{
	return state_.types();
}


template <cstype T1, cstype... T2>
std::unique_ptr<Algorithm> ARCSAlgorithm<T1, T2...>::do_clone() const
{
	return std::make_unique<ARCSAlgorithm>(*this);
}


// Explicit instantiations


template class UpdatableBase<cstype::ARCS1>;

template class UpdatableBase<cstype::ARCS2>;

template class UpdatableBase<cstype::ARCS1, cstype::ARCS2>;


//template class Updatable<cstype::ARCS1>;

//template class Updatable<cstype::ARCS2>;

//template class Updatable<cstype::ARCS1, cstype::ARCS2>;


template class ARCSAlgorithm<cstype::ARCS1>;

template class ARCSAlgorithm<cstype::ARCS2>;

template class ARCSAlgorithm<cstype::ARCS1, cstype::ARCS2>;

} // namespace details

} // namespace accuraterip
} // namespace v_1_0_0
} // namespace arcstk

