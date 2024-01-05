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
 * \brief Provide unified access method to checksum containers.
 *
 * A checksum container contains several blocks of checksums an in every block a
 * sequence of checksums. A single checksum is accessed by a block index in
 * combination with the index of the checksum within the block.
 *
 * A type \c T can be made available as a ChecksumSource via definig a subclass
 * \c S that inherits from GetChecksum<T>. Subclass \c S has to implement the
 * virtual functions of ChecksumSource. It can reuse the constructor of
 * GetChecksum<T>.
 *
 * \see GetChecksum
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
class GetChecksum : public ChecksumSource
{
	/**
	 * \brief Internal checksum source.
	 */
	const T* checksum_source_;

protected:

	/**
	 * \brief The wrapped checksum source.
	 *
	 * \return The wrapped checksum source.
	 */
	const T* source() const
	{
		return checksum_source_;
	}

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~GetChecksum() noexcept = default;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] t The primary checksum source
	 */
	GetChecksum(const T* t)
		: checksum_source_ { t }
	{
		// empty
	}
};


/**
 * \brief Access an ARResponse by block and index.
 *
 * Make an ARResponse available for verification.
 */
class FromResponse final : public GetChecksum<ARResponse>
{
	ARId do_id(const int block_idx) const final;
	Checksum do_checksum(const int block_idx, const int idx) const final;
	int do_confidence(const int block_idx, const int idx) const final;
	std::size_t do_size(const int block_idx) const final;
	std::size_t do_size() const final;

public:

	using GetChecksum::GetChecksum; // ctor
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
	 * \brief
	 *
	 * \param[in] track
	 * \param[in] r
	 *
	 * \return
	 */
	bool is_verified(const int track, const VerificationResult& r) const;

	/**
	 * \brief
	 *
	 * \param[in] r
	 *
	 * \return
	 */
	int total_unverified_tracks(const VerificationResult& r) const;

	/**
	 * \brief
	 *
	 * \return
	 */
	bool is_strict() const;
};


/**
 * \brief Print a VerificationResult to a stream
 */
std::ostream& operator << (std::ostream&, const VerificationResult& match);


/**
 * \brief Result of a verification process.
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
	 * The verification should be flagged as accurate iff this returns TRUE.
	 *
	 * \return TRUE iff each track is verified otherwise FALSE
	 */
	bool all_tracks_verified() const;

	/**
	 * \brief Total number of unverified tracks.
	 *
	 * \return
	 */
	int total_unverified_tracks() const;

	/**
	 * \brief TRUE if specified track is verified, otherwise FALSE.
	 *
	 * \param[in] track 0-based track
	 *
	 * \return
	 */
	bool is_verified(const int track) const;

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
	 * \brief Mark the checksum of a specified track in a specified block as
	 * 'matched'.
	 *
	 * \param[in] b  0-based index of the block to verify in the ARResponse
	 * \param[in] t  0-based index of the track to verify in the ARResponse
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
	 * What a concrete match means is dependent from the logic the VerificationResulter used.
	 * The call <tt>myVerificationResult.value(0,17,true)</tt> refers to the ARCSv2 of track
	 * 18 in the first block. If this call returns \c true, track 18 in the
	 * first ARBlock in the ARResponse was matched by the VerificationResulter. Whether this
	 * indicates that track 18 of the current Checksums caused the match is
	 * implementation defined. If the VerificationResult was calculated by an AlbumVerificationResulter or
	 * a ListVerificationResulter, track 18 of the input Checksums will only be matched
	 * against track 18 in each block. A TracksetVerificationResulter on the other hand will
	 * just indicate that \em one of the input checksums matched track 18.
	 *
	 * \param[in] b  0-based index of the block to verify in the ARResponse
	 * \param[in] t  0-based index of the track to verify in the ARResponse
	 * \param[in] v2 Returns the ARCSv2 flag iff \c TRUE, otherwise ARCSv1
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
	 * The concrete difference value may depend on the VerificationResulter
	 * implementation. For example, consider an album with 15 tracks. An ARBlock
	 * with no track matching and a different ARId will be a difference of 16
	 * assigned if the VerificationResult is produced by an AlbumMatcher.
	 * A TracksetMatcher ignores the ARId and hence it does not contribute to
	 * the difference. But therefore, the exact same ARBlock will have a
	 * difference of only 15.
	 *
	 * \param[in] b  0-based index of the block to verify in the ARResponse
	 * \param[in] v2 Returns the ARCSv2 iff \c TRUE, otherwise ARCSv1
	 *
	 * \throws std::runtime_error Iff \c b is out of range
	 *
	 * \return Difference of block \c b
	 */
	int difference(int b, bool v2) const;

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
	 * The size of a VerificationResult with a number \f$b\f$ of total_blocks() and
	 * \f$t\f$ tracks_p:er_block() is \f$b * (2 * t + 1)\f$. The coefficient
	 * \f$2\f$ is required since each block is matched against ARCSv1 and
	 * ARCSv2 sums. The \f$+1\f$ is required since the ARId of each block
	 * contributes an additional verification flag to the VerificationResult.
	 *
	 * \return Number of flags stored
	 */
	size_t size() const;

	/**
	 * \brief Identifiy best matching block (the one with smallest difference).
	 *
	 * If there is more than one block with the smalles difference, choose the
	 * first.
	 *
	 * \return 0-based index, ARCS version, and difference of the best block
	 */
	std::tuple<int, bool, int> best_block() const;

	/**
	 * \brief
	 *
	 * \return
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

	/**
	 * \brief Implements \link VerificationResult::verify_id(int b) verify_id(int) \endlink.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \return Index position to store the verification flag
	 */
	virtual int do_verify_id(const int b)
	= 0;

	/**
	 * \brief Implements \link VerificationResult::id(int b) const id(int) \endlink.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \throws Iff \c b is out of range
	 *
	 * \return \c TRUE iff the ARId of block \c b matches the ARId of \c
	 * result
	 */
	virtual bool do_id(const int b) const
	= 0;

	/**
	 * \brief Implements
	 * \link VerificationResult::verify_track(int b, int t, bool v2)
	 * verify_track(b, t, v2)
	 * \endlink.
	 *
	 * \param[in] b  0-based index of the block to verify in \c response
	 * \param[in] t  0-based index of the track to verify in \c response
	 * \param[in] v2 Verifies the ARCSv2 iff \c TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b or \c t are out of range
	 *
	 * \return Index position to store the verification flag
	 */
	virtual int do_verify_track(const int b, const int t, const bool v2)
	= 0;

	/**
	 * \brief Implements
	 * \link VerificationResult::track(int b, int t, bool v2) const
	 * track(int, int, bool)
	 * \endlink.
	 *
	 * \param[in] b  0-based index of the block to verify in \c response
	 * \param[in] t  0-based index of the track to verify in \c response
	 * \param[in] v2 Returns the ARCSv2 flag iff \c TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b or \c t are out of range
	 *
	 * \return Flag for ARCS of track \c t in block \c b
	 */
	virtual bool do_track(const int b, const int t, const bool v2) const
	= 0;

	/**
	 * \brief Implements
	 * \link VerificationResult::difference(int b, bool v2) const
	 * difference(int, bool)
	 * \endlink.
	 *
	 * \param[in] b  0-based index of the block to verify in \c response
	 * \param[in] v2 Returns the ARCSv2 iff \c TRUE, otherwise ARCSv1
	 *
	 * \throws Iff \c b is out of range
	 *
	 * \return Difference of block \c b
	 */
	virtual int do_difference(const int b, const bool v2) const
	= 0;

	/**
	 * \brief Implements \link VerificationResult::total_blocks() const
	 * total_blocks()
	 * \endlink.
	 *
	 * \return Total number of compared blocks.
	 */
	virtual int do_total_blocks() const
	= 0;

	/**
	 * \brief Implements \link VerificationResult::tracks_per_block() const
	 * tracks_per_block()
	 * \endlink.
	 *
	 * \return Total number of tracks per block.
	 */
	virtual int do_tracks_per_block() const
	= 0;

	/**
	 * \brief Implement \link VerificationResult::size() const size() \endlink.
	 *
	 * \return Number of flags stored
	 */
	virtual size_t do_size() const
	= 0;

	virtual std::tuple<int, bool, int> do_best_block() const
	= 0;

	virtual int do_best_block_difference() const
	= 0;

	virtual const TrackPolicy* do_policy() const
	= 0;

	/**
	 * \brief Implements \link VerificationResult::clone() clone() \endlink.
	 *
	 * \return Deep copy of this instance.
	 */
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
	 * \param[in] current  Index of the current set
	 * \param[in] counter  Index of the checksum within the set indexed by \c current
	 *
	 * \return The checksum under the specified indices
	 */
	Checksum get_reference(const ChecksumSource& ref_sums, const int current,
			const int counter) const;

	/**
	 * \brief Provide the number of checksums under the index \ current.
	 *
	 * \param[in] ref_sums The source of checksums
	 * \param[in] current  Index of the current set
	 *
	 * \return The checksum under the specified index
	 */
	std::size_t size(const ChecksumSource& ref_sums, const int current) const;

	/**
	 * \brief
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
	 * \return TrackPolicy to interpret track verification in the result.
	 */
	std::unique_ptr<TrackPolicy> get_policy() const;

};


/**
 * \brief VerificationResult items in a single loop.
 *
 * Apply the matches in any order.
 */
class MatchOrder
{
	virtual void do_perform(VerificationResult& result,
			const Checksums& actual_sums, const ChecksumSource& ref_sums,
			int index, const MatchTraversal& t) const
	= 0;

public:

	virtual ~MatchOrder() noexcept = default;

	void perform(VerificationResult& result,
			const Checksums& actual_sums, const ChecksumSource& ref_sums,
			int index, const MatchTraversal& t) const;
};


/**
 * \brief Perform a verification
 *
 * \param[in]     actual_sums Actual checksums to check for
 * \param[in]     actual_id   Actual ARId to check for
 * \param[in]     ref_sums    Reference checksums to match against
 * \param[in]     traversal   Traversal that defines which items to traverse
 * \param[in]     order       Order to be applied on each traversed item
 *
 * \return The verification result object
 */
std::unique_ptr<VerificationResult> verify(
		const Checksums& actual_sums, const ARId& actual_id,
		const ChecksumSource& ref_sums,
		const MatchTraversal& t, const MatchOrder& o);

} // namespace v_1_0_0

} // namespace arcstk

#endif

