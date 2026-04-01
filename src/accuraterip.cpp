/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of the AccurateRip checksum algorithm.
 */

#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"
#endif
#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"
#endif

#include <cstdint>       // for int32_t, uint_fast64_t
#include <memory>        // for make_unique, unique_ptr
#include <utility>       // for pair

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"              // for type, ChecksumSet
#endif
#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"              // for AudioSize
#endif

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
AccurateRipCS<T1, T2...>::AccurateRipCS()
	: st_     { /*default*/ }
	, update_ { /*default*/ }
{
	// empty
}


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


template <cstype T1, cstype... T2>
void AccurateRipCS<T1, T2...>::swap(AccurateRipCS& rhs) noexcept
{
	using std::swap;

	swap(this->st_,     rhs.st_);
	swap(this->update_, rhs.update_);
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


template <cstype T1, cstype... T2>
void ARCSAlgorithm<T1, T2...>::swap(ARCSAlgorithm& rhs) noexcept
{
	using std::swap;

	swap(this->state_,          rhs.state_);
	swap(this->current_result_, rhs.current_result_);
}


// Explicit instantiations


template class AccurateRipCS<cstype::ARCS1>;

template class AccurateRipCS<cstype::ARCS2>;

template class AccurateRipCS<cstype::ARCS1, cstype::ARCS2>;


template class ARCSAlgorithm<cstype::ARCS1>;

template class ARCSAlgorithm<cstype::ARCS2>;

template class ARCSAlgorithm<cstype::ARCS1, cstype::ARCS2>;

} // namespace details


namespace details
{

uint64_t sum_digits(const uint32_t number) noexcept
{
	return (number < 10) ? number : (number % 10) + sum_digits(number / 10);
}

} // namespace details

namespace id
{


uint32_t disc_id_1(const std::vector<int32_t>& offsets, const int32_t leadout)
	noexcept
{
	// disc id 1 is just the sum off all offsets + the leadout frame

	auto accum = int32_t { 0 };

	for (const auto& o : offsets)
	{
		accum += o;
	}

	return static_cast<uint32_t>(accum + leadout);
}


uint32_t disc_id_2(const std::vector<int32_t>& offsets, const int32_t leadout)
	noexcept
{
	// disc id 2 is the sum of the products of offsets and the corresponding
	// 1-based track number while normalizing offsets to be >= 1

	auto accum = int32_t { 0 };
	auto track { 1 };

	for (const auto& o : offsets)
	{
		accum += (o > 0 ? o : 1) * track;
		++track;
	}

	return static_cast<uint32_t>(accum + leadout * track);
}


uint32_t cddb_id(const std::vector<int32_t>& offsets, const int32_t leadout)
{
	const auto fps { static_cast<uint32_t>(CDDA::FRAMES_PER_SEC) };


	const auto start_audio = uint32_t { offsets.empty()
		? 0
		: static_cast<uint32_t>(offsets.front()) };

	const auto total_seconds = uint32_t {
		static_cast<uint32_t>(leadout) / fps  -  start_audio / fps };
			//static_cast<uint32_t>(offsets[0]) / fps };

	auto accum = uint32_t { 0 };
	for (const auto& o : offsets)
	{
		accum += details::sum_digits(static_cast<uint32_t>(o) / fps + 2u);
	}
	accum %= 0xFF; // normalize to 1 byte


	// since 0 <= offsets.size <= 99 narrowing is no problem
	const auto total_tracks = static_cast<uint32_t>(offsets.size());


	return (accum << 24u) | (total_seconds << 8u) | total_tracks;
}


std::string construct_filename(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	return "dBAR-" + construct_id(total_tracks, id_1, id_2, cddb_id) + ".bin";
}


std::string construct_url(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	return construct_url(total_tracks, id_1, id_2, cddb_id,
			ACCURATERIP::request_url_prefix());
}


std::string construct_url(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id,
		const std::string& prefix) noexcept
{
	auto ss = std::ostringstream {};

	auto hex_flags = std::ios_base::fmtflags { ss.flags() };
	hex_flags &= ~ss.adjustfield; // unset 'left' or 'internal'
	hex_flags |= ss.right;        // set 'right' only
	hex_flags &= ~ss.basefield;   // unset 'dec' and 'oct'
	hex_flags |= ss.hex;          // set 'hex' only
	hex_flags &= ~ss.uppercase;   // unset 'uppercase'
	hex_flags &= ~ss.showbase;    // unset 'showbase'

	ss.flags(hex_flags);

	ss  << prefix /* MUST end with '/' */
		<< std::setw(1) << (id_1       & 0xFu) << '/'
		<< std::setw(1) << (id_1 >> 4u & 0xFu) << '/'
		<< std::setw(1) << (id_1 >> 8u & 0xFu) << '/';

	return ss.str() + construct_filename(total_tracks, id_1, id_2, cddb_id);
}


std::string construct_id(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	auto out = std::ostringstream {};
	print(out, total_tracks, id_1, id_2, cddb_id);
	return out.str();
}


void print_impl(std::ostream& out, const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id, const std::string& delim)
{
	static const auto fmt_flags =
		[](const std::ostream& stream) -> std::ios_base::fmtflags
		{
			auto flags = std::ios_base::fmtflags { stream.flags() };

			flags &= ~stream.adjustfield; // unset 'left' or 'internal'
			flags |= stream.right;        // set 'right' only
			flags &= ~stream.basefield;   // unset 'dec', 'hex', and 'oct'
			flags &= ~stream.uppercase;   // unset 'uppercase'
			flags &= ~stream.showbase;    // unset 'showbase'

			return flags;
		};

	const auto prev_flags = std::ios_base::fmtflags { out.flags() };

	out.flags(fmt_flags(out));

	out << std::dec
		<< std::setw(3) << std::setfill('0') << total_tracks
		<< std::hex
		<< delim << std::setw(8) << std::setfill('0') << id_1
		<< delim << std::setw(8) << std::setfill('0') << id_2
		<< delim << std::setw(8) << std::setfill('0') << cddb_id;

	out.flags(prev_flags);
}


void print(std::ostream& out, const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id)
{
	print_impl(out, total_tracks, id_1, id_2, cddb_id, "-");
}

} // namespace id

} // namespace accuraterip


// ACCURATERIP


std::string ACCURATERIP::request_url_prefix_ =
		ACCURATERIP::default_request_url_prefix();


std::string ACCURATERIP::request_url_prefix() noexcept
{
	return ACCURATERIP::request_url_prefix_;
}


std::string ACCURATERIP::default_request_url_prefix() noexcept
{
	/* this defines the default URL prefix for AccurateRip requests */
	return "http://www.accuraterip.com/accuraterip/";
}


void ACCURATERIP::set_request_url_prefix(const std::string& prefix) noexcept
{
	ACCURATERIP::request_url_prefix_ = prefix;
}


void ACCURATERIP::reset_request_url_prefix() noexcept
{
	ACCURATERIP::set_request_url_prefix(
			ACCURATERIP::default_request_url_prefix());
}


std::string ACCURATERIP::default_arcs_format(const uint32_t number)
{
	return Checksum { number }.to_string();
}

} // namespace v_1_0_0
} // namespace arcstk

