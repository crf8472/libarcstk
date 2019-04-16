/**
 * \file parse.cpp Implementing a low-level API for AccurateRip response parsers
 */


#ifndef __LIBARCSTK_PARSE_HPP__
#include "parse.hpp"
#endif

#include <cerrno>   // for errno
#include <cstdio>   // for feof, ferror, fread, freopen, stdin
#include <cstring>  // for EOF, strerror
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif


namespace arcs
{

/// \cond IMPL_ONLY
/// \internal \defgroup parseImpl Implementation of an AccurateRip response parser
/// \ingroup parse
/// @{

inline namespace v_1_0_0
{

namespace
{


/**
 * \brief Wrap a vector in an istream.
 *
 * \see ARStdinParser
 */
template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class VectorIStream : public std::basic_streambuf<CharT, TraitsT>
{

public:

	/**
	 * Constructor
	 *
	 * \param[in] v The vector to wrap
	 */
    explicit VectorIStream(std::vector<CharT> &v)
	{
        this->setg(v.data(), v.data(), v.data() + v.size());
    }
};


/**
 * \brief Represents limited access to stdin.
 *
 * \see ARStdinParser
 */
class StdIn final
{

public:

	/**
	 * Constructor
	 *
	 * \param[in] buf_size Buffer size in bytes
	 */
	explicit StdIn(const std::size_t buf_size);

	/**
	 * Reads stdin bytes in binary mode to a vector<char>
	 *
	 * \return Bytes from stdin
	 */
	std::vector<char> bytes();

	/**
	 * Size of read buffer in bytes.
	 *
	 * \return Buffer size in bytes
	 */
	std::size_t buf_size() const;


private:

	/**
	 * Bytes per read
	 */
	const std::size_t buf_size_;
};


StdIn::StdIn(const std::size_t buf_size)
	: buf_size_(buf_size)
{
	// empty
}


std::vector<char> StdIn::bytes()
{
	// Note: all predefined iostreams (like std::cin) are _obligated_ to be
	// bound to corresponding C streams.
	// Confer: http://eel.is/c++draft/narrow.stream.objects
	// Therefore, it seems reasonable to just use freopen/fread for speed but
	// it feels a little bit odd to fallback to C-style stuff here.

	// Some systems may require to reopen stdin in binary mode. Even if this
	// maybe not required on some systems, it should be a portable solution to
	// just do it always:

#ifdef _WIN32

	// Note: freopen is portable by definition but windows may frown about the
	// nullptr for parameter 'path' as pointed out in the MSDN page on freopen:
	// https://msdn.microsoft.com/en-us/library/wk2h68td.aspx
	// We therefore use _setmode for windows. (It's '_setmode', not 'setmode'.)

	_setmode(_fileno(stdin), O_BINARY);
	// https://msdn.microsoft.com/en-us/library/tw4k6df8.aspx
#else

	std::freopen(nullptr, "rb", stdin); // ignore returned FILE ptr to stdin
#endif
	// binary mode from now on

	// Cancel on error
	if (std::ferror(stdin))
	{
		std::stringstream msg;
		msg << "While opening stdin for input: " << std::strerror(errno)
			<< " (errno " << errno << ")";

		throw std::runtime_error(msg.str());
	}

	// Commented out version with std::array (lines 149,155 and 167)

	std::vector<char> bytes; // collects the input bytes
	//std::array<char, 1024> buf; // input buffer
	auto buf = std::make_unique<char[]>(buf_size()); // input buffer
	std::size_t len; // number of bytes read from stdin

	// As long as there are any bytes, read them

	//while((len = std::fread(buf.data(), sizeof(buf[0]), buf.size(), stdin)) > 0)
	while((len = std::fread(buf.get(), sizeof(buf[0]), buf_size(), stdin)) > 0)
	{
		if (std::ferror(stdin) and not std::feof(stdin))
		{
			std::stringstream msg;
			msg << "While reading from stdin: " << std::strerror(errno)
				<< " (errno " << errno << ")";

			throw std::runtime_error(msg.str());
		}

		//bytes.insert(bytes.end(), buf.data(), buf.data() + len);
		bytes.insert(bytes.end(), buf.get(), buf.get() + len);
	}

	// Note:
	// Using std::array would entail to define buffer size at compile time.
	// Using std::unique_ptr<char[]> feels nonetheless quite unnatural.

	return bytes;
}


std::size_t StdIn::buf_size() const
{
	return buf_size_;
}

} // namespace


/**
 * \brief Implementation of an ARTriplet.
 *
 * \see ARTriplet
 */
class ARTripletImpl
{

public:

	/**
	 * Constructor.
	 *
	 * \param[in] arcs          The ARCS value of this triplet
	 * \param[in] confidence    The confidence value of this triplet
	 * \param[in] frame450_arcs The ARCS for frame 450 of this triplet
	 */
	ARTripletImpl(const uint32_t arcs, const uint32_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARTripletImpl(const ARTripletImpl &rhs) = delete;

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	ARTripletImpl(ARTripletImpl &&rhs) noexcept = delete;

	/**
	 * Virtual default destructor.
	 */
	virtual ~ARTripletImpl() noexcept = default;

	/**
	 * The track ARCS in this ARTripletImpl.
	 *
	 * The ARCS may be v1 as well as v2.
	 *
	 * \return ARCS value in this triplet
	 */
	uint32_t arcs() const;

	/**
	 * The confidence value in this ARTripletImpl.
	 *
	 * \return Confidence in this triplet
	 */
	uint32_t confidence() const;

	/**
	 * The ARCS of frame 450 of the particular track in this ARTripletImpl.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	uint32_t frame450_arcs() const;

	/**
	 * The track ARCS in this ARTripletImpl.
	 *
	 * The ARCS may be v1 as well as v2.
	 *
	 * \return ARCS value in this triplet
	 */
	virtual bool arcs_valid() const;

	/**
	 * The confidence value in this ARTripletImpl.
	 *
	 * \return Confidence in this triplet
	 */
	virtual bool confidence_valid() const;

	/**
	 * The ARCS of frame 450 of the particular track in this ARTripletImpl.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	virtual bool frame450_arcs_valid() const;

	/**
	 * Clone this instance, i.e. create a deep copy
	 *
	 * \return A clone of this instance
	 */
	virtual std::unique_ptr<ARTripletImpl> clone() const;

	/**
	 * Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARTripletImpl& operator = (const ARTripletImpl &rhs) = delete;

	/**
	 * Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARTripletImpl& operator = (ARTripletImpl &&rhs) noexcept = delete;


private:

	/**
	 * ARCS v1 or v2 of this track
	 */
	uint32_t arcs_;

	/**
	 * Confidence of arcs_
	 */
	uint32_t confidence_;

	/**
	 * ARCS of frame 450
	 */
	uint32_t frame450_arcs_;
};


ARTripletImpl::ARTripletImpl(const uint32_t arcs,
		const uint32_t confidence,
		const uint32_t frame450_arcs)
	: arcs_(arcs)
	, confidence_(confidence)
	, frame450_arcs_(frame450_arcs)
{
	// empty
}


uint32_t ARTripletImpl::arcs() const
{
	return arcs_;
}


uint32_t ARTripletImpl::confidence() const
{
	return confidence_;
}


uint32_t ARTripletImpl::frame450_arcs() const
{
	return frame450_arcs_;
}


bool ARTripletImpl::arcs_valid() const
{
	return true;
}


bool ARTripletImpl::confidence_valid() const
{
	return true;
}


bool ARTripletImpl::frame450_arcs_valid() const
{
	return true;
}


std::unique_ptr<ARTripletImpl> ARTripletImpl::clone() const
{
	return std::make_unique<ARTripletImpl>(this->arcs_, this->confidence_,
			this->frame450_arcs_);
}


/**
 * \brief Implementation of an incompletely parsed ARTriplet.
 *
 * It carries information about the validity of its parts.
 *
 * \see ARTripletImpl
 * \see ARTriplet
 */
class ARIncompleteTripletImpl : public ARTripletImpl
{

public:

	/**
	 * Constructor for incomplete ARTriplets.
	 *
	 * Each value can be flagged as invalid by passing FALSE.
	 *
	 * \param[in] arcs                The ARCS value of this triplet
	 * \param[in] confidence          The confidence value of this triplet
	 * \param[in] frame450_arcs       The ARCS for frame 450 of this triplet
	 * \param[in] arcs_valid          Validity of ARCS of this triplet
	 * \param[in] confidence_valid    Validity of confidence of this triplet
	 * \param[in] frame450_arcs_valid Validity of frame 450 ARCS of this triplet
	 */
	ARIncompleteTripletImpl(const uint32_t arcs, const uint32_t confidence,
			const uint32_t frame450_arcs, const bool arcs_valid,
			const bool confidence_valid, const bool frame450_arcs_valid);

	bool arcs_valid() const override;

	bool confidence_valid() const override;

	bool frame450_arcs_valid() const override;

	std::unique_ptr<ARTripletImpl> clone() const override;


private:

	/**
	 * Validity flags
	 */
	uint8_t flags_;
};


ARIncompleteTripletImpl::ARIncompleteTripletImpl(const uint32_t arcs,
		const uint32_t confidence,
		const uint32_t frame450_arcs,
		const bool arcs_valid,
		const bool confidence_valid,
		const bool frame450_arcs_valid)
	: ARTripletImpl(arcs, confidence, frame450_arcs)
	, flags_(0x00u // set bits 0,1,2 according to the validity flags
			| (arcs_valid          ? 0x01u : 0x00u)
			| (confidence_valid    ? 0x02u : 0x00u)
			| (frame450_arcs_valid ? 0x04u : 0x00u))
{
	// empty
}


bool ARIncompleteTripletImpl::arcs_valid() const
{
	return flags_ & uint8_t{0x01};
}


bool ARIncompleteTripletImpl::confidence_valid() const
{
	return flags_ & uint8_t{0x02};
}


bool ARIncompleteTripletImpl::frame450_arcs_valid() const
{
	return flags_ & uint8_t{0x04};
}


std::unique_ptr<ARTripletImpl> ARIncompleteTripletImpl::clone() const
{
	return std::make_unique<ARIncompleteTripletImpl>(this->arcs(),
			this->confidence(), this->frame450_arcs(),
			this->arcs_valid(), this->confidence_valid(),
			this->frame450_arcs_valid());
}


// ARTriplet


ARTriplet::ARTriplet(const uint32_t arcs,
		const uint32_t confidence,
		const uint32_t frame450_arcs)
	: impl_(std::make_unique<ARTripletImpl>(arcs, confidence, frame450_arcs))
{
	// empty
}


ARTriplet::ARTriplet(const uint32_t arcs,
		const uint32_t confidence,
		const uint32_t frame450_arcs,
		const bool arcs_valid,
		const bool confidence_valid,
		const bool frame450_arcs_valid)
	: impl_(std::make_unique<ARIncompleteTripletImpl>(arcs, confidence,
				frame450_arcs, arcs_valid, confidence_valid,
				frame450_arcs_valid))
{
	// empty
}


ARTriplet::ARTriplet(const ARTriplet &rhs)
	: impl_(rhs.impl_->clone()) // deep copy
{
	// empty
}


ARTriplet::ARTriplet(ARTriplet &&rhs) noexcept = default;


ARTriplet::~ARTriplet() noexcept = default;


uint32_t ARTriplet::arcs() const
{
	return impl_->arcs();
}


uint32_t ARTriplet::confidence() const
{
	return impl_->confidence();
}


uint32_t ARTriplet::frame450_arcs() const
{
	return impl_->frame450_arcs();
}


bool ARTriplet::arcs_valid() const
{
	return impl_->arcs_valid();
}


bool ARTriplet::confidence_valid() const
{
	return impl_->confidence_valid();
}


bool ARTriplet::frame450_arcs_valid() const
{
	return impl_->frame450_arcs_valid();
}


ARTriplet& ARTriplet::operator = (const ARTriplet &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	impl_ = rhs.impl_->clone();
	return *this;
}


ARTriplet& ARTriplet::operator = (ARTriplet &&rhs) noexcept = default;


/**
 * \brief Implements ARBlock.
 *
 * \see ARBlock
 */
class ARBlock::Impl final
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
	explicit Impl(const ARId &id);

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
	 * Const iterator pointing to the first triplet.
	 *
	 * \return Const iterator pointing to the first triplet
	 */
	iterator begin();

	/**
	 * Const iterator pointing to the last triplet.
	 *
	 * \return Const iterator pointing behind the last triplet
	 */
	iterator end();

	/**
	 * Const iterator pointing to the first triplet.
	 *
	 * \return Const iterator pointing to the first triplet
	 */
	const_iterator cbegin() const;

	/**
	 * Const iterator pointing to the last triplet.
	 *
	 * \return Const iterator pointing behind the last triplet
	 */
	const_iterator cend() const;

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


private:

	/**
	 * The AccurateRip identifier representing the header information of the
	 * block
	 */
	ARId ar_id_;

	/**
	 * Triplets in the order of their occurrence in the block.
	 */
	std::vector<ARTriplet> triplets_;
};


ARBlock::Impl::Impl(const ARId &id)
	: ar_id_(id)
	, triplets_()
{
	// empty
}


const ARId& ARBlock::Impl::id() const
{
	return ar_id_;
}


void ARBlock::Impl::append(const ARTriplet &triplet)
{
	triplets_.push_back(triplet);
}


ARBlock::iterator ARBlock::Impl::begin()
{
	return triplets_.begin();
}


ARBlock::iterator ARBlock::Impl::end()
{
	return triplets_.end();
}


ARBlock::const_iterator ARBlock::Impl::cbegin() const
{
	return triplets_.cbegin();
}


ARBlock::const_iterator ARBlock::Impl::cend() const
{
	return triplets_.cend();
}


uint32_t ARBlock::Impl::size() const
{
	return triplets_.size();
}


const ARTriplet& ARBlock::Impl::operator [](const int index) const
{
	return triplets_[index];
}


ARTriplet& ARBlock::Impl::operator [](const int index)
{
	// Confer Meyers, Scott: Effective C++, 3rd ed.,
	// Item 3, Section "Avoiding Duplication in const and Non-const member
	// Functions", p. 23ff
	return const_cast<ARTriplet&>(
			(*static_cast<const ARBlock::Impl*>(this))[index]);
}


// ARBlock


ARBlock::ARBlock(const ARId &id)
	: impl_(std::make_unique<ARBlock::Impl>(id))
{
	// empty
}


ARBlock::ARBlock(const ARBlock &rhs)
	: impl_(std::make_unique<ARBlock::Impl>(*rhs.impl_))
{
	// empty
}


ARBlock::ARBlock(ARBlock &&rhs) noexcept = default;


ARBlock::~ARBlock() noexcept = default;


const ARId& ARBlock::id() const
{
	return impl_->id();
}


void ARBlock::append(const ARTriplet &triplet)
{
	impl_->append(triplet);
}


uint32_t ARBlock::size() const
{
	return impl_->size();
}


ARBlock::iterator ARBlock::begin()
{
	return impl_->begin();
}


ARBlock::iterator ARBlock::end()
{
	return impl_->end();
}


ARBlock::const_iterator ARBlock::begin() const
{
	return impl_->cbegin();
}


ARBlock::const_iterator ARBlock::end() const
{
	return impl_->cend();
}


ARBlock::const_iterator ARBlock::cbegin() const
{
	return impl_->cbegin();
}


ARBlock::const_iterator ARBlock::cend() const
{
	return impl_->cend();
}


const ARTriplet& ARBlock::operator [](const int index) const
{
	return (*impl_)[index];
}


ARTriplet& ARBlock::operator [](const int index)
{
	return (*impl_)[index];
}


ARBlock& ARBlock::operator = (const ARBlock &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	impl_ = std::make_unique<ARBlock::Impl>(*rhs.impl_);
	return *this;
}


ARBlock& ARBlock::operator = (ARBlock &&rhs) noexcept = default;


/**
 * \brief Represents a response from AccurateRip.
 *
 * \see ARResponse
 */
class ARResponse::Impl final
{

public:

	/**
	 * Default constructor
	 */
	Impl();

	/**
	 * Implements ARResponse::append(const ARBlock &block)
	 */
	void append(const ARBlock &block);

	/**
	 * Implements ARResponse::block(const int i) const
	 */
	const ARBlock& block(const int i) const;

	/**
	 * Implements ARResponse::size() const
	 */
	std::size_t size() const;

	/**
	 * Implements ARResponse::tracks_per_block() const
	 */
	int tracks_per_block() const;

	/**
	 * Implements ARResponse::begin()
	 */
	ARResponse::iterator begin();

	/**
	 * Implements ARResponse::end()
	 */
	ARResponse::iterator end();

	/**
	 * Implements ARResponse::cbegin() const
	 */
	ARResponse::const_iterator cbegin() const;

	/**
	 * Implements ARResponse::cend() const
	 */
	ARResponse::const_iterator cend() const;

	/**
	 * Return the ARBlock with the specified index
	 *
	 * \return ARBlock at index
	 */
	const ARBlock& operator [](const int index) const;

	/**
	 * Return the ARBlock with the specified index
	 *
	 * \return ARBlock at index
	 */
	ARBlock& operator [](const int index);


private:

	/**
	 * Internal representation of the response data.
	 */
	std::vector<ARBlock> blocks_;
};


ARResponse::Impl::Impl()
	: blocks_()
{
	// empty
}


void ARResponse::Impl::append(const ARBlock &block)
{
	blocks_.push_back(block);
}


const ARBlock& ARResponse::Impl::block(const int i) const
{
	return blocks_.at(i);
}


std::size_t ARResponse::Impl::size() const
{
	return blocks_.size();
}


int ARResponse::Impl::tracks_per_block() const
{
	return blocks_.empty() ? 0 : blocks_[0].size();
}


ARResponse::iterator ARResponse::Impl::begin()
{
	return blocks_.begin();
}


ARResponse::iterator ARResponse::Impl::end()
{
	return blocks_.end();
}


ARResponse::const_iterator ARResponse::Impl::cbegin() const
{
	return blocks_.cbegin();
}


ARResponse::const_iterator ARResponse::Impl::cend() const
{
	return blocks_.cend();
}


const ARBlock& ARResponse::Impl::operator [](const int index) const
{
	return blocks_[index];
}


ARBlock& ARResponse::Impl::operator [](const int index)
{
	// Confer Meyers, Scott: Effective C++, 3rd ed.,
	// Item 3, Section "Avoiding Duplication in const and Non-const member
	// Functions", p. 23ff
	return const_cast<ARBlock&>(
			(*static_cast<const ARResponse::Impl*>(this))[index]);
}


// ARResponse


ARResponse::ARResponse()
	: impl_(std::make_unique<ARResponse::Impl>())
{
	// empty
}


ARResponse::ARResponse(const ARResponse &rhs)
	: impl_(std::make_unique<ARResponse::Impl>(*rhs.impl_))
{
	// empty
}


ARResponse::ARResponse(ARResponse &&rhs) noexcept = default;


ARResponse::~ARResponse() noexcept = default;


void ARResponse::append(const ARBlock &block)
{
	impl_->append(block);
}


const ARBlock& ARResponse::block(const int i) const
{
	return impl_->block(i);
}


std::size_t ARResponse::size() const
{
	return impl_->size();
}


int ARResponse::tracks_per_block() const
{
	return impl_->tracks_per_block();
}


ARResponse::iterator ARResponse::begin()
{
	return impl_->begin();
}


ARResponse::iterator ARResponse::end()
{
	return impl_->end();
}


ARResponse::const_iterator ARResponse::begin() const
{
	return impl_->cbegin();
}


ARResponse::const_iterator ARResponse::end() const
{
	return impl_->cend();
}


ARResponse::const_iterator ARResponse::cbegin() const
{
	return impl_->cbegin();
}


ARResponse::const_iterator ARResponse::cend() const
{
	return impl_->cend();
}


const ARBlock& ARResponse::operator [](const int index) const
{
	return (*impl_)[index];
}


ARBlock& ARResponse::operator [](const int index)
{
	return (*impl_)[index];
}


ARResponse& ARResponse::operator = (const ARResponse &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	impl_ = std::make_unique<ARResponse::Impl>(*rhs.impl_);
	return *this;
}


ARResponse& ARResponse::operator = (ARResponse &&rhs) noexcept = default;


/**
 * \brief Implementation of a DefaultContentHandler
 *
 * \see DefaultContentHandler
 */
class DefaultContentHandler::Impl final
{

public:

	/**
	 * Default constructor
	 */
	Impl();

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	Impl(const Impl &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	Impl(Impl &&rhs) noexcept;

	/**
	 * default destructor
	 */
	~Impl() noexcept;

	/**
	 * Implements DefaultContentHandler::start_input()
	 */
	void start_input();

	/**
	 * Implements DefaultContentHandler::start_block()
	 */
	void start_block();

	/**
	 * Implements DefaultContentHandler::id()
	 */
	void id(const uint8_t track_count,
			const uint32_t id1,
			const uint32_t id2,
			const uint32_t cddb_id);

	/**
	 * Implements DefaultContentHandler::triplet(const uint32_t arcs, const uint8_t confidence, const uint32_t frame450_arcs)
	 */
	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * Implements DefaultContentHandler::triplet(const uint32_t arcs, const uint8_t confidence, const uint32_t frame450_arcs, const bool arcs_valid, const bool confidence_valid, const bool frame450_arcs_valid)
	 */
	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs,
			const bool arcs_valid,
			const bool confidence_valid,
			const bool frame450_arcs_valid);

	/**
	 * Implements DefaultContentHandler::end_block()
	 */
	void end_block();

	/**
	 * Implements DefaultContentHandler::end_input()
	 */
	void end_input();

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
	Impl& operator = (const Impl &rhs);

	/**
	 * Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	Impl& operator = (Impl &&rhs) noexcept;


private:

	/**
	 * Internal representation of the current_block_
	 */
	std::unique_ptr<ARBlock> current_block_;

	/**
	 * Aggregating of the blocks parsed
	 */
	ARResponse response_;
};


DefaultContentHandler::Impl::Impl()
	: current_block_(nullptr)
	, response_()
{
	// empty
}


DefaultContentHandler::Impl::Impl(
		const DefaultContentHandler::Impl &rhs)
	: current_block_(rhs.current_block_
		? std::make_unique<ARBlock>(*(rhs.current_block_.get()))
		: nullptr
		)
	, response_(rhs.response_)
{
	// empty
}


DefaultContentHandler::Impl::~Impl() noexcept = default;


void DefaultContentHandler::Impl::start_input()
{
	// empty
}


void DefaultContentHandler::Impl::start_block()
{
	// empty
}


void DefaultContentHandler::Impl::id(const uint8_t track_count,
		const uint32_t id1,
		const uint32_t id2,
		const uint32_t cddb_id)
{
	current_block_ =
		std::make_unique<ARBlock>(ARId(track_count, id1, id2, cddb_id));
}


void DefaultContentHandler::Impl::triplet(const uint32_t arcs,
		const uint8_t confidence,
		const uint32_t frame450_arcs)
{
	current_block_->append(ARTriplet(arcs, confidence, frame450_arcs));
}


void DefaultContentHandler::Impl::triplet(const uint32_t arcs,
		const uint8_t confidence,
		const uint32_t frame450_arcs,
		const bool arcs_valid,
		const bool confidence_valid,
		const bool frame450_arcs_valid)
{
	if (arcs_valid and confidence_valid and frame450_arcs_valid)
	{
		current_block_->append(ARTriplet(arcs, confidence, frame450_arcs));
	} else
	{
		current_block_->append(ARTriplet(arcs, confidence,
					frame450_arcs, arcs_valid, confidence_valid,
					frame450_arcs_valid));
	}
}


void DefaultContentHandler::Impl::end_block()
{
	response_.append(*current_block_);
	current_block_.reset();
}


void DefaultContentHandler::Impl::end_input()
{
	// empty
}


ARResponse DefaultContentHandler::Impl::result() const
{
	return response_;
}


// DefaultContentHandler


DefaultContentHandler::DefaultContentHandler()
	: impl_(std::make_unique<DefaultContentHandler::Impl>())
{
	// empty
}


DefaultContentHandler::DefaultContentHandler(const DefaultContentHandler &rhs)
	: impl_(std::make_unique<DefaultContentHandler::Impl>(*rhs.impl_))
{
	// empty
}


DefaultContentHandler::~DefaultContentHandler() noexcept = default;


void DefaultContentHandler::start_input()
{
	impl_->start_input();
}


void DefaultContentHandler::start_block()
{
	impl_->start_block();
}


void DefaultContentHandler::id(const uint8_t track_count,
		const uint32_t id1,
		const uint32_t id2,
		const uint32_t cddb_id)
{
	impl_->id(track_count, id1, id2, cddb_id);
}


void DefaultContentHandler::triplet(const uint32_t arcs,
		const uint8_t confidence,
		const uint32_t frame450_arcs)
{
	impl_->triplet(arcs, confidence, frame450_arcs);
}


void DefaultContentHandler::triplet(const uint32_t arcs,
		const uint8_t confidence,
		const uint32_t frame450_arcs,
		const bool arcs_valid,
		const bool confidence_valid,
		const bool frame450_arcs_valid)
{
	impl_->triplet(arcs, confidence, frame450_arcs, arcs_valid,
			confidence_valid, frame450_arcs_valid);
}


void DefaultContentHandler::end_block()
{
	impl_->end_block();
}


void DefaultContentHandler::end_input()
{
	impl_->end_input();
}


std::unique_ptr<ContentHandler> DefaultContentHandler::clone() const
{
	return std::make_unique<DefaultContentHandler>(*this);
}


ARResponse DefaultContentHandler::result() const
{
	return impl_->result();
}


// DefaultErrorHandler


void DefaultErrorHandler::error(const uint32_t byte_pos,
		const uint32_t block, const uint32_t block_byte_pos)
{
	constexpr int BHB = 13; // number of block header bytes
	constexpr int BPT =  9; // number of bytes in a triplet

	std::stringstream cause;

	if (byte_pos > block_byte_pos) // This actually must be the case
	{
		const auto bytes_per_block = (byte_pos - block_byte_pos) / (block - 1);

		cause << "Current block ended after "     << block_byte_pos
			<< " bytes but was expected to have " << bytes_per_block
			<< " bytes ("
			<< (bytes_per_block - block_byte_pos) << " bytes missing).";
	} else
	{
		cause << "Unknown.";
	}

	std::stringstream logical_pos;
	logical_pos << "Block " << block;

	std::stringstream logical_err;

	if (block_byte_pos == 0) // error at block start (expected more blocks?)
	{
		logical_pos << ", begin of block " << block;
	}
	else if (block_byte_pos > BHB * sizeof(char)) // error in track information
	{
		const uint32_t triplets_byte_pos = block_byte_pos - BHB * sizeof(char);

		logical_pos << ", track "
			<< (triplets_byte_pos / BPT * sizeof(char) + 1);

		auto track_byte_pos = triplets_byte_pos % (BPT * sizeof(char));

		if (track_byte_pos == 0)
		{
			logical_pos << ", before track count";
			logical_err << "More tracks expected.";
		} else
		if (track_byte_pos == 1) // confidence ok
		{
			logical_pos << ", after track count";
			logical_err << "ARCS is missing.";
		} else
		if (track_byte_pos < 5) // arcs incomplete
		{
			logical_pos << ", within ARCS";
			logical_err << "ARCS is incomplete"
				<< ", only " << (track_byte_pos - 1) << " bytes instead of 4";
		} else
		if (track_byte_pos == 5) // confidence + arcs ok, frame450_arcs missing
		{
			logical_pos << ", after ARCS";
			logical_err << "Frame450_arcs is missing.";
		} else
		if (track_byte_pos < BPT) // frame450_arcs incomplete
		{
			logical_pos << ", within frame450_arcs";
			logical_err << "Frame450_arcs is incomplete"
				<< ", only " << (track_byte_pos - 5) << " bytes instead of 4";
		}
	}
	else // error in header bytes
	{
		logical_pos << ", header: ";

		if (block_byte_pos < 1 * sizeof(char))
		{
			logical_pos << "track count expected but missing";
			logical_err << "Expected header but none present";
		} else
		if (block_byte_pos == 1 * sizeof(char)) // track count ok
		{
			logical_pos << "disc id 1 expected but missing";
			logical_err << "Missing Disc id 1";
		} else
		if (block_byte_pos < 5 * sizeof(char)) // Disc Id 1 incomplete
		{
			logical_pos << "disc id 1"
					<< " is only " << (block_byte_pos - 1)
					<< " bytes wide instead of 4";
			logical_err << "Disc id 1 incomplete";
		} else
		if (block_byte_pos == 5 * sizeof(char)) // Disc Id 2 missing
		{
			logical_pos << "disc id 2 expected but missing";
			logical_err << "Missing Disc id 2";
		} else
		if (block_byte_pos < 9 * sizeof(char)) // Disc Id 2 incomplete
		{
			logical_pos << "disc id 2"
					<< " is only " << (block_byte_pos - 5)
					<< " bytes wide instead of 4";
			logical_err << "Disc id 2 incomplete";
		} else
		if (block_byte_pos == 9 * sizeof(char)) // CDDB Id missing
		{
			logical_pos << "cddb id expected but missing";
			logical_err << "Missing CDDB id";
		} else
		if (block_byte_pos < BHB * sizeof(char)) // CDDB Id incomplete
		{
			logical_pos << "cddb id"
					<< " is only " << (block_byte_pos - 9)
					<< " bytes wide instead of 4";
			logical_err << "CDDB id incomplete";
		}
		else
		{
			logical_pos << "After header";
			logical_err << "Unexpected end after header";
		}
	}

	std::stringstream message;
	message << "Read error after byte " << byte_pos
		<< ", Logical error position: " << logical_pos.str()
		<< ", Problem: " << logical_err.str()
		<< ", Cause: " << cause.str();
	ARCS_LOG_ERROR << message.str();

	throw StreamReadException(byte_pos, block, block_byte_pos, message.str());
}


std::unique_ptr<ErrorHandler> DefaultErrorHandler::clone()
	const
{
	return std::make_unique<DefaultErrorHandler>(*this);
}


// StreamReadException


StreamReadException::StreamReadException(const uint32_t byte_pos,
		const uint32_t block,
		const uint32_t block_byte_pos,
		const std::string &what_arg)
	: std::runtime_error(what_arg)
	, byte_pos_(byte_pos)
	, block_(block)
	, block_byte_pos_(block_byte_pos)
{
	// empty
}


StreamReadException::StreamReadException(const uint32_t byte_pos,
		const uint32_t block,
		const uint32_t block_byte_pos,
		const char *what_arg)
	: std::runtime_error(what_arg)
	, byte_pos_(byte_pos)
	, block_(block)
	, block_byte_pos_(block_byte_pos)
{
	// empty
}


uint32_t StreamReadException::byte_position() const
{
	return byte_pos_;
}


uint32_t StreamReadException::block() const
{
	return block_;
}


uint32_t StreamReadException::block_byte_position() const
{
	return block_byte_pos_;
}


/**
 * \brief Implements ARStreamParser.
 *
 * \see ARStreamParser
 */
class ARStreamParser::Impl final
{

public:

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	Impl(const Impl &rhs);

	/**
	 * Size in bytes of the block header containing disc_id_1, disc_id_2 and
	 * cddb_id.
	 */
	static constexpr int BLOCK_HEADER_BYTES = 13;

	/**
	 * Size in bytes of a triplet containing the ARCS for a single track,
	 * the confidence of this ARCS, and the ARCS of frame 450 of this track.
	 */
	static constexpr int TRIPLET_BYTES      =  9;

	/**
	 * Default constructor.
	 */
	explicit Impl(const ARStreamParser *parser);

	/**
	 * Default destructor.
	 */
	~Impl() noexcept = default;

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
	 * Parses the byte stream of an AccurateRip response.
	 *
	 * \param[in] in_stream The stream to be parsed
	 *
	 * \return Number of bytes parsed from \c in_stream
	 *
	 * \throw StreamReadException If reading of the stream fails
	 */
	uint32_t do_parse(std::istream &in_stream);

	/**
	 * Copy assignment
	 *
	 * \param[in] rhs The instance to assign
	 */
	Impl& operator = (const Impl &rhs);


private:

	/**
	 * React on parse errors.
	 *
	 * If an error handler is present, the error data is passed to the handler,
	 * otherwise a StreamReadException is thrown.
	 *
	 * \param[in] byte_pos       Last 1-based byte pos read before error
	 * \param[in] block          1-based block number
	 * \param[in] block_byte_pos Last 1-based block byte pos read before error
	 *
	 * \throw StreamReadException If reading of the stream fails
	 */
	virtual void on_error(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos);

	/**
	 * Worker method for parsing an input stream.
	 *
	 * \param[in] in_stream The stream to be parsed
	 *
	 * \throw StreamReadException If reading of the stream fails
	 *
	 * \return Number of parsed bytes
	 *
	 * \todo This implementation silently relies on a little endian plattform.
	 */
	uint32_t parse_stream(std::istream &in_stream);

	/**
	 * Service method: Interpret 4 bytes as a 32 bit unsigned integer
	 * with little endian storage, which means that the bits of b4 become the
	 * most significant bits of the result.
	 *
	 * \param[in] b1 First input byte, least significant bits of the result
	 * \param[in] b2 Second input byte
	 * \param[in] b3 Third input byte
	 * \param[in] b4 Fourth input byte, most significant bits of the result
	 *
	 * \return The bytes as 32 bit unsigned integer
	 */
	uint32_t le_bytes_to_uint32(const char b1,
		const char b2,
		const char b3,
		const char b4) const;

	/**
	 * Internal event handler instance.
	 */
	std::unique_ptr<ContentHandler> content_handler_;

	/**
	 * Internal error handler instance.
	 */
	std::unique_ptr<ErrorHandler> error_handler_;

	/**
	 * Back pointer to ARStreamParser instance.
	 */
	const ARStreamParser *parser_;
};


// ARStreamParser::Impl


ARStreamParser::Impl::Impl(const ARStreamParser *parser)
	: content_handler_(nullptr)
	, error_handler_(nullptr)
	, parser_(parser)
{
	// empty
}


ARStreamParser::Impl::Impl(const Impl &rhs)
	: content_handler_(rhs.content_handler_->clone())
	, error_handler_(rhs.error_handler_->clone())
	, parser_(nullptr) // Do not copy the parent object of rhs!
{
	// empty
}


void ARStreamParser::Impl::set_content_handler(
		std::unique_ptr<ContentHandler> handler)
{
	content_handler_ = std::move(handler);
}


const ContentHandler& ARStreamParser::Impl::content_handler() const
{
	return *content_handler_;
}


void ARStreamParser::Impl::set_error_handler(
		std::unique_ptr<ErrorHandler> handler)
{
	error_handler_ = std::move(handler);
}


const ErrorHandler& ARStreamParser::Impl::error_handler() const
{
	return *error_handler_;
}


uint32_t ARStreamParser::Impl::do_parse(std::istream &in_stream)
{
	uint32_t bytes{0};
	try
	{
		bytes = this->parse_stream(in_stream);
	}
	catch (const StreamReadException& sre)
	{
		parser_->on_catched_exception(in_stream, sre);

		ARCS_LOG_ERROR << "Unexcpected end of input stream: " << sre.what();

		throw sre;
	}
	catch (const std::exception& e)
	{
		parser_->on_catched_exception(in_stream, e);

		ARCS_LOG_ERROR << "Failed to parse input stream: " << e.what();

		throw e;
	}

	return bytes;
}


void ARStreamParser::Impl::on_error(const uint32_t byte_pos,
		const uint32_t block, const uint32_t block_byte_pos)
{
	if (error_handler_)
	{
		error_handler_->error(byte_pos, block, block_byte_pos);
	}

	std::stringstream ss;
	ss << "Error on input byte " << byte_pos << " (block " << block
		<< ", byte " << block_byte_pos << ")";

	throw StreamReadException(byte_pos, block, block_byte_pos, ss.str());
}


uint32_t ARStreamParser::Impl::parse_stream(std::istream &in)
{
	if (not content_handler_)
	{
		ARCS_LOG_WARNING
			<< "Parser has no content handler attached, skip parsing";
		return 0;
	}

	std::vector<char> id(BLOCK_HEADER_BYTES * sizeof(char));
	std::vector<char> triplet(TRIPLET_BYTES * sizeof(char));

	unsigned int track_count = 0;
	uint32_t discId1 = 0;
	uint32_t discId2 = 0;
	uint32_t cddbId = 0;
	unsigned int confidence = 0;
	uint32_t trk_arcs = 0;
	uint32_t frame450_arcs = 0;

	unsigned int bytes_read = 0;

	unsigned int byte_counter = 0;
	unsigned int block_counter = 0;
	unsigned int block_byte_counter = 0;

	content_handler_->start_input();

	// Commented out:
	// Provided that the file size is carefully verified to be "small",
	// we could also read the entire binary file content in a vector<uint8_t>
	// and proceed working on that vector:
	//
	// std::vector<uint8_t> file_content(const std::string &filename) const
	// {
	//   std::ifstream input(filename.c_str(), std::ios::in | std::ios::binary);
	//
	//   std::vector<uint8_t> bytes(
	//		(std::istreambuf_iterator<char>(input)),
	//		std::istreambuf_iterator<char>()
	//   );
	//
	//   return bytes;
	// }
	//
	// Currently, we read the file bytewise instead. This is more effort, but
	// enables instant detecting of unexpected input and very precise error
	// messages.

	while (in.good() and in.peek() != EOF)
	{
		++block_counter;

		block_byte_counter = 0;
		bytes_read = 0;

		content_handler_->start_block();

		// Read header of current block

		track_count = 0;
		discId1 = 0;
		discId2 = 0;
		cddbId  = 0;

		try
		{
			in.read(&id[0], BLOCK_HEADER_BYTES * sizeof(id[0]));
			bytes_read = in.gcount();

		} catch (const std::istream::failure& e)
		{
			bytes_read = in.gcount();
		}

		//ARCS_LOG_DEBUG << "Read " << bytes_read << " header bytes";

		byte_counter += bytes_read;
		block_byte_counter += bytes_read;

		//ARCS_LOG_DEBUG << "Read " << byte_counter << " bytes total";

		if (bytes_read == 0)
		{
			this->on_error(byte_counter, block_counter, 0);
			break;
		} else
		{
			// At least 1 byte has been read. We want to pass the bytes parsed
			// so far to the content handler

			track_count = id[0] & 0xFF;

			if (bytes_read <= 4 * sizeof(id[0]))
			{
				content_handler_->id(track_count, 0, 0, 0);

				this->on_error(byte_counter, block_counter, block_byte_counter);
				break;
			}

			discId1 = le_bytes_to_uint32(id[1], id[ 2], id[ 3], id[ 4]);

			if (bytes_read <= 8 * sizeof(id[0]))
			{
				content_handler_->id(track_count, discId1, 0, 0);

				this->on_error(byte_counter, block_counter, block_byte_counter);
				break;
			}

			discId2 = le_bytes_to_uint32(id[5], id[ 6], id[ 7], id[ 8]);

			if (bytes_read <= 12 * sizeof(id[0]))
			{
				content_handler_->id(track_count, discId1, discId2, 0);

				this->on_error(byte_counter, block_counter, block_byte_counter);
				break;
			}

			cddbId  = le_bytes_to_uint32(id[9], id[10], id[11], id[12]);

			ARCS_LOG_DEBUG << "New block starts with id: "
				<< ARId(track_count, discId1, discId2, cddbId).filename();

			content_handler_->id(track_count, discId1, discId2, cddbId);
		}

		if (not in.good())
		{
			this->on_error(byte_counter, block_counter, block_byte_counter);
			break;
		}

		bytes_read = 0;

		// Read triplets of current block

		for (uint8_t trk = 0; trk < track_count; ++trk)
		{
			try
			{
				in.read(&triplet[0], TRIPLET_BYTES * sizeof(triplet[0]));
				bytes_read = in.gcount();

			} catch (std::istream::failure& e)
			{
				bytes_read = in.gcount();
			}

			//ARCS_LOG_DEBUG << "Read " << bytes_read << " triplet bytes";

			byte_counter += bytes_read;
			block_byte_counter += bytes_read;

			//ARCS_LOG_DEBUG << "Read " << byte_counter << " bytes total";

			if (bytes_read == 0)
			{
				this->on_error(byte_counter, block_counter, block_byte_counter);
				break;
			} else
			{
				// At least 1 byte has been read. We want to pass the bytes
				// parsed so far to the content handler

				confidence = triplet[0] & 0xFF;

				if (bytes_read <= 4 * sizeof(id[0]))
				{
					// => We have read the confidence value, but reading
					// failed on the actual ARCS.

					// From a semantic perspective, it does not make sense to
					// pass a confidence value when we do not know the ARCS to
					// which it relates. The client could be mistake the 0 value
					// for the actual ARCS. From a technical point of view, it
					// may be interesting for the client to know how much bytes
					// could be read from the stream and have the values that
					// could be completely read without error. The client will
					// know that 0 is not a valid ARCS for a non-silent track.

					content_handler_->triplet(0, confidence, 0,
							false, true, false);

					this->on_error(byte_counter, block_counter,
							block_byte_counter);
					break;
				}

				trk_arcs = le_bytes_to_uint32(
								triplet[1], triplet[2],
								triplet[3], triplet[4]);

				if (bytes_read <= 8 * sizeof(id[0]))
				{
					content_handler_->triplet(trk_arcs, confidence, 0,
							true, true, false);

					this->on_error(byte_counter, block_counter,
							block_byte_counter);
					break;
				}

				frame450_arcs = le_bytes_to_uint32(
								triplet[5], triplet[6],
								triplet[7], triplet[8]);

				content_handler_->triplet(trk_arcs, confidence, frame450_arcs);
			}

			if (not in.good())
			{
				this->on_error(byte_counter, block_counter, block_byte_counter);
			}
		} // for

		content_handler_->end_block();
	}

	content_handler_->end_input();

	ARCS_LOG_DEBUG << "Parsing completed.";
	ARCS_LOG_INFO  << "Parsed " << byte_counter << " bytes";

	return byte_counter;
}


uint32_t ARStreamParser::Impl::le_bytes_to_uint32(const char b1,
		const char b2,
		const char b3,
		const char b4) const
{
	return  static_cast<uint32_t>(b4 & 0xFF) << 24 |
			static_cast<uint32_t>(b3 & 0xFF) << 16 |
			static_cast<uint32_t>(b2 & 0xFF) <<  8 |
			static_cast<uint32_t>(b1 & 0xFF);
}


ARStreamParser::Impl& ARStreamParser::Impl::operator = (
		const ARStreamParser::Impl &rhs)
{
	this->content_handler_ = rhs.content_handler_->clone();
	this->error_handler_   = rhs.error_handler_->clone();
	parser_ = nullptr;

	return *this;
}


// ARStreamParser


ARStreamParser::ARStreamParser()
	: impl_(std::make_unique<ARStreamParser::Impl>(this))
{
	//empty
}


ARStreamParser::~ARStreamParser() noexcept = default;


void ARStreamParser::set_content_handler(
		std::unique_ptr<ContentHandler> handler)
{
	impl_->set_content_handler(std::move(handler));
}


const ContentHandler& ARStreamParser::content_handler() const
{
	return impl_->content_handler();
}


void ARStreamParser::set_error_handler(
		std::unique_ptr<ErrorHandler> handler)
{
	impl_->set_error_handler(std::move(handler));
}


const ErrorHandler& ARStreamParser::error_handler() const
{
	return impl_->error_handler();
}


uint32_t ARStreamParser::do_parse(std::istream &in_stream)
{
	return impl_->do_parse(in_stream);
}


// ARFileParser


ARFileParser::ARFileParser()
	: filename_()
{
	// empty
}


ARFileParser::ARFileParser(const std::string &filename)
	: filename_(filename)
{
	// empty
}


void ARFileParser::set_file(const std::string &filename)
{
	this->filename_ = filename;
}


std::string ARFileParser::file() const
{
	return this->filename_;
}


uint32_t ARFileParser::parse()
{
	return this->parse(this->file());
}


uint32_t ARFileParser::parse(const std::string &filename)
{
	ARCS_LOG_DEBUG << "Open file: " << filename;

	std::ifstream filestream;

	filestream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		filestream.open(filename, std::ifstream::in | std::ifstream::binary);
	}
	catch (const std::ifstream::failure& f)
	{
		ARCS_LOG_ERROR << "Failed to open file: " << f.what();

		throw f;
	}

	return ARStreamParser::do_parse(filestream);
}


void ARFileParser::on_catched_exception(std::istream &istream,
		const std::exception & /* e */) const
{
	auto *filestream = dynamic_cast<std::ifstream*>(&istream);
	filestream->close();
}


// ARStdinParser


ARStdinParser::ARStdinParser() = default;


uint32_t ARStdinParser::parse()
{
	auto response_data = StdIn(1024).bytes();

	VectorIStream<char> response_data_w(response_data);
	std::istream stream(&response_data_w);

	return ARStreamParser::do_parse(stream);
}


void ARStdinParser::on_catched_exception(std::istream & /* istream */,
			const std::exception &/* e */) const
{
	// empty
}


} // namespace v_1_0_0

/// @}
/// \endcond
// IMPL_ONLY

} // namespace arcs

