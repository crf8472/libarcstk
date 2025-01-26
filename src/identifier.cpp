/**
 * \file
 *
 * \brief Implementation of a low-level API for representing AccurateRip ids
 */

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_DETAILS_HPP__
#include "identifier_details.hpp"
#endif
#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"      // for AudioSize, CDDA, ToC
#endif

#include <algorithm>         // for transform
#include <cstdint>           // for uint32_t, uint64_t
#include <iomanip>           // for operator<<, setw, setfill
#include <memory>            // for unique_ptr, make_unique, operator==
#include <sstream>           // for operator<<, basic_ostream, basic_strings...
#include <stdexcept>         // for logic_error
#include <string>            // for string, operator<<, char_traits
#include <utility>           // for move
#include <vector>            // for vector, vector<>::size_type
#include <iostream>

#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

const ARId EmptyARId = *make_empty_arid();


// identifier_details.hpp

namespace details
{

const std::string AR_URL_PREFIX { "http://www.accuraterip.com/accuraterip/" };


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
		accum += sum_digits(static_cast<uint32_t>(o) / fps + 2u);
	}
	accum %= 255; // normalize to 1 byte


	// since 0 <= offsets.size <= 99 narrowing is no problem
	const auto track_count = static_cast<uint32_t>(offsets.size());


	return (accum << 24u) | (total_seconds << 8u) | track_count;
}


uint64_t sum_digits(const uint32_t number) noexcept
{
	return (number < 10) ? number : (number % 10) + sum_digits(number / 10);
}


std::string construct_filename(const int track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	auto ss = std::stringstream {};

	auto dec_flags = std::ios_base::fmtflags { ss.flags() };
	dec_flags &= ~ss.basefield;
	dec_flags &= ~ss.adjustfield;
	dec_flags |= ss.right;
	dec_flags |= ss.dec;

	auto hex_flags = std::ios_base::fmtflags { ss.flags() };
	hex_flags &= ~ss.basefield;
	hex_flags &= ~ss.adjustfield;
	hex_flags |= ss.right;
	hex_flags |= ss.hex;

	ss.flags(dec_flags);
	ss << "dBAR" << '-' << std::setw(3) << std::setfill('0') << track_count;

	ss.flags(hex_flags);
	ss << std::nouppercase
	   << '-' << std::setw(8) << std::setfill('0') << id_1
	   << '-' << std::setw(8) << std::setfill('0') << id_2
	   << '-' << std::setw(8) << std::setfill('0') << cddb_id
	   << ".bin";

	return ss.str();
}


std::string construct_url(const int track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	auto ss = std::stringstream {};

	auto hex_flags = std::ios_base::fmtflags { ss.flags() };
	hex_flags &= ~ss.basefield;
	hex_flags &= ~ss.adjustfield;
	hex_flags |= ss.right;
	hex_flags |= ss.hex;

	ss.flags(hex_flags);
	ss << AR_URL_PREFIX
	   << std::setw(1) << (id_1       & 0xFu)
	   << '/'          << (id_1 >> 4u & 0xFu)
	   << '/'          << (id_1 >> 8u & 0xFu)
	   << '/' << construct_filename(track_count, id_1, id_2, cddb_id);

	return ss.str();
}


std::string construct_id(const int track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept
{
	auto id = std::stringstream {};

	id << std::dec
		<< std::setw(3) << std::setfill('0') << track_count
		<< "-"
		<< std::hex << std::nouppercase
		<< std::setw(8) << std::setfill('0') << id_1
		<< "-"
		<< std::setw(8) << std::setfill('0') << id_2
		<< "-"
		<< std::setw(8) << std::setfill('0') << cddb_id;

	return id.str();
}


// TODO Do this generically
std::vector<int32_t> to_frames(const std::vector<AudioSize>& offsets)
{
	auto integers { std::vector<int32_t>(offsets.size()) };

	using std::begin;
	using std::cbegin;
	using std::cend;

	std::transform(cbegin(offsets), cend(offsets), begin(integers),
			[](const AudioSize& a) -> int32_t
			{
				return a.total_frames();
			});

	return integers;
}

} // namespace details


/**
 * \internal
 * \ingroup id
 *
 * \brief Private implementation of ARId
 *
 * \see ARId
 */
class ARId::Impl final
{

public:

	/**
	 * \brief URL prefix for accessing AccurateRip
	 */

	/**
	 * \brief Implements ARId::ARId().
	 */
	Impl(const int track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id);

	/**
	 * \brief Implements ARId::url()
	 */
	std::string url() const noexcept;

	/**
	 * \brief Implements ARId::filename()
	 */
	std::string filename() const noexcept;

	/**
	 * \brief Implements ARId::track_count()
	 */
	int track_count() const noexcept;

	/**
	 * \brief Implements ARId::disc_id_1()
	 */
	uint32_t disc_id_1() const noexcept;

	/**
	 * \brief Implements ARId::disc_id_2()
	 */
	uint32_t disc_id_2() const noexcept;

	/**
	 * \brief Implements ARId::cddb_id()
	 */
	uint32_t cddb_id() const noexcept;

	/**
	 * \brief Implements ARId::empty()
	 */
	bool empty() const noexcept;


	std::string to_string() const noexcept;


	friend bool operator == (const Impl& lhs, const Impl& rhs) noexcept
	{
		return lhs.track_count_ == rhs.track_count_
			&& lhs.disc_id1_    == rhs.disc_id1_
			&& lhs.disc_id2_    == rhs.disc_id2_
			&& lhs.cddb_id_     == rhs.cddb_id_;
	}

private:

	/**
	 * \brief Number of tracks
	 */
	int track_count_;

	/**
	 * \brief Disc id no. 1
	 */
	uint32_t disc_id1_;

	/**
	 * \brief Disc id no. 2
	 */
	uint32_t disc_id2_;

	/**
	 * \brief CDDB disc id
	 */
	uint32_t cddb_id_;
};


ARId::Impl::Impl(const int track_count, const uint32_t id_1,
		const uint32_t id_2, const uint32_t cddb_id)
	: track_count_ { track_count }
	, disc_id1_    { id_1 }
	, disc_id2_    { id_2 }
	, cddb_id_     { cddb_id }
{
	// empty
}


std::string ARId::Impl::url() const noexcept
{
	return details::construct_url(track_count_, disc_id1_, disc_id2_, cddb_id_);
}


std::string ARId::Impl::filename() const noexcept
{
	return details::construct_filename(track_count_, disc_id1_, disc_id2_,
			cddb_id_);
}


int ARId::Impl::track_count() const noexcept
{
	return track_count_;
}


uint32_t ARId::Impl::disc_id_1() const noexcept
{
	return disc_id1_;
}


uint32_t ARId::Impl::disc_id_2() const noexcept
{
	return disc_id2_;
}


uint32_t ARId::Impl::cddb_id() const noexcept
{
	return cddb_id_;
}


bool ARId::Impl::empty() const noexcept
{
	return *EmptyARId.impl_ == *this;
}


std::string ARId::Impl::to_string() const noexcept
{
	return details::construct_id(track_count_, disc_id1_, disc_id2_, cddb_id_);
}


// ARId


ARId::ARId(const int track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id)
	: impl_ { std::make_unique<ARId::Impl>(track_count, id_1, id_2, cddb_id) }
{
	// empty
}


ARId::ARId(const ARId &id)
	: impl_ { std::make_unique<ARId::Impl>(*id.impl_) }
{
	// empty
}


ARId::ARId(ARId &&rhs) noexcept = default;


ARId::~ARId() noexcept = default;


std::string ARId::url() const noexcept
{
	return impl_->url();
}


std::string ARId::filename() const noexcept
{
	return impl_->filename();
}


int ARId::track_count() const noexcept
{
	return impl_->track_count();
}


uint32_t ARId::disc_id_1() const noexcept
{
	return impl_->disc_id_1();
}


uint32_t ARId::disc_id_2() const noexcept
{
	return impl_->disc_id_2();
}


uint32_t ARId::cddb_id() const noexcept
{
	return impl_->cddb_id();
}


std::string ARId::prefix() const noexcept
{
	return details::AR_URL_PREFIX;
}


bool ARId::empty() const noexcept
{
	return impl_->empty();
}


ARId& ARId::operator = (const ARId &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	// deep copy
	impl_ = std::make_unique<ARId::Impl>(*rhs.impl_);
	return *this;
}


ARId& ARId::operator = (ARId &&rhs) noexcept = default;


bool operator == (const ARId& lhs, const ARId& rhs) noexcept
{
	return &lhs == &rhs || *lhs.impl_ == *rhs.impl_;
}


std::string to_string(const ARId& id) noexcept
{
	return details::construct_id(
			id.track_count(),
			id.disc_id_1(),
			id.disc_id_2(),
			id.cddb_id());
}


// make_arid


std::unique_ptr<ARId> make_arid(const std::vector<int32_t>& offsets,
		const int32_t leadout)
{
	return std::make_unique<ARId>(
			offsets.size(),
			details::disc_id_1(offsets, leadout),
			details::disc_id_2(offsets, leadout),
			details::cddb_id  (offsets, leadout)
	);
}


std::unique_ptr<ARId> make_arid(const std::vector<AudioSize>& offsets,
		const AudioSize& leadout)
{
	const auto offset_frames { details::to_frames(offsets) };
	const auto leadout_frame { leadout.total_frames() };

	return make_arid(offset_frames, leadout_frame);
}


std::unique_ptr<ARId> make_arid(const ToC& toc, const AudioSize& leadout)
{
	return make_arid(toc.offsets(), leadout);
}


std::unique_ptr<ARId> make_arid(const ToC& toc)
{
	return make_arid(toc.offsets(), toc.leadout());
}

// make_empty_arid


std::unique_ptr<ARId> make_empty_arid() noexcept
{
	return std::make_unique<ARId>(0, 0, 0, 0);
}

} // namespace v_1_0_0
} // namespace arcstk

