#ifndef __LIBARCS_PARSE_HPP__
#define __LIBARCS_PARSE_HPP__


/**
 * \file parse.hpp AccurateRip response parser and syntactic entities
 *
 * ARResponse represents the entire response data from AccurateRip. It is
 * composed of a sequence of <tt>ARBlock</tt>s of which each is composed of a
 * header containing the ARId and a sequence of <tt>ARTriplet</tt>s, one
 * for each track.
 *
 * An ARBlock represents a single checksum profile for a compact disc while
 * an ARTriplet is the information on a single track.
 *
 * An ARStreamParser can be registered two handlers, a ContentHandler
 * and an ErrorHandler both of which have default implementations as there
 * are DefaultContentHandler and DefaultErrorHandler.
 *
 * A StreamReadException is thrown by an ARStreamParser when the input
 * byte stream ends prematurely or is otherwise corrupted. It contains exact
 * information about the stream-relative as well as the block-relative error
 * position.
 *
 * There are two standard implementations of ARStreamParser provided,
 * ARFileParser and ARStdinParser. ARFileParser parses files
 * (surprise) that can be obtained by just saving the payload of the AccurateRip
 * response to a file. The ARStdinParser is needed for tools that are
 * expected to parse piped input.
 */


#include <cstdint>
#include <istream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>


namespace arcs
{

/// \defgroup parse AccurateRip Response Parser
/// @{

inline namespace v_1_0_0
{


// Forward declaration for class ARId, used in the Interface
class ARId;

// Forward declaration for the private implementation
class ARTripletImpl;


/**
 * \brief A triplet of values from a block in a response of AccurateRip.
 *
 * Syntactically, an ARTriplet is an element of some ARBlock.
 *
 * Semantically, an ARTriplet carries information about a single track in
 * an AccurateRip data block describing the entire disc. An ARTriplet
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
 * An ARTriplet is a POD. It exposes getters but no actual behaviour. It
 * carries no information whether its ARCS are v1 or v2. It contains only parsed
 * data and no metadata.
 */
class ARTriplet final
{

public:

	/**
	 * Constructor.
	 *
	 * \param[in] arcs          The ARCS value of this triplet
	 * \param[in] confidence    The confidence value of this triplet
	 * \param[in] frame450_arcs The ARCS for frame 450 of this triplet
	 */
	ARTriplet(const uint32_t arcs, const uint32_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * Constructor.
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
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARTriplet(const ARTriplet &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	ARTriplet(ARTriplet &&rhs) noexcept;

	/**
	 * Default destructor.
	 */
	~ARTriplet() noexcept;

	/**
	 * The track ARCS in this triplet.
	 *
	 * The ARCS may be v1 as well as v2.
	 *
	 * \return ARCS value in this triplet
	 */
	uint32_t arcs() const;

	/**
	 * The confidence value in this triplet.
	 *
	 * \return Confidence in this triplet
	 */
	uint32_t confidence() const;

	/**
	 * The ARCS of frame 450 of the particular track in this triplet.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	uint32_t frame450_arcs() const;

	/**
	 * The track ARCS in this triplet.
	 *
	 * The ARCS may be v1 as well as v2.
	 *
	 * \return ARCS value in this triplet
	 */
	virtual bool arcs_valid() const;

	/**
	 * The confidence value in this triplet.
	 *
	 * \return Confidence in this triplet
	 */
	virtual bool confidence_valid() const;

	/**
	 * The ARCS of frame 450 of the particular track in this triplet.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	virtual bool frame450_arcs_valid() const;

	/**
	 * Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARTriplet& operator = (const ARTriplet &rhs);

	/**
	 * Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARTriplet& operator = (ARTriplet &&rhs) noexcept;


private:

	/**
	 * Private implementation of ARTriplet
	 */
	std::unique_ptr<ARTripletImpl> impl_;
};


/**
 * \brief A block of ARCSs as it occurrs in an AccurateRip response.
 *
 * An AccurateRip response is in fact parsed as a sequence of ARBlocks.
 *
 * Syntactically each ARBlock is an ordered pair of a header and a sequence
 * of ARTriplets.
 *
 * Semantically, an ARBlock contains exactly one ARTriplet for each
 * track of the original disc. This makes the ARBlock in fact an ARCS set
 * of the tracks on the disc along with their confidences and pressing offsets
 * for a disc.
 *
 * An AccurateRip response may contain different ARBlocks for a single disc id
 * and the ARBlocks may differ in the ARCSs they contain for the tracks. The
 * header contains the AccurateRip Id of the disc.
 */
class ARBlock final
{

public: /* types */

	using iterator = std::vector<ARTriplet>::iterator;

	using const_iterator = std::vector<ARTriplet>::const_iterator;


public: /* methods */

	/**
	 * Default constructor.
	 *
	 * \param[in] id ARId of the cd this block describes
	 */
	explicit ARBlock(const ARId &id);

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARBlock(const ARBlock &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	ARBlock(ARBlock &&rhs) noexcept;

	/**
	 * Default destructor.
	 */
	~ARBlock() noexcept;

	/**
	 * Returns the AccurateRip Id (i.e. the header) of the block.
	 *
	 * \return ARId of this block
	 */
	const ARId& id() const;

	/**
	 * Append a triplet to this block.
	 *
	 * \param[in] triplet Append a new triplet to this block
	 */
	void append(const ARTriplet &triplet);

	/**
	 * Returns the size of this ARBlock, i.e. the number of triplets it
	 * contains.
	 *
	 * \return The number of triplets in this block.
	 */
	uint32_t size() const;

	/**
	 * Return iterator pointing to first block
	 *
	 * \return iterator pointing to first block
	 */
	iterator begin();

	/**
	 * Return iterator pointing behind last block
	 *
	 * \return iterator pointing behind last block
	 */
	iterator end();

	/**
	 * Return const_iterator pointing to first block
	 *
	 * \return const_iterator pointing to first block
	 */
	const_iterator begin() const;

	/**
	 * Return const_iterator pointing behind last block
	 *
	 * \return const_iterator pointing behind last block
	 */
	const_iterator end() const;

	/**
	 * Return const_iterator pointing to first block
	 *
	 * \return const_iterator pointing to first block
	 */
	const_iterator cbegin() const;

	/**
	 * Return const_iterator pointing behind last block
	 *
	 * \return const_iterator pointing behind last block
	 */
	const_iterator cend() const;

	// TODO rbegin,rend + crbegin,crend

	/**
	 * Return the ARTriplet with the specified index
	 *
	 * \return ARTriplet at index
	 */
	const ARTriplet& operator [](const int index) const;

	/**
	 * Return the ARTriplet with the specified index
	 *
	 * \return ARTriplet at index
	 */
	ARTriplet& operator [](const int index);

	/**
	 * Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARBlock& operator = (const ARBlock &rhs);

	/**
	 * Move assignment operator.
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
	 * Private implementation of ARBlock
	 */
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief A response from AccurateRip.
 */
class ARResponse final
{

public: /* types */

	using iterator = std::vector<ARBlock>::iterator;

	using const_iterator = std::vector<ARBlock>::const_iterator;


public: /* methods */

	/**
	 * Default constructor
	 */
	ARResponse();

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARResponse(const ARResponse &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	ARResponse(ARResponse &&rhs) noexcept;

	/**
	 * Default destructor.
	 */
	~ARResponse() noexcept;

	/**
	 * Appends an ARBlock to the response
	 *
	 * \param[in] block The ARBlock to append
	 */
	void append(const ARBlock &block);

	/**
	 * Return block with 0-based index \c i
	 *
	 * \param[in] i Index of the ARBlock to get
	 *
	 * \return Block \c i .
	 */
	const ARBlock& block(const int i) const;

	/**
	 * Returns the number of blocks in this response
	 *
	 * \return Number of blocks in this response
	 */
	std::size_t size() const;

	/**
	 * Number of tracks per block.
	 *
	 * \return Number of tracks per block
	 */
	int tracks_per_block() const;

	/**
	 * Return iterator pointing to first block
	 *
	 * \return iterator pointing to first block
	 */
	iterator begin();

	/**
	 * Return iterator pointing behind last block
	 *
	 * \return iterator pointing behind last block
	 */
	iterator end();

	/**
	 * Return const_iterator pointing to first block
	 *
	 * \return const_iterator pointing to first block
	 */
	const_iterator begin() const;

	/**
	 * Return const_iterator pointing behind last block
	 *
	 * \return const_iterator pointing behind last block
	 */
	const_iterator end() const;

	/**
	 * Return const_iterator pointing to first block
	 *
	 * \return const_iterator pointing to first block
	 */
	const_iterator cbegin() const;

	/**
	 * Return const_iterator pointing behind last block
	 *
	 * \return const_iterator pointing behind last block
	 */
	const_iterator cend() const;

	// TODO rbegin,rend + crbegin,crend

	/**
	 * Return the ARBlock with the specified index
	 *
	 * \param[in] index The index of the ARBlock to get
	 *
	 * \return ARBlock at index
	 */
	const ARBlock& operator [](const int index) const;

	/**
	 * Return the ARBlock with the specified index
	 *
	 * \param[in] index The index of the ARBlock to get
	 *
	 * \return ARBlock at index
	 */
	ARBlock& operator [](const int index);

	/**
	 * Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARResponse& operator = (const ARResponse &rhs);

	/**
	 * Move assignment operator.
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
	 * Private implementation of ARResponse
	 */
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Interface for ARParser content handlers.
 *
 * Add a content handler to an ARStreamParser to actually add behaviour for
 * parsing an ARResponse.
 *
 * \see DefaultContentHandler
 */
class ContentHandler
{

public:

	/**
	 * Virtual default destructor.
	 */
	virtual ~ContentHandler() noexcept = default;

	/**
	 * React on the start of the input.
	 */
	virtual void start_input()
	= 0;

	/**
	 * React on the start of an ARBlock.
	 */
	virtual void start_block()
	= 0;

	/**
	 * React on a id.
	 *
	 * Each ARBlock starts with the id of the disc it describes, so this
	 * method is always called directly after <tt>start_block()</tt>.
	 *
	 * \param[in] track_count Track count in this id
	 * \param[in] id1         Disc id 1 in this id
	 * \param[in] id2         Disc id 2 in this id
	 * \param[in] cddb_id     CDDB id in this id
	 */
	virtual void id(const uint8_t track_count,
			const uint32_t id1,
			const uint32_t id2,
			const uint32_t cddb_id)
	= 0;

	/**
	 * React on an ARTriplet.
	 *
	 * \param[in] arcs          ARCS in this triplet
	 * \param[in] confidence    Confidence in this triplet
	 * \param[in] frame450_arcs ARCS of frame 450 in this triplet
	 */
	virtual void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs)
	= 0;

	/**
	 * React on an ARTriplet.
	 *
	 * \param[in] arcs                The ARCS value of this triplet
	 * \param[in] confidence          The confidence value of this triplet
	 * \param[in] frame450_arcs       The ARCS for frame 450 of this triplet
	 * \param[in] arcs_valid          Validity of ARCS of this triplet
	 * \param[in] confidence_valid    Validity of confidence of this triplet
	 * \param[in] frame450_arcs_valid Validity of frame 450 ARCS of this triplet
	 */
	virtual void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs,
			const bool arcs_valid,
			const bool confidence_valid,
			const bool frame450_arcs_valid)
	= 0;

	/**
	 * React on the end of an ARBlock.
	 */
	virtual void end_block()
	= 0;

	/**
	 * React on the end of the input.
	 */
	virtual void end_input()
	= 0;

	/**
	 * Clone this object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return Deep copy of this object
	 */
	virtual std::unique_ptr<ContentHandler> clone() const
	= 0;
};


/**
 * \brief Constructs an ARResponse instance from the parsed content.
 *
 * \see ContentHandler
 */
class DefaultContentHandler final : public ContentHandler
{

public:

	/**
	 * Default constructor
	 */
	DefaultContentHandler();

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	DefaultContentHandler(const DefaultContentHandler &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	DefaultContentHandler(DefaultContentHandler &&rhs) noexcept;

	/**
	 * Default destructor
	 */
	~DefaultContentHandler() noexcept override;

	void start_input() override;

	void start_block() override;

	void id(const uint8_t track_count,
			const uint32_t id1,
			const uint32_t id2,
			const uint32_t cddb_id) override;

	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs) override;

	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs,
			const bool arcs_valid,
			const bool confidence_valid,
			const bool frame450_arcs_valid) override;

	void end_block() override;

	void end_input() override;

	std::unique_ptr<ContentHandler> clone() const override;

	/**
	 * Returns the parse result
	 *
	 * \return The parse result
	 */
	ARResponse result() const;

	/**
	 * Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	DefaultContentHandler& operator = (const DefaultContentHandler &rhs);

	/**
	 * Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	DefaultContentHandler& operator = (DefaultContentHandler &&rhs) noexcept;


private:

	// Forward declaration for private implementation.
	class Impl;

	/**
	 * Internal implementation instance
	 */
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Interface for ARParser error handlers.
 *
 * Defines the handler methods to react on parse errors of an
 * ARStreamParser.
 *
 * \see DefaultErrorHandler
 */
class ErrorHandler
{

public:

	/**
	 * Virtual default destructor.
	 */
	virtual ~ErrorHandler() noexcept = default;

	/**
	 * Error notification
	 *
	 * \param[in] byte_pos       Last 1-based byte pos read before error
	 * \param[in] block          1-based block number
	 * \param[in] block_byte_pos Last 1-based block byte pos read before error
	 */
	virtual void error(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos)
	= 0;

	/**
	 * Clone this object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return Deep copy of this object
	 */
	virtual std::unique_ptr<ErrorHandler> clone() const
	= 0;
};


/**
 * \brief Error handler that just logs the error message to stdout.
 *
 * \see ErrorHandler
 */
class DefaultErrorHandler final : public ErrorHandler
{

public:

	void error(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos) override;

	std::unique_ptr<ErrorHandler> clone() const override;
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
 * \brief Abstract base class for parsing an AccurateRip response body.
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
 * Concrete subclasses are responsible for providing a <tt>parse(...)</tt>
 * method with the appropriate parameters. Hook method
 * <tt>on_catched_exception</tt> is called before the actual exception is
 * rethrown, so the actual stream can be closed or other cleanup can be
 * performed.
 *
 * \see ARFileParser
 * \see ARStdinParser
 */
class ARStreamParser
{

public:

	/**
	 * Default constructor.
	 */
	ARStreamParser();

	/**
	 * Virtual default destructor.
	 */
	virtual ~ARStreamParser() noexcept;

	/**
	 * Set the content handler for this parser instance.
	 *
	 * \param[in] handler The content handler for this instance
	 */
	void set_content_handler(std::unique_ptr<ContentHandler> handler);

	/**
	 * Return the content handler of this parser instance.
	 *
	 * \return Content handler of this instance
	 */
	const ContentHandler& content_handler() const;

	/**
	 * Set the error handler for this parser instance.
	 *
	 * \param[in] handler The error handler for this instance
	 */
	void set_error_handler(std::unique_ptr<ErrorHandler> handler);

	/**
	 * Return the error handler of this parser instance.
	 *
	 * \return Error handler of this instance
	 */
	const ErrorHandler& error_handler() const;

	/**
	 * Parses the configured input stream.
	 *
	 * \return Number of bytes parsed from configured input stream
	 */
	virtual uint32_t parse()
	= 0;


protected:

	/**
	 * Parses the byte stream of an AccurateRip response.
	 *
	 * The stream is required to be appropriately opened. It is further
	 * required that failbit and badbit are set and exceptions are activated.
	 *
	 * \param[in] in_stream The stream to be parsed
	 *
	 * \return Number of bytes parsed from \c in_stream
	 */
	uint32_t do_parse(std::istream &in_stream);


private:

	/**
	 * This method is called in the catch clause of a StreamReadException
	 * before the exception is rethrown.
	 *
	 * It can be used to close the stream in case this is required or to
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
 *
 * \see ARStreamParser
 */
class ARFileParser final : public ARStreamParser
{

public:

	/**
	 * Default constructor
	 */
	ARFileParser();

	/**
	 * Constructor for specific file.
	 *
	 * \param[in] filename Name of the file to parse
	 */
	explicit ARFileParser(const std::string &filename);

	/**
	 * Set the file to be parsed
	 *
	 * \param[in] filename Name of the file to parse
	 */
	void set_file(const std::string &filename);

	/**
	 * Name of the file to parse
	 *
	 * \return Name of the file that is parsed when <tt>parse()</tt> is called.
	 */
	std::string file() const;

	/**
	 * Parses a dBAR-\*.bin file received from AccurateRip.
	 *
	 * \param[in] filename The file to parse
	 *
	 * \return Number of bytes parsed from \c in_stream
	 */
	uint32_t parse(const std::string &filename);


	uint32_t parse() final;


private:

	void on_catched_exception(std::istream &istream,
			const std::exception &e) const final;

	/**
	 * Internal filename representation
	 */
	std::string filename_;

	// TODO Make this class a pimpl
};


/**
 * \brief Parser for AccurateRip response as a binary stream on stdin.
 *
 * \see ARStreamParser
 */
class ARStdinParser final : public ARStreamParser
{

public:

	/**
	 * Default constructor
	 */
	ARStdinParser();

	/**
	 * Parses the byte stream of an AccurateRip response from stdin.
	 *
	 * \return Number of bytes parsed from stdin
	 */
	uint32_t parse() final;


private:

	void on_catched_exception(std::istream &istream,
			const std::exception &e) const final;
};


} // namespace v_1_0_0

/// @}

} // namespace arcs

#endif

