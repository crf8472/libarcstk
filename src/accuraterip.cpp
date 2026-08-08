/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of the AccurateRip checksum algorithm.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"
#endif
#ifndef LIBARCSTK_ACCURATERIP_FUNC_HPP_
#include "accuraterip_func.hpp"
#endif

#include <cstdint>       // for int32_t, uint_fast64_t
#include <ios>           // for dec, hex
#include <iomanip>       // for setfill, setw
#include <ios>           // for ios_base
#include <ostream>       // for ostream
#include <sstream>       // for ostringstream
#include <string>        // for string
#include <vector>        // for vector

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"             // for Context, Updateable
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"              // for type, Checksum, ChecksumSet
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


// Explicit instantiations


template class UpdateableSubtotals <cstype::ARCS1>;

template class UpdateableSubtotals <cstype::ARCS2>;

template class UpdateableSubtotals <cstype::ARCS1, cstype::ARCS2>;


template class ARCSAlgorithm <cstype::ARCS1>;

template class ARCSAlgorithm <cstype::ARCS2>;

template class ARCSAlgorithm <cstype::ARCS1, cstype::ARCS2>;

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
	if (!out.good())
	{
		return;  // Maybe set badbit: out.setstate(std::ios_base::badbit);
	}

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

	[[maybe_unused]] const arcstk::details::StreamFlagsGuard guard { out };

	out.flags(fmt_flags(out));

	out << std::dec
		<< std::setw(3) << std::setfill('0') << total_tracks
		<< std::hex
		<< delim << std::setw(8) << std::setfill('0') << id_1
		<< delim << std::setw(8) << std::setfill('0') << id_2
		<< delim << std::setw(8) << std::setfill('0') << cddb_id;
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

