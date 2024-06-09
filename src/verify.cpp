/**
 * \file
 *
 * \brief Implementation of the checksum verification API
 */

#include <iomanip>        // for operator<<, setw, setfill, hex, uppercase
#include <stdexcept>

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"                   // for Checksums
#endif
#ifndef __LIBARCSTK_DBAR_HPP__
#include "dbar.hpp"                       // for DBAR
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif
#ifndef __LIBARCSTK_VERIFY_HPP__
#include "verify.hpp"
#endif
#ifndef __LIBARCSTK_VERIFY_DETAILS_HPP__
#include "verify_details.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

namespace details
{

// BestBlock


constexpr int BestBlock::MAX_DIFFERENCE;

std::tuple<int, bool, int> BestBlock::from(
		const VerificationResult& result) const
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

	for (auto b = int { 0 }; b < result.total_blocks(); ++b)
	{
		// Note: v2 matching will always be preferred over v1 matching

		ARCS_LOG(DEBUG1) << "Check block " << b;

		curr_diff_v1 = result.difference(b, false/*v1*/);
		curr_diff_v2 = result.difference(b, true /*v2*/);

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


ResultBits::ResultBits()
	: blocks_ { 0 }
	, tracks_per_block_ { 0 }
	, size_ { 0 }
	, flag_ ()
{
	// empty
}


int ResultBits::blocks() const
{
	return blocks_;
}


int ResultBits::tracks_per_block() const
{
	return tracks_per_block_;
}


ResultBits::size_type ResultBits::size() const
{
	return flag_.size();
}


bool ResultBits::init(int blocks, int tracks)
{
	try {
		validate(blocks, tracks);
	} catch (const std::exception& e)
	{
		//std::cerr << e.what();
		return false; // TODO Log something
	}

	blocks_ = blocks;
	tracks_per_block_ = tracks;

	size_ = static_cast<std::size_t>(blocks) *
				(2u * static_cast<std::size_t>(tracks) + 1u);

	flag_ = std::vector<bool>(size_, false); // No braces!

	return true;
}


int ResultBits::set_id(int b, bool value)
{
	this->validate_block(b);

	const auto offset { block_offset(b) };
	set_flag(offset, value);

	return offset;
}


bool ResultBits::id(int b) const
{
	this->validate_block(b);

	return this->operator[](block_offset(b));
}


int ResultBits::set_track(int b, int t, bool v2, bool value)
{
	this->validate_block(b);
	this->validate_track(t);

	const auto offset { index(b, t, v2) };

	set_flag(offset, value);

	return offset;
}


bool ResultBits::track(int b, int t, bool v2) const
{
	this->validate_block(b);
	this->validate_track(t);

	return this->operator[](index(b, t, v2));
}


ResultBits::size_type ResultBits::total_tracks_set(int b) const
{
	auto count = size_type { 0 };

	const auto end_block = block_offset(b) + 1 + 2 * tracks_per_block();
	for (auto i = block_offset(b) + 1; i <= end_block; ++i)
	{
		count += this->operator[](i);
	}

	return count;
}


void ResultBits::validate(int blocks, int tracks) const
{
	if (tracks < 0 or tracks > 99) // FIXME CDDA::MAX_TRACKCOUNT)
	{
		throw std::out_of_range("Illegal number of tracks: "
				+ std::to_string(tracks));
	}

	if (blocks < 0)
	{
		throw std::out_of_range("Illegal number of blocks: "
				+ std::to_string(blocks));
	}
}


bool ResultBits::operator[](const int i) const
{
	return flag_[static_cast<decltype(flag_)::size_type>(i)];
}


int ResultBits::flags_per_block() const
{
	return 2 * tracks_per_block() + 1;
}


int ResultBits::index(int b, int t, bool v2) const
{
	// b and t are 0-based
	return block_offset(b) + track_offset(t, v2);
}


int ResultBits::block_offset(int b) const
{
	return b * flags_per_block();
}


int ResultBits::track_offset(int t, bool v2) const
{
	return t + 1 + (v2 ? tracks_per_block_ : 0);
}


void ResultBits::set_flag(const int offset, const bool value)
{
	auto pos = flag_.begin() + offset; // TODO Work with index instead?
	*pos = value;
}


void ResultBits::validate_block(int b) const
{
	if (b >= blocks_)
	{
		auto msg = std::stringstream {};
		msg << "Block index " << b << " too big, only " << blocks_
			<< " blocks in response";

		throw std::runtime_error(msg.str());
	}
}


void ResultBits::validate_track(int t) const
{
	if (t >= tracks_per_block_)
	{
		auto msg = std::stringstream {};
		msg << "Track index " << t << " too big, only " << tracks_per_block_
			<< " tracks in album";

		throw std::runtime_error(msg.str());
	}
}


// VerificationPolicy


bool VerificationPolicy::is_verified(const int track,
		const VerificationResult& r) const
{
	return do_is_verified(track, r);
}


int VerificationPolicy::total_unverified_tracks(const VerificationResult& r)
	const
{
	return do_total_unverified_tracks(r);
}


bool VerificationPolicy::is_strict() const
{
	return do_is_strict();
}


int VerificationPolicy::do_total_unverified_tracks(const VerificationResult& r) const
{
	using size_type = details::ResultBits::size_type;

	const auto total_tracks = static_cast<size_type>(r.tracks_per_block());
	auto total_unverified = r.tracks_per_block();

	for (auto t = size_type { 0 }; t < total_tracks; ++t)
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
	const auto t = r.best_block();
	return r.track(std::get<0>(t), track, std::get<1>(t));
}


int StrictPolicy::do_total_unverified_tracks(const VerificationResult& r) const
{
	const auto t = r.best_block();
	// Do not count a non-matching id as unverified track
	return std::get<2>(t) - !r.id(std::get<0>(t));
}


bool StrictPolicy::do_is_strict() const
{
	return true;
}


// LiberalPolicy


bool LiberalPolicy::do_is_verified(const int track, const VerificationResult& r)
	const
{
	for (auto b = 0; b < r.total_blocks(); ++b)
	{
		if (r.track(b, track, true) or r.track(b, track, false))
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


// Result


Result::Result(std::unique_ptr<VerificationPolicy> p)
	: flags_  { ResultBits() }
	, policy_ { std::move(p) }
{
	// empty
}


void Result::init(const int blocks, const int tracks)
{
	flags_.init(blocks, tracks);
}


int Result::do_verify_id(int b)
{
	return flags_.set_id(b, true);
}


bool Result::do_id(int b) const
{
	return flags_.id(b);
}


int Result::do_verify_track(int b, int t, bool v2)
{
	return flags_.set_track(b, t, v2, true);
}


bool Result::do_track(int b, int t, bool v2) const
{
	return flags_.track(b, t, v2);
}


int Result::do_difference(int b, bool v2) const
{
	auto difference = int { (id(b) ? 0 : 1) }; // also calls validate_block()

	for (auto t = int { 0 }; t < flags_.tracks_per_block(); ++t)
	{
		difference += ( track(b, t, v2) ? 0 : 1 );
	}

	return difference;
}


int Result::do_total_blocks() const
{
	return flags_.blocks();
}


int Result::do_tracks_per_block() const
{
	return flags_.tracks_per_block();
}


size_t Result::do_size() const
{
	return flags_.size();
}


bool Result::do_is_verified(const int track) const
{
	return policy_->is_verified(track, *this);
}


int Result::do_total_unverified_tracks() const
{
	return policy_->total_unverified_tracks(*this);
}


std::tuple<int, bool, int> Result::do_best_block() const
{
	static const BestBlock best;
	return best.from(*this);
}


int Result::do_best_block_difference() const
{
	return std::get<2>(best_block());
}


bool Result::do_strict() const
{
	return policy()->is_strict();
}


const VerificationPolicy* Result::policy() const
{
	return policy_.get();
}


std::unique_ptr<VerificationResult> Result::do_clone() const
{
	return nullptr; // FIXME
}


// create_result


std::unique_ptr<VerificationResult> create_result(const int blocks,
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
	: selector_ { &selector }
	, source_   { &source }
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


SourceIterator::reference SourceIterator::operator * () const // dereferncing
{
	// Used for testing, commented out
	/*
	if (!source_)
	{
		throw std::invalid_argument("Cannot traverse null ChecksumSource");
	}
	if (!selector_)
	{
		throw std::invalid_argument("No selector available");
	}
	*/
	return selector_->get(*source_, current_, counter_);
}


SourceIterator::pointer SourceIterator::operator -> () const // dereferncing
{
	return &selector_->get(*source_, current_, counter_);
}


SourceIterator& SourceIterator::operator ++ () // prefix increment
{
	++counter_;
	return *this;
}


SourceIterator SourceIterator::operator ++ (int) // postfix increment
{
	SourceIterator tmp = *this;
	++(*this);
	return tmp;
}


// TraversalPolicy


TraversalPolicy::TraversalPolicy(std::unique_ptr<Selector> selector)
	: source_   { nullptr }
	, selector_ { std::move(selector) }
	, current_  { 0 }
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
	source_   = rhs.source_;
	selector_ = rhs.selector_->clone();
	current_  = rhs.current_;

	return *this;
}


const Selector& TraversalPolicy::selector() const
{
	return *selector_;
}


ChecksumSource::size_type TraversalPolicy::end_current() const
{
	return do_end_current(*source_);
}


ChecksumSource::size_type TraversalPolicy::end_counter() const
{
	return do_end_counter(*source_);
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
	source_ = &source;
}


ChecksumSource::size_type TraversalPolicy::current() const
{
	return current_;
}


void TraversalPolicy::set_current(const ChecksumSource::size_type current)
{
	current_ = current;
}


TraversalPolicy::const_iterator TraversalPolicy::begin() const
{
	check_source_for_null();
	return SourceIterator(*source(), current(), 0, selector());
}


TraversalPolicy::const_iterator TraversalPolicy::end() const
{
	check_source_for_null();
	return SourceIterator(*source(), current(), end_counter(), selector());
}


Checksums::size_type TraversalPolicy::current_block(const SourceIterator& i) const
{
	return do_current_block(i);
}


Checksums::size_type TraversalPolicy::current_track(const SourceIterator& i) const
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


Checksums::size_type BlockTraversal::do_current_block(const SourceIterator& i) const
{
	return i.current();
}


Checksums::size_type BlockTraversal::do_current_track(const SourceIterator& i) const
{
	return i.counter();
}


ChecksumSource::size_type BlockTraversal::do_end_current(const ChecksumSource& source) const
{
	return source.size(); // number of blocks in source
}


ChecksumSource::size_type BlockTraversal::do_end_counter(const ChecksumSource& source) const
{
	return source.size(0); // number of tracks per block in source
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
	const ChecksumSource& source) const
{
	return source.size(0); // traverses same track over all blocks
}


ChecksumSource::size_type TrackTraversal::do_end_counter(
	const ChecksumSource& source) const
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
	for (const auto& type : actual.types())
	{
		const bool is_v2 = (type == arcstk::checksum::type::ARCS2);

		if (ref == actual.get(type))
		{
			const auto bitpos = result.verify_track(block, track, is_v2);

			ARCS_LOG(DEBUG1) << "Track "
				<< std::setw(2) << std::setfill('0') << (track + 1)
				<< " v" << (is_v2 ? "2" : "1") << " verified: "
				<< result.track(block, track, is_v2)
				<< " (bit " << bitpos << ")";
		} else
		{
			ARCS_LOG(DEBUG1) << "Track "
				<< std::setw(2) << std::setfill('0') << (track + 1)
				<< " v" << (is_v2 ? "2" : "1") << " not verified: "
				<< result.track(block, track, is_v2);
		}
	}
}


// TrackOrderPolicy


void TrackOrderPolicy::do_perform(VerificationResult& result,
		const Checksums &actual_sums, const Checksum& ref,
		const int block, const Checksums::size_type track) const
{
	perform_match(result, actual_sums.at(track), ref, block, track);
}


// FindOrderPolicy


void FindOrderPolicy::do_perform(VerificationResult& result,
		const Checksums &actual_sums, const Checksum& ref,
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


void Verification::perform_ids(VerificationResult& result,
	const ARId &actual_id, const ChecksumSource& ref_sums) const
{
	using size_type = ChecksumSource::size_type;

	if (actual_id == EmptyARId)
	{
		// No actual ARId passed, set every id to 'verified'

		for (auto b = size_type { 0 }; b < ref_sums.size(); ++b)
		{
			result.verify_id(b);
		}
	} else
	{
		// Actually verify ids

		for (auto b = size_type { 0 }; b < ref_sums.size(); ++b)
		{
			if (actual_id == ref_sums.id(b))
			{
				result.verify_id(b);
			}
		}
	}
}


void Verification::perform_current(VerificationResult& result,
		const Checksums &actual_sums,
		const TraversalPolicy& traversal, const MatchPolicy& order) const
{
	for (auto it = traversal.begin(); it != traversal.end(); ++it)
	{
		if (result.id(traversal.current_block(it))) // ARId matched?
		{
			order.perform(result, actual_sums, *it, traversal.current_block(it),
				traversal.current_track(it));
		}
	}
}


void Verification::perform(VerificationResult& result,
	const Checksums &actual_sums, const ARId &actual_id,
	const ChecksumSource& ref_sums,
	TraversalPolicy& traversal, const MatchPolicy& order) const
{
	perform_ids(result, actual_id, ref_sums);
	// Always done once per block, regardless of traversal

	// From here on, result can be checked for whether the current block is
	// is actually considered relevant by its id.

	traversal.set_source(ref_sums);
	for (auto c = ChecksumSource::size_type { 0 };
			c < traversal.end_current(); ++c)
	{
		traversal.set_current(c);
		perform_current(result, actual_sums, traversal, order);
	}
}


// verify


std::unique_ptr<VerificationResult> verify(
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource &ref_sums,
		TraversalPolicy& traversal, const MatchPolicy& order)
{
	auto r = create_result(ref_sums.size()/* total blocks */,
			actual_sums.size()/* total tracks per block */,
			traversal.get_policy());

	const Verification v{};
	// Verification has no members so its instantiation does not
	// require extra memory.
	v.perform(*r, actual_sums, actual_id, ref_sums, traversal, order);

	return r;
}


// VerifierBase


VerifierBase::VerifierBase(const Checksums& actual_sums)
	: actual_sums_ { actual_sums }
	, is_strict_   { true        }
{
	// empty
}


const ARId& VerifierBase::actual_id() const noexcept
{
	return do_actual_id();
}


const Checksums& VerifierBase::actual_checksums() const noexcept
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
	const auto o = do_create_order();
	auto t = do_create_traversal();
	return verify(actual_checksums(), actual_id(), ref_sums, *t, *o);
}


const ARId& VerifierBase::do_actual_id() const noexcept
{
	return EmptyARId;
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

std::size_t ChecksumSource::size(const ChecksumSource::size_type block_idx) const
{
	return this->do_size(block_idx);
}

std::size_t ChecksumSource::size() const
{
	return this->do_size();
}


// DBARSource


ARId DBARSource::do_id(const ChecksumSource::size_type block_idx) const
{
	return source()->block(static_cast<DBAR::size_type>(block_idx)).id();
}


Checksum DBARSource::do_checksum(const ChecksumSource::size_type block_idx,
		const ChecksumSource::size_type idx) const
{
	return
		source()->block(static_cast<DBAR::size_type>(block_idx))
			.triplet(static_cast<DBAR::size_type>(idx)).arcs();
}


const uint32_t& DBARSource::do_arcs_value(
		const ChecksumSource::size_type block,
		const ChecksumSource::size_type track) const
{
	return source()->arcs_value(block, track);
}


const unsigned& DBARSource::do_confidence(const ChecksumSource::size_type block,
		const ChecksumSource::size_type track) const
{
	return source()->confidence_value(block, track);
}


const uint32_t& DBARSource::do_frame450_arcs_value(
		const ChecksumSource::size_type block,
		const ChecksumSource::size_type track) const
{
	return source()->frame450_arcs_value(block, track);
}


std::size_t DBARSource::do_size(const ChecksumSource::size_type /* block_idx */)
	const
{
	return source()->block(0).size();
}


std::size_t DBARSource::do_size() const
{
	return source()->size();
}


// VerificationResult


std::ostream& operator << (std::ostream &out, const VerificationResult &result)
{
	std::ios_base::fmtflags prev_settings = out.flags();

	const auto indent = std::string { "  " };
	for (auto b = int { 0 }; b < result.total_blocks(); ++b)
	{
		out << "Block " << b << std::endl;

		out << indent << "Id match: "
			<< std::boolalpha << result.id(b) << std::endl;

		for (auto t = int { 0 }; t < result.tracks_per_block(); ++t)
		{
			out << indent << "Track " << std::setw(2) << (t + 1)
				<< ": ARCSv1 is " << std::boolalpha << result.track(b, t, false)
				<< ", ARCSv2 is " << std::boolalpha << result.track(b, t, true)
				<< std::endl;
		}
	}

	out.flags(prev_settings);
	return out;
}


VerificationResult::~VerificationResult() noexcept = default;


bool VerificationResult::all_tracks_verified() const
{
	return total_unverified_tracks() == 0;
}


int VerificationResult::verify_id(int block)
{
	return do_verify_id(block);
}


bool VerificationResult::id(int b) const
{
	return do_id(b);
}


int VerificationResult::verify_track(int b, int t, bool v2)
{
	return do_verify_track(b, t, v2);
}


bool VerificationResult::track(int b, int t, bool v2) const
{
	return do_track(b, t, v2);
}


int VerificationResult::difference(int b, bool v2) const
{
	return do_difference(b, v2);
}


int VerificationResult::total_blocks() const
{
	return do_total_blocks();
}


int VerificationResult::tracks_per_block() const
{
	return do_tracks_per_block();
}


size_t VerificationResult::size() const
{
	return do_size();
}


bool VerificationResult::is_verified(const int track) const
{
	return do_is_verified(track);
}


int VerificationResult::total_unverified_tracks() const
{
	return do_total_unverified_tracks();
}


std::tuple<int, bool, int> VerificationResult::best_block() const
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


// Verifier


const ARId& Verifier::actual_id() const noexcept
{
	return do_actual_id();
}


const Checksums& Verifier::actual_checksums() const noexcept
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
	return do_perform(DBARSource{ &ref_sums });
}


// AlbumVerifier::Impl


AlbumVerifier::Impl::Impl(const Checksums& actual_sums, const ARId& actual_id)
	: details::VerifierBase { actual_sums }
	, actual_id_ { actual_id }
{
	// empty
}


std::unique_ptr<details::MatchPolicy> AlbumVerifier::Impl::do_create_order()
	const
{
	return std::make_unique<details::TrackOrderPolicy>();
}


const ARId& AlbumVerifier::Impl::do_actual_id() const noexcept
{
	return actual_id_;
}


// AlbumVerifier


AlbumVerifier::AlbumVerifier(const Checksums& actual_sums,
		const ARId& actual_id)
	: impl_      { std::make_unique<Impl>(actual_sums, actual_id) }
{
	// empty
}


AlbumVerifier::~AlbumVerifier() noexcept = default;


const ARId& AlbumVerifier::do_actual_id() const noexcept
{
	return impl_->actual_id();
}


const Checksums& AlbumVerifier::do_actual_checksums() const noexcept
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


// TracksetVerifier::Impl


TracksetVerifier::Impl::Impl(const Checksums& actual_sums)
	: details::VerifierBase { actual_sums }
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
	: impl_      { std::make_unique<Impl>(actual_sums) }
{
	// empty
}


TracksetVerifier::~TracksetVerifier() noexcept = default;


const ARId& TracksetVerifier::do_actual_id() const noexcept
{
	return impl_->actual_id();
}


const Checksums& TracksetVerifier::do_actual_checksums() const noexcept
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


} // namespace v_1_0_0
} // namespace arcstk

