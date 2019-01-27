#ifndef __LIBARCS_MATCH_HPP__
#define __LIBARCS_MATCH_HPP__


/**
 * \file match.hpp AccurateRip checksum matching
 *
 * Matcher provides an interface to match the ARCSs of some audio input
 * against a response from AccurateRip. There are two implementations.
 *
 * The ListMatcher matches a list of track-based Checksums against
 * an ARResponse. It can be used for matching the Checksums of a
 * complete disc image.
 *
 * The AnyMatcher matches a set of file-based Checksums against an
 * ARResponse. It is used for matching single track files.
 *
 * A Matcher returns a Match that encodes a complete matrix of numeric
 * comparisons that is accessible by addressing a single comparison by its
 * block, track and checksum algorithm version.
 */


#include <cstdint>
#include <cstddef>
#include <memory>

namespace arcs { inline namespace v_1_0_0 { class ARId;       } }
namespace arcs { inline namespace v_1_0_0 { class ARResponse; } }
namespace arcs { inline namespace v_1_0_0 { class Checksums;  } }


namespace arcs
{
/// \defgroup match AccurateRip Checksum Matcher
/// @{
inline namespace v_1_0_0
{


/**
 * Result of a match of some <tt>Checksum</tt>s against an ARResponse.
 *
 * The Match provides access to each element for which a match was tried
 * in terms of block:track:version.
 */
class Match
{

public:

	/**
	 * Virtual default destructor
	 */
	virtual ~Match() noexcept = default;

	/**
	 * Marks the ARId of block \c b as verified.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \return Index position to store the verification flag
	 */
	virtual uint32_t verify_id(const uint32_t b)
	= 0;

	/**
	 * TRUE iff the ARId of block \c b is equal to the ARId of
	 * \c result, otherwise FALSE.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \throws Iff \c b is out of range
	 *
	 * \return TRUE iff the ARId of block \c b matches the ARId of \c
	 * result
	 */
	virtual bool id(const uint32_t b) const
	= 0;

	/**
	 * Verifies a single ARCS of the specified track.
	 *
	 * \param[in] b  0-based index of the block to verify in \c response
	 * \param[in] t  0-based index of the track to verify in \c response
	 * \param[in] v2 Verifies the ARCSv2 iff TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b or \c t are out of range
	 *
	 * \return Index position to store the verification flag
	 */
	virtual uint32_t verify_track(const uint32_t b, const uint8_t t,
			const bool v2)
	= 0;

	/**
	 * Return the verification status of an ARCS of the specified track.
	 *
	 * \param[in] b  0-based index of the block to verify in \c response
	 * \param[in] t  0-based index of the track to verify in \c response
	 * \param[in] v2 Returns the ARCSv2 flag iff TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b or \c t are out of range
	 *
	 * \return Flag for ARCS of track \c t in block \c b
	 */
	virtual bool track(const uint32_t b, const uint8_t t, const bool v2) const
	= 0;

	/**
	 * Returns the difference for block \c b .
	 *
	 * The difference is the number of ARCSs in \c b that do not match their
	 * corresponsing positions in the original result<tt>+ 1</tt> iff the
	 * ARId of \c b does not match the ARId of the result.
	 *
	 * \param[in] b  0-based index of the block to verify in \c response
	 * \param[in] v2 Returns the ARCSv2 iff TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b is out of range
	 *
	 * \return Difference of block \c b
	 */
	virtual uint32_t difference(const uint32_t b, const bool v2) const
	= 0;

	/**
	 * Returns the number of compared blocks.
	 *
	 * \return Total number of compared blocks.
	 */
	virtual uint32_t total_blocks() const
	= 0;

	/**
	 * Returns the number of compared tracks per block.
	 *
	 * \return Total number of tracks per block.
	 */
	virtual uint8_t tracks_per_block() const
	= 0;

	/**
	 * Returns the number of comparison flags stored.
	 *
	 * \return Number of flags stored
	 */
	virtual size_t size() const
	= 0;

	/**
	 * Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	virtual std::unique_ptr<Match> clone() const
	= 0;
};


/**
 * Analyzes whether given <tt>Checksum</tt>s match a given ARResponse.
 *
 * The Matcher indicates whether a matching block was found, returns
 * the best difference value and the index position of the best matching block
 * in the ARResponse and whether the match occurred for ARCSv1 or ARCSv2.
 */
class Matcher
{

public:

	/**
	 * Virtual default destructor
	 */
	virtual ~Matcher() noexcept = default;

	/**
	 * Returns TRUE iff at least one block in \c response is identical to either
	 * the ARCSs v1 or the ARCSs v2 in \c result.
	 *
	 * \return TRUE if \c response contains a block matching \c result
	 */
	virtual bool matches() const
	= 0;

	/**
	 * Returns the 0-based index of the best matching block in \c response.
	 *
	 * \return 0-based index of the best matching block in \c response
	 */
	virtual uint32_t best_match() const
	= 0;

	/**
	 * Returns the difference value of the ARBlock with index
	 * \c best_match().
	 *
	 * The difference is the sum of the number of non-matching ARCSs and the
	 * number of non-matching <tt>ARId</tt>s in a single block. A block whose
	 * ARId does not match the ARId of the result has therefore at
	 * least a difference of \c 1 to the result.
	 *
	 * \return Difference value of best block
	 */
	virtual int best_difference() const
	= 0;

	/**
	 * Returns TRUE iff the ARBlock with index \c best_match() matches
	 * the ARCSsv2 of \c result, otherwise FALSE.
	 *
	 * \return TRUE if \c best_match() was a match to the ARCSsv2
	 */
	virtual bool matches_v2() const
	= 0;

	/**
	 * Returns the actual match information
	 *
	 * \return The actual match information.
	 */
	virtual const Match * match() const
	= 0;

	/**
	 * Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	virtual std::unique_ptr<Matcher> clone() const
	= 0;
};


/**
 * A Matcher for a list of track <tt>Checksum</tt>s.
 *
 * Tries to match each position \c i in the actual <tt>Checksum</tt>s with
 * position \c i in the AccurateRip response.
 */
class ListMatcher final : public Matcher
{

public:

	/**
	 * Default constructor
	 *
	 * \param[in] checksums The checksums to match
	 * \param[in] id        The ARId to match
	 * \param[in] response  The AccurateRip response to be matched
	 */
	ListMatcher(const Checksums &checksums, const ARId &id,
			const ARResponse &response);

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	ListMatcher(const ListMatcher &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	ListMatcher(ListMatcher &&rhs) noexcept;

	/**
	 * Default destructor
	 */
	~ListMatcher() noexcept override;

	bool matches() const override;

	uint32_t best_match() const override;

	int best_difference() const override;

	bool matches_v2() const override;

	const Match * match() const override;

	/**
	 * Copy assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	ListMatcher& operator = (const ListMatcher &rhs);

	/**
	 * Move assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	ListMatcher& operator = (ListMatcher &&rhs) noexcept;

	std::unique_ptr<Matcher> clone() const override;


private:

	// forward declaration
	class Impl;

	/**
	 * Private implementation
	 */
	std::unique_ptr<ListMatcher::Impl> impl_;
};


/**
 * A Matcher for a set of file <tt>Checksum</tt>s.
 *
 * Find any match of any actual Checksum in the ARResponse.
 */
class AnyMatcher final : public Matcher
{

public:

	/**
	 * Default constructor
	 *
	 * \param[in] checksums The checksums to match
	 * \param[in] response  The AccurateRip response to be matched
	 */
	AnyMatcher(const Checksums &checksums, const ARResponse &response);

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	AnyMatcher(const AnyMatcher &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	AnyMatcher(AnyMatcher &&rhs) noexcept;

	/**
	 * Default destructor
	 */
	~AnyMatcher() noexcept override;

	bool matches() const override;

	uint32_t best_match() const override;

	int best_difference() const override;

	bool matches_v2() const override;

	const Match * match() const override;

	/**
	 * Copy assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	AnyMatcher& operator = (const AnyMatcher &rhs);

	/**
	 * Move assignment
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	AnyMatcher& operator = (AnyMatcher &&rhs) noexcept;

	std::unique_ptr<Matcher> clone() const override;


private:

	// forward declaration
	class Impl;

	/**
	 * Private implementation
	 */
	std::unique_ptr<AnyMatcher::Impl> impl_;
};


} // namespace v_1_0_0

/// @}

} // namespace arcs

#endif

