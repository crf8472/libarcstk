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

class ARResponse;

namespace details
{

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
		const std::size_t tracks);


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

	virtual void do_traverse(VerificationResult& match, const Checksums &actual_sums,
		const ARId &actual_id, const ChecksumSource& ref_sums,
		const MatchOrder& order) const
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
			const int counter) const
	{
		return do_get_reference(ref_sums, current, counter);
	}

	/**
	 * \brief Provide the number of checksums under the index \ current.
	 *
	 * \param[in] ref_sums The source of checksums
	 * \param[in] current  Index of the current set
	 *
	 * \return The checksum under the specified index
	 */
	std::size_t size(const ChecksumSource& ref_sums, const int current) const
	{
		return do_size(ref_sums, current);
	}

	/**
	 * \brief
	 */
	void traverse(VerificationResult& match, const Checksums& actual_sums,
			const ARId &actual_id, const ChecksumSource& ref_sums,
			const MatchOrder& order) const
	{
		do_traverse(match, actual_sums, actual_id, ref_sums, order);
	}
};


/**
 * \brief
 */
class TraverseBlock final : public MatchTraversal
{
	virtual Checksum do_get_reference(const ChecksumSource& ref_sums,
			const int current, const int counter) const final;

	virtual std::size_t do_size(const ChecksumSource& ref_sums,
			const int current) const final;

	virtual void do_traverse(VerificationResult& match, const Checksums &actual_sums,
			const ARId &actual_id, const ChecksumSource& ref_sums,
			const MatchOrder& order) const final;

public:

	using MatchTraversal::MatchTraversal;
};


/**
 * \brief
 */
class TraverseTracks final : public MatchTraversal
{
	virtual Checksum do_get_reference(const ChecksumSource& ref_sums,
			const int current, const int counter) const final;

	virtual std::size_t do_size(const ChecksumSource& ref_sums,
			const int current) const final;

	virtual void do_traverse(VerificationResult& match, const Checksums &actual_sums,
			const ARId &actual_id, const ChecksumSource& ref_sums,
			const MatchOrder& order) const final;

public:

	using MatchTraversal::MatchTraversal;
};


class MatchPerformer;


/**
 * \brief VerificationResult items in a single loop.
 *
 * Apply the matches in any order.
 */
class MatchOrder
{
	virtual void do_perform(VerificationResult& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int index) const
	= 0;

public:

	MatchOrder();

	virtual ~MatchOrder() noexcept = default;

	const MatchPerformer* performer() const noexcept;

	void set_performer(MatchPerformer* const performer) noexcept;

	void perform(VerificationResult& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int index) const
	{
		do_perform(match, actual_sums, ref_sums, t, index);
	}

private:

	MatchPerformer* performer_;
};


/**
 * \brief
 */
class TrackOrder final : public MatchOrder
{
	using MatchOrder::MatchOrder;
	virtual void do_perform(VerificationResult& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int index) const final;
};


/**
 * \brief
 */
class Cartesian final : public MatchOrder
{
	using MatchOrder::MatchOrder;
	virtual void do_perform(VerificationResult& match, const Checksums& actual_sums,
			const ChecksumSource& ref_sums,
			const MatchTraversal& t, int index) const final;
};


/**
 * \brief
 */
class MatchPerformer
{
	virtual std::unique_ptr<VerificationResult> do_create_match_instance(
		const int blocks, const std::size_t tracks) const noexcept
	= 0;

	virtual bool do_matches(
		const ARId& actual, const ARId& reference) const noexcept
	= 0;

	virtual bool do_matches(
		const Checksum& actual, const Checksum& reference) const noexcept
	= 0;

protected:

	/**
	 * \brief
	 */
	std::unique_ptr<VerificationResult> create_match_instance(
		const int blocks, const std::size_t tracks) const noexcept
	{
		return do_create_match_instance(blocks, tracks);
	}

public:

	/**
	 * \brief
	 */
	virtual ~MatchPerformer() noexcept = default;


	/**
	 * \brief Implement matching an actual ARId against a reference.
	 *
	 * The matching is implemented by calling operator == on the input
	 * ARId instances.
	 *
	 * \param[in] actual    The actual ARId to be matched
	 * \param[in] reference The reference ARId to match
	 *
	 * \return TRUE if the sums match, otherwise FALSE
	 */
	bool matches(
		const ARId& actual, const ARId& reference) const noexcept
	{
		return do_matches(actual, reference);
	}


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
	bool matches(
		const Checksum& actual, const Checksum& reference) const noexcept
	{
		return do_matches(actual, reference);
	}
};


/**
 * \internal
 * \brief Actual match performing Functor.
 */
class DefaultPerformer final : public MatchPerformer
{
	virtual std::unique_ptr<VerificationResult> do_create_match_instance(
		const int blocks, const std::size_t tracks) const noexcept final;

	virtual bool do_matches(
		const ARId& actual, const ARId& reference) const noexcept final;

	virtual bool do_matches(
		const Checksum& actual, const Checksum& reference) const noexcept final;

public:

	/**
	 * \brief The checksum types to verify.
	 */
	static constexpr std::array<checksum::type, 2> types {
		checksum::type::ARCS1,
		checksum::type::ARCS2
	};

	DefaultPerformer(MatchTraversal* traversal, MatchOrder* order);

	virtual ~DefaultPerformer() noexcept = default;

	const MatchTraversal* traversal() const;

	const MatchOrder* order() const;

	std::unique_ptr<VerificationResult> operator() (
			const Checksums& actual_sums, const ARId& actual_id,
			const ChecksumSource& ref_sums) const;

private:

	MatchTraversal* const traversal_;
	MatchOrder* const order_;
};

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif
