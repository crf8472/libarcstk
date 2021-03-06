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

#include <cstdint>           // for uint32_t, uint8_t
#include <initializer_list>  // for initializer_list
#include <istream>           // for istream, size_t
#include <memory>            // for unique_ptr
#include <stdexcept>         // for runtime_error
#include <string>            // for string

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"     // for Checksum
#endif
#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{


// Forward declaration to avoid including other headers
class ARId;

// Forward declaration for the base class for a private implementation
class ARTripletImpl; // IWYU pragma keep


/**
 * \defgroup parse AccurateRip Response Parser
 *
 * \brief Parse an AccurateRip HTTP-response to an
 * \link ARResponse object representation\endlink
 *
 * \details
 *
 * An ARStreamParser push-parses the binary content of a HTTP-response from
 * AccurateRip and emits a series of events. There is a minimal default
 * implementations of ARStreamParser provided, ARParser, that accepts an opened
 * stream throwing exceptions on at least failbit and badbit.
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
 * ARId followed by a sequence of \link ARTriplet ARTriplets\endlink, one for
 * each track.
 *
 * While an ARBlock represents thus a single checksum profile for a compact disc
 * pressing an ARTriplet represents the AccurateRip information for a single
 * track in this pressing. Each ARTriplet contains an ARCSv1 or ARCSv2 checksum
 * value, a confidence value and a check value to identify the pressing.
 *
 * \note
 * There is no way to inform the client whether the actual ARCS in an ARTriplet
 * is an ARCSv1 or an ARCSv2. The AccurateRip response does not distinguish
 * blocks of ARCSv1 from blocks of ARCSv2 and provides no information about the
 * concrete checksum algorithm. A block of ARCSv1 is considered just an
 * information about another pressing of an album.
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
 * An ARTriplet carries no information whether its ARCS are ARCSv1 or ARCSv2. It
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
	 * All validity flags are set to \c FALSE.
	 */
	ARTriplet();

	/**
	 * \brief Constructor that sets all validity flags to \c TRUE.
	 *
	 * All validity flags are set to \c TRUE.
	 *
	 * \param[in] arcs          The ARCS value of this triplet
	 * \param[in] confidence    The confidence value of this triplet
	 * \param[in] frame450_arcs The ARCS for frame 450 of this triplet
	 */
	ARTriplet(const Checksum arcs, const uint32_t confidence,
			const Checksum frame450_arcs);

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
	ARTriplet(const Checksum arcs, const uint32_t confidence,
			const Checksum frame450_arcs, const bool arcs_valid,
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
	Checksum arcs() const noexcept;

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
	Checksum frame450_arcs() const noexcept;

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
	 * \brief Indicates whether parsed content is present.
	 *
	 * \return \c TRUE iff the ARTriplet holds not parsed content, otherwise
	 *         \c FALSE
	 */
	bool empty() const noexcept;


	ARTriplet& operator = (const ARTriplet &rhs);

	ARTriplet& operator = (ARTriplet &&rhs) noexcept;

	friend bool operator == (const ARTriplet &lhs, const ARTriplet &rhs)
		noexcept;

	friend void swap(ARTriplet &lhs, ARTriplet &rhs) noexcept;


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
 * Syntactically each ARBlock is an ordered pair of a header and a subsequent
 * sequence of \link ARTriplet ARTriplets \endlink.
 *
 * Semantically, an ARBlock contains exactly one ARTriplet for each
 * track of the original disc. This makes the ARBlock in fact an ARCS set
 * of the tracks on the disc along with their confidences and pressing offsets
 * for a disc. The header contains the AccurateRip Id of the disc.
 *
 * An AccurateRip response may contain different \link ARBlock ARBlocks \endlink
 * for a single disc id and the \link ARBlock ARBlocks \endlink may differ in
 * the ARCSs they contain for the tracks.
 */
class ARBlock final : public Comparable<ARBlock>
{
public: /* member types */

	/**
	 * \brief An iterator over the \link ARTriplet ARTriplets \endlink of an
	 * ARBlock.
	 */
	using iterator = ARTriplet*;

	/**
	 * \brief A const-iterator over the \link ARTriplet ARTriplets \endlink of
	 * an ARBlock.
	 */
	using const_iterator = const ARTriplet*;

	/**
	 * \brief Size type for ARBlock.
	 */
	using size_type = std::size_t;


public: /* member functions */

	/**
	 * \brief Constructor.
	 *
	 * \param[in] id \link ARId AccurateRip id\endlink of the CD this ARBlock
	 * describes
	 */
	explicit ARBlock(const ARId &id);

	/**
	 * \brief Constructor
	 *
	 * This constructor is intended for testing purposes only.
	 *
	 * \param[in] id       Id of this block
	 * \param[in] triplets Sequence of triplets
	 */
	ARBlock(ARId &&id, std::initializer_list<ARTriplet> triplets);

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
	 * \brief Total Number of \link ARTriplet ARTriplets\endlink in this
	 * ARBlock.
	 *
	 * \return The total number of \link ARTriplet ARTriplets\endlink in this
	 * block.
	 */
	size_type size() const noexcept;

	/**
	 * \copydoc cbegin()
	 */
	const_iterator begin() const;

	/**
	 * \copydoc cend()
	 */
	const_iterator end() const;

	/**
	 * \brief Obtain a const_iterator pointing to first ARTriplet.
	 *
	 * \return const_iterator pointing to first ARTriplet
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Obtain a const_iterator pointing behind last ARTriplet.
	 *
	 * \return const_iterator pointing behind last ARTriplet
	 */
	const_iterator cend() const;

	/**
	 * \brief The ARTriplet with the specified 0-based \c index.
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
	 * \return ARTriplet at index \c index.
	 *
	 * \throws std::out_of_range Iff \c index >= ARBlock::size().
	 */
	const ARTriplet& at(const size_type index) const;

	/**
	 * \copybrief at()
	 *
	 * No bounds checking is performed. For index based access with bounds
	 * checking, see
	 * \link ARBlock::at(const size_type index) const at()\endlink.
	 *
	 * \param[in] index The 0-based index of the ARTriplet to return
	 *
	 * \return ARTriplet at the specified index
	 */
	const ARTriplet& operator [](const size_type index) const;


	ARBlock& operator = (const ARBlock &rhs);

	ARBlock& operator = (ARBlock &&rhs) noexcept;

	friend bool operator == (const ARBlock &lhs, const ARBlock &rhs) noexcept;

	friend void swap(const ARBlock &lhs, const ARBlock &rhs);

	// Forward declaration for the opaque implementation
	class Impl;

	/**
	 * \brief Constructor for predefined implementation
	 *
	 * \param[in] impl Implementation of this block
	 */
	ARBlock(std::unique_ptr<Impl> impl) noexcept;


private:

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
 * Syntactically, an ARResponse is essentially an iterable sequence of
 * \link ARBlock ARBlocks\endlink.
 *
 * Semantically, an ARResponse represents the collected checksums AccurateRip
 * has obtained for albums with the specified ARId.
 *
 * Although an ARResponse represents content that is usually not created by the
 * client, the client may nonetheless create an ARResponse on its own or modify
 * an existing instance. This allows for easy testing ARResponse objects.
 */
class ARResponse final : public Comparable<ARResponse>
{
public: /* member types */

	/**
	 * \brief An iterator over the \link ARBlock ARBlocks \endlink of an
	 * ARResponse.
	 */
	using iterator = ARBlock*;

	/**
	 * \brief A const_iterator over the \link ARBlock ARBlocks \endlink of an
	 * ARResponse.
	 */
	using const_iterator = const ARBlock*;

	/**
	 * \brief Size type of ARResponse.
	 */
	using size_type = std::size_t;


public: /* member functions */

	/**
	 * \brief Constructor.
	 */
	ARResponse();

	/**
	 * \brief Constructor
	 *
	 * This constructor is intended for testing purposes only.
	 *
	 * \param[in] blocks Sequence of blocks
	 */
	ARResponse(std::initializer_list<ARBlock> blocks);

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
	 * \brief Destructor.
	 */
	~ARResponse() noexcept;

	/**
	 * \brief Number of tracks per ARBlock.
	 *
	 * \return Number of tracks per \link ARBlock ARBlocks\endlink
	 */
	int tracks_per_block() const noexcept;

	/**
	 * \brief Number of \link ARBlock ARBlocks\endlink in this
	 * instance.
	 *
	 * \return Number of \link ARBlock ARBlocks\endlink in this response
	 */
	size_type size() const noexcept;

	/**
	 * \brief Obtain a const_iterator pointing to the first ARBlock.
	 *
	 * \return const_iterator pointing to the first ARBlock
	 */
	const_iterator begin() const;

	/**
	 * \brief Obtain a const_iterator pointing behind the last ARBlock.
	 *
	 * \return const_iterator pointing behind the last ARBlock
	 */
	const_iterator end() const;

	/**
	 * \brief Obtain a const_iterator pointing to the first ARBlock.
	 *
	 * \return const_iterator pointing to the first ARBlock
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Obtain a const_iterator pointing behind the last ARBlock.
	 *
	 * \return const_iterator pointing behind the last ARBlock
	 */
	const_iterator cend() const;

	/**
	 * \brief The ARBlock with the specified 0-based \c index.
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
	 * \return ARBlock at index \c index.
	 *
	 * \throws std::out_of_range Iff \c index > ARResponse::size() - 1.
	 */
	const ARBlock& at(const size_type index) const;

	/**
	 * \copybrief at()
	 *
	 * \details
	 * No bounds checking is performed. For index based access with bounds
	 * checking, see
	 * \link ARResponse::at(const size_type index) const at()\endlink.
	 *
	 * \param[in] index The index of the ARBlock to get
	 *
	 * \return ARBlock at index
	 */
	const ARBlock& operator [](const size_type index) const;


	ARResponse& operator = (const ARResponse &rhs);

	ARResponse& operator = (ARResponse &&rhs) noexcept;

	friend bool operator == (const ARResponse &lhs, const ARResponse &rhs)
		noexcept;

	friend void swap(const ARResponse &lhs, const ARResponse &rhs);

	// Forward declaration for opaque implementation
	class Impl;

	/**
	 * \internal
	 *
	 * \brief Set the underlying implementation object.
	 *
	 * \param[in] impl Implementation of this ARResponse
	 */
	void reimplement(std::unique_ptr<Impl> impl) noexcept;


private:

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
 * A ContentHandler reacts to the following events:
 * <ul>
 *    <li>Start of input (once on beginning the parsing process)</li>
 *    <li>Start of a block (once on every block)</li>
 *    <li>ARId, i.e. the header of a block (once on every block)</li>
 *    <li>A triplet (once or more times on every block)</li>
 *    <li>End of a block (once on every block)</li>
 *    <li>End of input (once on the end of the parsing process)</li>
 * </ul>
 *
 * Event start_input() occurrs before the first actual bytes are parsed. Event
 * end_input() occurrs after the last actual bytes are parsed.
 *
 * The ARId is not represented as such, but as a sequence of the four numeric
 * constants that are required to compute the actual ARId.
 *
 * Each triplet comes with an ARCS for the entire track, a confidence value and
 * an ARCS for frame 450 of the track.
 *
 * If a parsing error occurrs, the ContentHandler will receive additional
 * validity flags to indicate which members of the last triplet were parsed
 * successfully and which were not.
 *
 * \see DefaultContentHandler
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
	 *
	 * \todo
	 * Since \c track_count is stored in a single byte, this parameter should be
	 * typed \c byte once migrating to C++17.
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
	 *
	 * \todo
	 * Since \c confidence is stored in a single byte, this parameter should be
	 * typed \c byte once migrating to C++17.
	 */
	void triplet(const Checksum arcs,
			const uint8_t confidence,
			const Checksum frame450_arcs);

	/**
	 * \brief React on an ARTriplet.
	 *
	 * If a parse error occurred on parsing the triplet, validity flags for each
	 * part of the triplet are passed. A validity flag that is \c TRUE indicates
	 * that the respective part of the triplet was parsed correctly. A validity
	 * value of \c FALSE reports that the parsed value is not reliable.
	 *
	 * \param[in] arcs                The ARCS value of this triplet
	 * \param[in] confidence          The confidence value of this triplet
	 * \param[in] frame450_arcs       The ARCS for frame 450 of this triplet
	 * \param[in] arcs_valid          Validity of ARCS of this triplet
	 * \param[in] confidence_valid    Validity of confidence of this triplet
	 * \param[in] frame450_arcs_valid Validity of frame 450 ARCS of this triplet
	 *
	 * \todo
	 * Since \c confidence is stored in a single byte, this parameter should be
	 * typed \c byte once migrating to C++17.
	 */
	void triplet(const Checksum arcs,
			const uint8_t confidence,
			const Checksum frame450_arcs,
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
	 * \brief Implements id()
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
	 * \brief Implements
	 * \link triplet(const Checksum,const uint8_t,const Checksum)
	 * triplet()\endlink.
	 *
	 * \param[in] arcs          ARCS in this triplet
	 * \param[in] confidence    Confidence in this triplet
	 * \param[in] frame450_arcs ARCS of frame 450 in this triplet
	 */
	virtual void do_triplet(const Checksum arcs,
			const uint8_t confidence,
			const Checksum frame450_arcs)
	= 0;

	/**
	 * \brief Implements \link triplet(const Checksum, const uint8_t, const Checksum , const bool, const bool, const bool)
	 * triplet() \endlink
	 *
	 * \param[in] arcs                The ARCS value of this triplet
	 * \param[in] confidence          The confidence value of this triplet
	 * \param[in] frame450_arcs       The ARCS for frame 450 of this triplet
	 * \param[in] arcs_valid          Validity of ARCS of this triplet
	 * \param[in] confidence_valid    Validity of confidence of this triplet
	 * \param[in] frame450_arcs_valid Validity of frame 450 ARCS of this triplet
	 */
	virtual void do_triplet(const Checksum arcs,
			const uint8_t confidence,
			const Checksum frame450_arcs,
			const bool arcs_valid,
			const bool confidence_valid,
			const bool frame450_arcs_valid)
	= 0;

	/**
	 * \brief Implements end_block()
	 */
	virtual void do_end_block()
	= 0;

	/**
	 * \brief Implements end_input()
	 */
	virtual void do_end_input()
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
 * DefaultContentHandler is movable but not copyable.
 *
 * \attention
 * The client is responsible for the lifetime management: the ARResponse
 * instance must exist at least until
 * \link ARStreamParser::parse() parse() \endlink is finished.
 */
class DefaultContentHandler final : public ContentHandler
{
public:

	/**
	 * \brief Constructor.
	 */
	DefaultContentHandler();

	DefaultContentHandler(const DefaultContentHandler &rhs) = delete;

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
	 * \brief Set the object to be populated with the parsed content.
	 *
	 * \param[in] object Object to populate with parsed content.
	 */
	void set_object(ARResponse &object);

	/**
	 * \brief Return the object to be populated with the parsed content.
	 *
	 * \return Object to be populated with parsed content.
	 */
	const ARResponse& object() const;


	DefaultContentHandler& operator = (const DefaultContentHandler &rhs)
	= delete;

	DefaultContentHandler& operator = (DefaultContentHandler &&rhs) noexcept;


private:

	void do_start_input() final;

	void do_start_block() final;

	void do_id(const uint8_t track_count,
			const uint32_t id1,
			const uint32_t id2,
			const uint32_t cddb_id) final;

	void do_triplet(const Checksum arcs,
			const uint8_t confidence,
			const Checksum frame450_arcs) final;

	void do_triplet(const Checksum arcs,
			const uint8_t confidence,
			const Checksum frame450_arcs,
			const bool arcs_valid,
			const bool confidence_valid,
			const bool frame450_arcs_valid) final;

	void do_end_block() final;

	void do_end_input() final;

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
 *
 * \see DefaultErrorHandler
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
};


/**
 * \brief Logs every error and throws StreamReadException afterwards.
 *
 * \details
 *
 * DefaultErrorHandler is movable but not copyable.
 */
class DefaultErrorHandler final : public ErrorHandler
{
public:

	/**
	 * \brief Default constructor.
	 */
	DefaultErrorHandler();

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	DefaultErrorHandler(DefaultErrorHandler &&rhs) noexcept;


	DefaultErrorHandler& operator = (DefaultErrorHandler &&rhs) noexcept;

	// non-copyable class
	DefaultErrorHandler(const DefaultErrorHandler &rhs) = delete;
	DefaultErrorHandler& operator = (const DefaultErrorHandler &rhs) = delete;


private:

	void do_error(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos) final;
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
 * ARStreamParser provides the capability for parsing a std::istream as an
 * AccurateRip response.
 *
 * Protected service function parse_stream() is provided as a building block
 * for subclasses to implement the parsing. 
 *
 * Concrete subclasses are responsible for implementing functions do_parse()
 * and on_catched_exception().
 *
 * Method do_parse() may call parse_stream() to perform the actual parsing and
 * do some pre- or postprocessing.
 *
 * Hook method on_catched_exception() is called after an exception was catched
 * and before the respective exception is rethrown, so the subclass has the
 * chance to perform some cleanup as closing the stream if this required.
 *
 * The stream must have been opened before passing it to parse_stream().
 *
 * ARStreamParser requires furthermore that the stream is configured to throw
 * exceptions for \c failbit and \c badbit incidents, thus before the input
 * istream is passed to parse_stream(), do something like:
 *
 * \code{.cpp}
 * my_istream.exceptions(std::istream::failbit | std::istream::badbit);
 * \endcode
 *
 * Leaving out the exception settings means the ARStreamParser instance will not
 * recognize parse errors. It will probably not react appropriate and validity
 * flags will not be reliable.
 *
 * A ContentHandler is required to actually process the parsed content.
 * DefaultContentHandler will just compose an ARResponse object from the parsed
 * input.
 *
 * An ErrorHandler can optionally be set to react on parse errors. If no
 * ErrorHandler is set, a StreamReadException or std::runtime_error is thrown
 * on a parse error.
 *
 * A minimal implementation is provided by ARParser.
 */
class ARStreamParser
{
public:

	/**
	 * \brief Default constructor.
	 */
	ARStreamParser();

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	ARStreamParser(ARStreamParser &&rhs) noexcept;

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

	ARStreamParser& operator = (ARStreamParser &&rhs) noexcept;


protected:

	/**
	 * \brief Parses the (opened) byte stream as an AccurateRip response.
	 *
	 * This function can be called in \c do_parse() implementations of
	 * subclasses to perform the actual parsing process.
	 *
	 * \attention
	 * It is in the responsibility of the client that
	 * <ul>
	 *     <li>the stream is opened before passing it.</li>
	 *     <li>Exceptions must be activated on failbit as well as badbit.</li>
	 * </ul>
	 *
	 * \param[in] in_stream The stream to be parsed
	 *
	 * \return Number of bytes parsed from \c in_stream
	 *
	 * \throw StreamReadException If any positional error occurred on parsing
	 */
	uint32_t parse_stream(std::istream &in_stream);

	/**
	 * \brief Base class swap
	 *
	 * \param[in] rhs Right hand side to swap
	 */
	void swap(ARStreamParser &rhs);


private:

	/**
	 * \brief Implements parse().
	 *
	 * \return Number of bytes parsed from configured input stream
	 */
	virtual uint32_t do_parse()
	= 0;

	/**
	 * \brief Implements swap().
	 *
	 * \param[in] rhs Right hand side to swap
	 */
	virtual void do_swap(ARStreamParser &rhs);

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
 * \brief Generic parser for std::istream instances.
 *
 * The client has complete control about opening and handling the ifstream.
 * Allows to parse stdin, files and strings.
 *
 * \see ARStreamParser
 */
class ARParser final : public ARStreamParser
{
public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] stream The std::istream to parse.
	 */
	ARParser(std::istream &stream);

	ARParser(const ARParser &parser) = delete;
	ARParser(ARParser &&parser) noexcept = delete;
	ARParser& operator = (const ARParser &parser) = delete;
	ARParser& operator = (ARParser &&parser) noexcept = delete;

private:

	uint32_t do_parse() override;

	void do_swap(ARStreamParser &rhs) override;

	void on_catched_exception(std::istream &stream,
			const std::exception &e) const override;

	/**
	 * \brief Internal reference to the passed std::istream.
	 */
	std::istream& stream_;
};

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

