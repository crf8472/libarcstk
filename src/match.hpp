#ifndef __LIBARCSTK_MATCH_HPP__
#define __LIBARCSTK_MATCH_HPP__

/** \file
 *
 * \brief Public API for AccurateRip checksum matching.
 *
 * @link arcstk::v_1_0_0::Matcher Matcher @endlink provides an interface to
 * match the ARCSs of some audio input against a response from AccurateRip.
 *
 * A @link arcstk::v_1_0_0::Matcher Matcher @endlink returns a
 * @link arcstk::v_1_0_0::Match Match @endlink that represents a matrix of
 * numeric comparisons: the @link arcstk::v_1_0_0::Checksums Checksums @endlink
 * are compared to each @link arcstk::v_1_0_0::ARBlock ARBlock @endlink in the
 * @link arcstk::v_1_0_0::ARResponse ARResponse @endlink.
 *
 * @link arcstk::v_1_0_0::Match::track(int, int, bool) const Match::track(block, track, isV2) @endlink
 * provides access to any single comparison by its block index, track index and
 * ARCS algorithm version
 *
 * Provided are two @link arcstk::v_1_0_0::Matcher Matcher @endlink
 * implementations.
 *
 * @link arcstk::v_1_0_0::AlbumMatcher AlbumMatcher @endlink matches each
 * checksum in a list of track-based
 * @link arcstk::v_1_0_0::Checksums Checksums @endlink against the value of the
 * corresponding track in each
 * @link arcstk::v_1_0_0::ARBlock ARBlock @endlink of the
 * @link arcstk::v_1_0_0::ARResponse ARResponse @endlink. This
 * @link arcstk::v_1_0_0::Matcher Matcher @endlink
 * can be used for matching the
 * @link arcstk::v_1_0_0::Checksums Checksums @endlink of a complete disc image.
 *
 * The @link arcstk::v_1_0_0::TracksetMatcher TracksetMatcher @endlink matches
 * a set of file-based
 * @link arcstk::v_1_0_0::Checksums Checksums @endlink against an
 * @link arcstk::v_1_0_0::ARResponse ARResponse @endlink by trying to match each
 * of the
 * @link arcstk::v_1_0_0::Checksums Checksums @endlink against each of the sums
 * in each @link arcstk::v_1_0_0::ARBlock ARBlock @endlink. It is used for
 * matching a set of track files in arbitrary order.
 */

#include <cstdint>
#include <cstddef>
#include <memory>

namespace arcstk
{
inline namespace v_1_0_0
{

// forward declarations
class ARId;
class ARResponse;
class Checksums;

/**
 * \defgroup match AccurateRip Checksum Matcher
 * @{
 */

/**
 * \brief Interface: Result of a match of Checksums against an ARResponse.
 *
 * The Match is the result produced by a Matcher. It reports any matching
 * operation the Matcher has performed. Access to each of these results is
 * provided in terms of <tt>block:track:version</tt>. The <tt>block</tt> and
 * <tt>track</tt> address components are integers, while <tt>version</tt> is a
 * boolean that indicates whether the match is for ARCSv2 (TRUE) or for ARCSv1
 * (FALSE).
 *
 * Match indicates whether a matching block was found, returns the best
 * difference value, the index position of the best matching block in the
 * ARResponse and whether the match occurred for ARCSv1 or ARCSv2.
 */
class Match
{

public:

	/**
	 * \brief Virtual default destructor
	 */
	virtual ~Match() noexcept;

	/**
	 * \brief Marks the ARId of the specified block as 'verified'.
	 *
	 * \param[in] block 0-based index of the block to verify
	 *
	 * \return Index position to store the verification flag
	 */
	int verify_id(int block);

	/**
	 * \brief TRUE iff the ARId of the specified block matches the ARId of
	 * the original request, otherwise FALSE.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \throws Iff \c b is out of range
	 *
	 * \return TRUE iff the ARId of block \c b matches the ARId of the request
	 */
	bool id(int b) const;

	/**
	 * \brief Marks a single ARCS of the specified track as 'verified'.
	 *
	 * \param[in] b  0-based index of the block to verify in the ARResponse
	 * \param[in] t  0-based index of the track to verify in the ARResponse
	 * \param[in] v2 Verifies the ARCSv2 iff TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b or \c t are out of range
	 *
	 * \return Index position to store the verification flag
	 */
	int verify_track(int b, int t, bool v2);

	/**
	 * \brief Return the verification status of an ARCS of the specified track.
	 *
	 * @m_class{m-block m-success}
	 *
	 * @par Example:
	 * The call <tt>myMatch.value(0,17,true)</tt> refers to the ARCSv2 of track
	 * 18 in the first block. If this call returns \c true, the first ARBlock in
	 * the ARResponse has a match on this track.
	 *
	 * \param[in] b  0-based index of the block to verify in the ARResponse
	 * \param[in] t  0-based index of the track to verify in the ARResponse
	 * \param[in] v2 Returns the ARCSv2 flag iff TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b or \c t are out of range
	 *
	 * \return Flag for ARCS of track \c t in block \c b
	 */
	bool track(int b, int t, bool v2) const;

	/**
	 * \brief Returns the difference for block \c b .
	 *
	 * The difference is the number of ARCSs in \c b that do not match their
	 * corresponsing positions in the request. The difference is only of
	 * relevance iff the ARId of \c b matchs the ARId of the request.
	 *
	 * \param[in] b  0-based index of the block to verify in the ARResponse
	 * \param[in] v2 Returns the ARCSv2 iff TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b is out of range
	 *
	 * \return Difference of block \c b
	 */
	int64_t difference(int b, bool v2) const;

	/**
	 * \brief Returns the number of compared blocks.
	 *
	 * \return Total number of compared blocks.
	 */
	int total_blocks() const;

	/**
	 * \brief Returns the number of compared tracks per block.
	 *
	 * \return Total number of tracks per block.
	 */
	int tracks_per_block() const;

	/**
	 * \brief Returns the number of comparison flags stored.
	 *
	 * The size of a Match is @f$b * (2 * t + 1)@f$ with @f$b@f$ being
	 * \c total_blocks() and @f$t@f$ being \c tracks_per_block(). The @f$+1@f$
	 * is added since for each block ARId also contributes a
	 * verification flag to the Match.
	 *
	 * \return Number of flags stored
	 */
	size_t size() const;

	/**
	 * \brief Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	std::unique_ptr<Match> clone() const;


private:

	/**
	 * \brief Implements @link Match::verify_id(int b) verify_id(int) @endlink.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \return Index position to store the verification flag
	 */
	virtual int do_verify_id(const int b)
	= 0;

	/**
	 * \brief Implements @link Match::id(int b) const id(int) @endlink.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \throws Iff \c b is out of range
	 *
	 * \return TRUE iff the ARId of block \c b matches the ARId of \c
	 * result
	 */
	virtual bool do_id(const int b) const
	= 0;

	/**
	 * \brief Implements @link Match::verify_track(int b, int t, bool v2) verify_track(b, t, v2) @endlink.
	 *
	 * \param[in] b  0-based index of the block to verify in \c response
	 * \param[in] t  0-based index of the track to verify in \c response
	 * \param[in] v2 Verifies the ARCSv2 iff TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b or \c t are out of range
	 *
	 * \return Index position to store the verification flag
	 */
	virtual int do_verify_track(const int b, const int t, const bool v2)
	= 0;

	/**
	 * \brief Implements @link Match::track(int b, int t, bool v2) const track(int, int, bool) @endlink.
	 *
	 * \param[in] b  0-based index of the block to verify in \c response
	 * \param[in] t  0-based index of the track to verify in \c response
	 * \param[in] v2 Returns the ARCSv2 flag iff TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b or \c t are out of range
	 *
	 * \return Flag for ARCS of track \c t in block \c b
	 */
	virtual bool do_track(const int b, const int t, const bool v2) const
	= 0;

	/**
	 * \brief Implements @link Match::difference(int b, bool v2) const difference(int, bool) @endlink.
	 *
	 * \param[in] b  0-based index of the block to verify in \c response
	 * \param[in] v2 Returns the ARCSv2 iff TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b is out of range
	 *
	 * \return Difference of block \c b
	 */
	virtual int64_t do_difference(const int b, const bool v2) const
	= 0;

	/**
	 * \brief Implements @link Match::total_blocks() const total_blocks() @endlink.
	 *
	 * \return Total number of compared blocks.
	 */
	virtual int do_total_blocks() const
	= 0;

	/**
	 * \brief Implements @link Match::tracks_per_block() const tracks_per_block() @endlink.
	 *
	 * \return Total number of tracks per block.
	 */
	virtual int do_tracks_per_block() const
	= 0;

	/**
	 * \brief Implement @link Match::size() const size() @endlink.
	 *
	 * \return Number of flags stored
	 */
	virtual size_t do_size() const
	= 0;

	/**
	 * \brief Implements @link Match::clone() clone() @endlink.
	 *
	 * \return Deep copy of this instance.
	 */
	virtual std::unique_ptr<Match> do_clone() const
	= 0;
};


/**
 * \brief Interface: Try to Match Checksums against a specified ARResponse.
 */
class Matcher
{

public:

	/**
	 * \brief Virtual default destructor
	 */
	virtual ~Matcher() noexcept;

	/**
	 * \brief Returns TRUE iff at least one block in the ARResponse is
	 * identical to either the ARCSs v1 or the ARCSs v2 in the request.
	 *
	 * \return TRUE if \c response contains a block matching \c result
	 */
	bool matches() const;

	/**
	 * \brief Returns the 0-based index of the best matching block in the
	 * ARResponse.
	 *
	 * \return 0-based index of the best matching block in \c response
	 */
	int best_match() const;

	/**
	 * \brief Returns the difference value of the ARBlock with index
	 * \c best_match().
	 *
	 * The difference is the sum of the number of non-matching ARCSs and the
	 * number of non-matching @link ARId ARIds @endlink in a single block. A
	 * block whose ARId does not match the ARId of the result has therefore at
	 * least a difference of \c 1 to the result.
	 *
	 * \return Difference value of best block
	 */
	int best_difference() const;

	/**
	 * \brief Returns TRUE iff the ARBlock with index \c best_match() matches
	 * the ARCSsv2 of the request, otherwise FALSE.
	 *
	 * \return TRUE if \c best_match() was a match to the ARCSsv2 in the ARResponse
	 */
	bool matches_v2() const;

	/**
	 * \brief Returns the actual match information.
	 *
	 * \return The actual match information.
	 */
	const Match* match() const;

	/**
	 * \brief Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	std::unique_ptr<Matcher> clone() const;


private:

	/**
	 * \brief Implements @link Matcher::matches() const matches() @endlink.
	 *
	 * \return TRUE if \c response contains a block matching \c result
	 */
	virtual bool do_matches() const
	= 0;

	/**
	 * \brief Implements @link Matcher::best_match() const best_match() @endlink.
	 *
	 * \return 0-based index of the best matching block in \c response
	 */
	virtual int do_best_match() const
	= 0;

	/**
	 * \brief Implements @link Matcher::best_difference() const best_difference() @endlink.
	 *
	 * \return Difference value of best block
	 */
	virtual int do_best_difference() const
	= 0;

	/**
	 * \brief Implements @link Matcher::matches_v2() const matches_v2() @endlink.
	 *
	 * \return TRUE if \c best_match() was a match to the ARCSsv2
	 */
	virtual bool do_matches_v2() const
	= 0;

	/**
	 * \brief Implements @link Matcher::match() const match() @endlink.
	 *
	 * \return The actual match information.
	 */
	virtual const Match* do_match() const
	= 0;

	/**
	 * \brief Implements @link Matcher::clone() const clone() @endlink.
	 *
	 * \return Deep copy of this instance.
	 */
	virtual std::unique_ptr<Matcher> do_clone() const
	= 0;
};


/**
 * \brief Match an album track list against an ARResponse.
 *
 * Tries to match each position \c i in the actual
 * @link Checksum Checksums @endlink with position \c i in each block of the
 * AccurateRip response. This is how an entire album can be matched.
 */
class AlbumMatcher final : public Matcher
{

public:

	/**
	 * \brief Default constructor.
	 *
	 * \param[in] checksums The checksums to match
	 * \param[in] id        The ARId to match
	 * \param[in] response  The AccurateRip response to be matched
	 */
	AlbumMatcher(const Checksums &checksums, const ARId &id,
			const ARResponse &response);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	AlbumMatcher(const AlbumMatcher &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	AlbumMatcher(AlbumMatcher &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~AlbumMatcher() noexcept final;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The instance after the assigment
	 */
	AlbumMatcher& operator = (const AlbumMatcher &rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The instance after the assigment
	 */
	AlbumMatcher& operator = (AlbumMatcher &&rhs) noexcept;


private:

	bool do_matches() const final;

	int do_best_match() const final;

	int do_best_difference() const final;

	bool do_matches_v2() const final;

	const Match* do_match() const final;

	std::unique_ptr<Matcher> do_clone() const final;

	// forward declaration
	class Impl;

	/**
	 * Private implementation
	 */
	std::unique_ptr<AlbumMatcher::Impl> impl_;
};


/**
 * \brief Match an arbitrary set of tracks against an ARResponse.
 *
 * Find any match of any actual Checksum in the ARResponse. This targets the
 * situation where a subset of tracks from the same album are tried to be
 * matched, but the subset may be incomplete.
 *
 * @m_class{m-block m-success}
 *
 * @par Example:
 * You have some or all tracks but you do not know their order. You can use the
 * TracksetMatcher to find out the order of the tracks.
 */
class TracksetMatcher final : public Matcher
{

public:

	/**
	 * \brief Default constructor.
	 *
	 * \param[in] checksums The checksums to match
	 * \param[in] response  The AccurateRip response to be matched
	 */
	TracksetMatcher(const Checksums &checksums, const ARResponse &response);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	TracksetMatcher(const TracksetMatcher &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	TracksetMatcher(TracksetMatcher &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~TracksetMatcher() noexcept final;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TracksetMatcher& operator = (const TracksetMatcher &rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TracksetMatcher& operator = (TracksetMatcher &&rhs) noexcept;


private:

	bool do_matches() const final;

	int do_best_match() const final;

	int do_best_difference() const final;

	bool do_matches_v2() const final;

	const Match* do_match() const final;

	std::unique_ptr<Matcher> do_clone() const final;

	// forward declaration
	class Impl;

	/**
	 * \brief Private implementation
	 */
	std::unique_ptr<TracksetMatcher::Impl> impl_;
};

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

