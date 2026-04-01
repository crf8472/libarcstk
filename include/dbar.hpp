#ifndef LIBARCSTK_DBAR_HPP_
#define LIBARCSTK_DBAR_HPP_

/**
 * \file
 *
 * \brief Public API for \link dbar reading and representing dBAR-files\endlink.
 *
 * \details
 *
 * Parse dBAR-formatted data from files and streams.
 */

#ifndef LIBARCSTK_POLICIES_HPP_
#include "policies.hpp"     // for Comparable, IteratorElement
#endif

#include <cstddef>          // for size_t, nullptr
#include <cstdint>          // for uint32_t
#include <initializer_list> // for initializer_list
#include <istream>          // for istream
#include <iterator>         // for forward_iterator_tag
#include <memory>           // for unique_ptr
#include <stdexcept>        // for runtime_error
#include <string>           // for string
#include <tuple>            // for tuple
#include <utility>          // for pair

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

// avoid includes
class ARId;
class Checksum;

/**
 * \defgroup dbar AccurateRip DBAR Parser
 *
 * \brief Parse a byte stream in AccurateRip HTTP-Response format.
 *
 * \details
 *
 * A DBAR object is a parsed representation of the binary data contained in the
 * response to an AccurateRip request. It is forward-iterable and provides
 * access to each numerical element represented in the original input.
 *
 * Intuitively, a DBAR object represents the ARCSs provided by AccurateRip for
 * the ARId specified in the related request. It contains the reference ARCSs
 * for all albums with this ARId known to AccurateRip.
 *
 * A DBAR is a sequence of \link DBARBlock DBARBlocks\endlink where each
 * DBARBlock consists of a single DBARBlockHeader and a subsequent sequence of
 * \link DBARTriplet DBARTriplets\endlink.
 *
 * A DBAR instance provides access to all ARIds and ARCSs contained by their
 * respective indices. The sequence of blocks is iterable. Each DBARBlock is
 * also iterable.
 *
 * A DBAR is \link DBAR::empty() empty()\endlink when constructed with its
 * default constructor. Empty DBARs will turn into FALSE when checked via
 * operator bool() while every non-empty DBAR will turn into TRUE.
 *
 * A DBARBlock represents a single sequence of triplets of values associated
 * with the requested ARId. A DBARBlockHeader is a fine-granular representation
 * of the ARId the DBARBlock relates to. A DBARTriplet contains the three values
 * each block contains once for each track. A DBARTriplet thus represents a
 * track. The DBAR instance therefore semantically represents a sequence of
 * Checksum sequences all related to an ARId.
 *
 * A DBARBlock object is called \e valid iff the total number of
 * \link DBARTriplet DBARTriplets\endlink it contains matches the track count
 * encoded in the ARId represented by its DBARBlockHeader.
 *
 * A DBAR object is \e valid if each of the DBARBlocks it contains is valid.
 * This property can be checked by function \link is_valid(const DBAR&)
 * is_valid()\endlink. DBAR objects created by make_dbar() without exception are
 * guaranteed to be valid.
 *
 * A DBAR object is called \e uniform iff the ARId represented by the
 * DBARBlockHeaders is the same for each DBARBlock within the DBAR instance.
 * This property can be checked by function is_uniform().
 *
 * A DBAR object is called \e regular iff it is valid and uniform. This property
 * can be checked by function is_regular().
 *
 * Functions parse_stream() and parse_file() can parse a byte stream and provide
 * each single value.
 *
 * The parse_*() functions emit the following 8 parse events:
 *
 * - <tt>start_input</tt>:    when parsing starts, before the first byte
 * - <tt>start_block</tt>:    a new DBARBlock starts
 * - <tt>header</tt>:         a DBARBlockHeader was parsed
 * - <tt>start_triplets</tt>: a sequence of DBARTriplets starts
 * - <tt>triplets</tt>:       a DBARTriplet was parsed
 * - <tt>end_triplets</tt>:   the current sequence of DBARTriplets is completed
 * - <tt>end_block</tt>:      the current DBARBlock is completed
 * - <tt>end_input</tt>:      after the last byte when parsing is completed
 *
 * To handle the parse events an instance of an implementation of ParseHandler
 * must be provided. A ParseHandler implements each of those parse events.
 *
 * A StreamParseException contains every positional information about a parse
 * error. When a parse error occurrs, a StreamParseException is thrown by
 * default.
 *
 * Since we are parsing binary data values, there is no level of syntactic
 * validity. A StreamParseException will therefore indicate that the input has
 * not the expected length or that an I/O problem occurred.
 *
 * This behaviour can be changed by providing an instance of ParseErrorHandler
 * that implements member function on_error().
 *
 * A DBARBuilder can be passed to the parse_*() functions as a ParseHandler that
 * constructs the DBAR object from the entire input stream. If meta information
 * about validity, uniformity and regularity of the resulting DBAR object is of
 * interest, a CheckingDBARBuilder can be passed. Alternatively, custom
 * implementations of ParseHandler can be used.
 *
 * Function validated_dbar() is a validating variant of make_dbar(). It returns
 * a tuple whose first element is the actual dbar. The second element is a flag
 * indicating whether the DBAR is valid. The third element is a flag indicating
 * whether the DBAR is uniform. If the second and the third element both are
 * TRUE, the resulting DBAR is regular.
 *
 * \note
 * There is no way to inform the client whether the actual ARCS in an ARTriplet
 * is an ARCSv1 or an ARCSv2. The AccurateRip API does not distinguish blocks of
 * ARCSv1 from blocks of ARCSv2 and provides no information about the concrete
 * checksum algorithm. Different blocks of checksums of the same type are
 * considered just like ARCS sequences about different pressings of an album.
 *
 * @{
 */


/**
 * \brief The header of a DBARBlock.
 *
 * It encodes the ARId the block refers to as a binary representation.
 *
 * A DBARBlockHeader is a POD and holds copies of the values.
 */
class DBARBlockHeader final : public Comparable<DBARBlockHeader>
{
	/**
	 * \brief Total number of tracks in this block as declared.
	 */
	unsigned total_tracks_;

	/**
	 * \brief Id1.
	 */
	uint32_t id1_;

	/**
	 * \brief Id2.
	 */
	uint32_t id2_;

	/**
	 * \brief CDDB Id.
	 */
	uint32_t cddb_id_;

public:

	/**
	 * \brief Constructor for DBAR block header.
	 *
	 * \param[in] total_tracks Total number of tracks in this block
	 * \param[in] id1          Id1 of the ARId
	 * \param[in] id2          Id2 of the ARId
	 * \param[in] cddb_id      CDDB Id
	 */
	DBARBlockHeader(const unsigned total_tracks, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id);

	/**
	 * \brief Total number of tracks in this block.
	 *
	 * \return Total number of tracks in this block
	 */
	unsigned total_tracks() const noexcept;

	/**
	 * \brief Id1.
	 *
	 * \return Id1
	 */
	uint32_t id1() const noexcept;

	/**
	 * \brief Id2.
	 *
	 * \return Id2
	 */
	uint32_t id2() const noexcept;

	/**
	 * \brief CDDB Id.
	 *
	 * \return CDDB Id
	 */
	uint32_t cddb_id() const noexcept;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(DBARBlockHeader& rhs) noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const DBARBlockHeader& rhs) const noexcept;

	/**
	 * \copydoc SNPT_mf_to_string
	 */
	std::string to_string() const;


	friend void swap(DBARBlockHeader& lhs, DBARBlockHeader& rhs) noexcept
	{
		lhs.swap(rhs);
	}

    friend bool operator == (const DBARBlockHeader& lhs,
			const DBARBlockHeader& rhs) noexcept
	{
		return lhs.equals(rhs);
	}

	friend std::string to_string(const DBARBlockHeader& h)
	{
		return h.to_string();
	}
};


/**
 * \brief A triplet in a DBARBlock.
 *
 * A DBARTriplet represents reference data about a single track containing the
 * actual ARCS value, the confidence value and the ARCS value of frame 450 of
 * this track.
 *
 * A DBARTriplet is a POD and holds copies of the values.
 */
class DBARTriplet final : public Comparable<DBARTriplet>
{
	/**
	 * \brief ARCS value.
	 */
	uint32_t arcs_;

	/**
	 * \brief Confidence value.
	 */
	unsigned confidence_;

	/**
	 * \brief ARCS value of frame 450 of this track.
	 */
	uint32_t frame450_arcs_;

public:

	/**
	 * \brief Constructor of an empty triplet.
	 */
	DBARTriplet();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] arcs          ARCS value
	 * \param[in] confidence    Confidence value
	 * \param[in] frame450_arcs ARCS value of frame 450 of this track
	 */
	DBARTriplet(const uint32_t arcs, const unsigned confidence,
			const uint32_t frame450_arcs);

	/**
	 * \brief ARCS value.
	 *
	 * \return ARCS value.
	 */
	uint32_t arcs() const noexcept;

	/**
	 * \brief Confidence value.
	 *
	 * \return Confidence value.
	 */
	unsigned confidence() const noexcept;

	/**
	 * \brief ARCS value of frame 450.
	 *
	 * \return ARCS value of frame 450.
	 */
	uint32_t frame450_arcs() const noexcept;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(DBARTriplet& rhs) noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const DBARTriplet& rhs) const noexcept;

	/**
	 * \copydoc SNPT_mf_to_string
	 */
	std::string to_string() const;


	friend void swap(DBARTriplet& lhs, DBARTriplet& rhs) noexcept
	{
		lhs.swap(rhs);
	}

    friend bool operator == (const DBARTriplet& lhs,
			const DBARTriplet& rhs) noexcept
	{
		return lhs.equals(rhs);
	}

	friend std::string to_string(const DBARTriplet& t)
	{
		return t.to_string();
	}
};


namespace details
{


/**
 * \brief Apply an index on an indexed container.
 *
 * \tparam T The container type to get an element from
 *
 * \return The element on the specified index
 */
template<typename T>
auto get_element(const T& /*container*/, const typename T::size_type /*index*/)
	-> typename T::value_type
{
	return nullptr; // Default implementation does nothing
};


/**
 * \brief Increment an element.
 *
 * May be either increasing or decreasing increment.
 *
 * \tparam T The value to increment
 * \tparam R Indicator for reverse increment
 */
template<typename T, bool R>
class Increment
{
	// empty
};


template<typename T>
class Increment<T, false>
{
public:

	void operator() (T& i)
	{
		++i;
	}
};


template<typename T>
class Increment<T, true>
{
public:

	void operator() (T& i)
	{
		--i;
	}
};


/**
 * \brief Forward iterator for DBAR related containers.
 *
 * \tparam T Type of object we iterate over, must define size_type
 * \tparam R TRUE indicates reverse iteration
 */
template<typename T, bool R>
class DBARForwardIterator final : public Comparable<DBARForwardIterator<T,R>>
{
	/**
	 * \copydoc SNPT_tp_size
	 */
	using size_type = typename T::size_type;

	// intentionally undocumented
	using type_of_iterated_elements = decltype( get_element<T>(
				std::declval<T &>(),
				std::declval<size_type>()) );

public:

	/**
	 * \brief LegacyForwardIterator
	 */
	using iterator_category = std::forward_iterator_tag;

	/**
	 * \copydoc SNPT_tp_value
	 */
    using value_type        = type_of_iterated_elements;

	/**
	 * \copydoc SNPT_tp_reference
	 *
	 * \details Not an actual reference type.
	 */
    using reference         = value_type;

	/**
	 * \copydoc SNPT_tp_pointer
	 */
    using pointer           = IteratorElement<value_type, size_type>;

	/**
	 * \copydoc SNPT_tp_difference
	 */
    using difference_type   = std::ptrdiff_t;

private:

	/**
	 * \brief Current index position of the container.
	 *
	 * This index is the position to iterate over.
	 */
	size_type idx_;

	/**
	 * \brief Internal incrementor.
	 */
	Increment<size_type, R> increment_;

	/**
	 * \brief Container object to iterate over.
	 */
	const T* container_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] container Container to iterate over
	 * \param[in] idx       Container index position to iterate over
	 */
	DBARForwardIterator(const T& container, const size_type idx)
		: idx_       { idx }
		, increment_ { /* default */ }
		, container_ { &container }
	{
		// empty
	}

	DBARForwardIterator(const DBARForwardIterator& rhs)
		: idx_       { rhs.idx_ }
		, increment_ { rhs.increment_ }
		, container_ { rhs.container_ }
	{
		// empty
	}

	DBARForwardIterator& operator=(const DBARForwardIterator& rhs)
	{
		if (&rhs != this)
		{
			idx_       = rhs.idx_;
			increment_ = rhs.increment_;
			container_ = rhs.container_;
		}
		return *this;
	}

	DBARForwardIterator(DBARForwardIterator&& rhs) noexcept
		: idx_       { std::move(rhs.idx_) }
		, increment_ { std::move(rhs.increment_) }
		, container_ { std::move(rhs.container_) }
	{
		// empty
	}

	DBARForwardIterator& operator=(DBARForwardIterator&& rhs) noexcept
	{
		idx_       = std::move(rhs.idx_);
		increment_ = std::move(rhs.increment_);
		container_ = std::move(rhs.container_);

		return *this;
	}

	~DBARForwardIterator() noexcept final = default;

	reference operator*() const
	{
		return get_element(*this->container_, this->idx_);
	}

    pointer operator->() const
	{
		return pointer { idx_, get_element(*this->container_, this->idx_) };
	}

    DBARForwardIterator& operator++()
	{
		this->increment_(idx_);
		return *this;
	}

    DBARForwardIterator operator++ (int)
	{
		DBARForwardIterator prev_val { *this };
		this->operator++();
		return prev_val;
	}

	size_type index() const
	{
		return idx_;
	}

    friend bool operator == (const DBARForwardIterator& lhs,
			const DBARForwardIterator& rhs)
	{
		return lhs.container_ == rhs.container_
			&& lhs.idx_       == rhs.idx_;
	}
};

} // namespace details


class DBARBlock;

/**
 * \brief Represents the content of dBAR file.
 */
class DBAR final
{
public:

	class Impl;

private:

	// intentionally undocumented
	std::unique_ptr<Impl> impl_;

	// intentionally undocumented
	using unspecified_forward_iterator_type =
		details::DBARForwardIterator<DBAR, false>;

	// intentionally undocumented
	using unspecified_reverse_forward_iterator_type =
		details::DBARForwardIterator<DBAR, true>;

public:

	/**
	 * \brief Size type for this type, also used for indexing.
	 */
	using size_type = std::size_t;

	/**
	 * \brief Value type for this type.
	 */
	using value_type = DBARBlock;

	/**
	 * \brief Unspecified forward iterator type.
	 */
	using iterator = unspecified_forward_iterator_type;

	/**
	 * \brief Unspecified reverse forward iterator type.
	 */
	using reverse_iterator = unspecified_reverse_forward_iterator_type;

	/**
	 * \brief Unspecified constant forward iterator type.
	 */
	using const_iterator = const iterator;

	/**
	 * \brief Unspecified reverse constant forward iterator type.
	 */
	using const_reverse_iterator = const reverse_iterator;

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	DBAR(); // required for use with IteratorElement

	/**
	 * \internal
	 *
	 * \brief Constructor for fabrication.
	 *
	 * \param[in] impl Impl of this DBAR
	 */
	explicit DBAR(std::unique_ptr<DBAR::Impl> impl);

	/**
	 * \brief Constructor.
	 *
	 * Intended for use in tests.
	 *
	 * \param[in] blocks List of DBAR block literals.
	 */
	DBAR(std::initializer_list<
			std::pair<
				std::tuple<unsigned, uint32_t, uint32_t, uint32_t>,
				std::initializer_list<std::tuple<uint32_t, int, uint32_t>>>>
			blocks);

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	DBAR(const DBAR& rhs);

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	DBAR& operator = (const DBAR& rhs);

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	DBAR(DBAR&& rhs) noexcept;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	DBAR& operator = (DBAR&& rhs) noexcept;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~DBAR() noexcept;

	/**
	 * \brief Total number of blocks.
	 *
	 * \return Total number of blocks
	 */
	size_type size() const noexcept;

	/**
	 * \brief Physical total number of tracks in the specified block.
	 *
	 * \param[in] block_idx Index of the block to specify
	 *
	 * \return Size of the specified block
	 */
	size_type size(const size_type block_idx) const;

	/**
	 * \brief ARCS value of a track.
	 *
	 * \param[in] block_idx Specified block index
	 * \param[in] track_idx Specified track index
	 *
	 * \return ARCS value of the specified track
	 */
	const uint32_t& arcs_value(const size_type block_idx,
			const size_type track_idx) const;

	/**
	 * \brief Confidence value of a track.
	 *
	 * \param[in] block_idx Specified block index
	 * \param[in] track_idx Specified track index
	 *
	 * \return Confidence value of the specified track
	 */
	const unsigned& confidence_value(const size_type block_idx,
			const size_type track_idx) const;

	/**
	 * \brief ARCS value of frame 450 of a track.
	 *
	 * \param[in] block_idx Specified block index
	 * \param[in] track_idx Specified track index
	 *
	 * \return ARCS value frame 450 of the specified track
	 */
	const uint32_t& frame450_arcs_value(const size_type block_idx,
			const size_type track_idx) const;

	/**
	 * \brief Total number of tracks the specified block declares.
	 *
	 * \param[in] block_idx Block to access
	 *
	 * \return Total number of tracks as declared
	 */
	unsigned total_tracks(const size_type block_idx) const;

	/**
	 * \brief Header of the specified block.
	 *
	 * The returned object will hold copies of the values.
	 *
	 * \param[in] block_idx Block to return header of
	 *
	 * \return Header of the specified block.
	 */
	DBARBlockHeader header(const size_type block_idx) const;

	/**
	 * \brief Triplet representing the specified track.
	 *
	 * The returned object will hold copies of the values.
	 *
	 * \param[in] block_idx Block to lookup track
	 * \param[in] track_idx Track to return
	 *
	 * \return Specified triplet
	 */
	DBARTriplet triplet(const size_type block_idx,
		const size_type track_idx) const;

	/**
	 * \brief Checksum block in DBAR object.
	 *
	 * The returned object will hold references to the values.
	 *
	 * \param[in] block_idx Index of the block in the DBAR object
	 *
	 * \return Block with index \c block_idx
	 */
	DBARBlock block(const size_type block_idx) const;

	/**
	 * \copydoc SNPT_mf_begin
	 */
	iterator begin();

	/**
	 * \copydoc SNPT_mf_end
	 */
	iterator end();

	/**
	 * \copydoc SNPT_mf_cbegin
	 */
	const_iterator cbegin() const;

	/**
	 * \copydoc SNPT_mf_cend
	 */
	const_iterator cend() const;

	/**
	 * \copydoc SNPT_mf_cbegin
	 */
	const_iterator begin() const;

	/**
	 * \copydoc SNPT_mf_cend
	 */
	const_iterator end() const;

	/**
	 * \copydoc SNPT_mf_rbegin
	 */
	reverse_iterator rbegin();

	/**
	 * \copydoc SNPT_mf_rend
	 */
	reverse_iterator rend();

	/**
	 * \copydoc SNPT_mf_crbegin
	 */
	const_reverse_iterator crbegin() const;

	/**
	 * \copydoc SNPT_mf_crend
	 */
	const_reverse_iterator crend() const;

	/**
	 * \copydoc SNPT_mf_crbegin
	 */
	const_reverse_iterator rbegin() const;

	/**
	 * \copydoc SNPT_mf_crend
	 */
	const_reverse_iterator rend() const;

	/**
	 * \copydoc SNPT_mf_empty
	 */
	bool empty() const noexcept;

	/**
	 * \copydoc SNPT_mf_op_bool_if_empty
	 */
	explicit operator bool() const noexcept;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(DBAR& rhs) noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const DBAR& rhs) const noexcept;


	friend void swap(DBAR& lhs, DBAR& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	friend bool operator == (DBAR& lhs, DBAR& rhs) noexcept
	{
		return lhs.equals(rhs);
	}
};

/**
 * \brief A block in a DBAR.
 *
 * A DBARBlock represents a single block of triplets containing the actual ARCS
 * values, the confidence values and the ARCS values of frame 450 for each
 * album track. A DBARBlock contains a DBARBlockHeader that encodes the ARId of
 * the respective album the block refers to.
 *
 * A DBARBlock is iterable by a forward iterator.
 *
 * \attention
 * Destroying the DBAR instance a DBARBlock refers to invalidates the DBARBlock
 * instance. Accessing a DBARBlock whose underlying DBAR instance was destroyed
 * results in undefined behaviour.
 */
class DBARBlock final
{
	/**
	 * \brief Internal pointer to underlying DBAR.
	 */
	const DBAR* dBAR_;

	/**
	 * \brief Internal index of this block.
	 */
	DBAR::size_type idx_;

	// intentionally undocumented
	using unspecified_forward_iterator_type =
		details::DBARForwardIterator<DBARBlock, false>;

	// intentionally undocumented
	using unspecified_reverse_forward_iterator_type =
		details::DBARForwardIterator<DBARBlock, true>;

public:

	/**
	 * \brief Size type for this type, also used for indexing.
	 */
	using size_type = std::size_t;

	/**
	 * \brief Value type for this type.
	 */
	using value_type = DBARTriplet;

	/**
	 * \brief Unspecified forward iterator type.
	 */
	using iterator = unspecified_forward_iterator_type;

	/**
	 * \brief Unspecified reverse forward iterator type.
	 */
	using reverse_iterator = unspecified_reverse_forward_iterator_type;

	/**
	 * \brief Unspecified constant forward iterator type.
	 */
	using const_iterator = const iterator;

	/**
	 * \brief Unspecified reverse constant forward iterator type.
	 */
	using const_reverse_iterator = const reverse_iterator;

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	DBARBlock(); // required by IteratorElement

	/**
	 * \brief Constructor
	 *
	 * \param[in] dBAR      Underlying DBAR object
	 * \param[in] block_idx Index of the block to represent
	 */
	DBARBlock(const DBAR& dBAR, const DBAR::size_type block_idx);

	/**
	 * \brief Set DBAR and block index
	 *
	 * \param[in] dBAR      Underlying DBAR object
	 * \param[in] block_idx Index of the block to represent
	 */
	void set(const DBAR& dBAR, const DBAR::size_type block_idx);

	/**
	 * \brief Index of this block.
	 *
	 * \return Index of this block.
	 */
	size_type index() const noexcept;

	/**
	 * \brief Size of this block.
	 *
	 * The size of a block is the total number of tracks it contains.
	 * In a valid block, it is equal to <tt>header().total_tracks()</tt>.
	 *
	 * \return Size of this block.
	 */
	size_type size() const;

	/**
	 * \brief Header of this DBAR block.
	 *
	 * \return Header fo this DBAR block.
	 */
	DBARBlockHeader header() const;

	/**
	 * \brief Access track of this block.
	 *
	 * \param[in] t Index of the track to access.
	 *
	 * \return Track \c t of this block
	 */
	DBARTriplet triplet(const size_type t) const;

	/**
	 * \brief ARId of this block.
	 *
	 * \return ARId of this block
	 */
	ARId id() const;

	/**
	 * \copydoc SNPT_mf_begin
	 */
	iterator begin();

	/**
	 * \copydoc SNPT_mf_end
	 */
	iterator end();

	/**
	 * \copydoc SNPT_mf_cbegin
	 */
	const_iterator cbegin() const;

	/**
	 * \copydoc SNPT_mf_cend
	 */
	const_iterator cend() const;

	/**
	 * \copydoc SNPT_mf_cbegin
	 */
	const_iterator begin() const;

	/**
	 * \copydoc SNPT_mf_cend
	 */
	const_iterator end() const;

	/**
	 * \copydoc SNPT_mf_rbegin
	 */
	reverse_iterator rbegin();

	/**
	 * \copydoc SNPT_mf_rend
	 */
	reverse_iterator rend();

	/**
	 * \copydoc SNPT_mf_crbegin
	 */
	const_reverse_iterator crbegin() const;

	/**
	 * \copydoc SNPT_mf_crend
	 */
	const_reverse_iterator crend() const;

	/**
	 * \copydoc SNPT_mf_crbegin
	 */
	const_reverse_iterator rbegin() const;

	/**
	 * \copydoc SNPT_mf_crend
	 */
	const_reverse_iterator rend() const;

	/**
	 * \copydoc SNPT_mf_empty
	 */
	bool empty() const; // TODO noexcept?

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(DBARBlock& rhs) noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const DBARBlock& rhs) const noexcept;


	friend void swap(DBARBlock& lhs, DBARBlock& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	friend bool operator == (DBARBlock& lhs, DBARBlock& rhs) noexcept
	{
		return lhs.equals(rhs);
	}
};

/**
 * \brief Returns TRUE iff the DBARBlock passed is valid.
 *
 * A DBARBlock is valid the number of the DBARTriplets it contains is identical
 * to the track_count in its header.
 *
 * \param[in] block The DBARBlock to check
 *
 * \return TRUE iff \c block is valid
 */
bool is_valid(const DBARBlock& block);

/**
 * \brief Returns TRUE iff the DBAR passed is valid.
 *
 * A DBAR is valid iff each of its DBARBlocks is valid.
 *
 * \param[in] dbar The DBAR to check
 *
 * \return TRUE iff \c dbar is valid
 */
bool is_valid(const DBAR& dbar);

/**
 * \brief Returns TRUE iff the DBAR passed is uniform.
 *
 * A DBAR is uniform iff each of its DBARBlockHeaders contain the same ARId.
 *
 * \param[in] dbar The DBAR to check
 *
 * \return TRUE iff \c dbar is uniform
 */
bool is_uniform(const DBAR& dbar);

/**
 * \brief Returns TRUE iff the DBAR passed is regular.
 *
 * A DBAR is regular iff it is \link is_valid(const DBAR&) valid\endlink and
 * \link is_uniform() uniform\endlink.
 *
 * \param[in] dbar The DBAR to check
 *
 * \return TRUE iff \c dbar is regular
 */
bool is_regular(const DBAR& dbar);


namespace details
{

// specialization for DBAR
template<>
auto get_element(const DBAR& object, const DBAR::size_type i)
	-> DBAR::value_type;


// specialization for DBARBlock
template<>
auto get_element(const DBARBlock& object, const DBARBlock::size_type i)
	-> DBARBlock::value_type;

} // namespace details


/**
 * \brief Global instance of an empty DBAR.
 *
 * This is for convenience since in most cases, the creation of an empty
 * DBAR can be avoided when a reference instance is at hand.
 */
extern const DBAR EmptyDBAR;


/**
 * \brief Interface: parsing a byte stream in dBAR format.
 *
 * The handler reacts on starting and ending input, starting and ending a
 * block, on block headers and on triplets.
 */
class ParseHandler
{
	/**
	 * \brief On starting input.
	 */
	virtual void do_start_input()
	= 0;

	/**
	 * \brief On starting a new block.
	 */
	virtual void do_start_block()
	= 0;

	/**
	 * \brief On a block header.
	 *
	 * \param[in] total_tracks Total number of tracks in this block
	 * \param[in] id1          Id1 of the ARId
	 * \param[in] id2          Id2 of the ARId
	 * \param[in] cddb_id      CDDB Id
	 */
	virtual void do_header(const uint8_t total_tracks, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id)
	= 0;

	/**
	 * \brief On starting a sequence of triplets.
	 */
	virtual void do_start_triplets()
	= 0;

	/**
	 * \brief On a triplet.
	 *
	 * \param[in] arcs          ARCS value
	 * \param[in] confidence    Confidence value
	 * \param[in] frame450_arcs ARCS value of frame 450 of this track
	 */
	virtual void do_triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs)
	= 0;

	/**
	 * \brief On ending a sequence of triplets.
	 */
	virtual void do_end_triplets()
	= 0;

	/**
	 * \brief On ending a block.
	 */
	virtual void do_end_block()
	= 0;

	/**
	 * \brief On ending the input.
	 */
	virtual void do_end_input()
	= 0;

public:

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~ParseHandler() noexcept = default;

	/**
	 * \brief React on the start of the input.
	 */
	void start_input();

	/**
	 * \brief React on the start of an ARBlock.
	 */
	void start_block();

	/**
	 * \brief React on a block header.
	 *
	 * \param[in] total_tracks Total number of tracks in this block
	 * \param[in] id1          Id1 of the ARId
	 * \param[in] id2          Id2 of the ARId
	 * \param[in] cddb_id      CDDB Id
	 */
	void header(const uint8_t total_tracks, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id);

	/**
	 * \brief On starting a sequence of triplets.
	 */
	void start_triplets();

	/**
	 * \brief React on a triplet.
	 *
	 * \param[in] arcs          ARCS value
	 * \param[in] confidence    Confidence value
	 * \param[in] frame450_arcs ARCS value of frame 450 of this track
	 */
	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * \brief On ending a sequence of triplets.
	 */
	void end_triplets();

	/**
	 * \brief React on the end of an ARBlock.
	 */
	void end_block();

	/**
	 * \brief React on the end of the input.
	 */
	void end_input();
};


/**
 * \brief ParseHandler to build a DBAR object.
 *
 * The DBARBuilder is the most basic implementation of non-validating DBAR
 * construction. It just adds every header and every triplet to the DBAR
 * instance it builds.
 */
class DBARBuilder final : public ParseHandler
{
	/**
	 * \brief Internal result representation
	 */
	std::unique_ptr<DBAR::Impl> result_;

	// ParseHandler

	void do_start_input() final;

	void do_start_block() final;

	void do_header(const uint8_t track_count, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id) final;

	void do_start_triplets() final;

	void do_triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs) final;

	void do_end_triplets() final;

	void do_end_block() final;

	void do_end_input() final;

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	DBARBuilder();

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~DBARBuilder() noexcept final;

	/**
	 * \brief Parsing result.
	 *
	 * If this function is called before parsing has happened, an exception
	 * will occur. After the parsing process is finished successfully, this
	 * function can be called once to move the result to the caller.
	 *
	 * \attention
	 * Only the first call of result() is supported.
	 * Every subsequent call of result() leads arbitray results.
	 *
	 * \return The DBAR object representing the parsed input.
	 */
	DBAR result();

	/**
	 * \brief Resets the instance to its initial state.
	 */
	void reset();
};


/**
 * \brief ParseHandler to build a DBAR object and check for regularity.
 */
class CheckingDBARBuilder final : public ParseHandler
{
	class Impl;

	std::unique_ptr<Impl> impl_;

	// ParseHandler

	void do_start_input() final;

	void do_start_block() final;

	void do_header(const uint8_t track_count, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id) final;

	void do_start_triplets() final;

	void do_triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs) final;

	void do_end_triplets() final;

	void do_end_block() final;

	void do_end_input() final;

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	CheckingDBARBuilder();

	/**
	 * \brief Return validity state.
	 *
	 * \return Current validity state.
	 */
	bool result_is_valid() const;

	/**
	 * \brief Return uniformity state.
	 *
	 * \return Current uniformity state.
	 */
	bool result_is_uniform() const;

	/**
	 * \brief Return uniformity state.
	 *
	 * \return Current uniformity state.
	 */
	bool result_is_regular() const;

	/**
	 * \copydoc DBARBuilder::result()
	 */
	DBAR result();

	/**
	 * \brief Resets the instance to its initial state.
	 */
	void reset();
};


/**
 * \brief Represents a byte position in a binary input stream.
 */
using byte_position_t = unsigned;


/**
 * \brief Interface: error handling while parsing a dBAR format.
 */
class ParseErrorHandler
{
	/**
	 * \brief React on error.
	 *
	 * \param[in] byte_counter       Absolute byte position of the error
	 * \param[in] block_counter      Block in which the error occurred
	 * \param[in] block_byte_counter Byte position relative to block start
	 */
	virtual void do_on_error(const byte_position_t byte_counter,
			const unsigned block_counter,
			const byte_position_t block_byte_counter)
	= 0;

public:

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~ParseErrorHandler() noexcept = default;

	/**
	 * \brief React on error.
	 *
	 * \param[in] byte_counter       Absolute byte position of the error
	 * \param[in] block_counter      Block in which the error occurred
	 * \param[in] block_byte_counter Byte position relative to block start
	 */
	void on_error(const byte_position_t byte_counter,
			const unsigned block_counter,
			const byte_position_t block_byte_counter);
};

/**
 * \brief Reports a read error during parsing a binary stream.
 *
 * \attention
 * All byte positions are interpreted as 1-based.
 */
class StreamParseException final : public std::runtime_error
{
	/**
	 * \brief Last 1-based global byte position before the exception occurred.
	 */
	byte_position_t byte_pos_;

	/**
	 * \brief The 1-based block number of the block in which the exception
	 * occurred.
	 */
	unsigned block_;

	/**
	 * \brief Last 1-based block-relative byte position read before the
	 * exception.
	 */
	byte_position_t block_byte_pos_;

public:

	/**
	 * \brief Constructor with custom message.
	 *
	 * \param[in] byte_pos       Last 1-based global byte pos before exception
	 * \param[in] block          Current block index (1-based)
	 * \param[in] block_byte_pos Last 1-based block byte pos before exception
	 * \param[in] what_arg       Error message
	 */
	StreamParseException(const byte_position_t byte_pos,
			const unsigned block, const byte_position_t block_byte_pos,
			const std::string& what_arg);

	/**
	 * \brief Constructor with default message.
	 *
	 * \param[in] byte_pos       Last 1-based global byte pos before exception
	 * \param[in] block          Current block index (1-based)
	 * \param[in] block_byte_pos Last 1-based block byte pos before exception
	 */
	StreamParseException(const byte_position_t byte_pos,
			const unsigned block, const byte_position_t block_byte_pos);

	/**
	 * \brief Last 1-based global byte position before the exception occurred.
	 *
	 * \return Last 1-based global byte position before the exception occurred
	 */
	byte_position_t byte_position() const noexcept;

	/**
	 * \brief The 1-based block index of the block in which the exception
	 * occurred.
	 *
	 * \return The 1-based block number of the block
	 */
	unsigned block() const noexcept;

	/**
	 * \brief Last 1-based byte position relative to the start of the current
	 * block before the exception occurred.
	 *
	 * \return Last 1-based block byte position read before the exception
	 */
	byte_position_t block_byte_position() const noexcept;
};

/**
 * \brief Parse an input stream.
 *
 * The input stream must not be open on call.
 *
 * The parsing process acknowledges the track count values at the start of a
 * block and validates that the input has the expected length. On the premature
 * end of the input stream, ParserErrorHandler::on_error() is called or, in
 * case \c e is \c nullptr, a StreamParseException is thrown, which is the
 * default behaviour.
 *
 * The resulting DBAR is guaranteed to be valid if no exception occurrs and \c e
 * is \c nullptr. If \c e is not \c nullptr, it's up to the implementation of
 * \c e whether any guarantees are given.
 *
 * \param[in] in Input stream
 * \param[in] p  Handler for parse events
 * \param[in] e  Handler for parse errors
 *
 * \throw StreamReadException If reading of the stream fails
 *
 * \return Total number of bytes parsed
 */
uint32_t parse_stream(std::istream& in, ParseHandler* p,
		ParseErrorHandler* e);

/**
 * \brief Parse a file.
 *
 * A StreamReadException is thrown if the input has not the expected length.
 * The resulting DBAR is guaranteed to be valid if no exception occurrs.
 *
 * \param[in] filename Name of the file to parse
 * \param[in] p        Handler for parse events
 * \param[in] e        Handler for parse errors
 *
 * \throw StreamReadException If reading of the file fails
 *
 * \return Total number of bytes parsed
 */
uint32_t parse_file(const std::string& filename, ParseHandler* p,
		ParseErrorHandler* e);

/**
 * \brief Read an AccurateRip response file to a DBAR object.
 *
 * A StreamReadException is thrown if a parse error occurrs. The resulting DBAR
 * is guaranteed to be valid if no exception occurrs.
 *
 * \param[in] filename Name of the file to parse
 *
 * \throw StreamReadException If reading of the file fails
 *
 * \return DBAR object representing the file content
 */
DBAR make_dbar(const std::string& filename);

/**
 * \brief Read an AccurateRip response file to a DBAR object.
 *
 * The first value of the resulting tuple is the actual DBAR instance. The
 * second value is TRUE iff the DBAR is valid. The third value is TRUE iff the
 * DBAR is uniform. The DBAR is regular iff the second and the third value are
 * TRUE.
 *
 * A StreamReadException is thrown if a parse error occurrs. The resulting DBAR
 * is guaranteed to be valid if no exception occurrs.
 *
 * \param[in] filename Name of the file to parse
 *
 * \throw StreamReadException If reading of the file fails
 *
 * \return Tuple. 0: DBAR object, 1: validity flag, 2: uniformity flag
 */
std::tuple<DBAR,bool,bool> validated_dbar(const std::string& filename);

/** @} */

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

