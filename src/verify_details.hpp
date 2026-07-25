#ifndef LIBARCSTK_VERIFY_HPP_
#error "Do not include verify_details.hpp, include verify.hpp instead"
#endif

#ifndef LIBARCSTK_VERIFY_DETAILS_HPP_
#define LIBARCSTK_VERIFY_DETAILS_HPP_

/**
 * \internal
 *
 * \file
 *
 * \brief Implementation details for verify.hpp.
 */

#include <cstddef>           // for size_t, ptrdiff_t
#include <cstdint>           // for uint32_t
#include <iterator>          // for input_iterator_tag
#include <memory>            // for unique_ptr
#include <utility>           // for swap
#include <vector>            // for vector

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"            // for ChecksumSource, VerificationResult, ...
#endif
#ifndef LIBARCSTK_MIXINS_HPP_
#include "mixins.hpp"            // for Comparable
#endif

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
// avoid includes
class ARId;

/**
 * \addtogroup verify
 *
 * @{
 */

namespace details
{


/**
 * \brief Service: identify block with smallest difference.
 */
struct BestBlock final
{
	/**
	 * \brief Maximal difference that is possible between two blocks,
	 * 99 tracks + 1 id.
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
	best_block_info_t from(const VerificationResult& r) const;
};


/**
 * \brief Implementation of the actual result flag store.
 */
class ResultBits final
{
public:

	/**
	 * \brief Index type of this class.
	 */
	using index_type = int;

	/**
	 * \copydoc SNPT_tp_size
	 */
	using size_type = std::size_t;

	/**
	 * \brief Maximal accepted number of blocks.
	 */
	static constexpr auto MAX_TOTAL_BLOCKS = size_type { 500 };

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	ResultBits()
		: ResultBits { 0, 0 }
	{
		// empty
	}

	/**
	 * \brief Constructor.
	 *
	 * \param[in] blocks Total number of blocks
	 * \param[in] tracks Total number of tracks per block
	 *
	 * \throw out_of_range If number of tracks is greater than
	 * CDDA::MAX_TRACKCOUNT or total blocks is greater than MAX_TOTAL_BLOCKS
	 */
	ResultBits(const size_type blocks, const size_type tracks);

	/**
	 * \brief Number of blocks in the store.
	 *
	 * \return Number of blocks stored
	 */
	size_type total_blocks() const;

	/**
	 * \brief Total number of tracks per block.
	 *
	 * \return Total number of tracks per block
	 */
	size_type tracks_per_block() const;

	/**
	 * \copydoc SNPT_mf_size
	 */
	size_type size() const;

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
	index_type set_track(index_type b, index_type t, bool v2, bool value);

	/**
	 * \brief Value for the specified track.
	 *
	 * \param[in] b     0-based index of the block in \c response
	 * \param[in] t     0-based index of the track in \c response
	 *
	 * \return Value for the specified track
	 */
	bool track(index_type b, index_type t, bool v2) const;

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
	index_type set_id(index_type b, bool value);

	/**
	 * \brief Value for the id of the specified block.
	 *
	 * \param[in] b     0-based index of the block in \c response
	 *
	 * \return Value for the specified track
	 */
	bool id(index_type b) const;

	/**
	 * \brief Total number of track flags in block \c b that are set to TRUE.
	 *
	 * \param[in] b Index of the block to count tracks in
	 *
	 * \return Number of tracks flagged as TRUE in specified block.
	 */
	size_type total_tracks_set(index_type b) const;

private:

	/**
	 * \brief Value of the flag with index \c i.
	 *
	 * \param[in] i Index to access
	 *
	 * \return Value of the flag with the specified absolute index.
	 */
	bool operator[](const index_type i) const;

	/**
	 * \brief Converts a logical position to an absolute flag index.
	 *
	 * Equivalent to the sum of \c block_offset(b) and \c track_offset(t, v2).
	 *
	 * \param[in] b  0-based index of the block in \c response
	 * \param[in] t  0-based index of the track in block \c b
	 * \param[in] v2 TRUE requests offset of ARCSv2, FALSE requests ARCSv1
	 *
	 * \return Flag index for single ARCS
	 */
	index_type index(index_type b, index_type t, bool v2) const;

	/**
	 * \brief Converts a logical block index to an absolute flag index.
	 *
	 * Note that the block start is also the position of the flag respresenting
	 * the ARId comparison.
	 *
	 * \param[in] b  0-based index of the block in the DBAR
	 *
	 * \return Index of the start for the logical block \c b
	 */
	index_type block_offset(index_type b) const;

	/**
	 * \brief Converts a 0-based track number to an offset position within a
	 * block.
	 *
	 * \param[in] t  0-based index of the track in \c response
	 * \param[in] v2 Iff TRUE, the offset of the ARCSv2 is returned
	 *
	 * \return Offset for the flag index to be added to the start of the block
	 */
	index_type track_offset(index_type t, bool v2) const;

	/**
	 * \brief Set the flag on position \c offset to \c value.
	 *
	 * \param[in] offset
	 * \param[in] value
	 *
	 * \return Absolute index position the operation modified
	 */
	void set_flag(const index_type offset, const bool value);

	/**
	 * \brief Total number of flags per block.
	 *
	 * \return Total number of flags per block
	 */
	size_type flags_per_block() const;

	/**
	 * \brief Validate position request.
	 *
	 * If this function does not throw, block and track index are valid
	 *
	 * \param[in] b 0-based index of the block in \c response
	 * \param[in] t 0-based index of the track in \c response
	 */
	void check_maximums(const size_type b, const size_type t) const;

	/**
	 * \brief Ensures that \c b is a legal block value.
	 *
	 * \param[in] b 0-based index of the block in \c response
	 *
	 * \throws Iff \c b is out of range
	 */
	void bounds_check_block(index_type b) const;

	/**
	 * \brief Ensures that \c t is a legal track value.
	 *
	 * \param[in] t 0-based index of the track in \c response
	 *
	 * \throws Iff \c t is out of range
	 */
	void bounds_check_track(index_type t) const;

	/**
	 * \brief Create an initialized flag store.
	 *
	 * \param[in] size Total number of flags
	 *
	 * \return Initialized flagstore
	 */
	std::vector<bool> create_flag_store(const size_type size) const;

	/**
	 * \brief Number of blocks represented.
	 */
	size_type blocks_ {};

	/**
	 * \brief Number of tracks in each block.
	 */
	size_type tracks_per_block_ {};

	/**
	 * \brief The result bits of the comparison.
	 */
	std::vector<bool> flag_ {};
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


/** \brief Interface: policy to decide whether a given track is verified or not.
 *
 * The policy decides whether an individual track counts as is_verified() or
 * not, i.e. whether it contributes to the result being a "total match" or not.
 *
 * Its implementations may define that matching tracks are only considered
 * verified if they all occurr in the same block. Alternatively, just any
 * matching track in any block may be considered as verified.
 */
class VerificationPolicy
{
public:

	/**
	 * \brief Index type of this class.
	 */
	using index_type = VerificationResult::index_type;

	/**
	 * \copydoc SNPT_tp_size
	 */
	using size_type = VerificationResult::size_type;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~VerificationPolicy() noexcept = default;

	/**
	 * \brief TRUE if this instance qualifies the given track as verified.
	 *
	 * The policy may interpret the result data to decide whether \c track is
	 * verified or not.
	 *
	 * \param[in] track  The 0-based track to check for
	 * \param[in] result The result to interpret
	 *
	 * \return TRUE if the track counts as verified, otherwise FALSE.
	 */
	bool is_verified(const index_type track, const VerificationResult& result)
		const;

	/**
	 * \brief Total number of unverified tracks in the result.
	 *
	 * \param[in] result The result to interpret
	 *
	 * \return Total number of unverified tracks
	 */
	size_type total_unverified_tracks(const VerificationResult& result) const;

	/**
	 * \brief TRUE iff this policy is strict.
	 *
	 * A strict policy will define a track as is_verified() iff its checksum
	 * in the best block will match the actual checksum. Matching checksums
	 * in other blocks will be ignored by a strict policy but respected by a
	 * non-strict policy.
	 *
	 * \return TRUE iff this policy is strict.
	 */
	bool is_strict() const;

	/**
	 * \copydoc SNPT_mf_clone
	 */
	std::unique_ptr<VerificationPolicy> clone() const;

private:

	virtual bool do_is_verified(const index_type track,
			const VerificationResult& r) const
	= 0;

	virtual size_type do_total_unverified_tracks(
			const VerificationResult& r) const;

	virtual bool do_is_strict() const
	= 0;

	virtual std::unique_ptr<VerificationPolicy> do_clone() const
	= 0;
};


/**
 * \brief VerificationPolicy that accepts track matches in the same block as
 * verified.
 */
class StrictPolicy final : public VerificationPolicy
{
	// VerificationPolicy

	bool do_is_verified(const index_type track, const VerificationResult& r)
		const final;

	size_type do_total_unverified_tracks(const VerificationResult& r) const
		final;

	bool do_is_strict() const final;

	std::unique_ptr<VerificationPolicy> do_clone() const final;
};


/**
 * \brief VerificationPolicy that accepts matches in any block as verified.
 */
class LiberalPolicy final : public VerificationPolicy
{
	// VerificationPolicy

	bool do_is_verified(const index_type track, const VerificationResult& r)
		const final;

	bool do_is_strict() const final;

	std::unique_ptr<VerificationPolicy> do_clone() const final;
};


/**
 * \brief Default implementation of a VerificationResult.
 */
class Result final : public VerificationResult
{
public:

	/**
	 * \brief Constructor
	 *
	 * \param[in] policy VerificationPolicy to use when interpreting the result.
	 */
	explicit Result(std::unique_ptr<VerificationPolicy> policy);

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	Result(const Result& rhs);

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	Result& operator = (const Result& rhs);

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	Result(Result&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	Result& operator = (Result&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~Result() noexcept final = default;

	/**
	 * \brief Initializer helper.
	 *
	 * Caller has to call this after construction for setting the dimensions
	 * of the result.
	 *
	 * \param[in] total_blocks     Number of blocks
	 * \param[in] tracks_per_block Number of tracks per block
	 */
	void init(const size_type total_blocks,
			const size_type tracks_per_block);

	/**
	 * \brief VerificationPolicy used for interpreting the verification result.
	 *
	 * \return VerificationPolicy of this instance
	 */
	const VerificationPolicy* policy() const;

private:

	/**
	 * \brief Implementation: actual result flags
	 */
	ResultBits flags_ {};

	/**
	 * \brief Policy to interpret the flags.
	 */
	std::unique_ptr<VerificationPolicy> policy_ {};

	/**
	 * \brief Convert checksum::type to bool.
	 *
	 * \return Return TRUE iff \c type is checksum::type::ARCS2, otherwise FALSE
	 */
	bool is_v2(const checksum::type type) const;

	// VerificationResult

	index_type do_verify_id(const index_type b) final;

	bool do_id(const index_type b) const final;

	index_type  do_verify_track(const index_type b, const index_type t,
			const checksum::type type) final;

	bool do_track(const index_type b, const index_type t,
			const checksum::type type) const final;

	index_type do_difference(const index_type b, const checksum::type type)
		const final;

	size_type do_total_blocks() const final;

	size_type do_tracks_per_block() const final;

	size_type do_size() const final;

	bool do_is_verified(const index_type track) const final;

	size_type do_total_unverified_tracks() const final;

	best_block_info_t do_best_block() const final;

	index_type  do_best_block_difference() const final;

	bool do_strict() const final;

	std::unique_ptr<VerificationResult> do_clone() const final;
};


/**
 * \brief Service: create a VerificationResult object of a specified size.
 *
 * This is considered the "default" way to instantiate an empty
 * VerificationResult object. Every implementation that creates a
 * VerificationResult should create it by this function except for good
 * reasons.
 *
 * \param[in] blocks Number of blocks
 * \param[in] tracks Number of tracks per block
 * \param[in] p      VerificationPolicy to apply
 *
 * \return VerificationResult object of the specified dimensions.
 */
std::unique_ptr<VerificationResult> create_result(const std::size_t blocks,
		const std::size_t tracks, std::unique_ptr<VerificationPolicy> p);


/**
 * \brief Interface: access ChecksumSource by indices 'current' and 'counter'.
 *
 * The 'current' index does not change while iterating or accessing while the
 * 'counter' index is modified. Concrete implementations define whether
 * 'current' is the block or the track index and set 'counter' accordingly.
 */
class Selector
{
	virtual const uint32_t& do_get(const ChecksumSource& source,
			const ChecksumSource::size_type current,
			const ChecksumSource::size_type counter) const
	= 0;

	virtual std::unique_ptr<Selector> do_clone() const
	= 0;

public:

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~Selector() noexcept = default;

	/**
	 * \brief Get a checksum from \c s by \c current and \c counter.
	 *
	 * \param[in] source  ChecksumSource to read a value off
	 * \param[in] current Current fixed position
	 * \param[in] counter Counted position
	 */
	const uint32_t& get(const ChecksumSource& source,
			const ChecksumSource::size_type current,
			const ChecksumSource::size_type counter) const;

	/**
	 * \copydoc SNPT_mf_clone
	 */
	std::unique_ptr<Selector> clone() const;
};


/**
 * \brief Interprets \c current as a block and \c counter as a track.
 */
class BlockSelector final : public Selector
{
	// Selector

	std::unique_ptr<Selector> do_clone() const final;

	const uint32_t& do_get(const ChecksumSource& s,
			const ChecksumSource::size_type block,
			const ChecksumSource::size_type track) const final;
};


/**
 * \brief Interprets \c current as a track and \c counter as a block.
 */
class TrackSelector final : public Selector
{
	// Selector

	std::unique_ptr<Selector> do_clone() const final;

	const uint32_t& do_get(const ChecksumSource& s,
			const ChecksumSource::size_type track,
			const ChecksumSource::size_type block) const final;
};


/**
 * \brief Iterates a ChecksumSource.
 *
 * The SourceIterator iterates the members of a current fixed position in the
 * source. This position may be a block or a track. The iterator provides this
 * information by \c current(), which is either the number of a block or a
 * track.
 */
class SourceIterator final : Comparable<SourceIterator>
{
	/**
	 * \brief The concrete Selector used by this instance.
	 */
	const Selector* selector_ {};

	/**
	 * \brief ChecksumSource to iterate over.
	 */
	const ChecksumSource* source_ {};

	/**
	 * \brief Value of the current fixed position.
	 */
	ChecksumSource::size_type current_ {};

	/**
	 * \brief Value of the current counted position.
	 */
	ChecksumSource::size_type counter_ {};

public:

	/**
	 * \brief LegacyInputIterator.
	 */
	using iterator_category = std::input_iterator_tag;

	/**
	 * \copydoc SNPT_tp_value
	 */
	using value_type        = uint32_t;

	/**
	 * \copydoc SNPT_tp_reference
	 */
	using reference         = const value_type&;

	/**
	 * \copydoc SNPT_tp_pointer
	 */
	using pointer           = const value_type*;

	/**
	 * \copydoc SNPT_tp_difference
	 */
	using difference_type   = std::ptrdiff_t;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] source   ChecksumSource to iterate over
	 * \param[in] current  Fixed current position to iterate
	 * \param[in] counter  Counted position
	 * \param[in] selector Selector to access \c source
	 */
	SourceIterator(const ChecksumSource& ref_sums,
			const ChecksumSource::size_type current,
			const ChecksumSource::size_type counter, const Selector& selector);

	/**
	 * \brief The value the iterator counts.
	 *
	 * \return The value that is counted when moving the iterator.
	 */
	ChecksumSource::size_type counter() const;

	/**
	 * \brief The fixed value whose members are enumerated by counter().
	 *
	 * \return The fixed value whose members are enumerated by counter()
	 */
	ChecksumSource::size_type current() const;

	/**
	 * \copydoc SNPT_mf_deref
	 */
	reference       operator * ()  const;

	/**
	 * \copydoc SNPT_mf_arrow
	 */
	pointer         operator -> () const;

	/**
	 * \copydoc SNPT_mf_inc_prefix
	 */
	SourceIterator& operator ++ ();

	/**
	 * \copydoc SNPT_mf_inc_postfix
	 */
	SourceIterator  operator ++ (int);

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(SourceIterator& lhs, SourceIterator& rhs) noexcept
	{
		using std::swap;

		swap(lhs.source_,  rhs.source_ );
		swap(lhs.current_, rhs.current_);
		swap(lhs.counter_, rhs.counter_);
	}

	/**
	 * \copydoc SNPT_nf_equality
	 */
	friend bool operator == (const SourceIterator& lhs, const SourceIterator&
			rhs)
	{
		return lhs.source_  == rhs.source_
			&& lhs.current_ == rhs.current_
			&& lhs.counter_ == rhs.counter_;
	}
};


/**
 * \brief Interface: policy for traversals.
 *
 * The implementation of a TraversalPolicy can decide whether to traverse over
 * blocks checking the same track or traversing over tracks within the same
 * block.
 */
class TraversalPolicy
{
	/**
	 * \brief ChecksumSource to traverse
	 */
	const ChecksumSource* source_ {};

	/**
	 * \brief Concrete selector to use.
	 */
	std::unique_ptr<Selector> selector_ {};

	/**
	 * \brief Fixed position to traverse.
	 *
	 * This can either be the block or the track.
	 */
	ChecksumSource::size_type current_ {};


	virtual Checksums::size_type do_current_block(const SourceIterator& i) const
	= 0;

	virtual Checksums::size_type do_current_track(const SourceIterator& i) const
	= 0;

	virtual ChecksumSource::size_type do_end_current(
			const ChecksumSource& source,
			const Checksums::size_type c) const
	= 0;

	virtual ChecksumSource::size_type do_end_counter(
			const ChecksumSource& source,
			const Checksums::size_type c) const
	= 0;

	/**
	 * \brief Create the VerificationPolicy of this instance.
	 *
	 * \return VerificationPolicy to use
	 */
	virtual std::unique_ptr<VerificationPolicy> create_track_policy() const
	= 0;

	/**
	 * \brief Create the Selector of this instance.
	 *
	 * \return Selector to use
	 */
	virtual std::unique_ptr<Selector> create_selector() const
	= 0;

protected:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] selector Selector of the concrete subclass
	 */
	explicit TraversalPolicy(std::unique_ptr<Selector> selector);

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	TraversalPolicy(const TraversalPolicy& rhs);

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	TraversalPolicy& operator = (const TraversalPolicy& rhs);

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	TraversalPolicy(TraversalPolicy&& rhs) = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	TraversalPolicy& operator = (TraversalPolicy&& rhs) = default;

	/**
	 * \brief The Selector of this instance
	 *
	 * \return Selector of this instance
	 */
	const Selector& selector() const;

	/**
	 * \brief Worker for checking whether source() is a nullptr.
	 *
	 * \throws invalid_argument If source is nullptr
	 */
	void check_source_for_null() const;

public:

	/**
	 * \copydoc SNPT_tp_iterator
	 */
	using iterator       = const SourceIterator;

	/**
	 * \copydoc SNPT_tp_const_iterator
	 */
	using const_iterator = const SourceIterator;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~TraversalPolicy() noexcept = default;

	/**
	 * \brief The ChecksumSource to iterate.
	 *
	 * \return ChecksumSource to iterate
	 */
	const ChecksumSource* source() const;

	/**
	 * \brief Set the ChecksumSource to traverse
	 *
	 * \param[in] source The ChecksumSource to traverse
	 */
	void set_source(const ChecksumSource& source);

	/**
	 * \brief The current block or track to be traversed.
	 *
	 * \return Value for the \c current() item
	 */
	ChecksumSource::size_type current() const;

	/**
	 * \brief Set the current() item
	 *
	 * \param[in] current Set the value for the current item
	 */
	void set_current(const ChecksumSource::size_type current);

	/**
	 * \brief The end() value for \c current().
	 *
	 * When iterating over values for current(), this is the smallest illegal
	 * value.
	 *
	 * \param[in] c Index of current block
	 *
	 * \return The maximal legal value for current() + 1
	 */
	ChecksumSource::size_type end_current(
			const Checksums::size_type c) const;

	/**
	 * \brief The end() value for \c counter().
	 *
	 * When iterating over values for counter(), this is the smallest illegal
	 * value.
	 *
	 * \param[in] c Index of current block
	 *
	 * \return The maximal legal value for counter() + 1
	 */
	ChecksumSource::size_type end_counter(
			const Checksums::size_type c) const;

	/**
	 * \brief Start value for traversal
	 *
	 * \param[in] c Index of current block
	 *
	 * \return Start iterator for traversal
	 */
	const_iterator begin(const ChecksumSource::size_type current) const;

	/**
	 * \brief End value for traversal (after last legal value)
	 *
	 * \param[in] c Index of current block
	 *
	 * \return End iterator for traversal
	 */
	const_iterator end(const ChecksumSource::size_type current) const;

	/**
	 * \brief Get current block for iterator position.
	 *
	 * \param[in] i Iterator position
	 *
	 * \return Reference block for \c i
	 */
	Checksums::size_type current_block(const SourceIterator& i) const;

	/**
	 * \brief Get current track for iterator position.
	 *
	 * \param[in] i Iterator position
	 *
	 * \return Reference track for \c i
	 */
	Checksums::size_type current_track(const SourceIterator& i) const;

	/**
	 * \brief VerificationPolicy of this instance.
	 *
	 * \return VerificationPolicy of this instance.
	 */
	std::unique_ptr<VerificationPolicy> get_policy() const;

	/**
	 * \brief Selector of this instance.
	 *
	 * \return Selector of this instance.
	 */
	std::unique_ptr<Selector> get_selector() const;
};


/**
 * \brief TraversalPolicy to traverse all tracks in a single block.
 *
 * Iterates over all tracks in the \c current() block.
 */
class BlockTraversal final : public TraversalPolicy
{
	// TraversalPolicy

	std::unique_ptr<VerificationPolicy> create_track_policy() const final;

	std::unique_ptr<Selector> create_selector() const final;

	Checksums::size_type do_current_block(const SourceIterator& i) const final;

	Checksums::size_type do_current_track(const SourceIterator& i) const final;

	ChecksumSource::size_type do_end_current(const ChecksumSource& source,
			const Checksums::size_type c) const final;

	ChecksumSource::size_type do_end_counter(const ChecksumSource& source,
			const Checksums::size_type c) const final;

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	BlockTraversal();
};


/**
 * \brief TraversalPolicy to traverse values for a single track in all blocks.
 *
 * Iterates over each \c current() track in all blocks.
 */
class TrackTraversal final : public TraversalPolicy
{
	// TraversalPolicy

	std::unique_ptr<VerificationPolicy> create_track_policy() const final;

	std::unique_ptr<Selector> create_selector() const final;

	Checksums::size_type do_current_block(const SourceIterator& i) const final;

	Checksums::size_type do_current_track(const SourceIterator& i) const final;

	ChecksumSource::size_type do_end_current(const ChecksumSource& source,
			const Checksums::size_type c) const final;

	ChecksumSource::size_type do_end_counter(const ChecksumSource& source,
			const Checksums::size_type c) const final;

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	TrackTraversal();
};


/**
 * \brief Interface: policy to match the actual checksums against their
 * reference values.
 *
 * This policy defines which tracks will be respected when performing a match.
 * Subclasses may e.g. define that for a given reference checksum only the the
 * actual checksum with the same track index will be respected. Alternatively,
 * other tracks may also be tried for a match.
 */
class MatchPolicy
{
	virtual void do_perform(VerificationResult& result,
			const Checksums& actual_sums, const Checksum& ref,
			const int block, const Checksums::size_type track) const
	= 0;

protected:

	/**
	 * \brief Worker for matching an actual checksum against a reference value.
	 *
	 * Matches every checksum in \c actual against \ref. Every checksum::type
	 * is respected thereby. If the match succeeds, verify the position of
	 * \c block and \c track in \c result.
	 *
	 * \param[in,out] result Result to set verification flags
	 * \param[in]     actual Actual Checksums for \c track
	 * \param[in]     ref    Reference Checksum value for \c track
	 * \param[in]     block  Current reference block
	 * \param[in]     track  Current track
	 */
	void perform_match(VerificationResult& result,
			const ChecksumSet& actual, const Checksum& ref,
			const int block, const Checksums::size_type track) const;

public:

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~MatchPolicy() noexcept = default;

	/**
	 * \brief Perform the match operation on actual Checksums.
	 *
	 * \param[in,out] result Result to set verification flags
	 * \param[in]     actual_sums Actual Checksums
	 * \param[in]     ref    Reference Checksums
	 * \param[in]     block  Current reference block
	 * \param[in]     track  Current track
	 */
	void perform(VerificationResult& result, const Checksums& actual_sums,
			const Checksum& ref, const int block,
			const Checksums::size_type track) const;
};


/**
 * \brief Match reference and actual value for only the same track.
 */
class TrackOrderPolicy final : public MatchPolicy
{
	void do_perform(VerificationResult& result,
			const Checksums& actual_sums, const Checksum& ref,
			const int block, const Checksums::size_type track) const final;
};


/**
 * \brief For any reference value match every actual value.
 */
class FindOrderPolicy final : public MatchPolicy
{
	void do_perform(VerificationResult& result,
			const Checksums& actual_sums, const Checksum& ref,
			const int block, const Checksums::size_type track) const final;
};


/**
 * \brief Perform a match between an actual instance and a reference instance.
 *
 * The actual match is performed by calling operator == on the instances.
 *
 * \tparam T Type of instances to match
 *
 * \param[in] actual The actual instance to match
 * \param[in] ref    The reference instance to be matched
 */
template <typename T>
bool is_match(const T& actual, const T& ref)
{
	return actual == ref;
}


/**
 * \brief Worker: implements the application of traversal and order.
 *
 * Internally calls verify().
 *
 * \see verify
 */
class Verification final
{
	/**
	 * \brief Verify all ids in the specified result.
	 *
	 * \param[in] result The result object to modify
	 */
	void set_all_ids_verified(VerificationResult& result) const;

	/**
	 * \brief Perform verification with specified parameters.
	 *
	 * \param[in,out] result      Result to set verification flags
	 * \param[in]     actual_id   Actual ARId
	 * \param[in]     ref_sums    Reference Checksums
	 */
	void perform_ids(VerificationResult& result, const ARId& actual_id,
		const ChecksumSource& ref_sums) const;

	/**
	 * \brief Perform verification with specified parameters.
	 *
	 * \param[in,out] result      Result to set verification flags
	 * \param[in]     actual_sums Actual Checksums
	 * \param[in]     traversal   TraversalPolicy to apply
	 * \param[in]     current     Index of current block to traverse
	 * \param[in]     match       MatchPolicy to apply
	 */
	void perform_checksums(VerificationResult& result,
		const Checksums& actual_sums,
		const TraversalPolicy& traversal,
		const ChecksumSource::size_type current, const MatchPolicy& match) const;

public:

	/**
	 * \brief Perform verification with specified parameters.
	 *
	 * \param[in,out] result      Result to set verification flags
	 * \param[in]     actual_sums Actual Checksums
	 * \param[in]     actual_id   Actual ARId
	 * \param[in]     ref_sums    Reference Checksums
	 * \param[in]     traversal   TraversalPolicy to apply
	 * \param[in]     match       MatchPolicy to apply
	 */
	void perform(VerificationResult& result,
		const Checksums& actual_sums, const ARId& actual_id,
		const ChecksumSource& ref_sums,
		const TraversalPolicy& traversal, const MatchPolicy& match) const;
};


/**
 * \brief Worker: perform a verification.
 *
 * The implementation uses create_result() to create a VerificationResult.
 * It thereby uses the VerificationPolicy provided by the MatchTraversal. While
 * traversing it uses the MatchPolicy passed.
 *
 * This can be considered the "default implementation" of a verification
 * process. Every implementation that performs a verification should implement
 * it in terms of this function except for good reasons.
 *
 * \param[in] actual_sums Actual checksums to check for
 * \param[in] actual_id   Actual ARId to check for
 * \param[in] ref_sums    Reference checksums to match against
 * \param[in] traversal   TraversalPolicy to apply
 * \param[in] match       MatchPolicy to apply
 *
 * \return The verification result object
 */
std::unique_ptr<VerificationResult> verify(
		const Checksums& actual_sums, const ARId& actual_id,
		const ChecksumSource& ref_sums,
		const TraversalPolicy& traversal, const MatchPolicy& match);


/**
 * \internal
 *
 * \brief Interface: base class for Verifiers.
 */
class VerifierBase
{
	/**
	 * \brief Actual checksums to be verified.
	 */
	const Checksums* actual_sums_ {}; // non-owning

	/**
	 * \brief Flag to indicate strictness.
	 */
	bool is_strict_ {};

	/**
	 * \brief Provide actual ARId.
	 *
	 * Default implementation returns nullptr.
	 */
	virtual const ARId* do_actual_id() const noexcept;

	/**
	 * \brief Provide actual TraversalPolicy.
	 *
	 * Default implementation returns BlockTraversal for strict instances,
	 * TrackTraversal for non-strict instances.
	 *
	 * \return TraversalPolicy
	 */
	virtual std::unique_ptr<TraversalPolicy> do_create_traversal() const;

	/**
	 * \brief Provide actual MatchPolicy.
	 *
	 * \return MatchPolicy
	 */
	virtual std::unique_ptr<MatchPolicy> do_create_order() const
	= 0;

protected:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 */
	explicit VerifierBase(const Checksums* actual_sums);

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	VerifierBase() = default;

	VerifierBase(const VerifierBase&) = default;
	VerifierBase& operator= (const VerifierBase&) = default;

	VerifierBase(VerifierBase&&) noexcept = default;
	VerifierBase& operator= (VerifierBase&&) noexcept = default;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~VerifierBase() noexcept = default;

	/**
	 * \brief Actual ARId to be used when verifying.
	 *
	 * \return Actual ARId.
	 */
	const ARId* actual_id() const noexcept;

	/**
	 * \brief The actual checksums to be verified.
	 *
	 * \return The actual checksums to be verified.
	 */
	const Checksums* actual_checksums() const noexcept;

	/**
	 * \brief TRUE iff this instances performs verification strictly.
	 *
	 * \return TRUE iff verification method is strict
	 */
	bool strict() const noexcept;

	/**
	 * \brief Turn on or off strict verification.
	 *
	 * \param[in] strict Activate or deactivate strictness
	 */
	void set_strict(const bool strict) noexcept;

	/**
	 * \brief Perform a verification.
	 *
	 * \param[in] ref_sums Reference checksums to match against
	 *
	 * \return The verification result
	 */
	std::unique_ptr<VerificationResult> perform(const ChecksumSource& ref_sums)
		const;
};

} // namespace details

namespace best_block
{

/**
 * \internal
 *
 * \brief Implementation details.
 */
namespace details
{

/**
 * \brief Element indices of type best_block_info_t.
 */
enum class TUPLE_IDX : uint8_t
{
	INDEX         = 0,
	CHECKSUM_TYPE = 1,
	DIFFERENCE    = 2
};

/**
 * \brief Abstract access to best_block_info_t.
 */
template <TUPLE_IDX TIDX>
auto best_block_get(const best_block_info_t& bb)
    -> std::tuple_element_t<static_cast<std::size_t>(TIDX), best_block_info_t>
{
    constexpr auto tuple_index = static_cast<std::size_t>(TIDX);
    return std::get<tuple_index>(bb);
}

/**
 *
 * \brief Type flag of the best block in the VerificationResult.
 *
 * \param[in] bb The best_block_info_t to query
 *
 * \return Type flag of the best block
 */
inline bool typeflag(const best_block_info_t& bb)
{
	return details::best_block_get<TUPLE_IDX::CHECKSUM_TYPE>(bb);
}

} // namespace details
} // namespace best_block


/**
 * \internal
 *
 * \brief Implementation of an AlbumVerifier.
 */
class AlbumVerifier::Impl final : public details::VerifierBase
{
	/**
	 * \brief Internal actual ARId.
	 */
	const ARId* actual_id_ {}; // non-owning

	// VerifierBase

	const ARId* do_actual_id() const noexcept final;

	std::unique_ptr<details::MatchPolicy> do_create_order() const final;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 * \param[in] actual_id   Actual ARId to check for
	 */
	Impl(const Checksums& actual_sums, const ARId& actual_id);

	Impl() = default;
	~Impl() noexcept final = default;

	Impl(const Impl&) = default;
	Impl& operator= (const Impl&) = default;

	Impl(Impl&&) noexcept = default;
	Impl& operator= (Impl&&) noexcept = default;
};


/**
 * \internal
 *
 * \brief Implementation of a TracksetVerifier.
 */
class TracksetVerifier::Impl final : public details::VerifierBase
{
	// VerifierBase

	std::unique_ptr<details::MatchPolicy> do_create_order() const final;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 */
	explicit Impl(const Checksums& actual_sums);
};

/** @} */

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

