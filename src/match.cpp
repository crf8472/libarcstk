/**
 * \file
 *
 * \brief Implementation of the AccurateRip checksum matching API.
 */

#ifndef __LIBARCSTK_MATCH_HPP__
#include "match.hpp"
#endif

#include <cstdint>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
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

/**
 * \internal \defgroup matchImpl Implementation
 * \ingroup match
 * @{
 */

// Match

/// \cond UNDOC_FUNCTION_BODIES

Match::~Match() noexcept = default;


int Match::verify_id(int b)
{
	return this->do_verify_id(b);
}


bool Match::id(int b) const
{
	return this->do_id(b);
}


int Match::verify_track(int b, int t, bool v2)
{
	return this->do_verify_track(b, t, v2);
}


bool Match::track(int b, int t, bool v2) const
{
	return this->do_track(b, t, v2);
}


int64_t Match::difference(int b, bool v2) const
{
	return this->do_difference(b, v2);
}


int Match::total_blocks() const
{
	return this->do_total_blocks();
}


int Match::tracks_per_block() const
{
	return this->do_tracks_per_block();
}


size_t Match::size() const
{
	return this->do_size();
}


std::unique_ptr<Match> Match::clone() const
{
	return this->do_clone();
}

/// \endcond

/**
 * \brief Base for @link DefaultMatch DefaultMatches @endlink.
 */
class DefaultMatchBase : public Match
{
public:

	using size_type = std::size_t;

public:

	/**
	 * \brief Default constructor.
	 *
	 * \param[in] blocks Number of @link ARBlock ARBlocks @endlink to represent
	 * \param[in] tracks Number of tracks per block
	 */
	DefaultMatchBase(int blocks, int tracks);

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~DefaultMatchBase() noexcept;


	int do_verify_id(int b) override;

	bool do_id(int b) const override;

	int do_verify_track(int b, int t, bool v2) override;

	bool do_track(int b, int t, bool v2) const override;

	int64_t do_difference(int b, bool v2) const override;

	int do_total_blocks() const override;

	int do_tracks_per_block() const override;

	size_t do_size() const override;


protected:

	/**
	 * \brief Converts a logical block index to an absolute single flag
	 * position.
	 *
	 * Note that the block start is also the position of the flag respresenting
	 * the ARId comparison.
	 *
	 * \param[in] b  0-based index of the block in the ARResponse
	 *
	 * \return Index of the start for the logical block \c b
	 */
	int block_start(int b) const;

	/**
	 * \brief Converts a 0-based track number to an offset position within a
	 * block.
	 *
	 * \param[in] t  0-based index of the track in \c response
	 * \param[in] v2 Iff TRUE, the offset of the ARCSv2 is returned
	 *
	 * \return Offset for the flag index to be added to the start of the block
	 */
	int track_offset(int t, bool v2) const;

	/**
	 * \brief Converts a logical ARCS position in ARResponse to an absolute
	 * single flag position.
	 *
	 * The sum of \c block_start(b) and \c track_offset(t, v2).
	 *
	 * \param[in] b  0-based index of the block in \c response
	 * \param[in] t  0-based index of the track in \c response
	 * \param[in] v2 Iff TRUE, the offset of the ARCSv2 is returned
	 *
	 * \return Flag index for a single ARCS
	 */
	int index(int b, int t, bool v2) const;

	/**
	 * \brief Set the verification flag for the ARId of block \b to \c value.
	 *
	 * \param[in] b     0-based index of the block in \c response
	 * \param[in] value New value for this flag
	 *
	 * \return Absolute index position to be set
	 *
	 * \throws Iff \c b is out of range
	 */
	int set_id(int b, bool value);

	/**
	 * \brief Set the verification flag for the ARCS specified by \c b, \c t and
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
	int set_track(int b, int t, bool v2, bool value);

	/**
	 * \brief Ensures that \c b is a legal block value.
	 *
	 * \param[in] b 0-based index of the block in \c response
	 *
	 * \throws Iff \c b is out of range
	 */
	void validate_block(int b) const;

	/**
	 * \brief Ensures that \c t is a legal track value.
	 *
	 * \param[in] t 0-based index of the track in \c response
	 *
	 * \throws Iff \c t is out of range
	 */
	void validate_track(int t) const;


private:

	/**
	 * \brief Number of @link ARBlock ARBlocks @endlink represented.
	 */
	int blocks_;

	/**
	 * \brief Number of tracks in each ARBlock.
	 */
	int tracks_per_block_;

	/**
	 * \brief Number of flags stored.
	 */
	std::size_t size_;

	/**
	 * \brief The result bits of the comparison.
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

/// \cond UNDOC_FUNCTION_BODIES

DefaultMatchBase::DefaultMatchBase(int blocks, int tracks)
	: blocks_(blocks)
	, tracks_per_block_(tracks)
	, size_(static_cast<unsigned int>(blocks) *
			(2u * static_cast<unsigned int>(tracks) + 1u))
	, flag_(size_, false)
{
	// TODO Validate block and track numbers
	//if (blocks < 0 or tracks < 0)
	//{
	//	throw ...
	//}
}


DefaultMatchBase::~DefaultMatchBase() noexcept = default;


int DefaultMatchBase::do_verify_id(int b)
{
	return this->set_id(b, true);
}


bool DefaultMatchBase::do_id(int b) const
{
	this->validate_block(b);

	const auto offset =
		static_cast<DefaultMatchBase::size_type>(block_start(b));

	if (offset > this->size() - 1)
	{
		throw std::runtime_error("Block index too big");
	}

	return flag_[offset];
}


int DefaultMatchBase::do_verify_track(int b, int t, bool v2)
{
	return this->set_track(b, t, v2, true);
}


bool DefaultMatchBase::do_track(int b, int t, bool v2) const
{
	this->validate_block(b);
	this->validate_track(t);

	const auto i = static_cast<DefaultMatchBase::size_type>(index(b, t, v2));

	if ( i > this->size() - 1)
	{
		throw std::runtime_error("Accessor too big");
	}

	return flag_[i];
}


int64_t DefaultMatchBase::do_difference(int b, bool v2) const
{
	this->validate_block(b);

	uint32_t difference = (id(b) ? 0 : 1u);

	for (int trk = 0; trk < tracks_per_block_; ++trk)
	{
		difference += ( track(b, trk, v2) ? 0u : 1u );
	}

	return difference;
}


int DefaultMatchBase::do_total_blocks() const
{
	return blocks_;
}


int DefaultMatchBase::do_tracks_per_block() const
{
	return tracks_per_block_;
}


size_t DefaultMatchBase::do_size() const
{
	return size_;
}


int DefaultMatchBase::block_start(int b) const
{
	return b * (2 * tracks_per_block_ + 1);
}


int DefaultMatchBase::track_offset(int t, bool v2) const
{
	return t + 1 + (v2 ? tracks_per_block_ : 0);
}


int DefaultMatchBase::index(int b, int t, bool v2) const
{
	// b and t are 0-based
	return block_start(b) + track_offset(t, v2);
}


int DefaultMatchBase::set_id(int b, bool value)
{
	this->validate_block(b);

	const auto offset = block_start(b);

	if (static_cast<DefaultMatchBase::size_type>(offset) > this->size() - 1)
	{
		throw std::runtime_error("Block index too big");
	}

	flag_.emplace(flag_.begin() + offset, value);
	return block_start(b);
}


int DefaultMatchBase::set_track(int b, int t, bool v2, bool value)
{
	this->validate_block(b);
	this->validate_track(t);

	const auto i = index(b, t, v2);

	if (static_cast<DefaultMatchBase::size_type>(i) > this->size() - 1)
	{
		throw std::runtime_error("Accessor too big");
	}

	flag_.emplace(flag_.begin() + i, value);
	return i;
}


void DefaultMatchBase::validate_block(int b) const
{
	if (blocks_ - b < 1)
	{
		std::stringstream msg;
		msg << "Block index " << b << " too big, only " << blocks_
			<< " blocks in response";

		throw std::runtime_error(msg.str());
	}
}


void DefaultMatchBase::validate_track(int t) const
{
	if (tracks_per_block_ - t < 1)
	{
		std::stringstream msg;
		msg << "Track index " << t << " too big, only " << tracks_per_block_
			<< " tracks in album";

		throw std::runtime_error(msg.str());
	}
}

/// \endcond

/**
 * \brief Default implementation of a Match.
 *
 * The result is encoded as a sequence of boolean flags, each representing the
 * result of a match operation between either two ARCSs or two
 * @link ARId ARIds @endlink in the order of their occurrence in the ARResponse.
 */
class DefaultMatch final : virtual public DefaultMatchBase
{
public:

	/**
	 * \brief Default constructor.
	 *
	 * \param[in] blocks Number of @link ARBlock ARBlocks @endlink to represent
	 * \param[in] tracks Number of tracks per block
	 */
	DefaultMatch(int blocks, int tracks);


private:

	std::unique_ptr<Match> do_clone() const final;
};

/// \cond UNDOC_FUNCTION_BODIES

DefaultMatch::DefaultMatch(int blocks, int tracks)
	: DefaultMatchBase(blocks, tracks)
{
	// empty
}


std::unique_ptr<Match> DefaultMatch::do_clone() const
{
	return std::make_unique<DefaultMatch>(*this);
}


// Matcher


Matcher::~Matcher() noexcept = default;


bool Matcher::matches() const
{
	return this->do_matches();
}


int Matcher::best_match() const
{
	return this->do_best_match();
}


int Matcher::best_difference() const
{
	return this->do_best_difference();
}


bool Matcher::matches_v2() const
{
	return this->do_matches_v2();
}


const Match * Matcher::match() const
{
	return this->do_match();
}


std::unique_ptr<Matcher> Matcher::clone() const
{
	return this->do_clone();
}

/// \endcond

/**
 * \brief Abstract base class for matcher implementations.
 */
class MatcherImplBase
{

public:

	/**
	 * \brief Default constructor.
	 */
	MatcherImplBase();

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	MatcherImplBase(const MatcherImplBase &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	MatcherImplBase(MatcherImplBase &&rhs) noexcept;

	/**
	 * \brief Initializes the match.
	 *
	 * \param[in] checksums Checksums to be matched
	 * \param[in] id        ARId to be matched
	 * \param[in] response  Response data to match
	 */
	void init_match(const Checksums &checksums, const ARId &id,
		const ARResponse &response);

	/**
	 * \brief Implements Matcher::matches() const.
	 */
	bool matches() const;

	/**
	 * \brief Implements Matcher::best_match() const.
	 */
	int best_match() const;

	/**
	 * \brief Implements Matcher::best_difference() const.
	 */
	int best_difference() const;

	/**
	 * \brief Implements Matcher::matches_v2() const.
	 */
	bool matches_v2() const;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	MatcherImplBase& operator = (const MatcherImplBase &rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	MatcherImplBase& operator = (MatcherImplBase &&rhs) noexcept;

	/**
	 * \brief Returns the actual match result.
	 *
	 * \return Actual match result.
	 */
	const DefaultMatch * match() const;


protected:

	/**
	 * \brief Default destructor.
	 */
	~MatcherImplBase() noexcept;

	/**
	 * \brief Performs the actual match.
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
	 * \brief Derive best matching block from match result.
	 *
	 * \param[in]  m          Match to analyze
	 *
	 * \param[out] block      Best matching block found
	 * \param[out] matches_v2 TRUE iff best match is for ARCSv2, otherwise FALSE
	 *
	 * \return Status value, 0 indicates success
	 */
	int best_block(const DefaultMatch &m, int &block, bool &matches_v2);

	/**
	 * \brief Internal service method for constructor.
	 *
	 * \return 0-based index of the best block
	 */
	int mark_best_block();


private:

	/**
	 * \brief State: representation of the comparison result.
	 */
	std::unique_ptr<DefaultMatch> match_;

	/**
	 * \brief State: index as pointer to best block.
	 */
	int best_block_;

	/**
	 * \brief State: stores information about best block.
	 */
	bool matches_v2_;
};

/// \cond UNDOC_FUNCTION_BODIES

MatcherImplBase::MatcherImplBase()
	: match_(nullptr)
	, best_block_(0)
	, matches_v2_(false)
{
	// empty
}


MatcherImplBase::MatcherImplBase(const MatcherImplBase &rhs)
	: match_(std::make_unique<DefaultMatch>(*rhs.match_))  // deep copy
	, best_block_(rhs.best_block_)
	, matches_v2_(rhs.matches_v2_)
{
	// empty
}


MatcherImplBase::MatcherImplBase(MatcherImplBase &&rhs) noexcept
	: match_(std::move(rhs.match_))
	, best_block_(rhs.best_block_)
	, matches_v2_(rhs.matches_v2_)
{
	// empty
}


MatcherImplBase::~MatcherImplBase() noexcept = default;


void MatcherImplBase::init_match(const Checksums &checksums, const ARId &id,
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


bool MatcherImplBase::matches() const
{
	return this->best_difference() == 0;
}


int MatcherImplBase::best_match() const
{
	return best_block_;
}


int MatcherImplBase::best_difference() const
{
	return match_->difference(best_block_, matches_v2_);
}


bool MatcherImplBase::matches_v2() const
{
	return matches_v2_;
}


int MatcherImplBase::best_block(const DefaultMatch &m,
		int &block, bool &matches_v2)
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


MatcherImplBase& MatcherImplBase::operator = (
		const MatcherImplBase &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	match_ = std::make_unique<DefaultMatch>(*rhs.match_);
	return *this;
}


MatcherImplBase& MatcherImplBase::operator = (
		MatcherImplBase &&rhs) noexcept = default;


const DefaultMatch * MatcherImplBase::match() const
{
	return match_.get();
}


int MatcherImplBase::mark_best_block()
{
	int  block   = 0;
	bool version = false;

	auto status = this->best_block(*match_, block, version);

	if (status == 0) // best block found?
	{
		best_block_ = block;
		matches_v2_ = version;

		ARCS_LOG(DEBUG1) << "Best block: " << best_block_;
		ARCS_LOG(DEBUG1) << "Match:      "
			<< (matches_v2_ ? std::string("v2") : std::string("v1"));
	}

	return status;
}

/// \endcond

/**
 * \brief Implementation of AlbumMatcher.
 */
class AlbumMatcher::Impl final : public MatcherImplBase
{

protected:

	std::unique_ptr<DefaultMatch> do_match(const Checksums &actual_sums,
			const ARId &id, const ARResponse &ref_sums) const override;
};


std::unique_ptr<DefaultMatch> AlbumMatcher::Impl::do_match(
		const Checksums &actual_sums, const ARId &id,
		const ARResponse &ref_sums) const
{
	auto ref_tracks = ref_sums.tracks_per_block() < 0
		? 0
		: ref_sums.tracks_per_block(); // TODO Better way to compare

	if (actual_sums.size() != static_cast<Checksums::size_type>(ref_tracks))
	{
		ARCS_LOG_ERROR << "No match possible."
			<< " Number of tracks in actual_sums (" << actual_sums.size()
			<< ") is different from number of tracks in ref_sums ("
			<< ref_tracks << ")";

		// TODO exception

		return nullptr;
	}

	auto match = std::make_unique<DefaultMatch>(
			ref_sums.size(), actual_sums.size());

	Checksum checksum;
	Checksums::size_type track_j { 0 };
	int block_i { 0 };
	int bitpos = 0;

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


/// \cond UNDOC_FUNCTION_BODIES


// AlbumMatcher


AlbumMatcher::AlbumMatcher(const Checksums &checksums, const ARId &id,
		const ARResponse &response)
	: impl_(std::make_unique<AlbumMatcher::Impl>())
{
	impl_->init_match(checksums, id, response);
}


AlbumMatcher::AlbumMatcher(const AlbumMatcher &rhs)
	: impl_(std::make_unique<AlbumMatcher::Impl>(*rhs.impl_))
{
	// empty
}


AlbumMatcher::AlbumMatcher(AlbumMatcher &&rhs) noexcept = default;


AlbumMatcher::~AlbumMatcher() noexcept = default;


bool AlbumMatcher::do_matches() const
{
	return impl_->matches();
}


int AlbumMatcher::do_best_match() const
{
	return impl_->best_match();
}


int AlbumMatcher::do_best_difference() const
{
	return impl_->best_difference();
}


bool AlbumMatcher::do_matches_v2() const
{
	return impl_->matches_v2();
}


AlbumMatcher& AlbumMatcher::operator = (const AlbumMatcher &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	impl_ = std::make_unique<AlbumMatcher::Impl>(*rhs.impl_);
	return *this;
}


AlbumMatcher& AlbumMatcher::operator = (AlbumMatcher &&rhs) noexcept
	= default;


const Match* AlbumMatcher::do_match() const
{
	return impl_->match();
}


std::unique_ptr<Matcher> AlbumMatcher::do_clone() const
{
	return std::make_unique<AlbumMatcher>(*this);
}

/// \endcond

/**
 * \brief Private implementation of TracksetMatcher.
 */
class TracksetMatcher::Impl final : public MatcherImplBase
{

protected:

	std::unique_ptr<DefaultMatch> do_match(const Checksums &actual_sums,
			const ARId &id, const ARResponse &ref_sums) const override;
};


// TracksetMatcher::Impl

/// \cond UNDOC_FUNCTION_BODIES

std::unique_ptr<DefaultMatch> TracksetMatcher::Impl::do_match(
		const Checksums &actual_sums, const ARId & /*id*/,
		const ARResponse &ref_sums) const
{
	auto ref_tracks = ref_sums.tracks_per_block() < 0
		? 0
		: ref_sums.tracks_per_block(); // TODO Better way to compare

	if (actual_sums.size() != static_cast<Checksums::size_type>(ref_tracks))
	{
		ARCS_LOG_ERROR << "No match possible."
			<< " Number of tracks in actual_sums (" << actual_sums.size()
			<< ") is different from number of tracks in ref_sums ("
			<< ref_tracks << ")";

		// TODO exception

		return nullptr;
	}

	auto match = std::make_unique<DefaultMatch>(
			ref_sums.size(), actual_sums.size());

	int block_i { 0 };
	int track_j { 0 };
	int bitpos      { 0 };
	Checksums::size_type start_track { 0 };

	Checksum checksum;

	for (auto block = ref_sums.begin(); block != ref_sums.end(); ++block)
	{
		ARCS_LOG_DEBUG << "Try to match block " << block_i
			<< " (" << block_i + 1 << "/" << ref_sums.size() << ")";

		track_j = 0;
		start_track = 0;

		for (auto track = block->begin();
				track != block->end() && start_track < actual_sums.size();
				++track)
		{
			ARCS_LOG_DEBUG << "Track " << (track_j + 1);

			for (const auto& entry : actual_sums)
			{
				checksum = *entry.find(checksum::type::ARCS2);

				ARCS_LOG(DEBUG1) << "Check track " << (track_j + 1) << ": "
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

				ARCS_LOG(DEBUG1) << "Check track " << (track_j + 1) << ": "
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


// TracksetMatcher


TracksetMatcher::TracksetMatcher(const Checksums &checksums,
		const ARResponse &response)
	: impl_(std::make_unique<TracksetMatcher::Impl>())
{
	impl_->init_match(checksums, ARId(0, 0, 0, 0), response);
	// ARId is skipped, so we spare the effort to standard-create an empty ARId
}


TracksetMatcher::TracksetMatcher(const TracksetMatcher &rhs)
	: impl_(std::make_unique<TracksetMatcher::Impl>(*rhs.impl_))
{
	// empty
}


TracksetMatcher::TracksetMatcher(TracksetMatcher &&rhs) noexcept = default;


TracksetMatcher::~TracksetMatcher() noexcept = default;


bool TracksetMatcher::do_matches() const
{
	return impl_->matches();
}


int TracksetMatcher::do_best_match() const
{
	return impl_->best_match();
}


int TracksetMatcher::do_best_difference() const
{
	return impl_->best_difference();
}


bool TracksetMatcher::do_matches_v2() const
{
	return impl_->matches_v2();
}


TracksetMatcher& TracksetMatcher::operator = (const TracksetMatcher &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	impl_ = std::make_unique<TracksetMatcher::Impl>(*rhs.impl_);
	return *this;
}


TracksetMatcher& TracksetMatcher::operator = (TracksetMatcher &&rhs) noexcept
	= default;


const Match * TracksetMatcher::do_match() const
{
	return impl_->match();
}


std::unique_ptr<Matcher> TracksetMatcher::do_clone() const
{
	return std::make_unique<TracksetMatcher>(*this);
}

/// \endcond

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

