#ifndef __LIBARCSTK_PARSE_HPP__
#define __LIBARCSTK_PARSE_HPP__

/** \file
 *
 * \brief Public API for \link parse AccurateRip response parsing and syntactic
 * entities\endlink.
 *
 * \details
 *
 * Includes the module for \link parse parsing AccurateRip responses\endlink.
 *
 * Parse and represent the content of an HTTP-response of AccurateRip as an
 * ARResponse object.
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

// Forward declaration for the base class for a private implementation
class ARTripletImpl;


/**
 * \defgroup parse AccurateRip Response Parser
 *
 * \brief Parse an AccurateRip HTTP-response to an object representation
 *
 * \details
 *
 * An ARStreamParser push-parses the binary content of a HTTP-response from
 * AccurateRip and emits a series of events. There are two default
 * implementations of ARStreamParser provided, ARFileParser and ARStdinParser.
 *
 * ARFileParser parses files obtained by just saving the payload of the
 * AccurateRip response to a (binary) file. ARStdinParser parses stdin for this
 * payload instead of a file. It can be used for parsing piped input.
 *
 * For handling the emitted events, an ARStreamParser can be registered two
 * types of handlers to, a ContentHandler and an ErrorHandler.
 * Both have default implementations as there are DefaultContentHandler and
 * DefaultErrorHandler. DefaultContentHandler just populates an ARResponse
 * object that represents the parsed information. DefaultErrorHandler just logs
 * the error position along with an error message and throws a
 * StreamReadException.
 *
 * A StreamReadException is also thrown by an ARStreamParser when the input byte
 * stream ends prematurely or is corrupted in a way the parser can recognize.
 * It contains exact information about the stream-relative as well as the
 * block-relative error position and a human-readable error message.
 *
 * If no exceptional situation occurrs, the DefaultContentHandler populates an
 * ARResponse with the parsed information. An ARResponse represents the entire
 * content data from an AccurateRip HTTP-response. It is composed of a sequence
 * of \link ARBlock ARBlocks \endlink of which each is composed of a leading
 * ARId followed by a sequence of ARTriplets, one for each track.
 *
 * While an ARBlock represents thus a single checksum profile for a compact disc
 * pressing an ARTriplet represents the AccurateRip information for a single
 * track in this pressing. Each ARTriplet contains an ARCSv1 or ARCSv2 checksum
 * value, a confidence value and a check value to identify the pressing.
 *
 * \note
 * There is no way to inform the client whether the ARCS is v1 or v2.
 * The AccurateRip response does not distinguish blocks of ARCSv1 from
 * blocks of ARCSv2 and provides no information about the concrete checksum
 * algorithm. A block of ARCSv1 is considered just an information about
 * another pressing of an album.
 *
 * @{
 */


/**
 * \brief A triplet of values describing a particular track in an AccurateRip
 * response.
 *
 * \details
 *
 * Syntactically, an ARTriplet is an element of some ARBlock.
 *
 * Semantically, an ARTriplet carries information about a single track in
 * an AccurateRip response, while the ARBlock describes the entire disc. A
 * triplet contains the ARCS of the track as its first element, a non-negative
 * integer that represents the confidence value of the ARCS and, as a third
 * element, the ARCS of frame 450 of the track. (The latter represents a
 * reference value for the pressing offset.)
 *
 * Which track the ARTriplet describes can be derived from its actual
 * position in the ARBlock. The first ARTriplet in the ARBlock describes track
 * 1, the second ARTriplet track 2 and so forth.
 *
 * An ARTriplet carries no information whether its ARCS are v1 or v2. It
 * contains only parsed data and metadata flags indicating whether the content
 * is valid, i.e. was correctly parsed.
 *
 * An ARTriplet represents parsed information. If some error occurrs during the
 * parsing process, the resulting ARTriplet may contain incomplete information.
 * Therefore, the ARTriplet carries a validity flag for each of its values to
 * indicate whether the corresponding value was parsed without errors.
 */
class ARTriplet final
{

public:

	/**
	 * \brief Constructor.
	 *
	 * \details
	 * All validity flags are set to \c TRUE.
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
	 * \return ARCS value in this triplet
	 */
	uint32_t arcs() const noexcept;

	/**
	 * \brief The confidence value in this triplet.
	 *
	 * \return Confidence in this triplet
	 */
	uint32_t confidence() const noexcept;

	/**
	 * \brief The ARCS of frame 450 of the particular track in this triplet.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	uint32_t frame450_arcs() const noexcept;

	/**
	 * \brief Validity flag for the track ARCS in this triplet.
	 *
	 * \return Validity flag for the ARCS value in this triplet
	 */
	bool arcs_valid() const noexcept;

	/**
	 * \brief Validity flag for the confidence value in this triplet.
	 *
	 * \return Validity flag for the confidence in this triplet
	 */
	bool confidence_valid() const noexcept;

	/**
	 * \brief Validity flag for the ARCS of frame 450 of the particular track in
	 * this triplet.
	 *
	 * \return Validity flag for the frame450 ARCS value in this triplet
	 */
	bool frame450_arcs_valid() const noexcept;

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
 * of \link ARTriplet ARTriplets \endlink.
 *
 * Semantically, an ARBlock contains exactly one ARTriplet for each
 * track of the original disc. This makes the ARBlock in fact an ARCS set
 * of the tracks on the disc along with their confidences and pressing offsets
 * for a disc. The header contains the AccurateRip Id of the disc.
 *
 * An AccurateRip response may contain different \link ARBlock ARBlocks \endlink
 * for a single disc id and the ARBlocks may differ in the ARCSs they contain
 * for the tracks.
 *
 * \todo Implementation of rbegin(), rend(), crbegin(), crend()
 */
class ARBlock final
{

public: /* member types */

	/**
	 * \brief An iterator over the \link ARTriplet ARTriplets \endlink of an
	 * ARBlock.
	 */
	using iterator = std::vector<ARTriplet>::iterator;
	// FIXME Presupposes/leaks implementation knowledge

	/**
	 * \brief A const-iterator over the \link ARTriplet ARTriplets \endlink of
	 * an ARBlock.
	 */
	using const_iterator = std::vector<ARTriplet>::const_iterator;
	// FIXME Presupposes/leaks implementation knowledge

	/**
	 * \brief Size type for ARBlock.
	 */
	using size_type = std::vector<ARTriplet>::size_type;
	// FIXME Presupposes/leaks implementation knowledge


public: /* member functions */

	/**
	 * \brief Constructor.
	 *
	 * \param[in] id \link ARId AccurateRip id\endlink of the CD this ARBlock
	 * describes
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
	 * \brief The \link ARId AccurateRip Id\endlink of this ARBlock.
	 *
	 * \return ARId of this block
	 */
	const ARId& id() const noexcept;

	/**
	 * \brief Append an ARTriplet as last element to this ARBlock.
	 *
	 * \param[in] triplet Append a new entry to this ARBlock
	 */
	void append(const ARTriplet &triplet);

	/**
	 * \brief The ARTriplet with 0-based index \c index.
	 *
	 * \details
	 *
	 * Bounds checking is performed. If \c index is illegal, an exception is
	 * thrown.
	 *
	 * \see \link ARBlock::operator [](const size_type index) const
	 * operator[]\endlink
	 *
	 * \param[in] index Index of the ARTriplet to read
	 *
	 * \throws std::out_of_range Iff \c index > ARBlock::size() - 1.
	 *
	 * \return ARTriplet at index \c index.
	 */
	const ARTriplet& triplet(const size_type index) const;

	/**
	 * \copybrief triplet(const size_type index) const
	 *
	 * \details
	 *
	 * Bounds checking is performed. If \c index is illegal, an exception is
	 * thrown.
	 *
	 * \see \link ARBlock::operator [](const size_type index)
	 * operator[]\endlink
	 *
	 * \param[in] index Index of the ARTriplet to get
	 *
	 * \throws std::out_of_range Iff \c index > ARBlock::size() - 1.
	 *
	 * \return ARTriplet at index \c index.
	 */
	ARTriplet& triplet(const size_type index);

	/**
	 * \brief Number of \link ARTriplet ARTriplets\endlink in this ARBlock.
	 *
	 * \return The number of \link ARTriplet ARTriplets\endlink in this block.
	 */
	uint32_t size() const noexcept;

	/**
	 * \brief iterator pointing to first ARTriplet.
	 *
	 * \return iterator pointing to first ARTriplet
	 */
	iterator begin();

	/**
	 * \brief iterator pointing behind last ARTriplet.
	 *
	 * \return iterator pointing behind last ARTriplet
	 */
	iterator end();

	/**
	 * \brief const_iterator pointing to first ARTriplet.
	 *
	 * \return const_iterator pointing to first ARTriplet
	 */
	const_iterator begin() const;

	/**
	 * \brief const_iterator pointing behind last ARTriplet.
	 *
	 * \return const_iterator pointing behind last ARTriplet
	 */
	const_iterator end() const;

	/**
	 * \brief const_iterator pointing to first ARTriplet.
	 *
	 * \return const_iterator pointing to first ARTriplet
	 */
	const_iterator cbegin() const;

	/**
	 * \brief const_iterator pointing behind last ARTriplet.
	 *
	 * \return const_iterator pointing behind last ARTriplet
	 */
	const_iterator cend() const;

	/**
	 * \brief The ARTriplet with the specified \c index.
	 *
	 * \see \link triplet(const size_type index) const triplet()\endlink
	 *
	 * \param[in] index The 0-based index of the ARTriplet to return
	 *
	 * \return ARTriplet at the specified index
	 */
	const ARTriplet& operator [](const size_type index) const;

	/**
	 * \brief The ARTriplet with the specified \c index.
	 *
	 * \see \link triplet(const size_type index) triplet()\endlink
	 *
	 * \param[in] index The 0-based index of the ARTriplet to return
	 *
	 * \return ARTriplet at the specified index
	 */
	ARTriplet& operator [](const size_type index);

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
 * \brief Response content from AccurateRip when responding to an request
 *
 * \details
 *
 * An ARResponse represents the HTTP-response of AccurateRip to an request for
 * some ARId.
 *
 * Technically, an ARResponse is essentially an iterable sequence of
 * \link ARBlock ARBlocks\endlink.
 *
 * Although an ARResponse represents content that is usually not created by the
 * client, the client may nonetheless create an ARResponse on its own or modify
 * an existing instance. This allows for easy testing ARResponse objects.
 *
 * \todo Implementation of rbegin(), rend(), crbegin(), crend()
 */
class ARResponse final
{

public: /* member types */

	/**
	 * \brief An iterator over the \link ARBlock ARBlocks \endlink of an
	 * ARResponse.
	 */
	using iterator = std::vector<ARBlock>::iterator;
	// FIXME Presupposes/leaks implementation knowledge

	/**
	 * \brief A const_iterator over the \link ARBlock ARBlocks \endlink of an
	 * ARResponse.
	 */
	using const_iterator = std::vector<ARBlock>::const_iterator;
	// FIXME Presupposes/leaks implementation knowledge

	/**
	 * \brief Size type of ARResponse.
	 */
	using size_type = std::vector<ARBlock>::size_type;
	// FIXME Presupposes/leaks implementation knowledge


public: /* member functions */

	/**
	 * \brief Constructor.
	 */
	ARResponse();

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARResponse(const ARResponse &rhs);

	/**
	 * \brief Default move constructor.
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
	 * \brief The ARBlock with 0-based index \c index.
	 *
	 * \details
	 *
	 * Bounds checking is performed. If \c index is illegal, an exception is
	 * thrown.
	 *
	 * \see \link ARResponse::operator [](const size_type index) const
	 * operator[]\endlink
	 *
	 * \param[in] index Index of the ARBlock to read
	 *
	 * \throws std::out_of_range Iff \c index > ARResponse::size() - 1.
	 *
	 * \return ARBlock at index \c index.
	 */
	const ARBlock& block(const size_type index) const;

	/**
	 * \copybrief block(const size_type index) const
	 *
	 * \details
	 *
	 * Bounds checking is performed. If \c index is illegal, an exception is
	 * thrown.
	 *
	 * \see \link ARResponse::operator [](const size_type index)
	 * operator[]\endlink
	 *
	 * \param[in] index Index of the ARBlock to get
	 *
	 * \throws std::out_of_range Iff \c index > ARResponse::size() - 1.
	 *
	 * \return ARBlock at index \c index.
	 */
	ARBlock& block(const size_type index);

	/**
	 * \brief Number of \link ARBlock ARBlocks\endlink in this
	 * instance.
	 *
	 * \return Number of \link ARBlock ARBlocks\endlink in this response
	 */
	size_type size() const noexcept;

	/**
	 * \brief Number of tracks per ARBlock.
	 *
	 * \return Number of tracks per \link ARBlock ARBlocks\endlink
	 */
	int tracks_per_block() const noexcept;

	/**
	 * \brief iterator pointing to the first ARBlock.
	 *
	 * \return iterator pointing to the first ARBlock
	 */
	iterator begin();

	/**
	 * \brief iterator pointing behind the last ARBlock.
	 *
	 * \return iterator pointing behind the last ARBlock
	 */
	iterator end();

	/**
	 * \brief const_iterator pointing to the first ARBlock.
	 *
	 * \return const_iterator pointing to the first ARBlock
	 */
	const_iterator begin() const;

	/**
	 * \brief const_iterator pointing behind the last ARBlock.
	 *
	 * \return const_iterator pointing behind the last ARBlock
	 */
	const_iterator end() const;

	/**
	 * \brief const_iterator pointing to the first ARBlock.
	 *
	 * \return const_iterator pointing to the first ARBlock
	 */
	const_iterator cbegin() const;

	/**
	 * \brief const_iterator pointing behind the last ARBlock.
	 *
	 * \return const_iterator pointing behind the last ARBlock
	 */
	const_iterator cend() const;

	/**
	 * \brief The ARBlock with the specified 0-based \c index.
	 *
	 * \details
	 * No bounds checking is performed. For index based access with bounds
	 * checking, see
	 * \link ARResponse::block(const size_type index) const block()\endlink.
	 *
	 * \see \link ARResponse::block(const size_type index) const block()\endlink
	 *
	 * \param[in] index The index of the ARBlock to get
	 *
	 * \return ARBlock at index
	 */
	const ARBlock& operator [](const size_type index) const;

	/**
	 * \brief Return the ARBlock with the specified 0-based \c index.
	 *
	 * \details
	 * No bounds checking is performed. For index based access with bounds
	 * checking, see
	 * \link ARResponse::block(const size_type index) block() \endlink.
	 *
	 * \see \link ARResponse::block(const size_type index) block()\endlink
	 *
	 * \param[in] index The index of the ARBlock to get
	 *
	 * \return ARBlock at index
	 */
	ARBlock& operator [](const size_type index);

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
 * \details
 *
 * Add actual behaviour to an ARStreamParser. A concrete subclass of
 * ContentHandler implements the handling of each syntactic entity the
 * ARStreamParser emits.
 *
 * Behaviour can be added at the start and end of the response content, at the
 * start and end of each block, at the id at the start of each block and on each
 * triplet.
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
	 * \brief React on an ARId.
	 *
	 * \details
	 *
	 * Each ARBlock starts with the id of the disc it describes, so this
	 * method is always called directly after start_block().
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
	 * \brief Implements start_input()
	 */
	virtual void do_start_input()
	= 0;

	/**
	 * \brief Implements start_block()
	 */
	virtual void do_start_block()
	= 0;

	/**
	 * \brief Implements \link ContentHandler::id(const uint8_t track_count, const uint32_t id1, const uint32_t id2, const uint32_t cddb_id)
	 * id()
	 * \endlink
	 *
	 * \details
	 *
	 * Each ARBlock starts with the id of the disc it describes, so this
	 * method is always called directly after start_block().
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
	 * \brief Implements \link ContentHandler::triplet(const uint32_t arcs, const uint8_t confidence, const uint32_t frame450_arcs)
	 * triplet()
	 * \endlink
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
	 * \brief Implements \link ContentHandler::triplet(const uint32_t arcs, const uint8_t confidence, const uint32_t frame450_arcs, const bool arcs_valid, const bool confidence_valid, const bool frame450_arcs_valid) triplet() \endlink
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
	 * \brief Implements \link ContentHandler::end_block() end_block() \endlink
	 */
	virtual void do_end_block()
	= 0;

	/**
	 * \brief Implements \link ContentHandler::end_input() end_input() \endlink
	 */
	virtual void do_end_input()
	= 0;

	/**
	 * \brief Implements \link ContentHandler::clone() const clone() \endlink
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
 * \brief Populates an existing ARResponse instance.
 *
 * \details
 *
 * Create an ARResponse instance and use set_object() to inform the handler
 * about that object. The handler instance will then populate this object with
 * the parsed content.
 *
 * \attention
 * The client is responsible for the lifetime management: the ARResponse object
 * must exist at least until \link ARStreamParser::parse() parse() \endlink is
 * finished.
 */
class DefaultContentHandler final : public ContentHandler
{

public:

	/**
	 * \brief Constructor.
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
	 * \brief Set the object to be constructed by the parsed content.
	 *
	 * \param[in,out] object Object to construct from parsed content.
	 */
	void set_object(ARResponse &object);

	/**
	 * \brief Get the object constructed by the parsed content.
	 *
	 * \return Object with parsed content.
	 */
	ARResponse& object();

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
 * \details
 *
 * Add actual behaviour to an ARStreamParser. A concrete subclass of
 * ErrorHandler implements the handling of parse errors the ARStreamParser
 * signals.
 *
 * \note
 * The AccurateRip reference data is binary data without any meta information
 * for consistency. Hence there is no perspective to sanitize the stream or
 * proceed the parsing process after an exception occurred. An ErrorHandler
 * is a mere interceptor for a StreamReadException before it is rethrown anyway.
 * This provides the convenience to get informative log messages by intercepting
 * the exceptions like DefaultErrorHandler does.
 */
class ErrorHandler
{

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ErrorHandler() noexcept;

	/**
	 * \brief Error notification with global and block-relative position.
	 *
	 * \attention
	 * The byte positions are all interpreted as 1-based.
	 *
	 * \param[in] byte_pos       Last 1-based global byte pos read before error
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
	 * \brief Implements \link error(const uint32_t byte_pos, const uint32_t block, const uint32_t block_byte_pos)
	 * error()
	 * \endlink
	 *
	 * \param[in] byte_pos       Last 1-based global byte pos read before error
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
 * \brief Logs every error and throws StreamReadException afterwards
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
 *
 * \attention
 * The byte positions are all interpreted as 1-based.
 */
class StreamReadException final : public std::runtime_error
{

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] byte_pos       Last 1-based global byte pos read before exception
	 * \param[in] block          1-based block number
	 * \param[in] block_byte_pos Last 1-based block byte pos read before exception
	 * \param[in] what_arg       Error message
	 */
	StreamReadException(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos, const std::string &what_arg);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] byte_pos       Last 1-based global byte pos read before exception
	 * \param[in] block          1-based block number
	 * \param[in] block_byte_pos Last 1-based block byte pos read before exception
	 * \param[in] what_arg       Error message
	 */
	StreamReadException(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos, const char *what_arg);

	/**
	 * \brief Last 1-based global byte position before the exception occurred.
	 *
	 * \return Last 1-based global byte position before the exception occurred
	 */
	uint32_t byte_position() const noexcept;

	/**
	 * \brief The 1-based block number of the block in which the exception
	 * occurred.
	 *
	 * \return The 1-based block number of the block
	 */
	uint32_t block() const noexcept;

	/**
	 * \brief Last 1-based byte position relative to the start of the current
	 * block before the exception occurred.
	 *
	 * \return Last 1-based block byte position read before the exception
	 */
	uint32_t block_byte_position() const noexcept;


private:

	/**
	 * \brief Last 1-based global byte position before the exception occurred.
	 */
	const uint32_t byte_pos_;

	/**
	 * \brief The 1-based block number of the block in which the exception
	 * occurred.
	 */
	const uint32_t block_;

	/**
	 * \brief Last 1-based block-relative byte position read before the
	 * exception.
	 */
	const uint32_t block_byte_pos_;
};


/**
 * \brief Abstract base for parsing the content of an AccurateRip HTTP-response.
 *
 * \details
 *
 * ARStreamParser parses a std::istream as an AccurateRip response.
 *
 * A ContentHandler is required to actually process the parsed content.
 *
 * An ErrorHandler can optionally be set to perform some processing of the
 * error information before a StreamReadException is thrown. If no
 * ErrorHandler is set, a StreamReadException or std::runtime_error is thrown
 * on a parse error.
 *
 * Concrete subclasses are responsible for implementing function
 * do_parse(). Protected service function parse_stream() is provided as a
 * building block for subclasses to implement the parsing. Hook method
 * on_catched_exception() is called before the exception is rethrown,
 * so the subclass has the chance to perform some cleanup as closing the
 * stream if this required.
 *
 * \todo
 * It should be possible to parse without exceptions. If intercepting is
 * preferred, the ErrorHandler should rethrow the exception instead of the
 * parser.
 *
 * \todo Specify/ensure all possible exceptions parse_stream() can throw
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
	 * \attention
	 * It is in the responsibility of the client that the stream is opened
	 * before passing. Exceptions must be activated and failbit as well as
	 * badbit must be set.
	 *
	 * \param[in] in_stream The stream to be parsed
	 *
	 * \return Number of bytes parsed from \c in_stream
	 */
	uint32_t parse_stream(std::istream &in_stream);


private:

	/**
	 * \brief Implements parse().
	 *
	 * \return Number of bytes parsed from configured input stream
	 */
	virtual uint32_t do_parse()
	= 0;

	/**
	 * \brief Hook: Called by parse_stream() on a StreamReadException before
	 * the exception is rethrown.
	 *
	 * \details
	 *
	 * This hook can be used to do cleanup before rethrowing. The implementing
	 * class can close the stream or perform other required steps.
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
 * \brief Parser for dBAR-\*.bin files.
 *
 * \details
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
	 * \link ARStreamParser::parse() parse() \endlink is called.
	 */
	std::string file() const noexcept;


private:

	uint32_t do_parse() final;

	void on_catched_exception(std::istream &istream,
			const std::exception &e) const final;

	/**
	 * \brief Internal filename representation
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

