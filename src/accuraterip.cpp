/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of the AccurateRip checksum algorithm.
 */

#ifndef __LIBARCSTK_ALGORITHMS_HPP__
#include "algorithms.hpp"
#endif
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


// Update


ChecksumSet Update<cstype::ARCS1>::value(const Subtotals& st) const
{
	return { 0, {{ cstype::ARCS1, st.subtotal_v1 }} };
}


std::string Update<cstype::ARCS1>::id_string() const
{
	return "v1";
}


ChecksumSet Update<cstype::ARCS2>::value(const Subtotals& st) const
{
	return { 0, {{ cstype::ARCS2, st.subtotal_v2 }} };
}


std::string Update<cstype::ARCS2>::id_string() const
{
	return "v2";
}


ChecksumSet Update<cstype::ARCS1, cstype::ARCS2>::value(
		const Subtotals& st) const
{
	return { 0, {
		{ cstype::ARCS1, st.subtotal_v1 },
		{ cstype::ARCS2, st.subtotal_v1 + st.subtotal_v2 },
	} };
}


std::string Update<cstype::ARCS1, cstype::ARCS2>::id_string()
	const
{
	return "v1+2";
}


// AccurateRipCS


template <cstype T1, cstype... T2>
uint_fast64_t AccurateRipCS<T1, T2...>::multiplier() const
{
	return st_.multiplier;
}


template <cstype T1, cstype... T2>
void AccurateRipCS<T1, T2...>::set_multiplier(const uint_fast64_t m)
{
	st_.multiplier = m;
}


template <cstype T1, cstype... T2>
void AccurateRipCS<T1, T2...>::update(const SampleInputIterator& start,
			const SampleInputIterator& stop)
{
	update_(start, stop, st_);
}


template <cstype T1, cstype... T2>
ChecksumSet AccurateRipCS<T1, T2...>::value() const
{
	return update_.value(st_);
}


template <cstype T1, cstype... T2>
void AccurateRipCS<T1, T2...>::reset()
{
	st_.update      = 0;
	st_.subtotal_v1 = 0;
	st_.subtotal_v2 = 0;
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
void ARCSAlgorithm<T1, T2...>::do_setup(const Settings* s)
{
	ARCS_LOG(DEBUG1) << "Context for Algorithm: " << to_string(s->context());

	if (any(Context::FIRST_TRACK & s->context()))
	{
		state_.set_multiplier(NUM_SKIP_SAMPLES::FRONT + 1);
	}

	ARCS_LOG(DEBUG1) << "Initialize multiplier to: " << state_.multiplier();
}


template <cstype T1, cstype... T2>
void ARCSAlgorithm<T1, T2...>::do_update(SampleInputIterator start,
		SampleInputIterator stop)
{
	ARCS_LOG(DEBUG3) << "First multiplier: " << state_.multiplier();

	state_.update(start, stop);

	ARCS_LOG(DEBUG3) << "Last multiplier:  " << state_.multiplier() - 1;
	// -1 because multiplier_ has already been updated to next input
}


template <cstype T1, cstype... T2>
void ARCSAlgorithm<T1, T2...>::do_track_finished(const int /*t*/,
		const AudioSize& s)
{
	current_result_ = state_.value();
	current_result_.set_length(s.frames());

	state_.reset();
	state_.set_multiplier(1);
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
std::pair<int32_t, int32_t> ARCSAlgorithm<T1, T2...>::do_range(
		const AudioSize& size, const Points& points) const
{
	const auto ctx = this->settings()->context();

	ARCS_LOG(DEBUG2) << "Get legal range for context " << to_string(ctx);

	auto from = int32_t { 0 };
	auto to   = int32_t { size.samples() - 1 };

	if (!points.empty())
	{
		from += points[0].samples(); // start on first offset

		ARCS_LOG(DEBUG2) << "Skip first " << from << " samples due to offset";
	}

	if (any(Context::FIRST_TRACK & ctx))
	{
		from += NUM_SKIP_SAMPLES::FRONT;

		ARCS_LOG(DEBUG2) << "Skip " << NUM_SKIP_SAMPLES::FRONT
			<< " samples after beginning";
	}

	if (any(Context::LAST_TRACK & ctx))
	{
		to -= NUM_SKIP_SAMPLES::BACK;

		ARCS_LOG(DEBUG2) << "Skip last " << NUM_SKIP_SAMPLES::BACK
			<< " samples";
	}

	ARCS_LOG(DEBUG2) << "Legal range is: " << from << " - " << to;

	return { from, to };
}


template <cstype T1, cstype... T2>
std::unique_ptr<Algorithm> ARCSAlgorithm<T1, T2...>::do_clone() const
{
	return std::make_unique<ARCSAlgorithm>(*this);
}


// Explicit instantiations


template class AccurateRipCS<cstype::ARCS1>;

template class AccurateRipCS<cstype::ARCS2>;

template class AccurateRipCS<cstype::ARCS1, cstype::ARCS2>;


template class ARCSAlgorithm<cstype::ARCS1>;

template class ARCSAlgorithm<cstype::ARCS2>;

template class ARCSAlgorithm<cstype::ARCS1, cstype::ARCS2>;

} // namespace details

} // namespace accuraterip
} // namespace v_1_0_0
} // namespace arcstk

