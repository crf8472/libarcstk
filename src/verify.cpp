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

// ChecksumSource

std::string ChecksumSource::id(const int block_idx) const
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


std::string FromResponse::do_id(const int block_idx) const
{
	return source()->at(static_cast<ARResponse::size_type>(block_idx)).id()
			.to_string();
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


namespace details
{

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


// MatchOrder

MatchOrder::MatchOrder()
	: performer_ { nullptr }
{
	// empty
}


void MatchOrder::set_performer(MatchPerformer* const performer) noexcept
{
	performer_ = performer;
}


const MatchPerformer* MatchOrder::performer() const noexcept
{
	return performer_;
}


// TrackOder


void TrackOrder::do_perform(VerificationResult& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int current) const
{
	auto bitpos   = int  { 0 };
	auto is_v2    = bool { false };
	auto actual_checksum    = Checksum {};
	auto reference_checksum = Checksum {};

	for (auto track = std::size_t { 0 }; track < ref_sums.size(current); ++track)
	{
		for (const auto& type : DefaultPerformer::types)
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

			if (performer()->matches(actual_checksum, reference_checksum))
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


// Cartesian


void Cartesian::do_perform(VerificationResult& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int current) const
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
			for (const auto& type : DefaultPerformer::types)
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

				if (performer()->matches(actual_checksum,reference_checksum))
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


// MatchTraversal


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


void TraverseBlock::do_traverse(VerificationResult& match, const Checksums &actual_sums,
		const ARId &actual_id, const ChecksumSource& ref_sums,
		const MatchOrder& order) const
{
	// Validation is assumed to be already performed

	auto bitpos = int  { 0 };
	auto actual_checksum    = Checksum {};
	auto reference_checksum = Checksum {};

	for (auto block_i = decltype (ref_sums.size()) { 0 };
			block_i < ref_sums.size(); ++block_i)
	{
		ARCS_LOG_DEBUG << "Try to match block " << block_i
			<< " (" << block_i + 1 << "/" << ref_sums.size() << ")";

		if (actual_id.empty())
		{
			bitpos = match.verify_id(block_i);
			ARCS_LOG_DEBUG << "Accept and ignore empty actual id for: "
				<< match.id(block_i) << " (bit " << bitpos << ")";
		}
		else if (ref_sums.id(block_i) == actual_id.to_string())
		{
			bitpos = match.verify_id(block_i);
			ARCS_LOG_DEBUG << "Id verified: " << match.id(block_i)
				<< " (bit " << bitpos << ")";
		}
		else
		{
			ARCS_LOG_DEBUG << "Id: " << match.id(block_i)
				<< " not verified";
		}

		order.perform(match, actual_sums, ref_sums, *this, block_i);
	}
}


// DefaultPerformer


constexpr std::array<checksum::type, 2> DefaultPerformer::types;


std::unique_ptr<VerificationResult> DefaultPerformer::do_create_match_instance(
		const int blocks, const std::size_t tracks) const noexcept
{
	return details::create_result(blocks, tracks);
}


bool DefaultPerformer::do_matches(const ARId &actual, const ARId &reference) const
	noexcept
{
	return actual == reference;
}


bool DefaultPerformer::do_matches(const Checksum &actual,
		const Checksum &reference) const noexcept
{
	return actual == reference;
}


DefaultPerformer::DefaultPerformer(MatchTraversal* traversal, MatchOrder* order)
	: traversal_ { traversal }
	, order_     { order     }
{
	order_->set_performer(this);
}


const MatchTraversal* DefaultPerformer::traversal() const
{
	return traversal_;
}


const MatchOrder* DefaultPerformer::order() const
{
	return order_;
}


std::unique_ptr<VerificationResult> DefaultPerformer::operator() (
			const Checksums& actual_sums, const ARId& actual_id,
			const ChecksumSource& ref_sums) const
{
	auto match { create_match_instance(ref_sums.size(), actual_sums.size()) };

	traversal_->traverse(*match, actual_sums, actual_id, ref_sums, *order_);

	return match;
}

} // namespace details


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


// VerificationResult


VerificationResult::~VerificationResult() noexcept = default;

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

int VerificationResult::total_unmatched_tracks() const
{
	return do_total_unmatched_tracks();
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

std::unique_ptr<VerificationResult> VerificationResult::clone() const
{
	return do_clone();
}


// Result


class Result final : public VerificationResult
{
	int do_verify_id(const int b) final;
	bool do_id(const int b) const final;
	int do_verify_track(const int b, const int t, const bool v2) final;
	bool do_track(const int b, const int t, const bool v2) const final;
	int do_difference(const int b, const bool v2) const final;
	int do_total_unmatched_tracks() const final;
	int do_total_blocks() const final;
	int do_tracks_per_block() const final;
	size_t do_size() const final;
	std::unique_ptr<VerificationResult> do_clone() const final;

	details::ResultBits flags_;

public:

	Result();
	void init(const int blocks, const int tracks);
};


Result::Result()
	: flags_ { details::ResultBits() }
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


int Result::do_total_unmatched_tracks() const
{
	using size_type = details::ResultBits::size_type;

	std::vector<bool> tracks(static_cast<size_type>(tracks_per_block()), false);

	// Count every track whose checksum was not matched in any block.

	auto b = size_type { 0 };
	for (auto t = size_type { 0 }; t <
			static_cast<size_type>(tracks_per_block()); ++t)
	{
		for (b = size_type { 0 };
				b < static_cast<size_type>(total_blocks()); ++b)
		{
			if (track(b, t, true) or track(b, t, false))
			{
				tracks[t] = true;
				break;
			}
		}
	}

	return tracks_per_block() -
		std::accumulate(tracks.begin(), tracks.end(), 0);
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


std::unique_ptr<VerificationResult> Result::do_clone() const
{
	return nullptr; // FIXME
}


namespace details
{

std::unique_ptr<VerificationResult> create_result(const int blocks,
		const std::size_t tracks)
{
	auto r = std::make_unique<Result>();
	r->init(blocks, tracks);
	return r;
}

}


// Verifier::Impl


class Verifier::Impl final
{
public:

	bool all_tracks_match() const noexcept;
	bool is_matched(const int track) const noexcept;
	int total_unmatched_tracks() const noexcept;
	int best_block_difference() const noexcept;
	std::tuple<int, bool, int> best_block() const;

	/**
	 * \brief Returns the actual VerificationResult.
	 *
	 * \return The actual VerificationResult
	 */
	const VerificationResult* result() const noexcept;

	/**
	 * \brief Clones this instance.
	 *
	 * \return Deep copy of this instance
	 */
	std::unique_ptr<Verifier> clone() const noexcept;

private:

	std::unique_ptr<VerificationResult> result_;
};


bool Verifier::Impl::all_tracks_match() const noexcept
{
	return total_unmatched_tracks() == 0;
}

bool Verifier::Impl::is_matched(const int track) const noexcept
{
	// TRUE iff <track> is matched, otherwise FALSE

	//strict:
	auto t = best_block();
	return result_->track(std::get<0>(t), track, std::get<1>(t));

	// no strict:
	for (auto b = 0; b < result_->total_blocks(); ++b)
	{
		if (result_->track(b, track, true) or result_->track(b, track, false))
		{
			return true;
		}
	}
	return false;
}

int Verifier::Impl::total_unmatched_tracks() const noexcept
{
	// strict:
	return best_block_difference();

	// no-strict:
	return result_->total_unmatched_tracks();
}

int Verifier::Impl::best_block_difference() const noexcept
{
	const auto t = best_block();
	return result_->difference(std::get<0>(t), std::get<1>(t));
}

std::tuple<int, bool, int> Verifier::Impl::best_block() const
{
	const BestBlock best_block;
	return best_block(*result_.get());
}

const VerificationResult* Verifier::Impl::result() const noexcept
{
	return result_.get();
}

std::unique_ptr<Verifier> Verifier::Impl::clone() const noexcept
{
	//result_->clone()
	return nullptr; // FIXME
}


// Verifier


bool Verifier::all_tracks_match() const noexcept
{
	return impl_->all_tracks_match();
}

bool Verifier::is_matched(const int track) const noexcept
{
	return impl_->is_matched(track);
}

int Verifier::total_unmatched_tracks() const noexcept
{
	return impl_->total_unmatched_tracks();
}

int Verifier::best_block_difference() const noexcept
{
	return impl_->best_block_difference();
}

std::tuple<int, bool, int> Verifier::best_block() const
{
	return impl_->best_block();
}

const VerificationResult* Verifier::result() const noexcept
{
	return impl_->result();
}

std::unique_ptr<Verifier> Verifier::clone() const noexcept
{
	return impl_->clone(); // FIXME
}

} // namespace v_1_0_0

} // namespace arcstk

