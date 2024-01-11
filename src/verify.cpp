#ifndef __LIBARCSTK_VERIFY_HPP__
#include "verify.hpp"
#endif
#ifndef __LIBARCSTK_VERIFY_DETAILS_HPP__
#include "verify_details.hpp"
#endif

#include <iomanip>        // for operator<<, setw, setfill, hex, uppercase
#include <numeric>        // for accumulate

#ifndef __LIBARCSTK_PARSE_HPP__
#include "parse.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

namespace details
{

// BestBlock


constexpr int BestBlock::MAX_DIFFERENCE;

std::tuple<int, bool, int> BestBlock::operator()(
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

		curr_diff_v1 = result.difference(b, /* v1 */false);
		curr_diff_v2 = result.difference(b, /* v2 */true);

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
		return false;
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

	const auto offset { index(b) };
	set_flag(offset, value);

	return offset;
}


bool ResultBits::id(int b) const
{
	this->validate_block(b);

	return this->operator[](index(b));
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

	const auto end_block = index(b) + 1 + 2 * tracks_per_block();
	for (auto i = index(b) + 1; i <= end_block; ++i)
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


int ResultBits::index(int b) const
{
	//return b * (2 * tracks_per_block_ + 1);
	return b * flags_per_block();
}


int ResultBits::index(int b, int t, bool v2) const
{
	// b and t are 0-based
	return index(b) + //track_offset(t, v2);
		(t + 1 + (v2 ? tracks_per_block_ : 0));
}


int ResultBits::track_offset(int t, bool v2) const
{
	return t + 1 + (v2 ? tracks_per_block_ : 0);
}


void ResultBits::set_flag(const int offset, const bool value)
{
	auto pos = flag_.begin() + offset;
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


// Result


Result::Result(std::unique_ptr<TrackPolicy> p)
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
	const BestBlock best;
	return best(*this);
}


int Result::do_best_block_difference() const
{
	return std::get<2>(best_block());
}


const TrackPolicy* Result::do_policy() const
{
	return policy_.get();
}


std::unique_ptr<VerificationResult> Result::do_clone() const
{
	return nullptr; // FIXME
}


//


std::unique_ptr<VerificationResult> create_result(const int blocks,
		const std::size_t tracks, std::unique_ptr<TrackPolicy> p)
{
	auto r = std::make_unique<Result>(std::move(p));
	r->init(blocks, tracks);
	return r;
}


//


std::unique_ptr<VerificationResult> verify_impl(
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource &ref_sums,
		const MatchTraversal& t, const MatchOrder& o)
{
	auto r = create_result(ref_sums.size()/* total blocks */,
			actual_sums.size()/* total tracks per block */,
			t.get_policy());
	t.traverse(*r, actual_sums, actual_id, ref_sums, o);
	return r;
}


// StrictPolicy


bool StrictPolicy::do_is_verified(const int track, const VerificationResult& r)
	const
{
	const auto best { details::BestBlock() };
	const auto t = best(r);
	return r.track(std::get<0>(t), track, std::get<1>(t));
}


int StrictPolicy::do_total_unverified_tracks(const VerificationResult& r) const
{
	const auto best { details::BestBlock() };
	const auto t = best(r);
	return std::get<2>(t);
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


int LiberalPolicy::do_total_unverified_tracks(const VerificationResult& r) const
{
	using size_type = details::ResultBits::size_type;

	std::vector<bool> tracks(
			static_cast<size_type>(r.tracks_per_block()), false);

	// Count every track whose checksum was not matched in any block.

	auto b = size_type { 0 };
	for (auto t = size_type { 0 }; t <
			static_cast<size_type>(r.tracks_per_block()); ++t)
	{
		for (b = size_type { 0 };
				b < static_cast<size_type>(r.total_blocks()); ++b)
		{
			if (r.track(b, t, true) or r.track(b, t, false))
			{
				tracks[t] = true;
				break;
			}
		}
	}

	return r.tracks_per_block() -
		std::accumulate(tracks.begin(), tracks.end(), 0);
}


bool LiberalPolicy::do_is_strict() const
{
	return false;
}


// TraverseBlock


Checksum TraverseBlock::do_get_reference(const ChecksumSource& ref_sums,
		const int current, const int counter) const
{
	return ref_sums.checksum(current, counter);
}


std::size_t TraverseBlock::do_size(const ChecksumSource& ref_sums,
		const int current) const
{
	return ref_sums.size(current);
}


void TraverseBlock::do_traverse(VerificationResult& result,
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource& ref_sums,
		const MatchOrder& order) const
{
	// Validation is assumed to be already performed

	auto bitpos = int { 0 };
	auto actual_checksum    = Checksum {};
	auto reference_checksum = Checksum {};

	for (auto block_i = decltype (ref_sums.size()) { 0 };
			block_i < ref_sums.size(); ++block_i)
	{
		ARCS_LOG_DEBUG << "Try to match block " << block_i
			<< " (" << block_i + 1 << "/" << ref_sums.size() << ")";

		if (actual_id.empty())
		{
			bitpos = result.verify_id(block_i);
			ARCS_LOG_DEBUG << "Accept and ignore empty actual id for: "
				<< result.id(block_i) << " (bit " << bitpos << ")";
		}
		else if (ref_sums.id(block_i) == actual_id)
		{
			bitpos = result.verify_id(block_i);
			ARCS_LOG_DEBUG << "Id verified: " << result.id(block_i)
				<< " (bit " << bitpos << ")";
		}
		else
		{
			ARCS_LOG_DEBUG << "Id: " << result.id(block_i)
				<< " not verified";
		}

		order.perform(result, actual_sums, ref_sums, block_i, *this);
	}
}


std::unique_ptr<TrackPolicy> TraverseBlock::do_get_policy() const
{
	return std::make_unique<StrictPolicy>();
}


// TODO TraverseTrack


// TrackOder


void TrackOrder::do_perform(VerificationResult& match,
		const Checksums& actual_sums,
		const ChecksumSource& ref_sums, int current,
		const MatchTraversal& t) const
{
	auto bitpos   = int  { 0 };
	auto is_v2    = bool { false };
	auto actual_checksum    = Checksum {};
	auto reference_checksum = Checksum {};

	for (auto track = std::size_t { 0 }; track < ref_sums.size(current); ++track)
	{
		for (const auto& type : supported_checksum_types)
		{
			is_v2 = (type == checksum::type::ARCS2);

			actual_checksum    = actual_sums[track].get(type);
			reference_checksum = Checksum {
				t.get_reference(ref_sums, current, track) };

			ARCS_LOG(DEBUG1) << "Check track "
				<< std::setw(2) << std::setfill('0') << (track + 1)
				<< ": "
				<< actual_checksum
				<< " to match "
				<< reference_checksum
				<< " (v" << (is_v2 ? "2" : "1") << ") ";

			if (actual_checksum == reference_checksum)
			{
				bitpos = match.verify_track(current, track, is_v2);

				ARCS_LOG_DEBUG << "Track "
					<< std::setw(2) << std::setfill('0') << (track + 1)
					<< " v" << (is_v2 ? "2" : "1") << " verified: "
					<< match.track(current, track, is_v2)
					<< " (bit " << bitpos << ")";
			}
			else
			{
				ARCS_LOG_DEBUG << "Track "
					<< std::setw(2) << std::setfill('0') << (track + 1)
					<< " v" << (is_v2 ? "2" : "1") << " not verified: "
					<< match.track(current, track, is_v2);
			}
		} // for type
	} // for track
}


// UnknownOrder


void UnknownOrder::do_perform(VerificationResult& match,
		const Checksums& actual_sums,
		const ChecksumSource& ref_sums, int current,
		const MatchTraversal& t) const
{
	auto bitpos   = int  { 0 };
	auto is_v2    = bool { false };
	auto actual_checksum    = Checksum {};
	auto reference_checksum = Checksum {};

	auto start_track = Checksums::size_type { 0 };

	for (auto track = std::size_t { 0 };
			track < ref_sums.size(current) && start_track < actual_sums.size();
			++track)
	{
		ARCS_LOG_DEBUG << "Track " << (track + 1);

		for (const auto& actual_track : actual_sums)
		{
			for (const auto& type : supported_checksum_types)
			{
				is_v2 = (type == checksum::type::ARCS2);

				actual_checksum    = actual_track.get(type);
				reference_checksum = Checksum {
					t.get_reference(ref_sums, current, track) };

				ARCS_LOG(DEBUG1) << "Check track "
					<< std::setw(2) << std::setfill('0') << (track + 1)
					<< ": "
					<< actual_checksum
					<< " to match "
					<< reference_checksum
					<< " (v" << (is_v2 ? "2" : "1") << ") ";

				if (actual_checksum == reference_checksum)
				{
					bitpos = match.verify_track(current, track, is_v2);

					ARCS_LOG_DEBUG << "  >Track "
						<< std::setw(2) << std::setfill('0')
						<< (track + 1)
						<< " v" << (is_v2 ? "2" : "1") << " verified: "
						<< match.track(current, track, is_v2)
						<< " (bit " << bitpos << ")"
						<< " matches tracklist pos " << track;

					++start_track;
					break;
				} else
				{
					ARCS_LOG_DEBUG << "Track "
						<< std::setw(2) << std::setfill('0')
						<< (track + 1)
						<< " v" << (is_v2 ? "2" : "1") << " not verified: "
						<< match.track(current, track, is_v2);
				}
			} // for type
		} // for actual track
	} // for ref track
}

} // namespace details


// ChecksumSource

ARId ChecksumSource::id(const int block_idx) const
{
	return this->do_id(block_idx);
}

Checksum ChecksumSource::checksum(const int block_idx, const int idx) const
{
	return this->do_checksum(block_idx, idx);
}

int ChecksumSource::confidence(const int block_idx, const int idx) const
{
	return this->do_confidence(block_idx, idx);
}

std::size_t ChecksumSource::size(const int block_idx) const
{
	return this->do_size(block_idx);
}

std::size_t ChecksumSource::size() const
{
	return this->do_size();
}


// FromResponse


ARId FromResponse::do_id(const int block_idx) const
{
	return source()->at(static_cast<ARResponse::size_type>(block_idx)).id();
}


Checksum FromResponse::do_checksum(const int block_idx, const int idx) const
{
	return
		source()->at(static_cast<ARResponse::size_type>(block_idx))
			.at(static_cast<ARResponse::size_type>(idx)).arcs();
}


int FromResponse::do_confidence(const int block_idx, const int idx) const
{
	return
		static_cast<int>(
			source()->at(static_cast<ARResponse::size_type>(block_idx))
				.at(static_cast<ARResponse::size_type>(idx)).confidence());
}


std::size_t FromResponse::do_size(const int /* block_idx */) const
{
	return static_cast<std::size_t>(source()->tracks_per_block());
}


std::size_t FromResponse::do_size() const
{
	return source()->size();
}


// TrackPolicy


bool TrackPolicy::is_verified(const int track,
		const VerificationResult& r) const
{
	return do_is_verified(track, r);
}


int TrackPolicy::total_unverified_tracks(const VerificationResult& r)
	const
{
	return do_total_unverified_tracks(r);
}


bool TrackPolicy::is_strict() const
{
	return do_is_strict();
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


const TrackPolicy* VerificationResult::policy() const
{
	return do_policy();
}


std::unique_ptr<VerificationResult> VerificationResult::clone() const
{
	return do_clone();
}


// MatchTraversal


Checksum MatchTraversal::get_reference(const ChecksumSource& ref_sums,
		const int current, const int counter) const
{
	return do_get_reference(ref_sums, current, counter);
}


std::size_t MatchTraversal::size(const ChecksumSource& ref_sums,
		const int current) const
{
	return do_size(ref_sums, current);
}


void MatchTraversal::traverse(VerificationResult& result,
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource& ref_sums,
		const MatchOrder& order) const
{
	do_traverse(result, actual_sums, actual_id, ref_sums, order);
}


std::unique_ptr<TrackPolicy> MatchTraversal::get_policy() const
{
	return do_get_policy();
}


// MatchOrder


void MatchOrder::perform(VerificationResult& result,
		const Checksums& actual_sums, const ChecksumSource& ref_sums,
		int index, const MatchTraversal& t) const
{
	do_perform(result, actual_sums, ref_sums, index, t);
}


// verify


std::unique_ptr<VerificationResult> verify(
		const Checksums& actual_sums, const ARId& actual_id,
		const ChecksumSource& ref_sums,
		const MatchTraversal& t, const MatchOrder& o)
{
	return details::verify_impl(actual_sums, actual_id, ref_sums, t, o);
}


// Verifier


Verifier::Verifier(const Checksums& actual_sums)
	: actual_sums_ { actual_sums }
	, is_strict_   { true        }
{
	// empty
}


void Verifier::set_strict(const bool strict)
{
	is_strict_ = strict;
}


bool Verifier::strict() const
{
	return is_strict_;
}


const Checksums& Verifier::actual_checksums() const
{
	return actual_sums_;
}


const ARId& Verifier::actual_id() const
{
	return do_actual_id();
}


std::unique_ptr<MatchTraversal> Verifier::do_create_traversal() const
{
	if (strict())
	{
		return std::make_unique<details::TraverseBlock>();
	}

	return nullptr; // FIXME
}


std::unique_ptr<VerificationResult> Verifier::perform(
		const ChecksumSource& ref_sums) const
{
	const auto t = do_create_traversal();
	const auto o = do_create_order();
	return verify(actual_checksums(), actual_id(), ref_sums, *t, *o);
}


std::unique_ptr<VerificationResult> Verifier::perform(
		const ARResponse& ref_sums) const
{
	return perform(FromResponse(&ref_sums));
}


const ARId& Verifier::do_actual_id() const
{
	return EmptyARId;
}


// AlbumVerifier


AlbumVerifier::AlbumVerifier(const Checksums& actual_sums, const ARId& actual_id)
	: Verifier   { actual_sums }
	, actual_id_ { actual_id   }
{
	// empty
}


std::unique_ptr<MatchOrder> AlbumVerifier::do_create_order() const
{
	return std::make_unique<details::TrackOrder>();
}


const ARId& AlbumVerifier::do_actual_id() const
{
	return actual_id_;
}


// TracksetVerifier


TracksetVerifier::TracksetVerifier(const Checksums& actual_sums)
	: Verifier { actual_sums }
{
	// empty
}


std::unique_ptr<MatchOrder> TracksetVerifier::do_create_order() const
{
	return std::make_unique<details::UnknownOrder>();
}


} // namespace v_1_0_0

} // namespace arcstk

