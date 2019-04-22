#ifndef __LIBARCSTK_PARSE_HPP__
#define __LIBARCSTK_PARSE_HPP__

/** \file
 *
 * \brief Public API for AccurateRip response parsing and syntactic entities.
 *
 * An @link arcstk::v_1_0_0::ARStreamParser ARStreamParser @endlink parses the
 * binary content of a response of AccurateRip
 * to an id request. It can be registered two handlers, a
 * @link arcstk::v_1_0_0::ContentHandler ContentHandler @endlink
 * and an @link arcstk::v_1_0_0::ErrorHandler ErrorHandler @endlink both of
 * which have default implementations as there are
 * @link arcstk::v_1_0_0::DefaultContentHandler DefaultContentHandler @endlink
 * and
 * @link arcstk::v_1_0_0::DefaultErrorHandler DefaultErrorHandler @endlink.
 * @link arcstk::v_1_0_0::DefaultContentHandler DefaultContentHandler @endlink
 * just constructs an
 * @link arcstk::v_1_0_0::ARResponse ARResponse @endlink
 * that represents the parsed information.
 * @link arcstk::v_1_0_0::DefaultErrorHandler DefaultErrorHandler @endlink
 * just logs the error information.
 *
 * @link arcstk::v_1_0_0::ARResponse ARResponse @endlink
 * represents the entire response data from AccurateRip. It is
 * composed of a sequence of
 * @link arcstk::v_1_0_0::ARBlock ARBlocks @endlink of which each is
 * composed of a header containing the
 * @link arcstk::v_1_0_0::ARId ARId @endlink and a sequence of
 * @link arcstk::v_1_0_0::ARTriplet ARTriplets @endlink, one for each track.
 *
 * An @link arcstk::v_1_0_0::ARBlock ARBlock @endlink represents thus a single
 * checksum profile for a compact disc
 * while an @link arcstk::v_1_0_0::ARTriplet ARTriplet @endlink represents the
 * information for a single track.
 *
 * There are two standard implementations of
 * @link arcstk::v_1_0_0::ARStreamParser ARStreamParser @endlink provided,
 * @link arcstk::v_1_0_0::ARFileParser ARFileParser @endlink and
 * @link arcstk::v_1_0_0::ARStdinParser ARStdinParser @endlink.
 * @link arcstk::v_1_0_0::ARFileParser ARFileParser @endlink (suprisingly)
 * parses files obtained by just saving the payload of the AccurateRip response
 * to a file. @link arcstk::v_1_0_0::ARStdinParser ARStdinParser @endlink parses
 * stdin for this payload instead of a file. It can be used for parsing piped
 * input.
 *
 * A @link arcstk::v_1_0_0::StreamReadException StreamReadException @endlink is
 * thrown by an @link arcstk::v_1_0_0::ARStreamParser ARStreamParser @endlink
 * when the input byte stream ends prematurely or is otherwise corrupted. It
 * contains exact information about the stream-relative as well as the
 * block-relative error position.
 */

#include <cstdint>
#include <istream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace arcstk
{
inline namespace v_1_0_0
{


// Forward declaration for class ARId, used in the Interface
class ARId;

// Forward declaration for the private implementation
class ARTripletImpl;


/**
 * \defgroup parse AccurateRip Response Parser
 * @{
 */


/**
 * \brief A track-related triplet of values from a block in an AccurateRip
 * response.
 *
 * Syntactically, an ARTriplet is an element of some ARBlock.
 *
 * Semantically, an ARTriplet carries information about a single track in
 * an AccurateRip, while the data block describes the entire disc. An ARTriplet
 * contains the ARCS of the track as first element, a non-negative integer that
 * represents the confidence value of the ARCS and, as a third value, the ARCS
 * of frame 450 of the track. (The latter represents a check value for the
 * pressing offset.)
 *
 * Which track the ARTriplet describes can be derived from its actual
 * position in the AccurateRip response. (The first ARTriplet in the
 * ARBlock describes track 1, the second ARTriplet track 2 and so
 * forth.)
 *
 * An ARTriplet carries no information whether its ARCS are v1 or v2. It
 * contains only parsed data and metadata flags indicating whether the content
 * is valid, i.e. was correctly parsed.
 */
class ARTriplet final
{

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] arcs          The ARCS value of this triplet
	 * \param[in] confidence    The confidence value of this triplet
	 * \param[in] frame450_arcs The ARCS for frame 450 of this triplet
	 */
	ARTriplet(const uint32_t arcs, const uint32_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] arcs          The ARCS value of this triplet
	 * \param[in] confidence    The confidence value of this triplet
	 * \param[in] frame450_arcs The ARCS for frame 450 of this triplet
	 * \param[in] arcs_valid          Validity of ARCS of this triplet
	 * \param[in] confidence_valid    Validity of confidence of this triplet
	 * \param[in] frame450_arcs_valid Validity of frame 450 ARCS of this triplet
	 */
	ARTriplet(const uint32_t arcs, const uint32_t confidence,
			const uint32_t frame450_arcs, const bool arcs_valid,
			const bool confidence_valid, const bool frame450_arcs_valid);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARTriplet(const ARTriplet &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	ARTriplet(ARTriplet &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~ARTriplet() noexcept;

	/**
	 * \brief The track ARCS in this triplet.
	 *
	 * The ARCS may be v1 as well as v2.
	 *
	 * \return ARCS value in this triplet
	 */
	uint32_t arcs() const;

	/**
	 * \brief The confidence value in this triplet.
	 *
	 * \return Confidence in this triplet
	 */
	uint32_t confidence() const;

	/**
	 * \brief The ARCS of frame 450 of the particular track in this triplet.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	uint32_t frame450_arcs() const;

	/**
	 * \brief The track ARCS in this triplet.
	 *
	 * The ARCS may be v1 as well as v2.
	 *
	 * \return ARCS value in this triplet
	 */
	bool arcs_valid() const;

	/**
	 * \brief The confidence value in this triplet.
	 *
	 * \return Confidence in this triplet
	 */
	bool confidence_valid() const;

	/**
	 * \brief The ARCS of frame 450 of the particular track in this triplet.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	bool frame450_arcs_valid() const;

	/**
	 * \brief Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARTriplet& operator = (const ARTriplet &rhs);

	/**
	 * \brief Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARTriplet& operator = (ARTriplet &&rhs) noexcept;


private:

	/**
	 * \brief Private implementation of ARTriplet
	 */
	std::unique_ptr<ARTripletImpl> impl_;
};


/**
 * \brief An album-related block of triplets as it occurrs in an AccurateRip
 * response.
 *
 * An AccurateRip response is in fact parsed as a sequence of ARBlocks.
 *
 * Syntactically each ARBlock is an ordered pair of a header and a sequence
 * of @link ARTriplet ARTriplets @endlink.
 *
 * Semantically, an ARBlock contains exactly one ARTriplet for each
 * track of the original disc. This makes the ARBlock in fact an ARCS set
 * of the tracks on the disc along with their confidences and pressing offsets
 * for a disc. The header contains the AccurateRip Id of the disc.
 *
 * An AccurateRip response may contain different @link ARBlock ARBlocks @endlink
 * for a single disc id and the ARBlocks may differ in the ARCSs they contain
 * for the tracks.
 */
class ARBlock final
{

public: /* types */

	/**
	 * \brief An iterator over the @link ARTriplet ARTriplets @endlink of an
	 * ARBlock.
	 */
	using iterator = std::vector<ARTriplet>::iterator;

	/**
	 * \brief A const-iterator over the @link ARTriplet ARTriplets @endlink of
	 * an ARBlock.
	 */
	using const_iterator = std::vector<ARTriplet>::const_iterator;


public: /* methods */

	/**
	 * \brief Default constructor.
	 *
	 * \param[in] id ARId of the cd this block describes
	 */
	explicit ARBlock(const ARId &id);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARBlock(const ARBlock &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	ARBlock(ARBlock &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~ARBlock() noexcept;

	/**
	 * \brief Returns the AccurateRip Id (i.e. the header) of the block.
	 *
	 * \return ARId of this block
	 */
	const ARId& id() const;

	/**
	 * \brief Append a triplet to this block.
	 *
	 * \param[in] triplet Append a new triplet to this block
	 */
	void append(const ARTriplet &triplet);

	/**
	 * \brief Returns the size of this ARBlock, i.e. the number of triplets it
	 * contains.
	 *
	 * \return The number of triplets in this block.
	 */
	uint32_t size() const;

	/**
	 * \brief Return iterator pointing to first block.
	 *
	 * \return iterator pointing to first block
	 */
	iterator begin();

	/**
	 * \brief Return iterator pointing behind last block.
	 *
	 * \return iterator pointing behind last block
	 */
	iterator end();

	/**
	 * \brief Return const_iterator pointing to first block.
	 *
	 * \return const_iterator pointing to first block
	 */
	const_iterator begin() const;

	/**
	 * \brief Return const_iterator pointing behind last block.
	 *
	 * \return const_iterator pointing behind last block
	 */
	const_iterator end() const;

	/**
	 * \brief Return const_iterator pointing to first block.
	 *
	 * \return const_iterator pointing to first block
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Return const_iterator pointing behind last block.
	 *
	 * \return const_iterator pointing behind last block
	 */
	const_iterator cend() const;

	// TODO rbegin,rend + crbegin,crend

	/**
	 * \brief Return the ARTriplet with the specified \c index.
	 *
	 * \return ARTriplet at index
	 */
	const ARTriplet& operator [](const int index) const;

	/**
	 * \brief Return the ARTriplet with the specified \c index.
	 *
	 * \return ARTriplet at index
	 */
	ARTriplet& operator [](const int index);

	/**
	 * \brief Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARBlock& operator = (const ARBlock &rhs);

	/**
	 * \brief Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARBlock& operator = (ARBlock &&rhs) noexcept;


private:

	// Forward declaration for the private implementation
	class Impl;

	/**
	 * \brief Private implementation of ARBlock.
	 */
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Response content from AccurateRip.
 */
class ARResponse final
{

public: /* types */

	/**
	 * \brief An iterator over the @link ARBlock ARBlocks @endlink of an
	 * ARResponse.
	 */
	using iterator = std::vector<ARBlock>::iterator;

	/**
	 * \brief A const_iterator over the @link ARBlock ARBlocks @endlink of an
	 * ARResponse.
	 */
	using const_iterator = std::vector<ARBlock>::const_iterator;


public: /* methods */

	/**
	 * \brief Default constructor.
	 */
	ARResponse();

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARResponse(const ARResponse &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	ARResponse(ARResponse &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~ARResponse() noexcept;

	/**
	 * \brief Appends an ARBlock to the response.
	 *
	 * \param[in] block The ARBlock to append
	 */
	void append(const ARBlock &block);

	/**
	 * \brief Return block with 0-based index \c i.
	 *
	 * \param[in] i Index of the ARBlock to get
	 *
	 * \return Block \c i .
	 */
	const ARBlock& block(const int i) const;

	/**
	 * \brief Returns the number of blocks in this response.
	 *
	 * \return Number of blocks in this response
	 */
	std::size_t size() const;

	/**
	 * \brief Number of tracks per block.
	 *
	 * \return Number of tracks per block
	 */
	int tracks_per_block() const;

	/**
	 * \brief Return iterator pointing to first block.
	 *
	 * \return iterator pointing to first block
	 */
	iterator begin();

	/**
	 * \brief Return iterator pointing behind last block.
	 *
	 * \return iterator pointing behind last block
	 */
	iterator end();

	/**
	 * \brief Return const_iterator pointing to first block.
	 *
	 * \return const_iterator pointing to first block
	 */
	const_iterator begin() const;

	/**
	 * \brief Return const_iterator pointing behind last block.
	 *
	 * \return const_iterator pointing behind last block
	 */
	const_iterator end() const;

	/**
	 * \brief Return const_iterator pointing to first block.
	 *
	 * \return const_iterator pointing to first block
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Return const_iterator pointing behind last block.
	 *
	 * \return const_iterator pointing behind last block
	 */
	const_iterator cend() const;

	// TODO rbegin,rend + crbegin,crend

	/**
	 * \brief Return the ARBlock with the specified \c index.
	 *
	 * \param[in] index The index of the ARBlock to get
	 *
	 * \return ARBlock at index
	 */
	const ARBlock& operator [](const int index) const;

	/**
	 * \brief Return the ARBlock with the specified \c index.
	 *
	 * \param[in] index The index of the ARBlock to get
	 *
	 * \return ARBlock at index
	 */
	ARBlock& operator [](const int index);

	/**
	 * \brief Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARResponse& operator = (const ARResponse &rhs);

	/**
	 * \brief Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARResponse& operator = (ARResponse &&rhs) noexcept;


private:

	// Forward declaration for the private implementation
	class Impl;

	/**
	 * \brief Private implementation of ARResponse.
	 */
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Interface for ARStreamParser content handlers.
 *
 * Add a content handler to an ARStreamParser to actually add behaviour for
 * parsing an ARResponse.
 */
class ContentHandler
{

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ContentHandler() noexcept;

	/**
	 * \brief React on the start of the input.
	 */
	void start_input();

	/**
	 * \brief React on the start of an ARBlock.
	 */
	void start_block();

	/**
	 * \brief React on an id.
	 *
	 * Each ARBlock starts with the id of the disc it describes, so this
	 * method is always called directly after <tt>start_block()</tt>.
	 *
	 * \param[in] track_count Track count in this id
	 * \param[in] id1         Disc id 1 in this id
	 * \param[in] id2         Disc id 2 in this id
	 * \param[in] cddb_id     CDDB id in this id
	 */
	void id(const uint8_t track_count,
			const uint32_t id1,
			const uint32_t id2,
			const uint32_t cddb_id);

	/**
	 * \brief React on an ARTriplet.
	 *
	 * \param[in] arcs          ARCS in this triplet
	 * \param[in] confidence    Confidence in this triplet
	 * \param[in] frame450_arcs ARCS of frame 450 in this triplet
	 */
	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * \brief React on an ARTriplet.
	 *
	 * \param[in] arcs                The ARCS value of this triplet
	 * \param[in] confidence          The confidence value of this triplet
	 * \param[in] frame450_arcs       The ARCS for frame 450 of this triplet
	 * \param[in] arcs_valid          Validity of ARCS of this triplet
	 * \param[in] confidence_valid    Validity of confidence of this triplet
	 * \param[in] frame450_arcs_valid Validity of frame 450 ARCS of this triplet
	 */
	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs,
			const bool arcs_valid,
			const bool confidence_valid,
			const bool frame450_arcs_valid);

	/**
	 * \brief React on the end of an ARBlock.
	 */
	void end_block();

	/**
	 * \brief React on the end of the input.
	 */
	void end_input();

	/**
	 * \brief Clone this object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return Deep copy of this object
	 */
	std::unique_ptr<ContentHandler> clone() const;


private:

	/**
	 * \brief Implements @link ContentHandler::start_input() start_input() @endlink
	 */
	virtual void do_start_input()
	= 0;

	/**
	 * \brief Implements @link ContentHandler::start_block() start_block() @endlink
	 */
	virtual void do_start_block()
	= 0;

	/**
	 * \brief Implements @link ContentHandler::id(const uint8_t track_count, const uint32_t id1, const uint32_t id2, const uint32_t cddb_id) id() @endlink
	 *
	 * Each ARBlock starts with the id of the disc it describes, so this
	 * method is always called directly after <tt>start_block()</tt>.
	 *
	 * \param[in] track_count Track count in this id
	 * \param[in] id1         Disc id 1 in this id
	 * \param[in] id2         Disc id 2 in this id
	 * \param[in] cddb_id     CDDB id in this id
	 */
	virtual void do_id(const uint8_t track_count,
			const uint32_t id1,
			const uint32_t id2,
			const uint32_t cddb_id)
	= 0;

	/**
	 * \brief Implements @link ContentHandler::triplet(const uint32_t arcs, const uint8_t confidence, const uint32_t frame450_arcs) triplet() @endlink
	 *
	 * \param[in] arcs          ARCS in this triplet
	 * \param[in] confidence    Confidence in this triplet
	 * \param[in] frame450_arcs ARCS of frame 450 in this triplet
	 */
	virtual void do_triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs)
	= 0;

	/**
	 * \brief Implements @link ContentHandler::triplet(const uint32_t arcs, const uint8_t confidence, const uint32_t frame450_arcs, const bool arcs_valid, const bool confidence_valid, const bool frame450_arcs_valid) triplet() @endlink
	 *
	 * \param[in] arcs                The ARCS value of this triplet
	 * \param[in] confidence          The confidence value of this triplet
	 * \param[in] frame450_arcs       The ARCS for frame 450 of this triplet
	 * \param[in] arcs_valid          Validity of ARCS of this triplet
	 * \param[in] confidence_valid    Validity of confidence of this triplet
	 * \param[in] frame450_arcs_valid Validity of frame 450 ARCS of this triplet
	 */
	virtual void do_triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs,
			const bool arcs_valid,
			const bool confidence_valid,
			const bool frame450_arcs_valid)
	= 0;

	/**
	 * \brief Implements @link ContentHandler::end_block() end_block() @endlink
	 */
	virtual void do_end_block()
	= 0;

	/**
	 * \brief Implements @link ContentHandler::end_input() end_input() @endlink
	 */
	virtual void do_end_input()
	= 0;

	/**
	 * \brief Implements @link ContentHandler::clone() const clone() @endlink
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return Deep copy of this object
	 */
	virtual std::unique_ptr<ContentHandler> do_clone() const
	= 0;
};


/**
 * \brief Constructs an ARResponse instance from parsed content.
 *
 * Create an ARResponse and use set_object() to inform the handler about that
 * object. The handler instance will then use this object for construction.
 * After parse() is finished, the object contains the parsed content.
 */
class DefaultContentHandler final : public ContentHandler
{

public:

	/**
	 * \brief Default constructor.
	 */
	DefaultContentHandler();

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	DefaultContentHandler(const DefaultContentHandler &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	DefaultContentHandler(DefaultContentHandler &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~DefaultContentHandler() noexcept final;

	/**
	 * \brief Set the object constructed by the parsed content.
	 *
	 * \param[in,out] object Object to construct from parsed content.
	 */
	void set_object(ARResponse &object);

	/**
	 * \brief Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	DefaultContentHandler& operator = (const DefaultContentHandler &rhs);

	/**
	 * \brief Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	DefaultContentHandler& operator = (DefaultContentHandler &&rhs) noexcept;


private:

	void do_start_input() final;

	void do_start_block() final;

	void do_id(const uint8_t track_count,
			const uint32_t id1,
			const uint32_t id2,
			const uint32_t cddb_id) final;

	void do_triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs) final;

	void do_triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs,
			const bool arcs_valid,
			const bool confidence_valid,
			const bool frame450_arcs_valid) final;

	void do_end_block() final;

	void do_end_input() final;

	std::unique_ptr<ContentHandler> do_clone() const final;

	// Forward declaration for private implementation.
	class Impl;

	/**
	 * Internal implementation instance
	 */
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Interface for ARStreamParser error handlers.
 *
 * Defines the handler methods to react on parse errors of an ARStreamParser.
 */
class ErrorHandler
{

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ErrorHandler() noexcept;

	/**
	 * \brief Error notification.
	 *
	 * \param[in] byte_pos       Last 1-based byte pos read before error
	 * \param[in] block          1-based block number
	 * \param[in] block_byte_pos Last 1-based block byte pos read before error
	 */
	void error(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos);

	/**
	 * \brief Clone this object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return Deep copy of this object
	 */
	std::unique_ptr<ErrorHandler> clone() const;


private:

	/**
	 * \brief Error notification.
	 *
	 * \param[in] byte_pos       Last 1-based byte pos read before error
	 * \param[in] block          1-based block number
	 * \param[in] block_byte_pos Last 1-based block byte pos read before error
	 */
	virtual void do_error(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos)
	= 0;

	/**
	 * \brief Clone this object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return Deep copy of this object
	 */
	virtual std::unique_ptr<ErrorHandler> do_clone() const
	= 0;
};


/**
 * \brief Logs error information to stdout.
 */
class DefaultErrorHandler final : public ErrorHandler
{

public:

	void do_error(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos) final;

	std::unique_ptr<ErrorHandler> do_clone() const final;
};


/**
 * \brief Reports a read error during parsing a binary stream.
 */
class StreamReadException final : public std::runtime_error
{

public:

	/**
	 * Constructor.
	 *
	 * \param[in] byte_pos       Last 1-based byte pos read before error
	 * \param[in] block          1-based block number
	 * \param[in] block_byte_pos Last 1-based block byte pos read before error
	 * \param[in] what_arg       Error message
	 */
	StreamReadException(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos, const std::string &what_arg);

	/**
	 * Constructor.
	 *
	 * \param[in] byte_pos       Last 1-based byte pos read before error
	 * \param[in] block          1-based block number
	 * \param[in] block_byte_pos Last 1-based block byte pos read before error
	 * \param[in] what_arg       Error message
	 */
	StreamReadException(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos, const char *what_arg);

	/**
	 * Return last 1-based byte position before the exception occurred.
	 *
	 * \return Last 1-based byte position before the exception occurred
	 */
	uint32_t byte_position() const;

	/**
	 * Return the 1-based block number of the block.
	 *
	 * \return The 1-based block number of the block
	 */
	uint32_t block() const;

	/**
	 * Return last 1-based byte position relative to the start of the current
	 * block before the exception occurred.
	 *
	 * \return Last 1-based block byte position read before the exception
	 */
	uint32_t block_byte_position() const;


private:

	/**
	 * Last 1-based byte position before the exception occurred
	 */
	const uint32_t byte_pos_;

	/**
	 * The 1-based block number of the block
	 */
	const uint32_t block_;

	/**
	 * Last 1-based block byte position read before the exception
	 */
	const uint32_t block_byte_pos_;
};


/**
 * \brief Abstract base class for parsing an AccurateRip response.
 *
 * ARStreamParser encapsulates the actual parsing process on an std::istream.
 *
 * A ContentHandler is required to actually process the parsed content. An
 * ErrorHandler can optionally be set to perform some processing of the
 * error information before a StreamReadException is thrown. Since the
 * AccurateRip reference data is binary data, there is no perspective to
 * sanitize the stream or proceed parsing after an exception. An ErrorHandler
 * is therefore mere convenience to get informative log messages.
 *
 * Concrete subclasses are responsible for implementing
 * do_parse() method. Method parse_stream() is provided as a building block for
 * this implementation. Hook method on_catched_exception() is called before the
 * exception is rethrown, so the actual stream can be closed or other cleanup
 * can be performed.
 */
class ARStreamParser
{

public:

	/**
	 * \brief Default constructor.
	 */
	ARStreamParser();

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ARStreamParser() noexcept;

	/**
	 * \brief Set the content handler for this parser instance.
	 *
	 * \param[in] handler The content handler for this instance
	 */
	void set_content_handler(std::unique_ptr<ContentHandler> handler);

	/**
	 * \brief Return the content handler of this parser instance.
	 *
	 * \return Content handler of this instance
	 */
	const ContentHandler& content_handler() const;

	/**
	 * \brief Set the error handler for this parser instance.
	 *
	 * \param[in] handler The error handler for this instance
	 */
	void set_error_handler(std::unique_ptr<ErrorHandler> handler);

	/**
	 * \brief Return the error handler of this parser instance.
	 *
	 * \return Error handler of this instance
	 */
	const ErrorHandler& error_handler() const;

	/**
	 * \brief Parses the configured input stream.
	 *
	 * \return Number of bytes parsed from configured input stream
	 */
	uint32_t parse();


protected:

	/**
	 * \brief Parses the (opened) byte stream of an AccurateRip response.
	 *
	 * The stream is required to be appropriately opened. It is further
	 * required that failbit and badbit are set and exceptions are activated.
	 *
	 * \param[in] in_stream The stream to be parsed
	 *
	 * \return Number of bytes parsed from \c in_stream
	 */
	uint32_t parse_stream(std::istream &in_stream);


private:

	/**
	 * \brief Implements @link ARStreamParser::parse() parse() @endlink.
	 *
	 * \return Number of bytes parsed from configured input stream
	 */
	virtual uint32_t do_parse()
	= 0;

	/**
	 * \brief Hook: Called by
	 * @link ARStreamParser::parse_stream() parse_stream() @endlink on a
	 * StreamReadException before the exception is rethrown.
	 *
	 * This hook can be used to close the stream in case this is required or to
	 * perform other steps before rethrowing.
	 *
	 * \param[in] stream The opened stream in the state after the exception
	 * \param[in] e      The std::exception thrown
	 */
	virtual void on_catched_exception(std::istream &stream,
			const std::exception &e) const
	= 0;


	// Forward declaration for private implementation.
	class Impl;

	/**
	 * Internal implementation instance
	 */
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Parser for AccurateRip response as a file.
 *
 * This class parses dBAR-\*.bin files saved by the actual ripper software
 * or achieved by an HTTP request to AccurateRip. Those files are just the byte
 * stream of the AccurateRip response persisted to the file system.
 */
class ARFileParser final : public ARStreamParser
{

public:

	/**
	 * \brief Constructor.
	 */
	ARFileParser();

	/**
	 * \brief Constructor for specific file.
	 *
	 * \param[in] filename Name of the file to parse
	 */
	explicit ARFileParser(const std::string &filename);

	/**
	 * \brief Set the file to be parsed.
	 *
	 * \param[in] filename Name of the file to parse
	 */
	void set_file(const std::string &filename);

	/**
	 * \brief Name of the file to parse.
	 *
	 * \return Name of the file that is parsed when
	 * @link ARStreamParser::parse() parse() @endlink is called.
	 */
	std::string file() const;


private:

	uint32_t do_parse() final;

	void on_catched_exception(std::istream &istream,
			const std::exception &e) const final;

	/**
	 * Internal filename representation
	 */
	std::string filename_;
};


/**
 * \brief Parser for AccurateRip response as a binary stream on stdin.
 */
class ARStdinParser final : public ARStreamParser
{

public:

	/**
	 * \brief Default constructor.
	 */
	ARStdinParser();


private:

	uint32_t do_parse() final;

	void on_catched_exception(std::istream &istream,
			const std::exception &e) const final;
};

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

