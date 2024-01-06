#ifndef __LIBARCSTK_VERIFY_HPP__
#define __LIBARCSTK_VERIFY_HPP__

/** \file
 *
 * \brief Public API for \link verify AccurateRip checksum verification\endlink.
 *
 * \details
 *
 * Includes the module for \link verify verifiying AccurateRip checksums and
 * ids\endlink.
 *
 * Fine-granular comparison between reference checksums and locally computed
 * checksums.
 */

#include <sstream>        // for operator<<, basic_ostream::operator<<, basi...
#include <memory>         // for unique_ptr
#include <vector>         // for vector


namespace arcstk
{
inline namespace v_1_0_0
{

class ARId;
class ARResponse;
class Checksum;
class Checksums;

/**
 * \defgroup verify AccurateRip Checksum Verification
 *
 * \brief Verify local Checksums against a ChecksumSource.
 *
 * \details
 *
 * @{
 */

/**
 * \brief Provide unified access method to checksum containers.
 *
 * A checksum container contains several blocks of checksums an in every block
 * an ARId and an ordered sequence of checksums. A single checksum is accessed
 * by a block index in combination with the index of the checksum within the
 * block.
 *
 * A type \c T can be made available as a ChecksumSource via definig a subclass
 * \c S that inherits from ChecksumSourceOf<T>. Subclass \c S has to implement
 * the virtual functions of ChecksumSource. It can reuse the constructor of
 * ChecksumSourceOf<T>.
 *
 * \see ChecksumSourceOf
 * \see FromResponse
 */
class ChecksumSource
{
	virtual ARId do_id(const int block_idx) const
	= 0;

	virtual Checksum do_checksum(const int block_idx, const int idx) const
	= 0;

	virtual int do_confidence(const int block_idx, const int idx) const
	= 0;

	virtual std::size_t do_size(const int block_idx) const
	= 0;

	virtual std::size_t do_size() const
	= 0;

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ChecksumSource() noexcept = default;

	/**
	 * \brief Read id in section with the specified \c block_idx.
	 *
	 * \param[in] block_idx 0-based block index to access
	 *
	 * \return The id of the specified block
	 */
	ARId id(const int block_idx) const;

	/**
	 * \brief Read checksum \c idx in section with the specified \c block_idx.
	 *
	 * \param[in] block_idx 0-based block index to access
	 * \param[in] track_idx 0-based track index to access
	 *
	 * \return The checksum of the specified index position
	 */
	Checksum checksum(const int block_idx, const int track_idx) const;

	/**
	 * \brief Read confidence \c idx in section with the specified \c block_idx.
	 *
	 * \param[in] block_idx 0-based block index to access
	 * \param[in] track_idx 0-based track index to access
	 *
	 * \return The confidence of the specified index position
	 */
	int confidence(const int block_idx, const int track_idx) const;

	/**
	 * \brief Size of the block specified by \c block_idx.
	 *
	 * The number of tracks in this block is its size.
	 *
	 * \param[in] block_idx 0-based block index to access
	 *
	 * \return The size of the specified block
	 */
	std::size_t size(const int block_idx) const;

	/**
	 * \brief Number of blocks.
	 *
	 * Greatest legal block is size() - 1.
	 *
	 * \return Number of blocks in this object
	 */
	std::size_t size() const;
};


/**
 * \brief Wrap a checksum container in a ChecksumSource.
 *
 * \tparam T The type to wrap
 */
template <typename T>
class ChecksumSourceOf : public ChecksumSource
{
	/**
	 * \brief Internal checksum source.
	 */
	const T* checksum_source_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] t The primary checksum source
	 */
	ChecksumSourceOf(const T* t)
		: checksum_source_ { t }
	{
		// empty
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs Instance to get a copy of
	 */
	ChecksumSourceOf(const ChecksumSourceOf& rhs)
		: checksum_source_ { rhs.checksum_source_ }
	{
		// empty
	}

	/**
	 * \brief Copy assignment operator
	 *
	 * \param[in] rhs Instance to get a copy of
	 *
	 * \return Copy of \c rhs
	 */
	ChecksumSourceOf& operator=(const ChecksumSourceOf& rhs)
	{
		checksum_source_ = rhs.checksum_source_;
		return *this;
	}

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ChecksumSourceOf() noexcept = default;

	/**
	 * \brief The wrapped checksum source.
	 *
	 * \return The wrapped checksum source.
	 */
	const T* source() const
	{
		return checksum_source_;
	}
};


/**
 * \brief Access an ARResponse by block and index.
 *
 * Make an ARResponse available for verification.
 */
class FromResponse final : public ChecksumSourceOf<ARResponse>
{
	ARId do_id(const int block_idx) const final;
	Checksum do_checksum(const int block_idx, const int idx) const final;
	int do_confidence(const int block_idx, const int idx) const final;
	std::size_t do_size(const int block_idx) const final;
	std::size_t do_size() const final;

public:

	using ChecksumSourceOf::ChecksumSourceOf;
	using ChecksumSourceOf::operator=;
};


class VerificationResult;


/**
 * \brief Policy for deciding whether a given track is verified or not.
 *
 * The policy decides whether matches only count when occurring in the same
 * block.
 */
class TrackPolicy
{
	virtual bool do_is_verified(const int track, const VerificationResult& r)
		const
	= 0;

	virtual int do_total_unverified_tracks(const VerificationResult& r) const
	= 0;

	virtual bool do_is_strict() const
	= 0;

public:

	/**
	 * \brief Virtual default destructor
	 */
	virtual ~TrackPolicy() noexcept = default;

	/**
	 * \brief TRUE if this instance qualifies the given track as verified.
	 *
	 * The policy may interpret the result data to decide whether \c track is
	 * verified or not.
	 *
	 * \param[in] track The track to check for
	 * \param[in] r     The result to interpret
	 *
	 * \return TRUE if the track counts as verified, otherwise FALSE.
	 */
	bool is_verified(const int track, const VerificationResult& r) const;

	/**
	 * \brief Total number of unverified tracks in the result.
	 *
	 * \param[in] r The result to interpret
	 *
	 * \return Total number of unverified tracks
	 */
	int total_unverified_tracks(const VerificationResult& r) const;

	/**
	 * \brief TRUE iff this policy is strict.
	 *
	 * A strict policy will count a track as verified iff its checksum
	 * in the best block will match the actual checksum. Matching checksums
	 * in other blocks will be ignored by a strict policy but respected by a
	 * non-strict policy.
	 *
	 * \return TRUE iff this policy is strict.
	 */
	bool is_strict() const;
};


/**
 * \brief Print a VerificationResult to a stream
 */
std::ostream& operator << (std::ostream&, const VerificationResult& r);


/**
 * \brief Interface: Result of a verification process.
 *
 * \details
 *
 * A VerificationResult is the result of a complete matching of actual Checksums
 * against a ChecksumSource of referene checksums.
 *
 * It holds the result of any matching operation between a pair of Checksums
 * that was performed. Access to each of these results is provided in terms of
 * <tt>block:track:version</tt>. The <tt>block</tt> and <tt>track</tt> address
 * components are integers, that refer to the respective 0-based block and
 * 0-based track in the ChecksumSource. The <tt>version</tt> is a boolean that
 * indicates whether the match is for ARCSv2 (\c TRUE) or for ARCSv1 (\c FALSE).
 *
 * The result contains also the track-based interpretation of the flags, i.e.
 * whether a given track is considered to be verified or not.
 */
class VerificationResult
{
	friend std::ostream& operator << (std::ostream&,
			const VerificationResult &match);

public:

	/**
	 * \brief Virtual default destructor
	 */
	virtual ~VerificationResult() noexcept;

	/**
	 * \brief TRUE iff each track is verified.
	 *
	 * This is shorthand for checking whether total_unverified_tracks is 0.
	 * The verification should be considered as successful and iff this returns
	 * TRUE.
	 *
	 * A rip for which all tracks are verified can be considered accurate
	 * relative to the applied verification method. Iff the method is strict,
	 * then there is a single block in the ChecksumSource that is identical
	 * with the list of actual Checksums. Iff the method is non-strict, every
	 * actual Checksum occurred at least in one block of the ChecksumSource.
	 *
	 * \return TRUE iff each track is verified otherwise FALSE
	 */
	bool all_tracks_verified() const;

	/**
	 * \brief Total number of unverified tracks.
	 *
	 * \return Total number of unverified tracks.
	 */
	int total_unverified_tracks() const;

	/**
	 * \brief TRUE iff specified 0-based track is verified, otherwise FALSE.
	 *
	 * \param[in] track 0-based track
	 *
	 * \return TRUE iff specified track is verified, otherwise FALSE
	 */
	bool is_verified(const int track) const;

	/**
	 * \brief Mark the checksum of a specified track in a specified block as
	 * 'matched'.
	 *
	 * \param[in] b  0-based index of the block to verify in the ChecksumSource
	 * \param[in] t  0-based index of the track to verify in the ChecksumSource
	 * \param[in] v2 Verifies the ARCSv2 iff \c TRUE, otherwise ARCSv1
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
	 * The call <tt>myVerificationResult.value(0,17,true)</tt> refers to the
	 * ARCSv2 of track 18 in the first block. If this call returns \c true,
	 * track 18 in the first block in the \c ref_sums was matched. Whether this
	 * indicates that track 18 of the actual Checksums caused the match is
	 * implementation defined and depends on the \c MatchOrder. It can also
	 * just indicate that \em one of the actual Checksums matched track 18.
	 *
	 * \param[in] b  0-based index of the block to verify in the ChecksumSource
	 * \param[in] t  0-based index of the track to verify in the ChecksumSource
	 * \param[in] v2 Returns the ARCSv2 flag iff \c TRUE, otherwise ARCSv1
	 *
	 * \throws std::runtime_error Iff \c b or \c t are out of range
	 *
	 * \return Flag for ARCS of track \c t in block \c b
	 */
	bool track(int b, int t, bool v2) const;

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
	 * \brief \c TRUE iff the ARId of the specified block matches the ARId of
	 * the original request, otherwise \c FALSE.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \throws std::runtime_error Iff \c b is out of range
	 *
	 * \return \c TRUE iff the ARId of block \c b matches the ARId of the
	 *         request
	 */
	bool id(int b) const;

	/**
	 * \brief Returns the difference for block \c b .
	 *
	 * The difference is the number of ARCSs in \c b that do not match their
	 * corresponsing positions in the request. The difference is only of
	 * relevance iff the ARId of \c b matchs the ARId of the request.
	 *
	 * \note
	 * The concrete difference value may depend on the implementation. For
	 * example, consider an album with 15 tracks. A block of reference checksums
	 * with each track not matching and a non-matching ARId will be a assigned
	 * a difference of 16 by a strict TrackPolicy when respecting the ARId.
	 * When ignoring the ARId and it does not contribute to the difference.
	 * In this case, the exact same block will have a difference of only 15.
	 *
	 * \param[in] b  0-based index of the block to verify in the ChecksumSource
	 * \param[in] v2 Returns the ARCSv2 iff \c TRUE, otherwise ARCSv1
	 *
	 * \throws std::runtime_error Iff \c b is out of range
	 *
	 * \return Difference of block \c b
	 */
	int difference(int b, bool v2) const;

	/**
	 * \brief Returns the number of analyzed reference blocks.
	 *
	 * This is identical with the total number of blocks in the ChecksumSource.
	 *
	 * \return Total number of analyzed blocks.
	 */
	int total_blocks() const;

	/**
	 * \brief Returns the number of compared tracks per reference block.
	 *
	 * This is identical with the total number of tracks in each block in the
	 * ChecksumSource.
	 *
	 * \return Total number of tracks per block.
	 */
	int tracks_per_block() const;

	/**
	 * \brief Returns the number of comparison flags stored.
	 *
	 * \note
	 * The size of a VerificationResult with a number \f$b\f$ of total_blocks()
	 * and \f$t\f$ tracks_p:er_block() is \f$b * (2 * t + 1)\f$. The coefficient
	 * \f$2\f$ is required since each block is matched against ARCSv1 and
	 * ARCSv2 sums. The \f$+1\f$ is required since the ARId of each block
	 * contributes an additional verification flag to the VerificationResult.
	 *
	 * \return Number of flags stored
	 */
	size_t size() const;

	/**
	 * \brief Identify best matching block (the one with smallest difference).
	 *
	 * If there is more than one block with the smallest difference, return the
	 * one with the lowest index position.
	 *
	 * \return 0-based index, ARCS version, and difference of the best block
	 */
	std::tuple<int, bool, int> best_block() const;

	/**
	 * \brief Difference of the best block in this result.
	 *
	 * This is shorthand for getting element 2 of \c best_block().
	 *
	 * \return Smallest difference of any block
	 */
	int best_block_difference() const;

	/**
	 * \brief The policy of this instance.
	 *
	 * \return Policy of this instance
	 */
	const TrackPolicy* policy() const;

	/**
	 * \brief Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	std::unique_ptr<VerificationResult> clone() const;

private:

	virtual int do_total_unverified_tracks() const
	= 0;

	virtual bool do_is_verified(const int track) const
	= 0;

	virtual int do_verify_track(const int b, const int t, const bool v2)
	= 0;

	virtual bool do_track(const int b, const int t, const bool v2) const
	= 0;

	virtual int do_verify_id(const int b)
	= 0;

	virtual bool do_id(const int b) const
	= 0;

	virtual int do_difference(const int b, const bool v2) const
	= 0;

	virtual int do_total_blocks() const
	= 0;

	virtual int do_tracks_per_block() const
	= 0;

	virtual size_t do_size() const
	= 0;

	virtual std::tuple<int, bool, int> do_best_block() const
	= 0;

	virtual int do_best_block_difference() const
	= 0;

	virtual const TrackPolicy* do_policy() const
	= 0;

	virtual std::unique_ptr<VerificationResult> do_clone() const
	= 0;
};


class MatchOrder;

/**
 * \brief Defines the traversal method of the reference checksums.
 *
 * The traversal method can e.g. be implemented as an iteration over a single
 * block in the ChecksumSource. Alternatively, it could be implemented as a
 * traversal over the same track in every block.
 */
class MatchTraversal
{
	virtual Checksum do_get_reference(const ChecksumSource& ref_sums,
			const int current, const int counter) const
	= 0;

	virtual std::size_t do_size(const ChecksumSource& ref_sums,
			const int current) const
	= 0;

	virtual void do_traverse(VerificationResult& result,
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource& ref_sums,
		const MatchOrder& order) const
	= 0;

	virtual std::unique_ptr<TrackPolicy> do_get_policy() const
	= 0;

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~MatchTraversal() noexcept = default;

	/**
	 * \brief Provide a Checksum from some current index and some counter index.
	 *
	 * \param[in] ref_sums The source of checksums
	 * \param[in] current  Index of the current set in \c ref_sums
	 * \param[in] counter  Index position within the set indexed by \c current
	 *
	 * \return The checksum under the specified indices
	 */
	Checksum get_reference(const ChecksumSource& ref_sums, const int current,
			const int counter) const;

	/**
	 * \brief Provide the number of checksums under the index \c current.
	 *
	 * \param[in] ref_sums The source of checksums
	 * \param[in] current  Index of the current set in \c ref_sums
	 *
	 * \return The checksum under the specified index
	 */
	std::size_t size(const ChecksumSource& ref_sums, const int current) const;

	/**
	 * \brief Apply the actual traversal logic to \c ref_sums.
	 *
	 * \param[in,out] result      The result object to store the match results
	 * \param[in]     actual_sums Actual checksums to check for
	 * \param[in]     actual_id   Actual ARId to check for
	 * \param[in]     ref_sums    Reference checksums to match against
	 * \param[in]     order       Order to be applied on each traversed item
	 */
	void traverse(VerificationResult& result,
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource& ref_sums,
		const MatchOrder& order) const;

	/**
	 * \brief Create a TrackPolicy for results creates by this Traversal.
	 *
	 * Whether a track is considered to be verified or not depends on the
	 * concrete match that produced the verification flag in the respective
	 * result position. Therefore, the result must know the traversal logic
	 * for the tracks when accessing the flags.
	 *
	 * \return TrackPolicy to interpret track verification in the result.
	 */
	std::unique_ptr<TrackPolicy> get_policy() const;
};


/**
 * \brief Order to match items during a single loop in a traversal.
 *
 * Defines the order in which the actual checksums are to be matched against
 * a current set of reference values.
 */
class MatchOrder
{
	virtual void do_perform(VerificationResult& result,
			const Checksums& actual_sums, const ChecksumSource& ref_sums,
			int index, const MatchTraversal& t) const
	= 0;

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~MatchOrder() noexcept = default;

	/**
	 * \brief Apply the order to index position \c current in \c ref_sums.
	 *
	 * The checksums of \c actual_sums will be matched against the checksums
	 * of \c index in \c ref_sums in the order defined by this instance.
	 * This may be just the order the tracks appear in \c actual_sums or any
	 * other this instance defines.
	 *
	 * Traversal \c t will decide which checksums from \c ref_sums are to
	 * select, especially if \c current is interpreted as a block or a track
	 * index in \c ref_sums.
	 *
	 * \param[in,out] result      The result object to store the match results
	 * \param[in]     actual_sums Actual checksums to check for
	 * \param[in]     ref_sums    Reference checksums to match against
	 * \param[in]     current     Index of the current set in \c ref_sums
	 * \param[in]     t           The traversal to interpret \c index
	 */
	void perform(VerificationResult& result,
			const Checksums& actual_sums, const ChecksumSource& ref_sums,
			int current, const MatchTraversal& t) const;
};


/**
 * \brief Perform a verification.
 *
 * \param[in] actual_sums Actual checksums to check for
 * \param[in] actual_id   Actual ARId to check for
 * \param[in] ref_sums    Reference checksums to match against
 * \param[in] traversal   Traversal that defines which items to traverse
 * \param[in] order       Order to be applied on each traversed item
 *
 * \return The verification result object
 */
std::unique_ptr<VerificationResult> verify(
		const Checksums& actual_sums, const ARId& actual_id,
		const ChecksumSource& ref_sums,
		const MatchTraversal& t, const MatchOrder& o);



/**
 * \brief Service class for performing a verification.
 */
class Verifier
{
	virtual const ARId& do_actual_id() const;

	virtual std::unique_ptr<MatchTraversal> do_create_traversal() const;

	virtual std::unique_ptr<MatchOrder> do_create_order() const
	= 0;

protected:

	/**
	 * \brief
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 */
	Verifier(const Checksums& actual_sums);

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~Verifier() noexcept = default;

	/**
	 * \brief
	 *
	 * \param[in] strict
	 */
	void set_strict(const bool strict);

	/**
	 * \brief
	 *
	 * \return
	 */
	bool strict() const;

	/**
	 * \brief
	 *
	 * \return
	 */
	const Checksums& actual_checksums() const;

	/**
	 * \brief
	 *
	 * \return
	 */
	const ARId& actual_id() const;

	/**
	 * \brief Perform a verification.
	 *
	 * \param[in] ref_sums    Reference checksums to match against
	 *
	 * \return The verification result
	 */
	std::unique_ptr<VerificationResult> perform(const ChecksumSource& ref_sums)
		const;

	/**
	 * \brief Perform a verification.
	 *
	 * \param[in] ref_sums    Reference checksums to match against
	 *
	 * \return The verification result
	 */
	std::unique_ptr<VerificationResult> perform(const ARResponse& ref_sums)
		const;

private:

	const Checksums& actual_sums_;
	bool is_strict_;
};


/**
 * \brief Verify an album track list.
 *
 * \details
 *
 * Tries to match each position \c i in the actual Checksums with position \c i
 * in each block of the ChecksumSource. Additonally checks the input id for
 * identity with the ARId of each respective block in the ARResponse.
 *
 * AlbumVerifier is the Verifier class suitable for easy matching of entire album
 * rips. Checksum lists whose ARId does not match have a difference of at least
 * \c 1.
 *
 * \see TracksetVerifier
 */
class AlbumVerifier final : public Verifier
{
	virtual std::unique_ptr<MatchOrder> do_create_order() const final;

	virtual const ARId& do_actual_id() const final;

public:

	/**
	 * \brief
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 * \param[in] actual_id   Actual ARId to check for
	 */
	AlbumVerifier(const Checksums& actual_sums, const ARId& actual_id);

private:

	const ARId& actual_id_;
};


/**
 * \brief Verify an arbitrary set of Checksums.
 *
 * \details
 *
 * Find any match of any actual Checksum in the reference. This targets the
 * situation where a subset of tracks from the same album are tried to be
 * matched, but the subset may be incomplete and the order of the tracks might
 * not be known.
 *
 * \note
 * The TracksetMatcher is a generalization of the AlbumVerifier. The
 * AlbumVerifier adds the restriction that the order of tracks in the reference
 * must be matched too.
 *
 * \see AlbumVerifier
 */
class TracksetVerifier : public Verifier
{
	virtual std::unique_ptr<MatchOrder> do_create_order() const final;

public:

	TracksetVerifier(const Checksums& actual_sums);
};


/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

