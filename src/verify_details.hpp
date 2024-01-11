#ifndef __LIBARCSTK_VERIFY_HPP__
#error "Do not include verify_details.hpp, include verify.hpp instead"
#endif

/**
 * \internal
 *
 * \file
 *
 * \brief Internal API for Matcher algorithms.
 */

#ifndef __LIBARCSTK_VERIFY_DETAILS_HPP__
#define __LIBARCSTK_VERIFY_DETAILS_HPP__

#ifndef __LIBARCSTK_VERIFY_HPP__
#include "verify.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif


namespace arcstk
{
inline namespace v_1_0_0
{

namespace details
{


/**
 * \internal
 * \brief The checksum types to verify.
 */
constexpr std::array<checksum::type, 2> supported_checksum_types {
	checksum::type::ARCS1,
	checksum::type::ARCS2
};


/**
 * \internal
 * \brief Block with smallest difference.
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
	 *
	 * \param[in] r The result to get the best block of
	 */
	std::tuple<int, bool, int> operator()(const VerificationResult& r) const;
};


/**
 * \internal
 * \brief Implementation of the actual result flag store.
 */
class ResultBits
{
public:

	/**
	 * \brief Size type of this class.
	 */
	using size_type = std::size_t;

	/**
	 * \brief Default constructor.
	 *
	 * \param[in] blocks Number of @link ARBlock ARBlocks @endlink to represent
	 * \param[in] tracks Number of tracks per block
	 */
	ResultBits();

	/**
	 * \brief Number of blocks in the store.
	 *
	 * \return Number of blocks stored
	 */
	int blocks() const;

	/**
	 * \brief Total number of tracks per block.
	 *
	 * \return Total number of tracks per block
	 */
	int tracks_per_block() const;

	/**
	 * \brief Total number of flags in the store.
	 *
	 * \return Size of the store
	 */
	size_type size() const;

	/**
	 * \brief Construct a flag store for the specified number of blocks and
	 * tracks per block.
	 *
	 * \param[in] blocks Number of blocks
	 * \param[in] tracks Number of tracks per block
	 *
	 * \return TRUE if initialization was successful
	 */
	bool init(int blocks, int tracks);

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
	 * \brief Value for the specified track.
	 *
	 * \param[in] b     0-based index of the block in \c response
	 * \param[in] t     0-based index of the track in \c response
	 *
	 * \return Value for the specified track
	 */
	bool track(int b, int t, bool v2) const;

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
	 * \brief Value for the id of the specified block.
	 *
	 * \param[in] b     0-based index of the block in \c response
	 *
	 * \return Value for the specified track
	 */
	bool id(int b) const;

	/**
	 * \brief Total number of track flags in block \c b that are set to TRUE.
	 *
	 * \param[in] b Index of the block to count tracks in
	 *
	 * \return Number of tracks flagged as TRUE in specified block.
	 */
	size_type total_tracks_set(int b) const;

protected:

	/**
	 * \brief Validate position request.
	 *
	 * If this method does not throw, block and track index are valid
	 *
	 * \param[in] b     0-based index of the block in \c response
	 * \param[in] t     0-based index of the track in \c response
	 */
	void validate(int b, int t) const;

	/**
	 * \brief Value of the flag with index \c i.
	 *
	 * \param[in] i Index to access
	 *
	 * \return Value of the flag with the specified absolute index.
	 */
	bool operator[](const int i) const;

	/**
	 * \brief Total number of flags per block.
	 *
	 * \return Total number of flags per block
	 */
	int flags_per_block() const;

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
	 * \brief Set the flag on position \c offset to \c value.
	 *
	 * \param[in] offset
	 * \param[in] value
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


/**
 * \internal
 * \brief Default implementation of a VerificationResult.
 */
class Result final : public VerificationResult
{
	virtual int do_verify_id(const int b) final;
	virtual bool do_id(const int b) const final;
	virtual int do_verify_track(const int b, const int t, const bool v2) final;
	virtual bool do_track(const int b, const int t, const bool v2) const final;
	virtual int do_difference(const int b, const bool v2) const final;
	virtual int do_total_blocks() const final;
	virtual int do_tracks_per_block() const final;
	virtual size_t do_size() const final;
	virtual bool do_is_verified(const int track) const final;
	virtual int do_total_unverified_tracks() const final;
	virtual std::tuple<int, bool, int> do_best_block() const final;
	virtual int do_best_block_difference() const final;
	virtual const TrackPolicy* do_policy() const final;
	virtual std::unique_ptr<VerificationResult> do_clone() const final;

	ResultBits flags_;
	std::unique_ptr<TrackPolicy> policy_;

public:

	Result(std::unique_ptr<TrackPolicy> p);
	void init(const int blocks, const int tracks);
};


/**
 * \internal
 * \brief Create a VerificationResult object of a specified size.
 *
 * This is considered the "default" way to instantiate an empty
 * VerificationResult object. Every implementation that creates a
 * VerificationResult should create it by this function except for good
 * reasons.
 *
 * \param[in] blocks Number of blocks
 * \param[in] tracks Number of tracks per block
 *
 * \return VerificationResult object of the specified dimensions.
 */
std::unique_ptr<VerificationResult> create_result(const int blocks,
		const std::size_t tracks, std::unique_ptr<TrackPolicy> p);


/**
 * \internal
 * \brief Implements verify().
 *
 * \param[in]     actual_sums Actual checksums to check for
 * \param[in]     actual_id   Actual ARId to check for
 * \param[in]     ref_sums    Reference checksums to match against
 * \param[in]     traversal   Traversal that defines which items to traverse
 * \param[in]     order       Order to be applied on each traversed item
 *
 * \return The verification result object
 */
std::unique_ptr<VerificationResult> verify_impl(
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource &ref_sums,
		const MatchTraversal& traversal, const MatchOrder& order);


/**
 * \internal
 * \brief TrackPolicy that accepts track matches in the same block as verified.
 */
class StrictPolicy final : public TrackPolicy
{
	virtual bool do_is_verified(const int track, const VerificationResult& r)
		const final;

	virtual int do_total_unverified_tracks(const VerificationResult& r) const
		final;

	virtual bool do_is_strict() const final;
};


/**
 * \internal
 * \brief TrackPolicy that accepts matches in any block as verified.
 */
class LiberalPolicy final : public TrackPolicy
{
	virtual bool do_is_verified(const int track, const VerificationResult& r)
		const final;

	virtual int do_total_unverified_tracks(const VerificationResult& r) const
		final;

	virtual bool do_is_strict() const final;
};


/**
 * \internal
 * \brief MatchTraversal for matching tracks in the same block.
 */
class TraverseBlock final : public MatchTraversal
{
	virtual Checksum do_get_reference(const ChecksumSource& ref_sums,
			const int current, const int counter) const final;

	virtual std::size_t do_size(const ChecksumSource& ref_sums,
			const int current) const final;

	virtual void do_traverse(VerificationResult& result,
			const Checksums &actual_sums, const ARId &actual_id,
			const ChecksumSource& ref_sums,
			const MatchOrder& order) const final;

	virtual std::unique_ptr<TrackPolicy> do_get_policy() const final;
};


/**
 * \internal
 * \brief MatchTraversal for matching tracks in any block.
 */
class TraverseTrack final : public MatchTraversal
{
	virtual Checksum do_get_reference(const ChecksumSource& ref_sums,
			const int current, const int counter) const final;

	virtual std::size_t do_size(const ChecksumSource& ref_sums,
			const int current) const final;

	virtual void do_traverse(VerificationResult& result,
			const Checksums &actual_sums, const ARId &actual_id,
			const ChecksumSource& ref_sums,
			const MatchOrder& order) const final;

	virtual std::unique_ptr<TrackPolicy> do_get_policy() const final;
};


/**
 * \internal
 * \brief MatchOrder that matches the tracks in their numbered order.
 */
class TrackOrder final : public MatchOrder
{
	virtual void do_perform(VerificationResult& result,
			const Checksums& actual_sums,
			const ChecksumSource& ref_sums, int current,
			const MatchTraversal& t) const final;
};


/**
 * \internal
 * \brief MatchOrder that tries any order to match the track checksums.
 */
class UnknownOrder final : public MatchOrder
{
	virtual void do_perform(VerificationResult& result,
			const Checksums& actual_sums,
			const ChecksumSource& ref_sums, int current,
			const MatchTraversal& t) const final;
};

} // namespace details
} // namespace v_1_0_0
} // namespace arcstk

#endif

