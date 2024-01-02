#ifndef __LIBARCSTK_VERIFY_HPP__
#define __LIBARCSTK_VERIFY_HPP__

/** \file
 *
 * \brief Public API for \link verify AccurateRip checksum verificytion \endlink.
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

class ARResponse;
class Checksum;

/**
 * \brief Provide unified access method to checksum containers.
 *
 * A checksum container contains several blocks of checksums an in every block a
 * sequence of checksums. A single checksum is accessed by a block index in
 * combination with the index of the checksum within the block.
 */
class ChecksumSource
{
	virtual std::string do_id(const int block_idx) const
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
	 */
	std::string id(const int block_idx) const;

	/**
	 * \brief Read checksum \c idx in section with the specified \c block_idx.
	 */
	Checksum checksum(const int block_idx, const int idx) const;

	/**
	 * \brief Read confidence \c idx in section with the specified \c block_idx.
	 */
	int confidence(const int block_idx, const int idx) const;

	/**
	 * \brief Read id in section with the specified \c block_idx.
	 */
	std::size_t size(const int block_idx) const;

	/**
	 * \brief Number of blocks.
	 *
	 * Greatest legal block is size() - 1.
	 */
	std::size_t size() const;
};


/**
 * \brief Wrap a checksum container in a ChecksumSource.
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
 */
class FromResponse final : public GetChecksum<ARResponse>
{
	using GetChecksum::GetChecksum; // ctor
	std::string do_id(const int block_idx) const final;
	Checksum do_checksum(const int block_idx, const int idx) const final;
	int do_confidence(const int block_idx, const int idx) const final;
	std::size_t do_size(const int block_idx) const final;
	std::size_t do_size() const final;
};

class VerificationResult;

/**
 * \brief
 */
struct BestBlock
{
	/**
	 * \brief Maximal difference that is possible between two blocks,
	 * 99 tracks + id.
	 */
	static constexpr int MAX_DIFFERENCE = 100;

	/**
	 * \brief Identify best matching block, i.e. the block with the smallest
	 * difference possible.
	 *
	 * If the result contains a negative value as its first value, this
	 * indicates an error.
	 *
	 * The result tuple contains the block index, the ARCS type that matches and
	 * the confidence value.
	 */
	std::tuple<int, bool, int> operator()(const VerificationResult& result) const;
};


/**
 * \brief
 */
std::ostream& operator << (std::ostream&, const VerificationResult& match);


/**
 * \brief
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

	int total_unmatched_tracks() const;

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
	 * \f$t\f$ tracks_per_block() is \f$b * (2 * t + 1)\f$. The coefficient
	 * \f$2\f$ is required since each block is matched against ARCSv1 and
	 * ARCSv2 sums. The \f$+1\f$ is required since the ARId of each block
	 * contributes an additional verification flag to the VerificationResult.
	 *
	 * \return Number of flags stored
	 */
	size_t size() const;

	/**
	 * \brief Clones this instance.
	 *
	 * \return Deep copy of this instance.
	 */
	std::unique_ptr<VerificationResult> clone() const;

private:

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

	virtual int do_total_unmatched_tracks() const
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

	/**
	 * \brief Implements \link VerificationResult::clone() clone() \endlink.
	 *
	 * \return Deep copy of this instance.
	 */
	virtual std::unique_ptr<VerificationResult> do_clone() const
	= 0;
};


class Verifier
{
public:

	/**
	 * \brief Virtual default destructor
	 */
	virtual ~Verifier() noexcept = default;

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

	/**
	 * \brief Return \c TRUE iff the specified VerificationResult equals this instance.
	 *
	 * \param[in] rhs The right hand side of the operation
	 *
	 * \return \c TRUE iff \c rhs equals this instance
	 */
	bool equals(const Verifier &rhs) const noexcept;

private:

	class Impl;

	/**
	 * Private implementation
	 */
	std::unique_ptr<Impl> impl_;
};

} // namespace v_1_0_0

} // namespace arcstk

#endif

