#ifndef __LIBARCSTK_MATCH_HPP__
#define __LIBARCSTK_MATCH_HPP__

/** \file
 *
 * \brief Public API for \link match AccurateRip checksum matching \endlink.
 *
 * \details
 *
 * Includes the module for \link match matching AccurateRip checksums and
 * ids\endlink.
 *
 * Fine-granular comparison between an AccurateRip response and locally computed
 * checksums.
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
 * \defgroup match AccurateRip Checksum Matching
 *
 * \brief Match local Checksums against an \link ARResponse
 * AccurateRip response \endlink.
 *
 * \details
 *
 * Matcher provides an interface to match the ARCSs of some audio input against
 * a response from AccurateRip.
 *
 * A Matcher returns a Match that represents a matrix of numeric comparisons:
 * the result of matching the Checksums to each ARBlock in the ARResponse.
 *
 * While Matcher implements the comparison strategy, the Match implements
 * the result of the comparison.
 *
 * \link arcstk::v_1_0_0::Match::track(int, int, bool) const
 * Match::track(block, track, isV2) \endlink
 * provides access to any single comparison by its block index, track index and
 * ARCS algorithm version.
 *
 * Provided are two Matcher implementations.
 *
 * AlbumMatcher matches each checksum in a list of track-based Checksums against
 * the value of the corresponding track in each ARBlock of the ARResponse. This
 * implements the verification process of a complete disc image. AlbumMatcher
 * requires an ARId and respects it in the match against the ARBlock.
 *
 * TracksetMatcher matches a set of file-based Checksums against an ARResponse
 * by trying to match each of the Checksums against \em each of the sums
 * in each ARBlock. It is used for matching a set of track files in arbitrary
 * order. An ARId is optional.
 *
 * @{
 */

/**
 * \brief Interface: Result of matching Checksums against an ARResponse.
 *
 * A Match is the result of checking given Checksums against an ARResponse.
 * A Match is produced by instantiating a Matcher. It holds the
 * result of any matching operation the Matcher has performed. Access to each of
 * these results is provided in terms of <tt>block:track:version</tt>. The
 * <tt>block</tt> and <tt>track</tt> address components are integers, while
 * <tt>version</tt> is a boolean that indicates whether the match is for ARCSv2
 * (TRUE) or for ARCSv1 (FALSE).
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
	 * \brief Mark the ARId of the specified block as 'matched'.
	 *
	 * \param[in] block 0-based index of the block to verify
	 *
	 * \throws std::runtime_error Iff \c b is out of range
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
	 * \throws std::runtime_error Iff \c b is out of range
	 *
	 * \return TRUE iff the ARId of block \c b matches the ARId of the request
	 */
	bool id(int b) const;

	/**
	 * \brief Mark the checksum of a specified track in a specified block as
	 * 'matched'.
	 *
	 * \param[in] b  0-based index of the block to verify in the ARResponse
	 * \param[in] t  0-based index of the track to verify in the ARResponse
	 * \param[in] v2 Verifies the ARCSv2 iff TRUE, otherwise ARCSv1
	 *
	 * \throws std::runtime_error Iff \c b or \c t are out of range
	 *
	 * \return Index position to store the verification flag
	 */
	int verify_track(int b, int t, bool v2);

	/**
	 * \brief Return the verification status of an ARCS of the specified track
	 * in the specified block.
	 *
	 * \note
	 * What a concrete match means is dependent from the logic the Matcher used.
	 * The call <tt>myMatch.value(0,17,true)</tt> refers to the ARCSv2 of track
	 * 18 in the first block. If this call returns \c true, track 18 in the
	 * first ARBlock in the ARResponse was matched by the Matcher. Whether this
	 * indicates that track 18 of the current Checksums caused the match is
	 * implementation defined. If the Match was calculated by an AlbumMatcher,
	 * track 18 of the input Checksums will only be matched against track 18 in
	 * each block. A TracksetMatcher on the other hand will just indicate that
	 * \em one of the input checksums matched track 18.
	 *
	 * \param[in] b  0-based index of the block to verify in the ARResponse
	 * \param[in] t  0-based index of the track to verify in the ARResponse
	 * \param[in] v2 Returns the ARCSv2 flag iff TRUE, otherwise ARCSv1
	 *
	 * \throws std::runtime_error Iff \c b or \c t are out of range
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
	 * \note
	 * The concrete difference value may depend on the Matcher implementation.
	 * For example, consider an album with 15 tracks. An ARBlock with no track
	 * matching and a different ARId will be a difference of 16 assigned if the
	 * Match is produced by an AlbumMatcher. A TracksetMatcher ignores the ARId
	 * and hence it does not contribute to the difference. But therefore, the
	 * exact same ARBlock will have a difference of only 15.
	 *
	 * \param[in] b  0-based index of the block to verify in the ARResponse
	 * \param[in] v2 Returns the ARCSv2 iff TRUE, otherwise ARCSv1
	 *
	 * \throws std::runtime_error Iff \c b is out of range
	 *
	 * \return Difference of block \c b
	 */
	int64_t difference(int b, bool v2) const;

	/**
	 * \brief Returns the number of analyzed blocks.
	 *
	 * This is identical with the total number of blocks in the ARResponse.
	 *
	 * \return Total number of analyzed blocks.
	 */
	int total_blocks() const;

	/**
	 * \brief Returns the number of compared tracks per block.
	 *
	 * This is identical with the total number of tracks in each block in the
	 * ARResponse.
	 *
	 * \return Total number of tracks per block.
	 */
	int tracks_per_block() const;

	/**
	 * \brief Returns the number of comparison flags stored.
	 *
	 * \note
	 * The size of a Match with a number \f$b\f$ of total_blocks() and
	 * \f$t\f$ tracks_per_block() is \f$b * (2 * t + 1)\f$.
	 * The \f$+1\f$ is required since the ARId of each block contributes an
	 * additional verification flag to the Match.
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
	 * \brief Implements \link Match::verify_id(int b) verify_id(int) \endlink.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \return Index position to store the verification flag
	 */
	virtual int do_verify_id(const int b)
	= 0;

	/**
	 * \brief Implements \link Match::id(int b) const id(int) \endlink.
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
	 * \brief Implements
	 * \link Match::verify_track(int b, int t, bool v2)
	 * verify_track(b, t, v2)
	 * \endlink.
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
	 * \brief Implements
	 * \link Match::track(int b, int t, bool v2) const
	 * track(int, int, bool)
	 * \endlink.
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
	 * \brief Implements
	 * \link Match::difference(int b, bool v2) const
	 * difference(int, bool)
	 * \endlink.
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
	 * \brief Implements \link Match::total_blocks() const
	 * total_blocks()
	 * \endlink.
	 *
	 * \return Total number of compared blocks.
	 */
	virtual int do_total_blocks() const
	= 0;

	/**
	 * \brief Implements \link Match::tracks_per_block() const
	 * tracks_per_block()
	 * \endlink.
	 *
	 * \return Total number of tracks per block.
	 */
	virtual int do_tracks_per_block() const
	= 0;

	/**
	 * \brief Implement \link Match::size() const size() \endlink.
	 *
	 * \return Number of flags stored
	 */
	virtual size_t do_size() const
	= 0;

	/**
	 * \brief Implements \link Match::clone() clone() \endlink.
	 *
	 * \return Deep copy of this instance.
	 */
	virtual std::unique_ptr<Match> do_clone() const
	= 0;
};


/**
 * \brief Interface: Try to Match Checksums against a specified ARResponse.
 *
 * A Matcher implements a concrete logic of matching Checksums against a
 * specified ARResponse. This logic defines which comparisons are actually
 * performed, in which order and which input information is actually considered.
 * Different \link Matcher Matchers \endlink may therefore produce
 * \link Match Matches \endlink with different content on identical input.
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
	 * has a \link Match::difference() difference() \endlink of \c 0 to either
	 * the ARCSs v1 or the ARCSs v2 in the request.
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
	 * best_match().
	 *
	 * The difference is the sum of the number of non-matching ARCSs and the
	 * number of non-matching \link ARId ARIds \endlink in a single block. A
	 * block whose ARId does not match the ARId of the result has therefore at
	 * least a difference of \c 1 to the result.
	 *
	 * \return Difference value of best block
	 */
	int best_difference() const;

	/**
	 * \brief Returns TRUE iff the ARBlock with index best_match() matches
	 * the ARCSsv2 of the request, otherwise FALSE.
	 *
	 * \return TRUE if \c best_match() was to the ARCSsv2 in the ARResponse
	 */
	bool matches_v2() const;

	/**
	 * \brief Returns the actual Match.
	 *
	 * \return The actual Match
	 */
	const Match* match() const;

	/**
	 * \brief Clones this instance.
	 *
	 * \return Deep copy of this instance
	 */
	std::unique_ptr<Matcher> clone() const;


private:

	/**
	 * \brief Implements \link Matcher::matches() const matches() \endlink.
	 *
	 * \return TRUE if \c response contains a block matching \c result
	 */
	virtual bool do_matches() const
	= 0;

	/**
	 * \brief Implements \link Matcher::best_match() const
	 * best_match()
	 * \endlink.
	 *
	 * \return 0-based index of the best matching block in \c response
	 */
	virtual int do_best_match() const
	= 0;

	/**
	 * \brief Implements \link Matcher::best_difference() const
	 * best_difference()
	 * \endlink.
	 *
	 * \return Difference value of best block
	 */
	virtual int do_best_difference() const
	= 0;

	/**
	 * \brief Implements \link Matcher::matches_v2() const
	 * matches_v2()
	 * \endlink.
	 *
	 * \return TRUE if \c best_match() was a match to the ARCSsv2
	 */
	virtual bool do_matches_v2() const
	= 0;

	/**
	 * \brief Implements \link Matcher::match() const match() \endlink.
	 *
	 * \return The actual match information.
	 */
	virtual const Match* do_match() const
	= 0;

	/**
	 * \brief Implements \link Matcher::clone() const clone() \endlink.
	 *
	 * \return Deep copy of this instance.
	 */
	virtual std::unique_ptr<Matcher> do_clone() const
	= 0;
};


/**
 * \brief Match an album track list against an ARResponse.
 *
 * \details
 *
 * Tries to match each position \c i in the actual Checksums with position \c i
 * in each ARBlock of the ARResponse. This is how an entire album can be
 * matched.
 */
class AlbumMatcher final : public Matcher
{

public:

	/**
	 * \brief Constructor.
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
 * \details
 *
 * Find any match of any actual Checksum in the ARResponse. This targets the
 * situation where a subset of tracks from the same album are tried to be
 * matched, but the subset may be incomplete.
 *
 * \note
 * The TracksetMatcher is a generalization of the AlbumMatcher. The AlbumMatcher
 * adds the restriction that the order of tracks in the ARResponse must be
 * matched too.
 */
class TracksetMatcher final : public Matcher
{

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] checksums The checksums to match
	 * \param[in] id        The ARId to match
	 * \param[in] response  The AccurateRip response to be matched
	 */
	TracksetMatcher(const Checksums &checksums, const ARId &id,
			const ARResponse &response);

	/**
	 * \brief Constructor.
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

