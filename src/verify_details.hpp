#ifndef __LIBARCSTK_VERIFY_DETAILS_HPP__
#define __LIBARCSTK_VERIFY_DETAILS_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Internal API for Matcher algorithms.
 */

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
 * \brief The checksum types to verify.
 */
constexpr std::array<checksum::type, 2> supported_checksum_types {
	checksum::type::ARCS1,
	checksum::type::ARCS2
};


/**
 * \internal
 * \brief Implementation of the actual verification result.
 */
class ResultBits
{
public:

	using size_type = std::size_t;

	/**
	 * \brief Default constructor.
	 *
	 * \param[in] blocks Number of @link ARBlock ARBlocks @endlink to represent
	 * \param[in] tracks Number of tracks per block
	 */
	ResultBits();

	int blocks() const;

	int tracks_per_block() const;

	size_type size() const;

	bool init(int blocks, int tracks);

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

	bool id(int b) const;

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

	bool track(int b, int t, bool v2) const;

	size_type total_tracks_set(int b) const;

protected:

	void validate(int blocks, int tracks) const;

	bool operator[](const int i) const;

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
 * \brief Default implementation of a VerificationResult
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
	virtual const VerificationPolicy* do_policy() const final;
	virtual std::unique_ptr<VerificationResult> do_clone() const final;

	details::ResultBits flags_;
	std::unique_ptr<VerificationPolicy> policy_;

public:

	Result(std::unique_ptr<VerificationPolicy> p);
	void init(const int blocks, const int tracks);
};


/**
 * \internal
 * \brief Create a VerificationResult object of a specified size.
 *
 * This is the "standard" way to instantiate an empty VerificationResult
 * object. Every implementation that creates a VerificationResult should
 * create it by this function except for good reasons.
 *
 * \param[in] blocks Number of blocks
 * \param[in] tracks Number of tracks per block
 *
 * \return VerificationResult object of the specified dimensions.
 */
std::unique_ptr<VerificationResult> create_result(const int blocks,
		const std::size_t tracks, std::unique_ptr<VerificationPolicy> p);


/**
 * \internal
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
std::unique_ptr<VerificationResult> verify_impl(
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource &ref_sums,
		const MatchTraversal& t, const MatchOrder& o);


/**
 * \brief Policy that accepts track matches all in the same block.
 */
class StrictPolicy final : public VerificationPolicy
{
	virtual bool do_is_verified(const int track, const VerificationResult& r)
		const final;

	virtual int do_total_unverified_tracks(const VerificationResult& r) const
		final;

	virtual bool do_is_strict() const final;
};


/**
 * \brief Policy that accepts track matches in any block.
 */
class LiberalPolicy final : public VerificationPolicy
{
	virtual bool do_is_verified(const int track, const VerificationResult& r)
		const final;

	virtual int do_total_unverified_tracks(const VerificationResult& r) const
		final;

	virtual bool do_is_strict() const final;
};


/**
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

	virtual std::unique_ptr<VerificationPolicy> do_get_policy() const final;
};


/**
 * \brief MatchTraversal for matching tracks in any block.
 */
class TraverseTracks final : public MatchTraversal
{
	virtual Checksum do_get_reference(const ChecksumSource& ref_sums,
			const int current, const int counter) const final;

	virtual std::size_t do_size(const ChecksumSource& ref_sums,
			const int current) const final;

	virtual void do_traverse(VerificationResult& result,
			const Checksums &actual_sums, const ARId &actual_id,
			const ChecksumSource& ref_sums,
			const MatchOrder& order) const final;

	virtual std::unique_ptr<VerificationPolicy> do_get_policy() const final;
};


/**
 * \brief MatchOrder that matches the tracks in their numbered order.
 */
class TrackOrder final : public MatchOrder
{
	virtual void do_perform(VerificationResult& result,
			const Checksums& actual_sums,
			const ChecksumSource& ref_sums, int index,
			const MatchTraversal& t) const final;
};


/**
 * \brief MatchOrder that tries any order to match the tracks.
 */
class Cartesian final : public MatchOrder
{
	virtual void do_perform(VerificationResult& result,
			const Checksums& actual_sums,
			const ChecksumSource& ref_sums, int index,
			const MatchTraversal& t) const final;
};


} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif
