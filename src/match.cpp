/**
 * \file match.cpp Implementation of a low-level API to ARCSs comparison
 */


#ifndef __LIBARCS_MATCH_HPP__
#include "match.hpp"
#endif

#include <cstdint>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef __LIBARCS_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCS_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCS_PARSE_HPP__
#include "parse.hpp"
#endif
#ifndef __LIBARCS_LOGGING_HPP__
#include "logging.hpp"
#endif


namespace arcs
{

/// \internal \defgroup matchImpl Implementation of ARCSs comparison
/// \ingroup match
/// @{

inline namespace v_1_0_0
{
// unnamed namespace for declarations only used in .cpp-file
namespace {

/// \cond IMPL_ONLY

/**
 * \brief Default implementation of a Match.
 *
 * The result is encoded as a sequence of boolean flags, each representing the
 * result of a match operation between either two ARCSs or two <tt>ARId</tt>s in
 * the order of there occurrence in the ARResponse.
 */
class DefaultMatch final : public Match
{

public:

	/**
	 * Default constructor
	 *
	 * \param[in] blocks Number of <tt>ARBlock</tt>s to represent
	 * \param[in] tracks Number of tracks per block
	 */
	DefaultMatch(const uint32_t blocks, const uint8_t tracks);

	uint32_t verify_id(const uint32_t b) override;

	bool id(const uint32_t b) const override;

	uint32_t verify_track(const uint32_t b, const uint8_t t, const bool v2)
		override;

	bool track(const uint32_t b, const uint8_t t, const bool v2) const override;

	uint32_t difference(const uint32_t b, const bool v2) const override;

	int total_blocks() const override;

	int tracks_per_block() const override;

	size_t size() const override;

	std::unique_ptr<Match> clone() const override;


protected:

	/**
	 * Converts a logical block index to an absolute single flag position.
	 *
	 * Note that the block start is also the position of the flag respresenting
	 * the ARId comparison.
	 *
	 * \param[in] b  0-based index of the block in \c response
	 *
	 * \return Flag index for the logical block \c b
	 */
	uint32_t block_start(const uint32_t b) const;

	/**
	 * Converts a 0-based track number to an offset position within a block.
	 *
	 * \param[in] t  0-based index of the track in \c response
	 * \param[in] v2 Iff TRUE, the offset of the ARCSv2 is returned
	 *
	 * \return Offset for the flag index to be added to the start of the block
	 */
	uint32_t track_offset(const uint8_t t, const bool v2) const;

	/**
	 * Converts a logical ARCS position in \c response to an absolute single
	 * flag position.
	 *
	 * The sum of \c block_start(b) and \c track_offset(t, v2).
	 *
	 * \param[in] b  0-based index of the block in \c response
	 * \param[in] t  0-based index of the track in \c response
	 * \param[in] v2 Iff TRUE, the offset of the ARCSv2 is returned
	 *
	 * \return Flag index for a single ARCS
	 */
	uint32_t index(const uint32_t b, const int8_t t, const bool v2) const;

	/**
	 * Set the verification flag for the ARId of block \b to \c value .
	 *
	 * \param[in] b     0-based index of the block in \c response
	 * \param[in] value New value for this flag
	 *
	 * \return Absolute index position to be set
	 *
	 * \throws Iff \c b is out of range
	 */
	uint32_t set_id(const uint32_t b, bool value);

	/**
	 * Set the verification flag for the ARCS specified by \c b, \c t and
	 * \c v2 .
	 *
	 * \param[in] b     0-based index of the block in \c response
	 * \param[in] t     0-based index of the track in \c response
	 * \param[in] v2    Iff TRUE, the offset of the ARCSv2 is set
	 * \param[in] value New value for this flag
	 *
	 * \return Absolute index position to be set
	 *
	 * \throws Iff \c b or \c t are out of range
	 */
	uint32_t set_track(const uint32_t b, const uint8_t t, const bool v2,
			bool value);

	/**
	 * Ensures that \c b is a legal block value.
	 *
	 * \param[in] b 0-based index of the block in \c response
	 *
	 * \throws Iff \c b is out of range
	 */
	void validate_block(uint32_t b) const;

	/**
	 * Ensures that \c t is a legal track value.
	 *
	 * \param[in] t 0-based index of the track in \c response
	 *
	 * \throws Iff \c t is out of range
	 */
	void validate_track(int t) const;


private:

	/**
	 * Number of <tt>ARBlock</tt>s represented
	 */
	uint32_t blocks_;

	/**
	 * Number of tracks in each ARBlock
	 */
	int tracks_per_block_;

	/**
	 * Number of flags stored.
	 */
	uint32_t size_;

	/**
	 * The result bits of the comparison.
	 */
	std::vector<bool> flag_;
	// layout is:
	// id(1),v1,v1,v1, ... ,v2,v2,v2,id(2),v1,v1,v1, ... ,v2,v2,v2
	// with 1 + t * v1 + t * v2 flags in each block
	// 1 == equal to corresponding value in response, 0 == different

	// Commented out: bitset is copy assignable, so you can modify its size
	//const size_t total_bits = sizeof(long) * 8;
	//using field = std::bitset<99>;
	//field f(total_bits);
	//f = field(total_bits - 1);
};

/// \endcond
// IMPL_ONLY

} // namespace


DefaultMatch::DefaultMatch(const uint32_t blocks, const uint8_t tracks)
	: blocks_(blocks)
	, tracks_per_block_(tracks)
	, size_(blocks * (2 * tracks + 1))
	, flag_(size_, false)
{
	// empty
}


uint32_t DefaultMatch::verify_id(const uint32_t b)
{
	return this->set_id(b, true);
}


bool DefaultMatch::id(const uint32_t b) const
{
	this->validate_block(b);

	const uint32_t i = block_start(b);

	if (i > this->size() - 1)
	{
		throw std::runtime_error("Block index too big");
	}

	return flag_[i];
}


uint32_t DefaultMatch::verify_track(const uint32_t b, const uint8_t t,
		const bool v2)
{
	return this->set_track(b, t, v2, true);
}


bool DefaultMatch::track(const uint32_t b, const uint8_t t, const bool v2)
	const
{
	this->validate_block(b);
	this->validate_track(t);

	const uint32_t i = index(b, t, v2);

	if ( i > this->size() - 1)
	{
		throw std::runtime_error("Accessor too big");
	}

	return flag_[i];
}


uint32_t DefaultMatch::difference(const uint32_t b, const bool v2) const
{
	this->validate_block(b);

	uint32_t difference = (id(b) ? 0 : 1u);

	for (int trk = 0; trk < tracks_per_block_; ++trk)
	{
		difference += ( track(b, trk, v2) ? 0 : 1 );
	}

	return difference;
}


int DefaultMatch::total_blocks() const
{
	return blocks_;
}


int DefaultMatch::tracks_per_block() const
{
	return tracks_per_block_;
}


size_t DefaultMatch::size() const
{
	return size_;
}


std::unique_ptr<Match> DefaultMatch::clone() const
{
	return std::make_unique<DefaultMatch>(*this);
}


uint32_t DefaultMatch::block_start(const uint32_t b) const
{
	return b * (2 * tracks_per_block_ + 1);
}


uint32_t DefaultMatch::track_offset(const uint8_t t, const bool v2) const
{
	return t + 1 + (v2 ? tracks_per_block_ : 0);
}


uint32_t DefaultMatch::index(const uint32_t b, const int8_t t,
		const bool v2) const
{
	// b and t are 0-based
	return block_start(b) + track_offset(t, v2);
}


uint32_t DefaultMatch::set_id(const uint32_t b, bool value)
{
	this->validate_block(b);

	const auto offset = block_start(b);

	if (offset > this->size() - 1)
	{
		throw std::runtime_error("Block index too big");
	}

	flag_.emplace(flag_.begin() + offset, value);
	return block_start(b);
}


uint32_t DefaultMatch::set_track(const uint32_t b, const uint8_t t,
		const bool v2, const bool value)
{
	this->validate_block(b);
	this->validate_track(t);

	const uint32_t i = index(b, t, v2);

	if (i > this->size() - 1)
	{
		throw std::runtime_error("Accessor too big");
	}

	flag_.emplace(flag_.begin() + i, value);
	return i;
}


void DefaultMatch::validate_block(uint32_t b) const
{
	if (b > blocks_ - 1)
	{
		throw std::runtime_error("Block index too big");
	}
}


void DefaultMatch::validate_track(int t) const
{
	if (t > tracks_per_block_ - 1)
	{
		throw std::runtime_error("Track index too big");
	}
}

/// \cond IMPL_ONLY

/**
 * \brief Abstract base class for matcher implementations.
 */
class BaseMatcherImpl
{

public:

	/**
	 * Default constructor
	 */
	BaseMatcherImpl();

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	BaseMatcherImpl(const BaseMatcherImpl &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	BaseMatcherImpl(BaseMatcherImpl &&rhs) noexcept;

	/**
	 * Initializes the match
	 *
	 * \param[in] checksums Checksums to be matched
	 * \param[in] id        ARId to be matched
	 * \param[in] response  Response data to match
	 */
	void init_match(const Checksums &checksums, const ARId &id,
		const ARResponse &response);

	/**
	 * Implements Matcher::matches() const
	 */
	bool matches() const;

	/**
	 * Implements Matcher::best_match() const
	 */
	uint32_t best_match() const;

	/**
	 * Implements Matcher::best_difference() const
	 */
	int best_difference() const;

	/**
	 * Implements Matcher::matches_v2() const
	 */
	bool matches_v2() const;

	/**
	 * Copy assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	BaseMatcherImpl& operator = (const BaseMatcherImpl &rhs);

	/**
	 * Move assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	BaseMatcherImpl& operator = (BaseMatcherImpl &&rhs) noexcept;

	/**
	 * Returns the actual match result.
	 *
	 * \return Actual match result.
	 */
	const DefaultMatch * match() const;


protected:

	/**
	 * Default destructor
	 */
	~BaseMatcherImpl() noexcept;

	/**
	 * Performs the actual match.
	 *
	 * \param[in] actual_sums The checksums to match
	 * \param[in] id          The id to match
	 * \param[in] ref_sums    The reference checksums to be matched
	 *
	 * \return Match information
	 */
	virtual std::unique_ptr<DefaultMatch> do_match(const Checksums &actual_sums,
			const ARId &id, const ARResponse &ref_sums) const
	= 0;

	/**
	 * Derive best matching block from match result.
	 *
	 * \param[in]  m          Match to analyze
	 *
	 * \param[out] block      Best matching block found
	 * \param[out] matches_v2 TRUE iff best match is for ARCSv2, otherwise FALSE
	 *
	 * \return Status value, 0 indicates success
	 */
	int best_block(const DefaultMatch &m, uint32_t &block,
			bool &matches_v2);

	/**
	 * Internal service method for constructor
	 */
	int mark_best_block();


private:

	/**
	 * State: representation of the comparison result
	 */
	std::unique_ptr<DefaultMatch> match_;

	/**
	 * State: pointer to best block
	 */
	uint32_t best_block_;

	/**
	 * State: stores information about best block
	 */
	bool matches_v2_;
};


/// \endcond
// IMPL_ONLY


BaseMatcherImpl::BaseMatcherImpl()
	: match_(nullptr)
	, best_block_(0)
	, matches_v2_(false)
{
	// empty
}


BaseMatcherImpl::BaseMatcherImpl(const BaseMatcherImpl &rhs)
	: match_(std::make_unique<DefaultMatch>(*rhs.match_))  // deep copy
	, best_block_(rhs.best_block_)
	, matches_v2_(rhs.matches_v2_)
{
	// empty
}


BaseMatcherImpl::BaseMatcherImpl(BaseMatcherImpl &&rhs) noexcept
	: match_(std::move(rhs.match_))
	, best_block_(rhs.best_block_)
	, matches_v2_(rhs.matches_v2_)
{
	// empty
}


BaseMatcherImpl::~BaseMatcherImpl() noexcept = default;


void BaseMatcherImpl::init_match(const Checksums &checksums, const ARId &id,
		const ARResponse &response)
{
	if (checksums.size() == 0)
	{
		return;
	}

	if (response.size() == 0)
	{
		return;
	}

	match_ = this->do_match(checksums, id, response);

	this->mark_best_block();
}


bool BaseMatcherImpl::matches() const
{
	return this->best_difference() == 0;
}


uint32_t BaseMatcherImpl::best_match() const
{
	return best_block_;
}


int BaseMatcherImpl::best_difference() const
{
	return match_->difference(best_block_, matches_v2_);
}


bool BaseMatcherImpl::matches_v2() const
{
	return matches_v2_;
}


int BaseMatcherImpl::best_block(const DefaultMatch &m,
		uint32_t &block, bool &matches_v2)
{
	if (m.size() == 0)
	{
		return -1;
	}

	auto best_diff    = 100; // maximal difference possible, 99 tracks + id
	int  curr_diff_v1 = 0;
	int  curr_diff_v2 = 0;

	for (int b = 0; b < m.total_blocks(); ++b)
	{
		// Note: v2 matching will always be preferred over v1 matching

		ARCS_LOG_DEBUG << "Check block " << b;

		curr_diff_v1 = m.difference(b, /* v1 */false);
		curr_diff_v2 = m.difference(b, /* v2 */true);

		// Note the less-equal for v2: last match wins!
		if (curr_diff_v2 <= best_diff or curr_diff_v1 < best_diff)
		{
			block      = b;
			matches_v2 = curr_diff_v2 <= curr_diff_v1;
			best_diff  = matches_v2 ? curr_diff_v2 : curr_diff_v1;

			ARCS_LOG_DEBUG << "Set block " << b << " ("
				<< (matches_v2+1) << ") as best";
		}
	}

	return 0;
}


BaseMatcherImpl& BaseMatcherImpl::operator = (
		const BaseMatcherImpl &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	match_ = std::make_unique<DefaultMatch>(*rhs.match_);
	return *this;
}


BaseMatcherImpl& BaseMatcherImpl::operator = (
		BaseMatcherImpl &&rhs) noexcept = default;


const DefaultMatch * BaseMatcherImpl::match() const
{
	return match_.get();
}


int BaseMatcherImpl::mark_best_block()
{
	uint32_t block   = 0;
	bool     version = false;

	auto status = this->best_block(*match_, block, version);

	if (status == 0) // best block found?
	{
		best_block_ = block;
		matches_v2_ = version;

		ARCS_LOG(LOG_DEBUG1) << "Best block: " << best_block_;
		ARCS_LOG(LOG_DEBUG1) << "Match:      "
			<< (matches_v2_ ? std::string("v2") : std::string("v1"));
	}

	return status;
}


/// \cond IMPL_ONLY


/**
 * \brief Implementation of ListMatcher.
 */
class ListMatcher::Impl final : public BaseMatcherImpl
{

protected:

	std::unique_ptr<DefaultMatch> do_match(const Checksums &actual_sums,
			const ARId &id, const ARResponse &ref_sums) const override;
};


/// \endcond
// IMPL_ONLY


std::unique_ptr<DefaultMatch> ListMatcher::Impl::do_match(
		const Checksums &actual_sums, const ARId &id,
		const ARResponse &ref_sums) const
{
	uint64_t ref_tracks = ref_sums.tracks_per_block() < 0
		? 0
		: ref_sums.tracks_per_block(); // TODO Better way to compare

	if (actual_sums.size() != ref_tracks)
	{
		ARCS_LOG_ERROR << "No match possible."
			<< " Number of tracks in actual_sums (" << actual_sums.size()
			<< ") is different from number of tracks in ref_sums ("
			<< ref_tracks << ")";

		return nullptr;
	}

	auto match = std::make_unique<DefaultMatch>(
			ref_sums.size(), actual_sums.size());

	int block_i { 0 };
	int track_j { 0 };
	uint32_t bitpos = 0;
	Checksum checksum;

	for (auto block = ref_sums.begin(); block != ref_sums.end(); ++block)
	{
		ARCS_LOG_DEBUG << "Try to match block " << block_i
			<< " (" << block_i + 1 << "/" << ref_sums.size() << ")";

		if (block->id() == id)
		{
			bitpos = match->verify_id(block_i);
			ARCS_LOG_DEBUG << "Id verified: " << match->id(block_i)
				<< " (bit " << bitpos << ")";
		}
		else
		{
			ARCS_LOG_DEBUG << "Id: " << match->id(block_i)
				<< " not verified";
		}

		track_j = 0;

		for (auto track = block->begin(); track != block->end(); ++track)
		{
			checksum = *actual_sums[track_j].find(checksum::type::ARCS1);

			if (track->arcs() == checksum.value())
			{
				bitpos = match->verify_track(block_i, track_j, false);

				ARCS_LOG_DEBUG << "Track "
					<< std::setw(2) << std::setfill('0') << track_j + 1
					<< " v1 verified: " << match->track(block_i, track_j, false)
					<< " (bit " << bitpos << ")";
			}
			else
			{
				ARCS_LOG_DEBUG << "Track "
					<< std::setw(2) << std::setfill('0') << track_j + 1
					<< " v1 not verified: "
					<< match->track(block_i, track_j, false);
			}

			checksum = *actual_sums[track_j].find(checksum::type::ARCS2);

			if (track->arcs() == checksum.value())
			{
				bitpos = match->verify_track(block_i, track_j, true);

				ARCS_LOG_DEBUG << "Track "
				<< std::setw(2) << std::setfill('0') << track_j + 1
				<< " v2 verified: " << match->track(block_i, track_j, true)
				<< " (bit " << bitpos << ")";
			}
			else
			{
				ARCS_LOG_DEBUG << "Track "
					<< std::setw(2) << std::setfill('0') << track_j + 1
					<< " v2 not verified: "
					<< match->track(block_i, track_j, true);
			}

			++track_j;
		}

		++block_i;
	}

	return match;
}


// ListMatcher


ListMatcher::ListMatcher(const Checksums &checksums, const ARId &id,
		const ARResponse &response)
	: impl_(std::make_unique<ListMatcher::Impl>())
{
	impl_->init_match(checksums, id, response);
}


ListMatcher::ListMatcher(const ListMatcher &rhs)
	: impl_(std::make_unique<ListMatcher::Impl>(*rhs.impl_))
{
	// empty
}


ListMatcher::ListMatcher(ListMatcher &&rhs) noexcept = default;


ListMatcher::~ListMatcher() noexcept = default;


bool ListMatcher::matches() const
{
	return impl_->matches();
}


uint32_t ListMatcher::best_match() const
{
	return impl_->best_match();
}


int ListMatcher::best_difference() const
{
	return impl_->best_difference();
}


bool ListMatcher::matches_v2() const
{
	return impl_->matches_v2();
}


ListMatcher& ListMatcher::operator = (const ListMatcher &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	impl_ = std::make_unique<ListMatcher::Impl>(*rhs.impl_);
	return *this;
}


ListMatcher& ListMatcher::operator = (ListMatcher &&rhs) noexcept
	= default;


const Match * ListMatcher::match() const
{
	return impl_->match();
}


std::unique_ptr<Matcher> ListMatcher::clone() const
{
	return std::make_unique<ListMatcher>(*this);
}


/// \cond IMPL_ONLY


/**
 * \brief Implementation of AnyMatcher.
 */
class AnyMatcher::Impl final : public BaseMatcherImpl
{

protected:

	std::unique_ptr<DefaultMatch> do_match(const Checksums &actual_sums,
			const ARId &id, const ARResponse &ref_sums) const override;
};


/// \endcond
// IMPL_ONLY


// AnyMatcher::Impl


std::unique_ptr<DefaultMatch> AnyMatcher::Impl::do_match(
		const Checksums &actual_sums, const ARId & /*id*/,
		const ARResponse &ref_sums) const
{
	uint64_t ref_tracks = ref_sums.tracks_per_block() < 0
		? 0
		: ref_sums.tracks_per_block(); // TODO Better way to compare

	if (actual_sums.size() != ref_tracks)
	{
		ARCS_LOG_ERROR << "No match possible."
			<< " Number of tracks in actual_sums (" << actual_sums.size()
			<< ") is different from number of tracks in ref_sums ("
			<< ref_tracks << ")";

		return nullptr;
	}

	auto match = std::make_unique<DefaultMatch>(
			ref_sums.size(), actual_sums.size());

	int block_i { 0 };
	int track_j { 0 };
	uint32_t bitpos      { 0 };
	uint32_t start_track { 0 };

	Checksum checksum;

	for (auto block = ref_sums.begin(); block != ref_sums.end(); ++block)
	{
		ARCS_LOG_DEBUG << "Try to match block " << block_i
			<< " (" << block_i + 1 << "/" << ref_sums.size() << ")";

		track_j = 0;
		start_track = 0;

		for (auto track = block->begin();
				track != block->end() and start_track < actual_sums.size();
				++track)
		{
			ARCS_LOG_DEBUG << "Track " << (track_j + 1);

			for (const auto& entry : actual_sums)
			{
				checksum = *entry.find(checksum::type::ARCS2);

				ARCS_LOG(LOG_DEBUG1) << "Check track " << (track_j + 1) << ": "
					<< std::hex
					<< std::setw(8) << std::setfill('0') << std::uppercase
					<< track->arcs()
					<< " to (v2) "
					<< std::hex
					<< std::setw(8) << std::setfill('0') << std::uppercase
					<< checksum.value();

				if (track->arcs() == checksum.value())
				{
					bitpos = match->verify_track(block_i, track_j, true);

					ARCS_LOG_DEBUG << "  >Track "
						<< std::setw(2) << (track_j + 1)
						<< " v2 verified: "
						<< match->track(block_i, track_j, true)
						<< " (bit " << bitpos << ")"
						<< " matches tracklist pos " << track_j;

					++start_track;
					break;
				}

				checksum = *entry.find(checksum::type::ARCS1);

				ARCS_LOG(LOG_DEBUG1) << "Check track " << (track_j + 1) << ": "
					<< std::hex
					<< std::setw(8) << std::setfill('0') << std::uppercase
					<< track->arcs()
					<< " to (v1) "
					<< std::hex
					<< std::setw(8) << std::setfill('0') << std::uppercase
					<< checksum.value();

				if (track->arcs() == checksum.value())
				{
					bitpos = match->verify_track(block_i, track_j, false);

					ARCS_LOG_DEBUG << "  >Track "
						<< std::setw(2) << (track_j + 1)
						<< " v1 verified: "
						<< match->track(block_i, track_j, false)
						<< " (bit " << bitpos << ")"
						<< " matches tracklist pos " << track_j;

					++start_track;
				}
			}

			++track_j;
		}

		++block_i;
	}

// Print match: Just for debugging
//
//	for (int b = 0; b < response.size(); ++b)
//	{
//		for (int t = 0; t < response.tracks_per_block(); ++t)
//		{
//			for (int v = 0; v < 2; ++v)
//			{
//				ARCS_LOG_DEBUG << "B " << b << " T " << (t+1) << " v" << (v+1)
//					<< ": " << match->track(b, t, v);
//			}
//		}
//	}

	return match;
}


// AnyMatcher


AnyMatcher::AnyMatcher(const Checksums &checksums, const ARResponse &response)
	: impl_(std::make_unique<AnyMatcher::Impl>())
{
	impl_->init_match(checksums, ARId(0, 0, 0, 0), response);
	// ARId is skipped, so we spare the effort to standard-create an empty ARId
}


AnyMatcher::AnyMatcher(const AnyMatcher &rhs)
	: impl_(std::make_unique<AnyMatcher::Impl>(*rhs.impl_))
{
	// empty
}


AnyMatcher::AnyMatcher(AnyMatcher &&rhs) noexcept = default;


AnyMatcher::~AnyMatcher() noexcept = default;


bool AnyMatcher::matches() const
{
	return impl_->matches();
}


uint32_t AnyMatcher::best_match() const
{
	return impl_->best_match();
}


int AnyMatcher::best_difference() const
{
	return impl_->best_difference();
}


bool AnyMatcher::matches_v2() const
{
	return impl_->matches_v2();
}


AnyMatcher& AnyMatcher::operator = (const AnyMatcher &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	impl_ = std::make_unique<AnyMatcher::Impl>(*rhs.impl_);
	return *this;
}


AnyMatcher& AnyMatcher::operator = (AnyMatcher &&rhs) noexcept
	= default;


const Match * AnyMatcher::match() const
{
	return impl_->match();
}


std::unique_ptr<Matcher> AnyMatcher::clone() const
{
	return std::make_unique<AnyMatcher>(*this);
}

} // namespace v_1_0_0

/// @}

} // namespace arcs

