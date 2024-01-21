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

#include <iterator> // for std::input_iterator_tag
#include <cstddef>  // for std::ptrdiff_t
#include <utility>  // for std::swap

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
	virtual bool do_strict() const final;
	virtual std::unique_ptr<VerificationResult> do_clone() const final;

	ResultBits flags_;
	std::unique_ptr<TrackPolicy> policy_;

public:

	Result(std::unique_ptr<TrackPolicy> p);
	void init(const int blocks, const int tracks);
	const TrackPolicy* policy() const;
};


/**
 * \brief Access ChecksumSource by a 'current' and 'counter' index.
 */
class Selector
{
	virtual const Checksum& do_get(const ChecksumSource& source,
			const int current, const int counter) const
	= 0;

public:

	virtual ~Selector() noexcept = default;

	const Checksum& get(const ChecksumSource& s, const int current,
			const int counter) const;
};


/**
 * \brief Current == block, counter == track
 */
class BlockSelector final : public Selector
{
	virtual const Checksum& do_get(const ChecksumSource& s, const int block,
			const int track) const final;
};


/**
 * \brief Current == track, counter == block
 */
class TrackSelector final : public Selector
{
	virtual const Checksum& do_get(const ChecksumSource& s, const int track,
			const int block) const final;
};


/**
 * \brief Iterates a ChecksumSource by track.
 */
class SourceIterator
{
public:

	using iterator_category = std::input_iterator_tag;
	using value_type        = Checksum;
	using difference_type   = std::ptrdiff_t;
	using reference         = const value_type&;
	using pointer           = const value_type*;

public:

	SourceIterator(const ChecksumSource& sums, const int current,
			const int counter, const Selector& selector);

	virtual ~SourceIterator() noexcept = default;

	int counter() const;
	int current() const;

	reference operator * () const; // dereferncing

	pointer operator -> () const; // dereferncing

	SourceIterator& operator ++ (); // prefix increment

	SourceIterator operator ++ (int); // postfix increment

	friend bool operator == (const SourceIterator& lhs, const SourceIterator&
			rhs)
	{
		return lhs.source_  == rhs.source_
			&& lhs.current_ == rhs.current_
			&& lhs.counter_ == rhs.counter_;
	}

	friend bool operator != (const SourceIterator& lhs, const SourceIterator&
			rhs)
	{
		return not(lhs == rhs);
	}

	friend void swap(SourceIterator& lhs, SourceIterator& rhs)
	{
		using std::swap;

		swap(lhs.source_,  rhs.source_);
		swap(lhs.current_, rhs.current_);
		swap(lhs.counter_, rhs.counter_);
	}

private:

	const Selector* selector_;
	const ChecksumSource* source_;
	int current_;
	int counter_;
};


/**
 * \internal
 * \brief Policy for traversals
 */
class TraversalPolicy
{
public:

	using iterator       = const SourceIterator;
	using const_iterator = const SourceIterator;

private:

	const std::unique_ptr<Selector> selector_;
	const ChecksumSource* source_;
	int current_;

	virtual std::unique_ptr<TrackPolicy> create_track_policy() const
	= 0;
	virtual const_iterator do_begin() const
	= 0;
	virtual const_iterator do_end() const
	= 0;
	virtual Checksums::size_type do_current_block(const SourceIterator& i) const
	= 0;
	virtual Checksums::size_type do_current_track(const SourceIterator& i) const
	= 0;
	virtual int do_end_current(const ChecksumSource& source) const
	= 0;
	virtual int do_end_counter(const ChecksumSource& source) const
	= 0;

protected:

	TraversalPolicy(std::unique_ptr<Selector> selector);

	const Selector& selector() const;

	void check_source_for_null() const;

public:

	virtual ~TraversalPolicy() noexcept = default;

	const ChecksumSource* source() const;
	void set_source(const ChecksumSource& source);
	int current() const;
	void set_current(const int current);

	int end_current() const;
	int end_counter() const;

	const_iterator begin() const;
	const_iterator end()   const;
	Checksums::size_type current_block(const SourceIterator& i) const;
	Checksums::size_type current_track(const SourceIterator& i) const;

	std::unique_ptr<TrackPolicy> get_policy() const;
};


/**
 * \brief Traverse tracks in a single block.
 */
class BlockTraversal final : public TraversalPolicy
{
	std::unique_ptr<TrackPolicy> create_track_policy() const final;
	const_iterator do_begin() const final;
	const_iterator do_end() const final;
	Checksums::size_type do_current_block(const SourceIterator& i) const final;
	Checksums::size_type do_current_track(const SourceIterator& i) const final;
	int do_end_current(const ChecksumSource& source) const final;
	int do_end_counter(const ChecksumSource& source) const final;

public:

	BlockTraversal();
};


/**
 * \brief Traverse values for a single track in all blocks.
 */
class TrackTraversal final : public TraversalPolicy
{
	std::unique_ptr<TrackPolicy> create_track_policy() const final;
	const_iterator do_begin() const final;
	const_iterator do_end() const final;
	Checksums::size_type do_current_block(const SourceIterator& i) const final;
	Checksums::size_type do_current_track(const SourceIterator& i) const final;
	int do_end_current(const ChecksumSource& source) const final;
	int do_end_counter(const ChecksumSource& source) const final;

public:

	TrackTraversal();
};


/**
 * \brief
 */
class MatchPolicy
{
	virtual void do_perform(VerificationResult& result,
			const Checksums &actual_sums, const Checksum& ref,
			const int block, const Checksums::size_type track) const
	= 0;

protected:

	void check_match(VerificationResult& result,
			const ChecksumSet& actual, const Checksum& ref,
			const int block, const Checksums::size_type track) const;

public:

	virtual ~MatchPolicy() noexcept = default;

	void perform(VerificationResult& result, const Checksums &actual_sums,
			const Checksum& ref, const int block,
			const Checksums::size_type track) const;
};


/**
 * \brief
 */
class TrackOrderPolicy final : public MatchPolicy
{
	void do_perform(VerificationResult& result,
			const Checksums &actual_sums, const Checksum& ref,
			const int block, const Checksums::size_type track) const final;
};


/**
 * \brief
 */
class FindOrderPolicy final : public MatchPolicy
{
	void do_perform(VerificationResult& result,
			const Checksums &actual_sums, const Checksum& ref,
			const int block, const Checksums::size_type track) const final;
};


/**
 * \internal
 * \brief Defines the traversal method of the reference checksums.
 *
 * The traversal method can e.g. be implemented as an iteration over a single
 * block in the ChecksumSource. Alternatively, it could be implemented as a
 * traversal over the same track in every block.
 */
class SourceTraversal final
{
	void perform_current(VerificationResult& result,
		const Checksums &actual_sums, const ARId &actual_id,
		const TraversalPolicy& t, const MatchPolicy& m) const;

public:

	void perform(VerificationResult& result,
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource& ref_sums,
		TraversalPolicy& t, const MatchPolicy& p) const;
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
 * \brief Worker: perform a verification.
 *
 * The implementation uses create_result() to create a VerificationResult.
 * It thereby uses the TrackPolicy provided by the MatchTraversal. While
 * traversing it uses the MatchOrder passed.
 *
 * This can be considered the "default implementation" of a verification
 * process.Every implementation that performs a verification should implement it
 * by calling this function except for good reasons.
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
		const Checksums &actual_sums, const ARId &actual_id,
		const ChecksumSource &ref_sums,
		TraversalPolicy& traversal, const MatchPolicy& order);


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
 * \brief Base class for Verifiers
 */
class VerifierBase
{
	virtual const ARId& do_actual_id() const;

	virtual std::unique_ptr<MatchPolicy> do_create_order() const
	= 0;

protected:

	/**
	 * \brief
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 */
	VerifierBase(const Checksums& actual_sums);

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~VerifierBase() noexcept = default;

	/**
	 * \brief
	 *
	 * \return
	 */
	const ARId& actual_id() const;

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
	bool strict() const;

	/**
	 * \brief
	 *
	 * \param[in] strict
	 */
	void set_strict(const bool strict);

	/**
	 * \brief Perform a verification.
	 *
	 * \param[in] ref_sums    Reference checksums to match against
	 *
	 * \return The verification result
	 */
	std::unique_ptr<VerificationResult> perform(const ChecksumSource& ref_sums)
		const;

private:

	const Checksums& actual_sums_;
	bool is_strict_;
};


} // namespace details


class AlbumVerifier::Impl : public details::VerifierBase
{
	virtual std::unique_ptr<details::MatchPolicy> do_create_order() const final;

	virtual const ARId& do_actual_id() const final;

public:

	/**
	 * \brief
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 * \param[in] actual_id   Actual ARId to check for
	 */
	Impl(const Checksums& actual_sums, const ARId& actual_id);

private:

	const ARId& actual_id_;

};


class TracksetVerifier::Impl : public details::VerifierBase
{
	virtual std::unique_ptr<details::MatchPolicy> do_create_order() const final;

public:

	/**
	 * \brief
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 */
	Impl(const Checksums& actual_sums);
};

} // namespace v_1_0_0
} // namespace arcstk

#endif

