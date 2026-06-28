/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of the checksum verification API
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"
#endif
#ifndef LIBARCSTK_VERIFY_DETAILS_HPP_
#include "verify_details.hpp"
#endif

#include <cstddef>        // for size_t
#include <cstdint>        // for uint32_t
#include <iomanip>        // for setw, setfill
#include <ios>            // for boolalpha
#include <memory>         // for addressof, make_unique
#include <ostream>        // for ostream
#include <sstream>        // for ostringstream
#include <stdexcept>      // for runtime_error
#include <string>         // for string
#include <tuple>          // for tuple
#include <utility>        // for move
#include <vector>         // for vector

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"                   // for Checksums, checksum::
#endif
#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"                       // for DBAR
#endif
#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"                 // for ARId
#endif
#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"                   // for CDDA
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

namespace details
{

// BestBlock


constexpr int BestBlock::MAX_DIFFERENCE;

best_block_info_t BestBlock::from(const VerificationResult& result) const
{
	ARCS_LOG(DEBUG1) << "Find best block:";

	if (result.size() == 0)
	{
		return std::make_tuple(-1, false, -1);
	}

	auto block            = int  { -1 };
	auto best_block_is_v2 = bool { false };

	auto best_diff    = int { MAX_DIFFERENCE };
	auto curr_diff_v1 = int { 0 };
	auto curr_diff_v2 = int { 0 };

	for (auto b = int { 0 }; b < static_cast<int>(result.total_blocks()); ++b)
	{
		// Note: v2 matching will always be preferred over v1 matching

		ARCS_LOG(DEBUG1) << "Check block " << b;

		curr_diff_v1 = result.difference(b, checksum::type::ARCS1);
		curr_diff_v2 = result.difference(b, checksum::type::ARCS2);

		// Note the less-equal for v2: last match wins!
		if (curr_diff_v2 <= best_diff or curr_diff_v1 < best_diff)
		{
			block      = b;
			best_block_is_v2 = curr_diff_v2 <= curr_diff_v1;
			best_diff  = best_block_is_v2 ? curr_diff_v2 : curr_diff_v1;

			ARCS_LOG_DEBUG << "Declare block " << b << " as best match"
				<< " (is ARCSv" << (best_block_is_v2 + 1) << ")";
		}
	}

	return std::make_tuple(block, best_block_is_v2, best_diff);
}


// ResultBits


ResultBits::ResultBits(const size_type blocks, const size_type tracks)
	: blocks_          { blocks }
	, tracks_per_block_{ tracks }
	, flag_            { create_flag_store(total_blocks() * flags_per_block()) }
{
	check_maximums(blocks, tracks);
}


ResultBits::ResultBits() // TODO redundant? could just be default
	: ResultBits { 0, 0 }
{
	// empty
}


ResultBits::size_type ResultBits::total_blocks() const
{
	return blocks_;
}


ResultBits::size_type ResultBits::tracks_per_block() const
{
	return tracks_per_block_;
}


ResultBits::size_type ResultBits::size() const
{
	return flag_.size();
}


ResultBits::index_type ResultBits::set_id(ResultBits::index_type b, bool value)
{
	bounds_check_block(b);

	const auto offset { block_offset(b) };
	set_flag(offset, value);

	return offset;
}


bool ResultBits::id(ResultBits::index_type b) const
{
	bounds_check_block(b);

	return this->operator[](block_offset(b));
}


ResultBits::index_type ResultBits::set_track(ResultBits::index_type b,
		ResultBits::index_type t, bool v2, bool value)
{
	bounds_check_block(b);
	bounds_check_track(t);

	const auto offset { index(b, t, v2) };

	set_flag(offset, value);

	return offset;
}


bool ResultBits::track(ResultBits::index_type b, ResultBits::index_type t,
		bool v2) const
{
	bounds_check_block(b);
	bounds_check_track(t);

	return this->operator[](index(b, t, v2));
}


ResultBits::size_type ResultBits::total_tracks_set(ResultBits::index_type b)
	const
{
	bounds_check_block(b);

	const auto block_start = block_offset(b);
	const auto block_len   = flags_per_block() > 0
		? static_cast<index_type>(flags_per_block()) - 1
		: 0;

	const auto first_flag_in_tracks = block_start + 1; /* skip id */
	const auto last_flag_in_tracks  = block_start + block_len;

	auto count = size_type { 0 };

	for (auto i = first_flag_in_tracks; i <= last_flag_in_tracks; ++i)
	{
		count += this->operator[](i);
	}

	return count;
}


bool ResultBits::operator[](const ResultBits::index_type i) const
{
	using flag_size_type = decltype( flag_ )::size_type;

	return flag_[static_cast<flag_size_type>(i)];
}


ResultBits::index_type ResultBits::index(ResultBits::index_type b,
		ResultBits::index_type t, bool v2) const
{
	// b and t are 0-based
	return block_offset(b) + track_offset(t, v2);
}


ResultBits::index_type ResultBits::block_offset(ResultBits::index_type b) const
{
	return b * static_cast<index_type>(flags_per_block());
}


ResultBits::index_type ResultBits::track_offset(ResultBits::index_type t,
		bool v2) const
{
	return t + 1 + (v2 ? static_cast<index_type>(tracks_per_block_) : 0);
}


void ResultBits::set_flag(const ResultBits::index_type offset, const bool value)
{
	flag_[static_cast<std::size_t>(offset)] = value;
}


ResultBits::size_type ResultBits::flags_per_block() const
{
	return 2 /* v1 + v2 */ * tracks_per_block() + 1 /* id */;
}


void ResultBits::check_maximums(const ResultBits::size_type blocks,
		const ResultBits::size_type tracks) const
{
	if (blocks > MAX_TOTAL_BLOCKS)
	{
		using std::to_string;
		throw std::out_of_range("Illegal number of blocks: "
				+ to_string(blocks));
	}

	if (tracks > CDDA::MAX_TRACKCOUNT)
	{
		using std::to_string;
		throw std::out_of_range("Illegal number of tracks: "
				+ to_string(tracks));
	}
}


void ResultBits::bounds_check_block(ResultBits::index_type b) const
{
	if (b >= static_cast<index_type>(blocks_))
	{
		auto msg = std::ostringstream {};
		msg << "Block index " << b << " too big, only " << blocks_
			<< " blocks in response";

		throw std::runtime_error(msg.str());
	}
}


void ResultBits::bounds_check_track(ResultBits::index_type t) const
{
	if (t >= static_cast<index_type>(tracks_per_block_))
	{
		auto msg = std::ostringstream {};
		msg << "Track index " << t << " too big, only " << tracks_per_block_
			<< " tracks in album";

		throw std::runtime_error(msg.str());
	}
}


std::vector<bool> ResultBits::create_flag_store(const size_type size) const
{
	return std::vector<bool>(size, false);
}


// VerificationPolicy


bool VerificationPolicy::is_verified(const int track,
		const VerificationResult& r) const
{
	return do_is_verified(track, r);
}


VerificationPolicy::size_type VerificationPolicy::total_unverified_tracks(
		const VerificationResult& r) const
{
	return do_total_unverified_tracks(r);
}


bool VerificationPolicy::is_strict() const
{
	return do_is_strict();
}


std::unique_ptr<VerificationPolicy> VerificationPolicy::clone() const
{
	return do_clone();
}


VerificationPolicy::size_type VerificationPolicy::do_total_unverified_tracks(
		const VerificationResult& r) const
{
	const auto total_tracks = static_cast<int>(r.tracks_per_block());
	auto total_unverified   = r.tracks_per_block();

	for (auto t = int { 0 }; t < total_tracks; ++t)
	{
		if (is_verified(t, r))
		{
			--total_unverified;
		}
	}

	return total_unverified;
}


// StrictPolicy


bool StrictPolicy::do_is_verified(const int track, const VerificationResult& r)
	const
{
	const auto bb = r.best_block();

	return r.track(best_block::index(bb), track, best_block::checksumtype(bb));
}


VerificationPolicy::size_type StrictPolicy::do_total_unverified_tracks(
		const VerificationResult& r) const
{
	const auto bb = r.best_block();

	// Do not count a non-matching id as unverified track
	return static_cast<size_type>(best_block::difference(bb))
		- !r.id(best_block::index(bb));
}


bool StrictPolicy::do_is_strict() const
{
	return true;
}


std::unique_ptr<VerificationPolicy> StrictPolicy::do_clone() const
{
	return std::make_unique<StrictPolicy>(*this);
}


// LiberalPolicy


bool LiberalPolicy::do_is_verified(const int track, const VerificationResult& r)
	const
{
	for (auto b = int { 0 }; b < static_cast<int>(r.total_blocks()); ++b)
	{
		// if track matches either v2 or v1
		if (r.track(b, track, checksum::type::ARCS2) ||
				r.track(b, track, checksum::type::ARCS1))
		{
			return true;
		}
	}
	return false;
}


bool LiberalPolicy::do_is_strict() const
{
	return false;
}


std::unique_ptr<VerificationPolicy> LiberalPolicy::do_clone() const
{
	return std::make_unique<LiberalPolicy>(*this);
}


// Result


Result::Result(std::unique_ptr<VerificationPolicy> p)
	: flags_  {}
	, policy_ { std::move(p) }
{
	// empty
}


Result::Result(const Result& rhs)
	: flags_  { rhs.flags_ }
	, policy_ { rhs.policy_->clone() }
{
	// empty
}


Result& Result::operator = (const Result& rhs)
{
	if (&rhs != this)
	{
		flags_  = rhs.flags_;
		policy_ = rhs.policy_->clone();
	}
	return *this;
}


void Result::init(const size_type blocks, const size_type tracks)
{
	flags_ = ResultBits { blocks, tracks };
}


Result::index_type Result::do_verify_id(const index_type b)
{
	return flags_.set_id(b, true);
}


bool Result::do_id(const index_type b) const
{
	return flags_.id(b);
}


Result::index_type Result::do_verify_track(const index_type b,
		const index_type t, const checksum::type type )
{
	return flags_.set_track(b, t, is_v2(type), true);
}


bool Result::do_track(const index_type b, const index_type t,
		const checksum::type type) const
{
	return flags_.track(b, t, is_v2(type));
}


int Result::do_difference(const index_type b, const checksum::type type) const
{
	const auto total_tracks = static_cast<int>(flags_.tracks_per_block());

	auto difference = int { (id(b) ? 0 : 1) }; // also calls bounds_check_block()

	for (auto t = int { 0 }; t < total_tracks; ++t)
	{
		difference += ( track(b, t, type) ? 0 : 1 );
	}

	return difference;
}


Result::size_type Result::do_total_blocks() const
{
	return flags_.total_blocks();
}


Result::size_type Result::do_tracks_per_block() const
{
	return flags_.tracks_per_block();
}


Result::size_type Result::do_size() const
{
	return flags_.size();
}


bool Result::do_is_verified(const index_type track) const
{
	return policy_->is_verified(track, *this);
}


Result::size_type Result::do_total_unverified_tracks() const
{
	return policy_->total_unverified_tracks(*this);
}


best_block_info_t Result::do_best_block() const
{
	static const BestBlock best;
	return best.from(*this);
}


int Result::do_best_block_difference() const
{
	return best_block::difference(best_block());
}


bool Result::do_strict() const
{
	return policy()->is_strict();
}


const VerificationPolicy* Result::policy() const
{
	return policy_.get();
}


bool Result::is_v2(const checksum::type type) const
{
	return checksum::type::ARCS2 == type;
}


std::unique_ptr<VerificationResult> Result::do_clone() const
{
	return std::make_unique<Result>(*this);
}


// create_result


std::unique_ptr<VerificationResult> create_result(const std::size_t blocks,
		const std::size_t tracks, std::unique_ptr<VerificationPolicy> p)
{
	auto r = std::make_unique<Result>(std::move(p));
	r->init(blocks, tracks);
	return r;
}


// Selector


const uint32_t& Selector::get(const ChecksumSource& s,
		const ChecksumSource::size_type current,
		const ChecksumSource::size_type counter) const
{
	return do_get(s, current, counter);
}


std::unique_ptr<Selector> Selector::clone() const
{
	return do_clone();
}


// BlockSelector


const uint32_t& BlockSelector::do_get(const ChecksumSource& s,
		const ChecksumSource::size_type block,
		const ChecksumSource::size_type track) const
{
	return s.arcs_value(block, track);
}


std::unique_ptr<Selector> BlockSelector::do_clone() const
{
	return std::make_unique<BlockSelector>();
}


// TrackSelector


const uint32_t& TrackSelector::do_get(const ChecksumSource& s,
		const ChecksumSource::size_type track,
		const ChecksumSource::size_type block) const
{
	return s.arcs_value(block, track);
}


std::unique_ptr<Selector> TrackSelector::do_clone() const
{
	return std::make_unique<TrackSelector>();
}


// SourceIterator


SourceIterator::SourceIterator(const ChecksumSource& source,
		const ChecksumSource::size_type current,
		const ChecksumSource::size_type counter, const Selector& selector)
	: selector_ { std::addressof(selector) }
	, source_   { std::addressof(source)   }
	, current_  { current }
	, counter_  { counter }
{
	// empty
}


ChecksumSource::size_type SourceIterator::counter() const
{
	return counter_;
}


ChecksumSource::size_type SourceIterator::current() const
{
	return current_;
}


SourceIterator::reference SourceIterator::operator * () const // dereferencing
{
	// Used for testing, commented out

	// if (!source_)
	// {
	// 	throw std::invalid_argument("Cannot traverse null ChecksumSource");
	// }
	// if (!selector_)
	// {
	// 	throw std::invalid_argument("No selector available");
	// }

	return selector_->get(*source_, current_, counter_);
}


SourceIterator::pointer SourceIterator::operator -> () const // dereferncing
{
	return std::addressof(selector_->get(*source_, current_, counter_));
}


SourceIterator& SourceIterator::operator ++ () // prefix increment
{
	++counter_;
	return *this;
}


SourceIterator SourceIterator::operator ++ (int) // postfix increment
{
	auto tmp = SourceIterator { *this };
	++(*this);
	return tmp;
}


// TraversalPolicy


TraversalPolicy::TraversalPolicy(std::unique_ptr<Selector> selector)
	: selector_ { std::move(selector) }
{
	// empty
}


TraversalPolicy::TraversalPolicy(const TraversalPolicy& rhs)
	: source_   { rhs.source_ }
	, selector_ { rhs.selector_->clone() }
	, current_  { rhs.current_ }
{
	// empty
}


TraversalPolicy& TraversalPolicy::operator = (const TraversalPolicy& rhs)
{
	if (&rhs != this)
	{
		source_   = rhs.source_;
		selector_ = rhs.selector_->clone();
		current_  = rhs.current_;
	}
	return *this;
}


const Selector& TraversalPolicy::selector() const
{
	return *selector_;
}


ChecksumSource::size_type TraversalPolicy::end_current(
		const ChecksumSource::size_type c) const
{
	return do_end_current(*source_, c);
}


ChecksumSource::size_type TraversalPolicy::end_counter(
		const ChecksumSource::size_type c) const
{
	return do_end_counter(*source_, c);
}


void TraversalPolicy::check_source_for_null() const
{
	if (!source())
	{
		throw std::invalid_argument("Cannot traverse null ChecksumSource");
	}
}


const ChecksumSource* TraversalPolicy::source() const
{
	return source_;
}


void TraversalPolicy::set_source(const ChecksumSource& source)
{
	source_ = std::addressof(source);
}


TraversalPolicy::const_iterator TraversalPolicy::begin(
		const ChecksumSource::size_type current) const
{
	check_source_for_null();
	return SourceIterator(*source(), current, 0, selector());
}


TraversalPolicy::const_iterator TraversalPolicy::end(
		const ChecksumSource::size_type current) const
{
	check_source_for_null();
	return SourceIterator(*source(), current, end_counter(current), selector());
}


Checksums::size_type TraversalPolicy::current_block(const SourceIterator& i)
	const
{
	return do_current_block(i);
}


Checksums::size_type TraversalPolicy::current_track(const SourceIterator& i)
	const
{
	return do_current_track(i);
}


std::unique_ptr<VerificationPolicy> TraversalPolicy::get_policy() const
{
	return create_track_policy();
}


std::unique_ptr<Selector> TraversalPolicy::get_selector() const
{
	return create_selector();
}


// BlockTraversal


BlockTraversal::BlockTraversal()
	: TraversalPolicy { std::make_unique<BlockSelector>() }
{
	// empty
}


std::unique_ptr<VerificationPolicy> BlockTraversal::create_track_policy() const
{
	return std::make_unique<StrictPolicy>();
}


std::unique_ptr<Selector> BlockTraversal::create_selector() const
{
	return std::make_unique<BlockSelector>();
}


Checksums::size_type BlockTraversal::do_current_block(const SourceIterator& i)
	const
{
	return i.current();
}


Checksums::size_type BlockTraversal::do_current_track(const SourceIterator& i)
	const
{
	return i.counter();
}


ChecksumSource::size_type BlockTraversal::do_end_current(
		const ChecksumSource& source, const Checksums::size_type /*c*/) const
{
	return source.size(); // number of blocks in source
}


ChecksumSource::size_type BlockTraversal::do_end_counter(
		const ChecksumSource& source, const Checksums::size_type c) const
{
	return source.size(c); // number of tracks per block in source
}


// TrackTraversal


TrackTraversal::TrackTraversal()
	: TraversalPolicy { std::make_unique<TrackSelector>() }
{
	// empty
}


std::unique_ptr<VerificationPolicy> TrackTraversal::create_track_policy() const
{
	return std::make_unique<LiberalPolicy>();
}


std::unique_ptr<Selector> TrackTraversal::create_selector() const
{
	return std::make_unique<TrackSelector>();
}


Checksums::size_type TrackTraversal::do_current_block(const SourceIterator& i)
	const
{
	return i.counter();
}


Checksums::size_type TrackTraversal::do_current_track(const SourceIterator& i)
	const
{
	return i.current();
}


ChecksumSource::size_type TrackTraversal::do_end_current(
	const ChecksumSource& source, const Checksums::size_type c) const
{
	return source.size(c); // traverses same track over all blocks
}


ChecksumSource::size_type TrackTraversal::do_end_counter(
	const ChecksumSource& source, const Checksums::size_type /*c*/) const
{
	return source.size(); // traverses same track over all blocks
}


// MatchPolicy


void MatchPolicy::perform(VerificationResult& result,
		const Checksums& actual_sums, const Checksum& ref,
		const int block, const Checksums::size_type track) const
{
	do_perform(result, actual_sums, ref, block, track);
}


void MatchPolicy::perform_match(VerificationResult& result,
		const ChecksumSet& actual, const Checksum& ref,
		const int block, const Checksums::size_type track) const
{
	const auto track_no = static_cast<int>(track);

	for (const auto& type : actual.types())
	{
		if (const auto p = actual.get(type); is_match(p.first, ref))
		{
			const auto bitpos = result.verify_track(block, track_no, type);

			ARCS_LOG(DEBUG2) << "Track "
				<< std::setw(2) << std::setfill('0') << (track + 1)
				<< " v" << (checksum::type::ARCS2 == type ? "2" : "1")
				<< " verified: "
				<< result.track(block, track_no, type)
				<< " (bit " << bitpos << ")";
		} else
		{
			ARCS_LOG(DEBUG2) << "Track "
				<< std::setw(2) << std::setfill('0') << (track + 1)
				<< " v" << (checksum::type::ARCS2 == type ? "2" : "1")
				<< " not verified: "
				<< result.track(block, track_no, type);
		}
	}
}


// TrackOrderPolicy


void TrackOrderPolicy::do_perform(VerificationResult& result,
		const Checksums& actual_sums, const Checksum& ref,
		const int block, const Checksums::size_type track) const
{
	perform_match(result, actual_sums.at(track), ref, block, track);
}


// FindOrderPolicy


void FindOrderPolicy::do_perform(VerificationResult& result,
		const Checksums& actual_sums, const Checksum& ref,
		const int block, const Checksums::size_type /* track */) const
{
	auto t = Checksums::size_type { 0 };
	for (const auto& set : actual_sums)
	{
		perform_match(result, set, ref, block, t);
		++t;
	}
}


// Verification


void Verification::set_all_ids_verified(VerificationResult& result) const
{
	for (auto b = int { 0 }; b < static_cast<int>(result.total_blocks()); ++b)
	{
		result.verify_id(b);
	}
}


void Verification::perform_ids(VerificationResult& result,
	const ARId& actual_id, const ChecksumSource& ref_sums) const
{
	using size_type = ChecksumSource::size_type;

	for (auto b = size_type { 0 }; b < ref_sums.size(); ++b)
	{
		if (is_match(actual_id, ref_sums.id(b)))
		{
			result.verify_id(static_cast<int>(b));
		}
	}
}


void Verification::perform_checksums(VerificationResult& result,
		const Checksums& actual_sums,
		const TraversalPolicy& traversal,
		const ChecksumSource::size_type current, const MatchPolicy& order) const
{
	auto block = int { 0 };

	for (auto t = traversal.begin(current); t != traversal.end(current); ++t)
	{
		block = static_cast<int>(traversal.current_block(t));

		if (result.id(block)) // ARId matched?
		{
			order.perform(result, actual_sums, Checksum { *t }, block,
				traversal.current_track(t));
		}
	}
}


void Verification::perform(VerificationResult& result,
	const Checksums& actual_sums, const ARId& actual_id,
	const ChecksumSource& ref_sums,
	const TraversalPolicy& traversal, const MatchPolicy& order) const
{
	// Always done once per block, regardless of traversal
	if (actual_id.empty())
	{
		set_all_ids_verified(result);
	} else
	{
		perform_ids(result, actual_id, ref_sums);
	}

	// From here on, result can be checked for whether the current block
	// (based on its id) is considered relevant for verification in the first
	// place.

	for (auto c = ChecksumSource::size_type { 0 };
			c < traversal.end_current(c); ++c)
	{
		perform_checksums(result, actual_sums, traversal, c, order);
	}
}


// verify


std::unique_ptr<VerificationResult> verify(
		const Checksums& actual_sums, const ARId& actual_id,
		const ChecksumSource& ref_sums,
		const TraversalPolicy& traversal, const MatchPolicy& order)
{
	auto result = create_result(ref_sums.size()/* total blocks */,
			actual_sums.size()/* total tracks per block */,
			traversal.get_policy());

	const auto v = Verification{};
	// Verification has no members so its instantiation does not
	// require extra memory.
	v.perform(*result, actual_sums, actual_id, ref_sums, traversal, order);

	return result;
}


// VerifierBase


VerifierBase::VerifierBase(const Checksums* actual_sums)
	: actual_sums_ { actual_sums }
	, is_strict_   { true }
{
	// empty
}


const ARId* VerifierBase::actual_id() const noexcept
{
	return do_actual_id();
}


const Checksums* VerifierBase::actual_checksums() const noexcept
{
	return actual_sums_;
}


bool VerifierBase::strict() const noexcept
{
	return is_strict_;
}


void VerifierBase::set_strict(const bool strict) noexcept
{
	is_strict_ = strict;
}


std::unique_ptr<VerificationResult> VerifierBase::perform(
			const ChecksumSource& ref_sums) const
{
	const auto id = actual_id() ? *actual_id() : ARId{};
	auto traversal = do_create_traversal();
	traversal->set_source(ref_sums);
	const auto order = do_create_order();

	return verify(*actual_checksums(), id, ref_sums, *traversal, *order);
}


const ARId* VerifierBase::do_actual_id() const noexcept
{
	return nullptr;
}


std::unique_ptr<TraversalPolicy> VerifierBase::do_create_traversal() const
{
	if (strict())
	{
		return std::make_unique<BlockTraversal>();
	} else
	{
		return std::make_unique<TrackTraversal>();
	}
}

} // namespace details


// ChecksumSource


ARId ChecksumSource::id(const ChecksumSource::size_type block_idx) const
{
	return this->do_id(block_idx);
}

Checksum ChecksumSource::checksum(
		const ChecksumSource::size_type block_idx,
		const ChecksumSource::size_type idx) const
{
	return this->do_checksum(block_idx, idx);
}

const uint32_t& ChecksumSource::arcs_value(
		const ChecksumSource::size_type block_idx,
		const ChecksumSource::size_type idx) const
{
	return this->do_arcs_value(block_idx, idx);
}

const uint32_t& ChecksumSource::confidence(
		const ChecksumSource::size_type block_idx,
		const ChecksumSource::size_type idx) const
{
	return this->do_confidence(block_idx, idx);
}

const uint32_t& ChecksumSource::frame450_arcs_value(
		const ChecksumSource::size_type block_idx,
		const ChecksumSource::size_type idx) const
{
	return this->do_arcs_value(block_idx, idx);
}

std::size_t ChecksumSource::size(const ChecksumSource::size_type block_idx)
	const
{
	return this->do_size(block_idx);
}

std::size_t ChecksumSource::size() const
{
	return this->do_size();
}

std::unique_ptr<ChecksumSource> ChecksumSource::clone() const
{
	return this->do_clone();
}


// DBARSource


DBARSource::DBARSource(const DBAR* dbar)
	: dbar_ { dbar }
{
	// empty
}


const DBAR* DBARSource::dbar() const
{
	return dbar_;
}


ARId DBARSource::do_id(const ChecksumSource::size_type block_idx) const
{
	return dbar_->block(static_cast<DBAR::size_type>(block_idx)).id();
}


Checksum DBARSource::do_checksum(const ChecksumSource::size_type block_idx,
		const ChecksumSource::size_type idx) const
{
	return Checksum {
		dbar_->block(static_cast<DBAR::size_type>(block_idx))
			.triplet(static_cast<DBAR::size_type>(idx)).arcs()
	};
}


const uint32_t& DBARSource::do_arcs_value(
		const ChecksumSource::size_type block,
		const ChecksumSource::size_type track) const
{
	return dbar_->arcs_value(block, track);
}


const unsigned& DBARSource::do_confidence(const ChecksumSource::size_type block,
		const ChecksumSource::size_type track) const
{
	return dbar_->confidence_value(block, track);
}


const uint32_t& DBARSource::do_frame450_arcs_value(
		const ChecksumSource::size_type block,
		const ChecksumSource::size_type track) const
{
	return dbar_->frame450_arcs_value(block, track);
}


std::size_t DBARSource::do_size(const ChecksumSource::size_type /* block_idx */)
	const
{
	return dbar_->block(0).size();
}


std::size_t DBARSource::do_size() const
{
	return dbar_->size();
}


std::unique_ptr<ChecksumSource> DBARSource::do_clone() const
{
	return std::make_unique<DBARSource>(*this);
}


// best_block_info


namespace best_block
{

int index(const best_block_info_t& bb)
{
	return details::best_block_get<details::TUPLE_IDX::INDEX>(bb);
}


int difference(const best_block_info_t& bb)
{
	return details::best_block_get<details::TUPLE_IDX::DIFFERENCE>(bb);
}


checksum::type checksumtype(const best_block_info_t& bb)
{
	// true -> ARCS2, false -> ARCS1
	return details::typeflag(bb) ? checksum::types[1] : checksum::types[0];
}


} // namespace best_block


// VerificationResult


VerificationResult::~VerificationResult() noexcept = default; // Pimpl
															  // requirement


bool VerificationResult::all_tracks_verified() const
{
	return total_unverified_tracks() == 0;
}


VerificationResult::index_type VerificationResult::verify_id(
		const index_type block)
{
	return do_verify_id(block);
}


bool VerificationResult::id(index_type b) const
{
	return do_id(b);
}


VerificationResult::index_type VerificationResult::verify_track(
		const index_type b, const index_type t, checksum::type type)
{
	return do_verify_track(b, t, type);
}


bool VerificationResult::track(const index_type b, const index_type t,
		const checksum::type type) const
{
	return do_track(b, t, type);
}


int VerificationResult::difference(const index_type b,
		const checksum::type type) const
{
	return do_difference(b, type);
}


VerificationResult::size_type VerificationResult::total_blocks() const
{
	return do_total_blocks();
}


VerificationResult::size_type VerificationResult::tracks_per_block() const
{
	return do_tracks_per_block();
}


VerificationResult::size_type VerificationResult::size() const
{
	return do_size();
}


bool VerificationResult::is_verified(const index_type track) const
{
	return do_is_verified(track);
}


VerificationResult::size_type VerificationResult::total_unverified_tracks()
	const
{
	return do_total_unverified_tracks();
}


best_block_info_t VerificationResult::best_block() const
{
	return do_best_block();
}


int VerificationResult::best_block_difference() const
{
	return do_best_block_difference();
}


bool VerificationResult::strict() const
{
	return do_strict();
}


std::unique_ptr<VerificationResult> VerificationResult::clone() const
{
	return do_clone();
}


std::ostream& operator << (std::ostream& out, const VerificationResult& result)
{
	if (!out.good())
	{
		// Maybe set badbit: out.setstate(std::ios_base::badbit);
		return out;
	}

	[[maybe_unused]] const arcstk::details::StreamFlagsGuard guard { out };

	const auto indent = std::string { "  " };
	for (auto b = int { 0 }; b < static_cast<int>(result.total_blocks()); ++b)
	{
		out << "Block " << b << '\n';

		out << indent << "Id match: "
			<< std::boolalpha << result.id(b) << '\n';

		for (auto t = int { 0 };
				t < static_cast<int>(result.tracks_per_block()); ++t)
		{
			out << indent << "Track " << std::setw(2) << (t + 1)
				<< ": ARCSv1 is " << std::boolalpha
				<< result.track(b, t, checksum::type::ARCS1)
				<< ", ARCSv2 is " << std::boolalpha
				<< result.track(b, t, checksum::type::ARCS2)
				<< '\n';
		}
	}

	return out;
}


// Verifier


const ARId* Verifier::actual_id() const noexcept
{
	return do_actual_id();
}


const Checksums* Verifier::actual_checksums() const noexcept
{
	return do_actual_checksums();
}


bool Verifier::strict() const noexcept
{
	return do_strict();
}


void Verifier::set_strict(const bool strict) noexcept
{
	do_set_strict(strict);
}


std::unique_ptr<VerificationResult> Verifier::perform(
		const ChecksumSource& ref_sums) const
{
	return do_perform(ref_sums);
}


std::unique_ptr<VerificationResult> Verifier::perform(const DBAR& ref_sums)
	const
{
	return do_perform(DBARSource{ std::addressof(ref_sums) });
}


// AlbumVerifier::Impl


AlbumVerifier::Impl::Impl(const Checksums& actual_sums, const ARId& actual_id)
	: details::VerifierBase { std::addressof(actual_sums) }
	, actual_id_            { std::addressof(actual_id)   }
{
	// empty
}


std::unique_ptr<details::MatchPolicy> AlbumVerifier::Impl::do_create_order()
	const
{
	return std::make_unique<details::TrackOrderPolicy>();
}


const ARId* AlbumVerifier::Impl::do_actual_id() const noexcept
{
	return actual_id_;
}


// AlbumVerifier


AlbumVerifier::AlbumVerifier(const Checksums& actual_sums,
		const ARId& actual_id)
	: impl_ { std::make_unique<Impl>(actual_sums, actual_id) }
{
	// empty
}


AlbumVerifier::AlbumVerifier(const AlbumVerifier& rhs)
	: impl_ { std::make_unique<Impl>(*rhs.impl_) }
{
	// empty
}


AlbumVerifier& AlbumVerifier::operator = (const AlbumVerifier& rhs)
{
	if (&rhs == this)
	{
		auto tmp = std::make_unique<Impl>(*rhs.impl_);
		impl_ = std::move(tmp);
	}
	return *this;
}


AlbumVerifier::AlbumVerifier(AlbumVerifier&& rhs) noexcept = default;


AlbumVerifier& AlbumVerifier::operator = (AlbumVerifier&& rhs) noexcept
= default;


AlbumVerifier::~AlbumVerifier() noexcept = default; // Pimpl requirement


const ARId* AlbumVerifier::do_actual_id() const noexcept
{
	return impl_->actual_id();
}


const Checksums* AlbumVerifier::do_actual_checksums() const noexcept
{

	return impl_->actual_checksums();
}


bool AlbumVerifier::do_strict() const noexcept
{
	return impl_->strict();
}


void AlbumVerifier::do_set_strict(const bool strict) noexcept
{
	return impl_->set_strict(strict);
}


std::unique_ptr<VerificationResult> AlbumVerifier::do_perform(
			const ChecksumSource& ref_sums) const
{
	return impl_->perform(ref_sums);
}


std::unique_ptr<Verifier> AlbumVerifier::do_clone() const
{
	return std::make_unique<AlbumVerifier>(*this);
}


// TracksetVerifier::Impl


TracksetVerifier::Impl::Impl(const Checksums& actual_sums)
	: details::VerifierBase { std::addressof(actual_sums) }
{
	// empty
}


std::unique_ptr<details::MatchPolicy> TracksetVerifier::Impl::do_create_order()
	const
{
	return std::make_unique<details::FindOrderPolicy>();
}


// TracksetVerifier


TracksetVerifier::TracksetVerifier(const Checksums& actual_sums)
	: impl_  { std::make_unique<Impl>(actual_sums) }
{
	// empty
}


TracksetVerifier::TracksetVerifier(const TracksetVerifier& rhs)
	: impl_  { std::make_unique<Impl>(*rhs.impl_) }
{
	// empty
}


TracksetVerifier& TracksetVerifier::operator = (const TracksetVerifier& rhs)
{
	if (&rhs == this)
	{
		auto tmp = std::make_unique<Impl>(*rhs.impl_);
		impl_ = std::move(tmp);
	}
	return *this;
}


TracksetVerifier::TracksetVerifier(TracksetVerifier&& rhs) noexcept = default;


TracksetVerifier& TracksetVerifier::operator = (TracksetVerifier&& rhs) noexcept
= default;


TracksetVerifier::~TracksetVerifier() noexcept = default; // Pimpl requirement


const ARId* TracksetVerifier::do_actual_id() const noexcept
{
	return impl_->actual_id();
}


const Checksums* TracksetVerifier::do_actual_checksums() const noexcept
{
	return impl_->actual_checksums();
}


bool TracksetVerifier::do_strict() const noexcept
{
	return impl_->strict();
}


void TracksetVerifier::do_set_strict(const bool strict) noexcept
{
	return impl_->set_strict(strict);
}


std::unique_ptr<VerificationResult> TracksetVerifier::do_perform(
			const ChecksumSource& ref_sums) const
{
	return impl_->perform(ref_sums);
}


std::unique_ptr<Verifier> TracksetVerifier::do_clone() const
{
	return std::make_unique<TracksetVerifier>(*this);
}


} // namespace v_1_0_0
} // namespace arcstk

