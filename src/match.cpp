/**
 * \file
 *
 * \brief Implementation of the AccurateRip checksum matching API.
 */

#ifndef __LIBARCSTK_MATCH_HPP__
#include "match.hpp"
#endif

#include <array>          // for array
#include <cstdint>        // for int64_t, uint32_t
#include <iomanip>        // for operator<<, setw, setfill, hex, uppercase
#include <memory>         // for unique_ptr, make_unique
#include <sstream>        // for operator<<, basic_ostream::operator<<, basi...
#include <stdexcept>      // for runtime_error
#include <string>         // for char_traits, operator<<, string
#include <utility>        // for move
#include <vector>         // for vector, _Bit_iterator

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


bool Match::equals(const Match &rhs) const noexcept
{
	return this->do_equals(rhs);
}


std::ostream& operator << (std::ostream &out, const Match &match)
{
	std::ios_base::fmtflags prev_settings = out.flags();

	const auto indent = std::string { "  " };
	for (auto b = int { 0 }; b < match.total_blocks(); ++b)
	{
		out << "Block " << b << std::endl;

		out << indent << "Id match: "
			<< std::boolalpha << match.id(b) << std::endl;

		for (auto t = int { 0 }; t < match.tracks_per_block(); ++t)
		{
			out << indent << "Track " << std::setw(2) << (t + 1)
				<< ": ARCSv1 is " << std::boolalpha << match.track(b, t, false)
				<< ", ARCSv2 is " << std::boolalpha << match.track(b, t, true)
				<< std::endl;
		}
	}

	out.flags(prev_settings);
	return out;
}


/**
 * \brief Base for @link DefaultMatch DefaultMatches @endlink.
 */
class DefaultMatchBase : public Match
{
public:

	using size_type = std::size_t;

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
	virtual ~DefaultMatchBase() noexcept override;

protected:

	/**
	 * \brief Converts a logical ARCS position in ARResponse to an absolute
	 * flag index.
	 *
	 * Equivalent to the sum of \c index(b) and \c track_offset(t, v2).
	 *
	 * \param[in] b  0-based index of the block in \c response
	 * \param[in] t  0-based index of the track in block \c b
	 * \param[in] v2 TRUE requests offset of ARCSv2, FALSE requests ARCSv1
	 *
	 * \return Flag index for single ARCS
	 */
	int index(int b, int t, bool v2) const;

	/**
	 * \brief Converts a logical block index to an absolute flag index.
	 *
	 * Note that the block start is also the position of the flag respresenting
	 * the ARId comparison.
	 *
	 * \param[in] b  0-based index of the block in the ARResponse
	 *
	 * \return Index of the start for the logical block \c b
	 */
	int index(int b) const;

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
	 * \brief Set the flag on position \c offset to \c value.
	 *
	 * \return Absolute index position the operation modified
	 */
	void set_flag(const int offset, const bool value);

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

	int do_verify_id(int b) override;

	bool do_id(int b) const override;

	int do_verify_track(int b, int t, bool v2) override;

	bool do_track(int b, int t, bool v2) const override;

	int64_t do_difference(int b, bool v2) const override;

	int do_total_blocks() const override;

	int do_tracks_per_block() const override;

	size_t do_size() const override;

	bool do_equals(const Match &rhs) const noexcept override;

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


DefaultMatchBase::DefaultMatchBase(int blocks, int tracks)
	: blocks_ { blocks }
	, tracks_per_block_ { tracks }
	, size_ { static_cast<std::size_t>(blocks) *
			(2u * static_cast<std::size_t>(tracks) + 1u) }
	, flag_ ( size_, false ) // No braces!
{
	if (tracks < 0 or tracks > CDDA.MAX_TRACKCOUNT)
	{
		throw std::out_of_range("Illegal number of tracks: "
				+ std::to_string(tracks));
	}

	if (blocks < 0)
	{
		ARCS_LOG_WARNING << "Match: set negative number of blocks to 0";
		blocks_ = 0;
	}
}


DefaultMatchBase::~DefaultMatchBase() noexcept = default;


int DefaultMatchBase::do_verify_id(int b)
{
	return this->set_id(b, true);
}


bool DefaultMatchBase::do_id(int b) const
{
	this->validate_block(b);

	const auto i { static_cast<DefaultMatchBase::size_type>(index(b)) };

	return flag_[i];
}


int DefaultMatchBase::do_verify_track(int b, int t, bool v2)
{
	return this->set_track(b, t, v2, true);
}


bool DefaultMatchBase::do_track(int b, int t, bool v2) const
{
	this->validate_block(b);
	this->validate_track(t);

	const auto i { static_cast<DefaultMatchBase::size_type>(index(b, t, v2)) };

	return flag_[i];
}


int64_t DefaultMatchBase::do_difference(int b, bool v2) const
{
	auto difference = int { (id(b) ? 0 : 1) }; // also calls validate_block()

	for (auto trk = int { 0 }; trk < tracks_per_block_; ++trk)
	{
		difference += ( track(b, trk, v2) ? 0 : 1 );
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


bool DefaultMatchBase::do_equals(const Match &rhs) const noexcept
{
	auto rhs_dm = dynamic_cast<const DefaultMatchBase*>(&rhs);

	if (!rhs_dm)
	{
		return false;
	}

	return blocks_ == rhs_dm->blocks_
		and tracks_per_block_ == rhs_dm->tracks_per_block_
		and size_ == rhs_dm->size_
		and flag_ == rhs_dm->flag_;
}


int DefaultMatchBase::index(int b) const
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
	return index(b) + //track_offset(t, v2);
		(t + 1 + (v2 ? tracks_per_block_ : 0));
}


int DefaultMatchBase::set_id(int b, bool value)
{
	this->validate_block(b);

	const auto offset { index(b) };
	set_flag(offset, value);

	return offset;
}


int DefaultMatchBase::set_track(int b, int t, bool v2, bool value)
{
	this->validate_block(b);
	this->validate_track(t);

	const auto offset { index(b, t, v2) };

	set_flag(offset, value);

	return offset;
}


void DefaultMatchBase::set_flag(const int offset, const bool value)
{
	auto pos = flag_.begin() + offset;
	*pos = value;
}


void DefaultMatchBase::validate_block(int b) const
{
	if (blocks_ - b < 1)
	{
		auto msg = std::stringstream {};
		msg << "Block index " << b << " too big, only " << blocks_
			<< " blocks in response";

		throw std::runtime_error(msg.str());
	}
}


void DefaultMatchBase::validate_track(int t) const
{
	if (tracks_per_block_ - t < 1)
	{
		auto msg = std::stringstream {};
		msg << "Track index " << t << " too big, only " << tracks_per_block_
			<< " tracks in album";

		throw std::runtime_error(msg.str());
	}
}


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
	 * \brief Constructor.
	 *
	 * \param[in] blocks Number of @link ARBlock ARBlocks @endlink to represent
	 * \param[in] tracks Number of tracks per block
	 */
	DefaultMatch(int blocks, int tracks);

private:

	std::unique_ptr<Match> do_clone() const final;
};


DefaultMatch::DefaultMatch(int blocks, int tracks)
	: DefaultMatchBase { blocks, tracks }
{
	// empty
}


std::unique_ptr<Match> DefaultMatch::do_clone() const
{
	return std::make_unique<DefaultMatch>(*this);
}


// Matcher


Matcher::~Matcher() noexcept = default;


bool Matcher::matches() const noexcept
{
	return this->do_matches();
}


int Matcher::best_match() const noexcept
{
	return this->do_best_match();
}


int Matcher::best_difference() const noexcept
{
	return this->do_best_difference();
}


bool Matcher::matches_v2() const noexcept
{
	return this->do_matches_v2();
}


const Match * Matcher::match() const noexcept
{
	return this->do_match();
}


std::unique_ptr<Matcher> Matcher::clone() const noexcept
{
	return this->do_clone();
}


bool Matcher::equals(const Matcher &rhs) const noexcept
{
	return this->do_equals(rhs);
}


namespace details
{

std::unique_ptr<Match> create_match(const int blocks, const std::size_t tracks)
	noexcept
{
	return std::make_unique<DefaultMatch>(blocks, tracks);
}

} // namespace details


/**
 * \brief Abstract base class for matcher implementations.
 *
 * All Matchers share basically the same implementation but differ in how
 * Matcher::do_match() is implemented.
 */
class MatcherBase::Impl
{
public:

	/**
	 * \brief Default constructor.
	 */
	Impl();

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~Impl() noexcept;

	/**
	 * \brief Hook for post-construction initialization.
	 */
	void init() noexcept;

	/**
	 * \brief Implements Matcher::matches() const.
	 */
	bool matches() const noexcept;

	/**
	 * \brief Implements Matcher::best_match() const.
	 */
	int best_match() const noexcept;

	/**
	 * \brief Implements Matcher::best_difference() const.
	 */
	int best_difference() const noexcept;

	/**
	 * \brief Implements Matcher::matches_v2() const.
	 */
	bool matches_v2() const noexcept;

	/**
	 * \brief Returns the actual match result.
	 *
	 * \return Actual match result.
	 */
	const Match * match() const noexcept;

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
	int best_block(const Match &m, int &block, bool &matches_v2) const noexcept;

	/**
	 * \brief Set the match instance and mark best block.
	 *
	 * \param[in] match Match instance to set
	 */
	void set_match(std::unique_ptr<Match> match);

	/**
	 * \brief Create a match object for internal use.
	 *
	 * \param[in] blocks
	 * \param[in] tracks_per_block
	 *
	 * \return Empty Match object.
	 */
	std::unique_ptr<Match> create_match_instance(const int blocks,
			const std::size_t tracks_per_block) const noexcept;

	/**
	 * \brief Implement matching an actual Checksum against a reference.
	 *
	 * The matching is implemented by calling operator == on the input
	 * Checksum instances.
	 *
	 * \param[in] actual    The actual Checksum to be matched
	 * \param[in] reference The reference Checksum to match
	 *
	 * \return TRUE if the sums match, otherwise FALSE
	 */
	bool matching(const Checksum &actual, const Checksum &reference) const
		noexcept;

	/**
	 * \brief Clone this instance.
	 *
	 * \return A clone (deep copy) of the instance.
	 */
	std::unique_ptr<MatcherBase::Impl> clone() const;

	/**
	 * \brief Returns TRUE if the right hand side equals the instance.
	 */
	bool equals(const MatcherBase::Impl &rhs) const noexcept;

protected:

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	Impl(const Impl &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	Impl(Impl &&rhs) noexcept;

	/**
	 * \brief The checksum types to verify.
	 */
	static constexpr std::array<checksum::type, 2> types {
		checksum::type::ARCS1,
		checksum::type::ARCS2
	};

	/**
	 * \brief Marks the "best block" as matching block.
	 *
	 * Internal service method for constructor.
	 *
	 * \return 0-based index of the best block
	 */
	int mark_best_block() noexcept;

	/**
	 * \brief Worker for clone(): clones the base class parts.
	 *
	 * Subclasses can use clone_base() to receive a base class pointer to an
	 * instance of the concrete subclass type, downcast it and clone the
	 * properties specific for the subclass. The downcast will be safe as
	 * clone_base() uses do_create_instance() to instantiate the clone instance.
	 *
	 * \return Concrete subclass instance with cloned base class parts.
	 */
	std::unique_ptr<MatcherBase::Impl> clone_base() const;

	/**
	 * \brief Worker for equals(): check base class parts for equality.
	 *
	 * \param[in] rhs Right hand side to compare
	 *
	 * \return TRUE iff base class parts are equal, otherwise FALSE
	 */
	bool equals_base(const MatcherBase::Impl &rhs) const noexcept;

private:

	/**
	 * \brief Implements init().
	 */
	virtual void do_init() noexcept
	= 0;

	/**
	 * \brief Creates an (empty) instance of the subclass to clone.
	 *
	 * Used in clone_base() to create an instance of the concrete subclass.
	 * Since do_clone() is supposed to downcast the result of clone_base(), the
	 * downcast is always safe if the subclass implements do_create_instance().
	 *
	 * \return Instance of the subclass to clone.
	 */
	virtual std::unique_ptr<MatcherBase::Impl> do_create_instance() const
	= 0;

	/**
	 * \brief Implements clone() for subclasses.
	 *
	 * Call clone_base(), downcast to the concrete subclass type (which is safe
	 * since clone_base() uses do_create_instance() to create the instance),
	 * clone all settings specific for the subclass and return the instance.
	 *
	 * \return A clone (deep copy) of the object
	 */
	virtual std::unique_ptr<MatcherBase::Impl> do_clone() const;

	/**
	 * \brief Implements equals() for subclasses.
	 *
	 * \param[in] rhs Right hand side to compare
	 *
	 * \return TRUE iff base class parts are equal, otherwise FALSE
	 */
	virtual bool do_equals(const MatcherBase::Impl &rhs) const noexcept;

	/**
	 * \brief State: representation of the comparison result.
	 */
	std::unique_ptr<Match> match_;

	/**
	 * \brief State: index as pointer to best block.
	 */
	int best_block_;

	/**
	 * \brief State: stores information about best block.
	 */
	bool matches_v2_;
};


constexpr std::array<checksum::type, 2> MatcherBase::Impl::types;


MatcherBase::Impl::Impl()
	: match_ { nullptr }
	, best_block_ { 0 }
	, matches_v2_ { false }
{
	// empty
}


MatcherBase::Impl::~Impl() noexcept = default;


bool MatcherBase::Impl::matches() const noexcept
{
	return this->best_difference() == 0;
}


int MatcherBase::Impl::best_match() const noexcept
{
	return best_block_;
}


int MatcherBase::Impl::best_difference() const noexcept
{
	return match_->difference(best_block_, matches_v2_);
}


bool MatcherBase::Impl::matches_v2() const noexcept
{
	return matches_v2_;
}


const Match * MatcherBase::Impl::match() const noexcept
{
	return match_.get();
}


int MatcherBase::Impl::best_block(const Match &m, int &block, bool &matches_v2)
	const noexcept
{
	ARCS_LOG(DEBUG1) << "Find best block:";

	if (m.size() == 0)
	{
		return -1;
	}

	auto best_diff    = int { 100 }; // max difference possible, 99 tracks + id
	auto curr_diff_v1 = int {   0 };
	auto curr_diff_v2 = int {   0 };

	for (auto b = int { 0 }; b < m.total_blocks(); ++b)
	{
		// Note: v2 matching will always be preferred over v1 matching

		ARCS_LOG(DEBUG1) << "Check block " << b;

		curr_diff_v1 = m.difference(b, /* v1 */false);
		curr_diff_v2 = m.difference(b, /* v2 */true);

		// Note the less-equal for v2: last match wins!
		if (curr_diff_v2 <= best_diff or curr_diff_v1 < best_diff)
		{
			block      = b;
			matches_v2 = curr_diff_v2 <= curr_diff_v1;
			best_diff  = matches_v2 ? curr_diff_v2 : curr_diff_v1;

			ARCS_LOG_DEBUG << "Declare block " << b << " as best match"
				<< " (is ARCSv" << (matches_v2 + 1) << ")";
		}
	}

	return 0;
}


void MatcherBase::Impl::set_match(std::unique_ptr<Match> match)
{
	match_ = std::move(match);
	this->mark_best_block();
}


std::unique_ptr<Match> MatcherBase::Impl::create_match_instance(
		const int blocks, const std::size_t tracks) const noexcept
{
	return details::create_match(blocks, tracks);
}


std::unique_ptr<MatcherBase::Impl> MatcherBase::Impl::clone() const
{
	return this->do_clone();
}


bool MatcherBase::Impl::equals(const MatcherBase::Impl &rhs) const noexcept
{
	return this->do_equals(rhs);
}


void MatcherBase::Impl::init() noexcept
{
	this->do_init();
}


bool MatcherBase::Impl::matching(const Checksum &actual,
		const Checksum &reference) const noexcept
{
	return actual == reference;
}


int MatcherBase::Impl::mark_best_block() noexcept
{
	auto block   = int  { 0 };
	auto version = bool { false };

	const auto status { this->best_block(*match_, block, version) };

	if (status == 0) // best block found?
	{
		best_block_ = block;
		matches_v2_ = version;

		ARCS_LOG(DEBUG1) << "Best block: " << best_block_;
		ARCS_LOG(DEBUG1) << "Match:      ARCSv" << (matches_v2_ ? "2" : "1");
	}

	return status;
}


std::unique_ptr<MatcherBase::Impl> MatcherBase::Impl::clone_base() const
{
	auto instance = this->do_create_instance();

	instance->match_ = match_->clone();
	instance->matches_v2_ = matches_v2_;
	instance->best_block_ = best_block_;

	return instance;
}


bool MatcherBase::Impl::equals_base(const MatcherBase::Impl &rhs) const noexcept
{
	return best_block_ == rhs.best_block_
		and matches_v2_ == rhs.matches_v2_
		and match_->equals(*rhs.match_);
}


std::unique_ptr<MatcherBase::Impl> MatcherBase::Impl::do_clone() const
{
	return this->clone_base();
	// Seems a sensible default. Only subclasses that add member variables
	// need to override this, call clone_base() and than clone their respective
	// members.
}


bool MatcherBase::Impl::do_equals(const MatcherBase::Impl &rhs) const noexcept
{
	return this->equals_base(rhs);
	// Seems a sensible default. Only subclasses that add member variables
	// need to override this, call equals_base() and than compare their
	// respective members for equality.
}


/**
 * \brief Default implementation of a matcher
 */
class DefaultMatcherBaseImpl : public MatcherBase::Impl
{
private:

	void do_init() noexcept override;

	std::unique_ptr<MatcherBase::Impl> do_create_instance() const override;
};


void DefaultMatcherBaseImpl::do_init() noexcept
{
	// empty
}


std::unique_ptr<MatcherBase::Impl> DefaultMatcherBaseImpl::do_create_instance()
	const
{
	return std::make_unique<DefaultMatcherBaseImpl>();
}


// MatcherBase


MatcherBase::MatcherBase()
	: impl_ { std::make_unique<DefaultMatcherBaseImpl>() }
{
	// empty
}


MatcherBase::MatcherBase(std::unique_ptr<MatcherBase::Impl> impl) noexcept
	: impl_ { std::move(impl) }
{
	// empty
}


MatcherBase::MatcherBase(const MatcherBase &rhs)
	: MatcherBase { rhs.impl_->clone() }
{
	// empty
}


MatcherBase::~MatcherBase() noexcept = default;


std::unique_ptr<Match> MatcherBase::create_match(const int refblocks,
			const std::size_t tracks) const
{
	return impl_->create_match_instance(refblocks, tracks);
}


std::unique_ptr<Matcher> MatcherBase::clone_base() const noexcept
{
	auto impl = impl_->clone();
	return this->do_create_instance(std::move(impl));
}


MatcherBase::Impl& MatcherBase::access_impl()
{
	return *impl_;
}


MatcherBase& MatcherBase::operator = (const MatcherBase &rhs)
{
	impl_ = rhs.impl_->clone();
	return *this;
}


bool MatcherBase::do_matches() const noexcept
{
	return impl_->matches();
}


int MatcherBase::do_best_match() const noexcept
{
	return impl_->best_match();
}


int MatcherBase::do_best_difference() const noexcept
{
	return impl_->best_difference();
}


bool MatcherBase::do_matches_v2() const noexcept
{
	return impl_->matches_v2();
}


const Match* MatcherBase::do_match() const noexcept
{
	return impl_->match();
}


std::unique_ptr<Matcher> MatcherBase::do_clone() const noexcept
{
	return this->clone_base();
}


bool MatcherBase::do_equals(const Matcher &rhs) const noexcept
{
	auto rhs_mb = dynamic_cast<const MatcherBase*>(&rhs);

	if (!rhs_mb)
	{
		return false;
	}

	return impl_->equals(*rhs_mb->impl_);
}


/**
 * \brief Matcher for matching against ARResponses.
 *
 * Must be init()'ed after construction!
 */
class ResponseMatcherBaseImpl : public MatcherBase::Impl
{
public:

	/**
	 * \brief Construct a Matcher for the specified input
	 *
	 * \param[in] actual_sums Checksums to match against reference values
	 * \param[in] actual_id   Id for the \c actual_sums
	 * \param[in] ref_sums    Reference values to be matched by \c checksums
	 */
	ResponseMatcherBaseImpl(const Checksums &checksums, const ARId &id,
		const ARResponse &response);

	ResponseMatcherBaseImpl(const ResponseMatcherBaseImpl &) = delete;

	ResponseMatcherBaseImpl& operator = (const ResponseMatcherBaseImpl &)
		= delete;

protected:

	/**
	 * \brief Constructor
	 */
	ResponseMatcherBaseImpl();

	/**
	 * \brief Worker for do_init(): initializes the match.
	 *
	 * \param[in] actual_sums Checksums to match against reference values
	 * \param[in] actual_id   Id for the \c actual_sums
	 * \param[in] ref_sums    Reference values to be matched by \c checksums
	 */
	void init_match(const Checksums &checksums, const ARId &id,
		const ARResponse &response);

	/**
	 * \brief Worker for init_match(): validate input.
	 *
	 * \param[in] actual_sums Checksums to match against reference values
	 * \param[in] actual_id   Id for the \c actual_sums
	 * \param[in] ref_sums    Reference values to be matched by \c checksums
	 *
	 * \throws std::invalid_argument If \c actual_sums and \c ref_sums are
	 * not matchable
	 */
	void validate(const Checksums &actual_sums, const ARId &actual_id,
			const ARResponse &ref_sums) const;

	/**
	 * \brief Worker for clone(): clones the base class parts.
	 *
	 * Subclasses can use clone_base() to receive a base class pointer to an
	 * instance of the concrete subclass type, downcast it and clone the
	 * properties specific for the subclass. The downcast will be safe as
	 * clone_base() uses do_create_instance() to instantiate the clone instance.
	 *
	 * \return Concrete subclass instance with cloned base class parts.
	 */
	std::unique_ptr<MatcherBase::Impl> clone_base() const;

private:

	/**
	 * \brief Pointer to actual checksums.
	 */
	const Checksums*  actual_sums_;

	/**
	 * \brief Pointer to actual id.
	 */
	const ARId*       actual_id_;

	/**
	 * \brief Pointer to reference checksums.
	 */
	const ARResponse* ref_sums_;

	/**
	 * \brief Performs the actual match.
	 *
	 * \param[in] actual_sums Checksums to match against reference values
	 * \param[in] actual_id   Id for the \c actual_sums
	 * \param[in] ref_sums    Reference values to be matched by \c checksums
	 *
	 * \return Match information
	 */
	virtual std::unique_ptr<Match> perform_match(
			const Checksums &actual_sums, const ARId &actual_id,
			const ARResponse &ref_sums) const noexcept
	= 0;

	void do_init() noexcept override;

	std::unique_ptr<MatcherBase::Impl> do_clone() const override;

	bool do_equals(const MatcherBase::Impl &rhs) const noexcept override;
};


ResponseMatcherBaseImpl::ResponseMatcherBaseImpl()
	: MatcherBase::Impl {}
	, actual_sums_ { nullptr }
	, actual_id_   { nullptr }
	, ref_sums_    { nullptr }
{
	// empty
}


ResponseMatcherBaseImpl::ResponseMatcherBaseImpl(const Checksums &checksums,
		const ARId &id, const ARResponse &response)
	: MatcherBase::Impl {}
	, actual_sums_ { &checksums }
	, actual_id_   { &id }
	, ref_sums_    { &response }
{
	// empty
}


void ResponseMatcherBaseImpl::init_match(const Checksums &checksums,
		const ARId &id, const ARResponse &response)
{
	validate(checksums, id, response);
	set_match(perform_match(checksums, id, response));
	mark_best_block();
}


void ResponseMatcherBaseImpl::validate(const Checksums &actual_sums,
		const ARId &/* unused */, const ARResponse &ref_sums) const
{
	if (actual_sums.size() == 0)
	{
		throw std::invalid_argument("Cannot match empty checksums");
	}

	if (ref_sums.size() == 0)
	{
		throw std::invalid_argument("Cannot match against empty ARResponse");
	}

	if (actual_sums.size() !=
			static_cast<std::size_t>(ref_sums.tracks_per_block()))
	{
		throw std::invalid_argument("Number of tracks in input"
			" does not match number of tracks in ARResponse");
	}
}


void ResponseMatcherBaseImpl::do_init() noexcept
{
	init_match(*actual_sums_, *actual_id_, *ref_sums_);
}


std::unique_ptr<MatcherBase::Impl> ResponseMatcherBaseImpl::clone_base() const
{
	auto clone = MatcherBase::Impl::clone_base();

	// Clone my members
	{
		auto my_clone = dynamic_cast<ResponseMatcherBaseImpl*>(clone.get());
		my_clone->actual_sums_ = actual_sums_;
		my_clone->actual_id_   = actual_id_;
		my_clone->ref_sums_    = ref_sums_;
	}

	return clone;
}


std::unique_ptr<MatcherBase::Impl> ResponseMatcherBaseImpl::do_clone() const
{
	return this->clone_base();
}


bool ResponseMatcherBaseImpl::do_equals(const MatcherBase::Impl &rhs) const
	noexcept
{
	auto rhs_rmbi = dynamic_cast<const ResponseMatcherBaseImpl*>(&rhs);
	if (!rhs_rmbi)
	{
		return false;
	}

	return this->equals_base(rhs)
		and actual_sums_ == rhs_rmbi->actual_sums_
		and actual_id_ == rhs_rmbi->actual_id_
		and ref_sums_ == rhs_rmbi->ref_sums_;
}


/**
 * \brief Implementation of AlbumMatcher.
 */
class AlbumMatcherImpl final : public ResponseMatcherBaseImpl
{
public:

	using ResponseMatcherBaseImpl::ResponseMatcherBaseImpl;

private:

	std::unique_ptr<MatcherBase::Impl> do_create_instance() const override;

	std::unique_ptr<Match> perform_match(const Checksums &actual_sums,
			const ARId &id, const ARResponse &ref_sums) const noexcept override;
};


std::unique_ptr<MatcherBase::Impl> AlbumMatcherImpl::do_create_instance() const
{
	return std::make_unique<AlbumMatcherImpl>();
}


std::unique_ptr<Match> AlbumMatcherImpl::perform_match(
		const Checksums &actual_sums, const ARId &actual_id,
		const ARResponse &ref_sums) const noexcept
{
	// Validation is assumed to be already performed

	auto match { create_match_instance(ref_sums.size(), actual_sums.size()) };

	auto block_i  = int  { 0 };
	auto bitpos   = int  { 0 };
	auto is_v2    = bool { false };
	auto track_j  = Checksums::size_type { 0 };
	auto actual_checksum    = Checksum {};
	auto reference_checksum = Checksum {};

	for (auto block { ref_sums.begin() }; block != ref_sums.end(); ++block)
	{
		ARCS_LOG_DEBUG << "Try to match block " << block_i
			<< " (" << block_i + 1 << "/" << ref_sums.size() << ")";

		if (block->id() == actual_id)
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

		for (auto track { block->begin() }; track != block->end(); ++track)
		{
			for (const auto& type : MatcherBase::Impl::types)
			{
				is_v2 = (type == checksum::type::ARCS2);

				actual_checksum    = actual_sums[track_j].get(type);
				reference_checksum = Checksum { track->arcs() };

				ARCS_LOG(DEBUG1) << "Check track "
					<< std::setw(2) << std::setfill('0') << (track_j + 1)
					<< ": "
					<< actual_checksum
					<< " to match "
					<< reference_checksum
					<< " (v" << (is_v2 ? "2" : "1") << ") ";

				if (matching(actual_checksum, reference_checksum))
				{
					bitpos = match->verify_track(block_i, track_j, is_v2);

					ARCS_LOG_DEBUG << "Track "
						<< std::setw(2) << std::setfill('0') << (track_j + 1)
						<< " v" << (is_v2 ? "2" : "1") << " verified: "
						<< match->track(block_i, track_j, is_v2)
						<< " (bit " << bitpos << ")";
				}
				else
				{
					ARCS_LOG_DEBUG << "Track "
						<< std::setw(2) << std::setfill('0') << (track_j + 1)
						<< " v" << (is_v2 ? "2" : "1") << " not verified: "
						<< match->track(block_i, track_j, is_v2);
				}
			}

			++track_j;
		}

		++block_i;
	}

	return match;
}


// AlbumMatcher


AlbumMatcher::AlbumMatcher(const Checksums &checksums, const ARId &id,
		const ARResponse &response)
	: MatcherBase {
		std::make_unique<AlbumMatcherImpl>(checksums, id, response) }
{
	access_impl().init(); // This actually performs the match
}


AlbumMatcher::AlbumMatcher(const AlbumMatcher &rhs)
	: MatcherBase { rhs }
{
	// empty
}


AlbumMatcher::AlbumMatcher(AlbumMatcher &&rhs) noexcept
	: MatcherBase { std::move(rhs) }
{
	// empty
}


AlbumMatcher::~AlbumMatcher() noexcept = default;


AlbumMatcher& AlbumMatcher::operator = (const AlbumMatcher &rhs)
{
	MatcherBase::operator=(rhs);
	return *this;
}


AlbumMatcher& AlbumMatcher::operator = (AlbumMatcher &&rhs) noexcept
{
	MatcherBase::operator=(std::move(rhs));
	return *this;
}


std::unique_ptr<MatcherBase> AlbumMatcher::do_create_instance(
			std::unique_ptr<Impl> impl) const noexcept
{
	return std::make_unique<AlbumMatcher>(std::move(impl));
}


/**
 * \brief Private implementation of TracksetMatcher.
 */
class TracksetMatcherImpl final : public ResponseMatcherBaseImpl
{
public:

	using ResponseMatcherBaseImpl::ResponseMatcherBaseImpl;

private:

	std::unique_ptr<MatcherBase::Impl> do_create_instance() const override;

	std::unique_ptr<Match> perform_match(const Checksums &actual_sums,
			const ARId &id, const ARResponse &ref_sums) const noexcept override;
};


// TracksetMatcherImpl


std::unique_ptr<MatcherBase::Impl> TracksetMatcherImpl::do_create_instance()
	const
{
	return std::make_unique<TracksetMatcherImpl>();
}


std::unique_ptr<Match> TracksetMatcherImpl::perform_match(
		const Checksums &actual_sums, const ARId &actual_id,
		const ARResponse &ref_sums) const noexcept
{
	// Validation is assumed to be already performed

	auto match { create_match_instance(ref_sums.size(), actual_sums.size()) };

	auto block_i = int  { 0 };
	auto track_j = int  { 0 };
	auto bitpos  = int  { 0 };
	auto is_v2   = bool { false };
	auto start_track = Checksums::size_type { 0 };
	auto actual_checksum    = Checksum {};
	auto reference_checksum = Checksum {};

	for (auto block { ref_sums.begin() }; block != ref_sums.end(); ++block)
	{
		ARCS_LOG_DEBUG << "Try to match block " << block_i
			<< " (" << block_i + 1 << "/" << ref_sums.size() << ")";

		if (actual_id.empty() or block->id() == actual_id)
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
		start_track = 0;

		for (auto track { block->begin() };
				track != block->end() && start_track < actual_sums.size();
				++track)
		{
			ARCS_LOG_DEBUG << "Track " << (track_j + 1);

			for (const auto& actual_track : actual_sums)
			{
				for (const auto& type : MatcherBase::Impl::types)
				{
					is_v2 = (type == checksum::type::ARCS2);

					actual_checksum    = actual_track.get(type);
					reference_checksum = Checksum { track->arcs() };

					ARCS_LOG(DEBUG1) << "Check track "
						<< std::setw(2) << std::setfill('0') << (track_j + 1)
						<< ": "
						<< actual_checksum
						<< " to match "
						<< reference_checksum
						<< " (v" << (is_v2 ? "2" : "1") << ") ";

					if (matching(actual_checksum,reference_checksum))
					{
						bitpos = match->verify_track(block_i, track_j, is_v2);

						ARCS_LOG_DEBUG << "  >Track "
							<< std::setw(2) << std::setfill('0')
							<< (track_j + 1)
							<< " v" << (is_v2 ? "2" : "1") << " verified: "
							<< match->track(block_i, track_j, is_v2)
							<< " (bit " << bitpos << ")"
							<< " matches tracklist pos " << track_j;

						++start_track;
						break;
					} else
					{
						ARCS_LOG_DEBUG << "Track "
							<< std::setw(2) << std::setfill('0')
							<< (track_j + 1)
							<< " v" << (is_v2 ? "2" : "1") << " not verified: "
							<< match->track(block_i, track_j, is_v2);
					}
				}
			}

			++track_j;
		}

		++block_i;
	}

	return match;
}


// TracksetMatcher


TracksetMatcher::TracksetMatcher(const Checksums &checksums, const ARId &id,
		const ARResponse &response)
	: MatcherBase {
		std::make_unique<TracksetMatcherImpl>(checksums, id, response) }
{
	access_impl().init(); // This actually performs the match
}


TracksetMatcher::TracksetMatcher(const Checksums &checksums,
		const ARResponse &response)
	: MatcherBase {
		std::make_unique<TracksetMatcherImpl>(checksums, EmptyARId, response) }
{
	access_impl().init();
}


TracksetMatcher::TracksetMatcher(const TracksetMatcher &rhs)
	: MatcherBase { rhs }
{
	// empty
}


TracksetMatcher::TracksetMatcher(TracksetMatcher &&rhs) noexcept
	: MatcherBase { std::move(rhs) }
{
	// empty
}


TracksetMatcher::~TracksetMatcher() noexcept = default;


TracksetMatcher& TracksetMatcher::operator = (const TracksetMatcher &rhs)
{
	MatcherBase::operator=(rhs);
	return *this;
}


TracksetMatcher& TracksetMatcher::operator = (TracksetMatcher &&rhs) noexcept
{
	MatcherBase::operator=(std::move(rhs));
	return *this;
}


std::unique_ptr<MatcherBase> TracksetMatcher::do_create_instance(
			std::unique_ptr<Impl> impl) const noexcept
{
	return std::make_unique<TracksetMatcher>(std::move(impl));
}


// ListMatcher


ListMatcher::ListMatcher(const ListMatcher &rhs)
	: MatcherBase { rhs }
{
	// empty
}


ListMatcher::ListMatcher(ListMatcher &&rhs) noexcept
	: MatcherBase { std::move(rhs) }
{
	// empty
}


ListMatcher::~ListMatcher() noexcept = default;


ListMatcher& ListMatcher::operator = (const ListMatcher &rhs)
{
	MatcherBase::operator= (rhs);
	return *this;
}


ListMatcher& ListMatcher::operator = (ListMatcher &&rhs) noexcept
{
	MatcherBase::operator= (rhs);
	return *this;
}


void ListMatcher::update(std::unique_ptr<Match> match)
{
	access_impl().set_match(std::move(match));
}


std::unique_ptr<MatcherBase> ListMatcher::do_create_instance(
			std::unique_ptr<Impl> impl) const noexcept
{
	return std::make_unique<ListMatcher>(std::move(impl));
}


/** @} */

} // namespace v_1_0_0

} // namespace arcstk

