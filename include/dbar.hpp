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
 * \brief Parse a stream in AccurateRip HTTP-Response format to a DBAR object.
 *
 * \details
 *
 * A DBAR object is a parsed representation of the binary data contained in the
 * response to an AccurateRip request.
 *
 * A DBAR is a sequence of \link DBARBlock DBARBlocks\endlink where each
 * DBARBlock consists of a single DBARBlockHeader and a sequence of
 * \link DBARTriplet DBARTriplets\endlink.
 *
 * Intuitively, a DBAR object represents the ARCSs provided by AccurateRip for
 * the ARId specified in the related request. It contains the reference ARCSs
 * for all albums with this ARId.
 *
 * A DBAR instance provides access to all ARIds and ARCSs by their respective
 * indices. The sequence of blocks is iterable. Each DBARBlock is also iterable.
 *
 * A DBARBlock represents a single sequence of reference ARCSs associated with
 * an ARId. A DBARBlockHeader is a representation of the ARId the DBARBlock
 * relates to. A DBARTriplet contains the three values each block contains
 * once for each track and thus represents a track. The DBAR instance therefore
 * semantically represents a sequence of ARId-related Checksum sequences.
 *
 * This entails that a valid DBAR object has the following properties:
 *
 * - The ARId represented by the DBARBlockHeader is identical for each DBARBlock
 * within the DBAR instance.
 * - The total number of \link DBARTriplet DBARTriplets\endlink in a DBARBlock
 * is identical for every DBARBlock in a given DBAR instance.
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
 * To handle also possible parse errors, an instance of an implementation of
 * ParseErrorHandler can optionally be provided. A ParseErrorHandler implements
 * on_error(). If no ParseErrorHandler is provided, a StreamParseException is
 * thrown on any parse error.
 *
 * A DBARBuilder can be passed to the parse_*() functions as a ParseHandler that
 * constructs the DBAR object from the entire input stream. Alternatively,
 * custom implementations of ParseHandler can be used.
 *
 * DBARErrorHandler is the default ParseErrorHandler implementation that just
 * throws a StreamParseException on each error.
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
	 * \brief Swap with another instance.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap(DBARBlockHeader& rhs) noexcept;

	/**
	 * \brief TRUE iff this instance is equal to another instance.
	 *
	 * \param[in] rhs Instance to check for equality
	 *
	 * \return TRUE iff \c rhs == \c this
	 */
	bool equals(const DBARBlockHeader& rhs) const noexcept;

	/**
	 * \brief Create a string representation of this instance.
	 *
	 * \return String representation
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
	 * \brief Swap with another instance.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap(DBARTriplet& rhs) noexcept;

	/**
	 * \brief TRUE iff this instance is equal to another instance.
	 *
	 * \param[in] rhs Instance to check for equality
	 *
	 * \return TRUE iff \c rhs == \c this
	 */
	bool equals(const DBARTriplet& rhs) const noexcept;

	/**
	 * \brief Create a string representation of this instance.
	 *
	 * \return String representation
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


/**
 * \internal
 * \brief Apply an index on an indexed container.
 */
template<typename T>
auto get_element(const T& /*container*/, const typename T::size_type /*index*/)
	-> typename T::value_type
{
	return nullptr; // Default implementation does nothing
};


/**
 * \internal
 * \brief Forward iterator for DBAR related containers.
 *
 * \tparam T Type of object we iterate over, must define size_type
 */
template<typename T>
class DBARForwardIterator final : public Comparable<DBARForwardIterator<T>>
{
	using size_type = typename T::size_type;

public:

	using iterator_category = std::forward_iterator_tag;

	/**
	 * \brief Specialization of get_element<T>() yields the actual value_type
	 */
    using value_type        = decltype( get_element<T>(
				std::declval<T &>(),
				std::declval<size_type>()) );

    using reference         = value_type; // not a reference
    using pointer           = IteratorElement<value_type, size_type>;
    using difference_type   = std::ptrdiff_t;

private:

	/**
	 * \brief Current index position of the container.
	 *
	 * This index is the position to iterate over.
	 */
	size_type idx_;

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
		, container_ { &container }
	{
		// empty
	}

	DBARForwardIterator(const DBARForwardIterator& rhs)
		: idx_       { rhs.idx_ }
		, container_ { rhs.container_ }
	{
		// empty
	}

	DBARForwardIterator& operator=(const DBARForwardIterator& rhs)
	{
		if (&rhs != this)
		{
			idx_       = rhs.idx_;
			container_ = rhs.container_;
		}
		return *this;
	}

	DBARForwardIterator(DBARForwardIterator&& rhs) noexcept
		: idx_       { std::move(rhs.idx_) }
		, container_ { std::move(rhs.container_) }
	{
		// empty
	}

	DBARForwardIterator& operator=(DBARForwardIterator&& rhs) noexcept
	{
		idx_       = std::move(rhs.idx_);
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
		++idx_;
		return *this;
	}

    DBARForwardIterator operator++(int)
	{
		DBARForwardIterator i { *this };
		++i;
		return i;
	}

    friend bool operator == (const DBARForwardIterator& lhs,
			const DBARForwardIterator& rhs)
	{
		return lhs.container_ == rhs.container_
			&& lhs.idx_       == rhs.idx_;
	}
};


class DBARBlock;

/**
 * \brief Represents the content of dBAR file.
 */
class DBAR final
{
public:

	class Impl;

private:

	/**
	 * \brief Internal implementation.
	 */
	std::unique_ptr<Impl> impl_;

public:

	using size_type      = std::size_t;
	using value_type     = DBARBlock;
	using iterator       = DBARForwardIterator<DBAR>;
	using const_iterator = const iterator;

	/**
	 * \brief Default constructor.
	 */
	DBAR(); // required for IteratorElement

	/**
	 * \internal
	 * \brief Constructor for fabrication.
	 *
	 * \param[in] impl Impl of this DBAR
	 */
	explicit DBAR(std::unique_ptr<DBAR::Impl> impl);

	/**
	 * \brief Constructor.
	 *
	 * Intended for writing tests.
	 *
	 * \param[in] blocks List of DBAR block literals.
	 */
	DBAR(std::initializer_list<
			std::pair<
				std::tuple<unsigned, uint32_t, uint32_t, uint32_t>,
				std::initializer_list<std::tuple<uint32_t, int, uint32_t>>>>
			blocks);

	DBAR(const DBAR& rhs);
	DBAR& operator= (const DBAR& rhs);

	DBAR(DBAR&& rhs) noexcept;
	DBAR& operator= (DBAR&& rhs) noexcept;

	/**
	 * \brief Default destructor
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
	 * \brief Returns TRUE iff DBAR is empty, otherwise FALSE.
	 *
	 * \return TRUE iff DBAR is empty, otherwise FALSE.
	 */
	bool empty() const noexcept;

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

	iterator begin();
	iterator end();
	const_iterator cbegin() const;
	const_iterator cend() const;
	const_iterator begin() const;
	const_iterator end() const;

	/**
	 * \brief Swap with another instance.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap(DBAR& rhs) noexcept;

	/**
	 * \brief TRUE iff this instance is equal to another instance.
	 *
	 * \param[in] rhs Instance to check for equality
	 *
	 * \return TRUE iff \c rhs == \c this
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
	 * \brief Pointer to underlying DBAR.
	 */
	const DBAR* dBAR_;

	/**
	 * \brief Index of this block.
	 */
	DBAR::size_type idx_;

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
	 * \brief Iterator type.
	 */
	using iterator = DBARForwardIterator<DBARBlock>;

	/**
	 * \brief Constant iterator type.
	 */
	using const_iterator = const iterator;

	/**
	 * \brief Default constructor.
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

	iterator begin();
	iterator end();
	const_iterator cbegin() const;
	const_iterator cend() const;
	const_iterator begin() const;
	const_iterator end() const;

	/**
	 * \brief Swap with another instance.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap(DBARBlock& rhs) noexcept;

	/**
	 * \brief TRUE iff this instance is equal to another instance.
	 *
	 * \param[in] rhs Instance to check for equality
	 *
	 * \return TRUE iff \c rhs == \c this
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


// specialization for DBAR
template<>
auto get_element(const DBAR& object, const DBAR::size_type i)
	-> DBAR::value_type;


// specialization for DBARBlock
template<>
auto get_element(const DBARBlock& object, const DBARBlock::size_type i)
	-> DBARBlock::value_type;


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
	 * \brief Virtual default destructor.
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
	 * \brief Default constructor.
	 */
	DBARBuilder();

	/**
	 * \brief Default destructor.
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
	 * Every subsequent call of result() is undefined behaviour.
	 *
	 * \return The DBAR object representing the parsed input.
	 */
	DBAR result();
};


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
	virtual void do_on_error(const unsigned byte_counter,
			const unsigned block_counter, const unsigned block_byte_counter)
	= 0;

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ParseErrorHandler() noexcept = default;

	/**
	 * \brief React on error.
	 *
	 * \param[in] byte_counter       Absolute byte position of the error
	 * \param[in] block_counter      Block in which the error occurred
	 * \param[in] block_byte_counter Byte position relative to block start
	 */
	void on_error(const unsigned byte_counter, const unsigned block_counter,
			const unsigned block_byte_counter);
};


/**
 * \brief Default ParseErrorHandler for parsing DBAR objects.
 *
 * \todo This adds nothing to the default behaviour, maybe remove it
 */
class DBARErrorHandler final : public ParseErrorHandler
{
	void do_on_error(const unsigned byte_counter,
			const unsigned block_counter, const unsigned block_byte_counter)
		final;
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
	unsigned byte_pos_;

	/**
	 * \brief The 1-based block number of the block in which the exception
	 * occurred.
	 */
	unsigned block_;

	/**
	 * \brief Last 1-based block-relative byte position read before the
	 * exception.
	 */
	unsigned block_byte_pos_;

public:

	/**
	 * \brief Constructor with custom message.
	 *
	 * \param[in] byte_pos       Last 1-based global byte pos before exception
	 * \param[in] block          Current block index (1-based)
	 * \param[in] block_byte_pos Last 1-based block byte pos before exception
	 * \param[in] what_arg       Error message
	 */
	StreamParseException(const unsigned byte_pos, const unsigned block,
			const unsigned block_byte_pos, const std::string& what_arg);

	/**
	 * \brief Constructor with default message.
	 *
	 * \param[in] byte_pos       Last 1-based global byte pos before exception
	 * \param[in] block          Current block index (1-based)
	 * \param[in] block_byte_pos Last 1-based block byte pos before exception
	 */
	StreamParseException(const unsigned byte_pos, const unsigned block,
			const unsigned block_byte_pos);

	/**
	 * \brief Last 1-based global byte position before the exception occurred.
	 *
	 * \return Last 1-based global byte position before the exception occurred
	 */
	unsigned byte_position() const noexcept;

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
	unsigned block_byte_position() const noexcept;

private:

	/**
	 * \brief Compose default error message.
	 *
	 * \param[in] byte_pos       Last 1-based global byte pos before exception
	 * \param[in] block          Current block index (1-based)
	 * \param[in] block_byte_pos Last 1-based block byte pos before exception
	 */
	std::string default_message(const unsigned byte_pos, const unsigned block,
			const unsigned block_byte_pos) const;
};

/**
 * \brief Check a parsed value whether it is a valid ARCS (also frame 450 ARCS).
 *
 * \param[in] value Value to check for validity
 *
 * \return TRUE iff value is valid i.e. was parsed correctly
 */
bool is_valid_arcs(const uint32_t value);

/**
 * \brief Check a parsed value whether it is a valid confidence.
 *
 * \param[in] value Value to check for validity
 *
 * \return TRUE iff value is valid i.e. was parsed correctly
 */
bool is_valid_confidence(const unsigned value);

/**
 * \brief Parse an input stream.
 *
 * \param[in] in Input stream
 * \param[in] p  Handler for parse events
 * \param[in] e  Handler for parse errors
 *
 * \return Total number of bytes parsed
 */
uint32_t parse_stream(std::istream& in, ParseHandler* p,
		ParseErrorHandler* e);

/**
 * \brief Parse a file.
 *
 * \param[in] filename Name of the file to parse
 * \param[in] p  Handler for parse events
 * \param[in] e  Handler for parse errors
 *
 * \return Total number of bytes parsed
 */
uint32_t parse_file(const std::string& filename, ParseHandler* p,
		ParseErrorHandler* e);

/**
 * \brief Read an AccurateRip response file to a DBAR object.
 *
 * \param[in] filename Name of the file to parse
 *
 * \return DBAR object representing the file content
 */
DBAR load_file(const std::string& filename);

/** @} */

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

