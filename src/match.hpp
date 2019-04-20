#ifndef __LIBARCSTK_MATCH_HPP__
#define __LIBARCSTK_MATCH_HPP__


/**
 * \file
 *
 * \brief Public API for AccurateRip checksum matching.
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

// forward declarations
namespace arcstk { inline namespace v_1_0_0 { class ARId;       } }
namespace arcstk { inline namespace v_1_0_0 { class ARResponse; } }
namespace arcstk { inline namespace v_1_0_0 { class Checksums;  } }


namespace arcstk
{

inline namespace v_1_0_0
{

/// \defgroup match AccurateRip Checksum Matcher
/// @{


/**
 * \brief Interface: Result of a match of Checksums against an ARResponse.
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
	virtual ~Match() noexcept;

	/**
	 * Marks the ARId of block \c b as verified.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \return Index position to store the verification flag
	 */
	uint32_t verify_id(const uint32_t b);

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
	bool id(const uint32_t b) const;

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
	uint32_t verify_track(const uint32_t b, const uint8_t t, const bool v2);

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
	bool track(const uint32_t b, const uint8_t t, const bool v2) const;

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
	uint32_t difference(const uint32_t b, const bool v2) const;

	/**
	 * Returns the number of compared blocks.
	 *
	 * \return Total number of compared blocks.
	 */
	int total_blocks() const;

	/**
	 * Returns the number of compared tracks per block.
	 *
	 * \return Total number of tracks per block.
	 */
	int tracks_per_block() const;

	/**
	 * Returns the number of comparison flags stored.
	 *
	 * \return Number of flags stored
	 */
	size_t size() const;

	/**
	 * Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	std::unique_ptr<Match> clone() const;


private:

	/**
	 * Marks the ARId of block \c b as verified.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \return Index position to store the verification flag
	 */
	virtual uint32_t do_verify_id(const uint32_t b)
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
	virtual bool do_id(const uint32_t b) const
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
	virtual uint32_t do_verify_track(const uint32_t b, const uint8_t t,
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
	virtual bool do_track(const uint32_t b, const uint8_t t, const bool v2)
		const
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
	virtual uint32_t do_difference(const uint32_t b, const bool v2) const
	= 0;

	/**
	 * Returns the number of compared blocks.
	 *
	 * \return Total number of compared blocks.
	 */
	virtual int do_total_blocks() const
	= 0;

	/**
	 * Returns the number of compared tracks per block.
	 *
	 * \return Total number of tracks per block.
	 */
	virtual int do_tracks_per_block() const
	= 0;

	/**
	 * Returns the number of comparison flags stored.
	 *
	 * \return Number of flags stored
	 */
	virtual size_t do_size() const
	= 0;

	/**
	 * Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	virtual std::unique_ptr<Match> do_clone() const
	= 0;
};


/**
 * \brief Interface: Try to match given Checksums against a specified
 * ARResponse.
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
	virtual ~Matcher() noexcept;

	/**
	 * Returns TRUE iff at least one block in \c response is identical to either
	 * the ARCSs v1 or the ARCSs v2 in \c result.
	 *
	 * \return TRUE if \c response contains a block matching \c result
	 */
	bool matches() const;

	/**
	 * Returns the 0-based index of the best matching block in \c response.
	 *
	 * \return 0-based index of the best matching block in \c response
	 */
	uint32_t best_match() const;

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
	int best_difference() const;

	/**
	 * Returns TRUE iff the ARBlock with index \c best_match() matches
	 * the ARCSsv2 of \c result, otherwise FALSE.
	 *
	 * \return TRUE if \c best_match() was a match to the ARCSsv2
	 */
	bool matches_v2() const;

	/**
	 * Returns the actual match information
	 *
	 * \return The actual match information.
	 */
	const Match * match() const;

	/**
	 * Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	std::unique_ptr<Matcher> clone() const;


private:

	/**
	 * Returns TRUE iff at least one block in \c response is identical to either
	 * the ARCSs v1 or the ARCSs v2 in \c result.
	 *
	 * \return TRUE if \c response contains a block matching \c result
	 */
	virtual bool do_matches() const
	= 0;

	/**
	 * Returns the 0-based index of the best matching block in \c response.
	 *
	 * \return 0-based index of the best matching block in \c response
	 */
	virtual uint32_t do_best_match() const
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
	virtual int do_best_difference() const
	= 0;

	/**
	 * Returns TRUE iff the ARBlock with index \c best_match() matches
	 * the ARCSsv2 of \c result, otherwise FALSE.
	 *
	 * \return TRUE if \c best_match() was a match to the ARCSsv2
	 */
	virtual bool do_matches_v2() const
	= 0;

	/**
	 * Returns the actual match information
	 *
	 * \return The actual match information.
	 */
	virtual const Match * do_match() const
	= 0;

	/**
	 * Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	virtual std::unique_ptr<Matcher> do_clone() const
	= 0;
};


/**
 * \brief Match an album track list against an ARResponse.
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
	~ListMatcher() noexcept final;

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


private:

	bool do_matches() const final;

	uint32_t do_best_match() const final;

	int do_best_difference() const final;

	bool do_matches_v2() const final;

	const Match* do_match() const final;

	std::unique_ptr<Matcher> do_clone() const final;

	// forward declaration
	class Impl;

	/**
	 * Private implementation
	 */
	std::unique_ptr<ListMatcher::Impl> impl_;
};


/**
 * \brief Match an arbitrary set of tracks against an ARResponse.
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


private:

	bool do_matches() const final;

	uint32_t do_best_match() const final;

	int do_best_difference() const final;

	bool do_matches_v2() const final;

	const Match* do_match() const final;

	std::unique_ptr<Matcher> do_clone() const final;

	// forward declaration
	class Impl;

	/**
	 * Private implementation
	 */
	std::unique_ptr<AnyMatcher::Impl> impl_;
};


/// @}

} // namespace v_1_0_0

} // namespace arcstk

#endif

