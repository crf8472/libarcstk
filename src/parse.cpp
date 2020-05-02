/**
 * \file
 *
 * \brief Implementing the API for AccurateRip response parsing and syntactic
 * entities.
 */

#ifndef __LIBARCSTK_PARSE_HPP__
#include "parse.hpp"
#endif

#include <cerrno>              // for errno
#include <cstdio>              // for ferror, stdin, feof, freopen, fread, EOF
#include <cstring>             // for strerror
#include <fstream>             // for basic_ifstream
#include <memory>              // for unique_ptr, make_unique, allocator, swap
#include <sstream>             // for basic_stringstream
#include <stdexcept>           // for runtime_error, domain_error
#include <string>              // for char_traits, operator+, operator<<
#include <type_traits>         // for remove_reference<>::type
#include <typeinfo>            // for type_info
#include <utility>             // for move, swap
#include <vector>              // for vector

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif
#ifndef __LIBARCSTK_APPENDABLESEQ_HPP__
#include "appendableseq.hpp"
#endif


namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \internal \defgroup parseImpl Implementation
 * \ingroup parse
 * @{
 */

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
	 * \brief Constructor
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
	 * \brief Constructor.
	 *
	 * \param[in] buf_size Buffer size in bytes
	 */
	explicit StdIn(const std::size_t buf_size);

	/**
	 * \brief Reads stdin bytes in binary mode to a vector<char>.
	 *
	 * \return Bytes from stdin
	 */
	std::vector<char> bytes();

	/**
	 * \brief Size of read buffer in bytes.
	 *
	 * \return Buffer size in bytes
	 */
	std::size_t buf_size() const;


private:

	/**
	 * \brief Bytes per read
	 */
	const std::size_t buf_size_;
};


StdIn::StdIn(const std::size_t buf_size)
	: buf_size_ { buf_size }
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
		auto msg = std::stringstream {};
		msg << "While opening stdin for input: " << std::strerror(errno)
			<< " (errno " << errno << ")";

		throw std::runtime_error(msg.str());
	}

	// Commented out version with std::array (lines 149,155 and 167)

	auto bytes = std::vector<char> {}; // collects the input bytes
	auto len   = std::size_t { 0 }; // number of bytes read from stdin
	auto buf { std::make_unique<char[]>(buf_size()) }; // input buffer

	// As long as there are any bytes, read them

	while((len = std::fread(buf.get(), sizeof(buf[0]), buf_size(), stdin)) > 0)
	{
		if (std::ferror(stdin) and not std::feof(stdin))
		{
			auto msg = std::stringstream {};
			msg << "While reading from stdin: " << std::strerror(errno)
				<< " (errno " << errno << ")";

			throw std::runtime_error(msg.str());
		}

		bytes.insert(bytes.end(), buf.get(), buf.get() + len);
	}

	return bytes;
}


std::size_t StdIn::buf_size() const
{
	return buf_size_;
}

} // namespace


/**
 * \brief Interface for implementations of ARTriplet.
 *
 * \see ARTriplet
 */
class ARTripletImpl
{
public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ARTripletImpl() noexcept = default;

	/**
	 * \brief The track ARCS in this ARTripletImpl.
	 *
	 * The ARCS may be v1 as well as v2.
	 *
	 * \return ARCS value in this triplet
	 */
	uint32_t arcs() const noexcept;

	/**
	 * \brief The confidence value in this ARTripletImpl.
	 *
	 * \return Confidence in this triplet
	 */
	uint32_t confidence() const noexcept;

	/**
	 * \brief ARCS of frame 450 of the particular track in this ARTripletImpl.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	uint32_t frame450_arcs() const noexcept;

	/**
	 * \brief The track ARCS in this ARTripletImpl.
	 *
	 * The ARCS may be v1 as well as v2.
	 *
	 * \return ARCS value in this triplet
	 */
	bool arcs_valid() const noexcept;

	/**
	 * \brief The confidence value in this ARTripletImpl.
	 *
	 * \return Confidence in this triplet
	 */
	bool confidence_valid() const noexcept;

	/**
	 * \brief ARCS of frame 450 of the particular track in this ARTripletImpl.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	bool frame450_arcs_valid() const noexcept;

	/**
	 * \brief Returns TRUE iff this instance does not hold any parsed values,
	 * otherwise FALSE.
	 *
	 * \return TRUE iff this instance is empty, otherwise FALSE.
	 */
	bool empty() const noexcept;

	/**
	 * \brief Clone this instance, i.e. create a deep copy
	 *
	 * \return A clone of this instance
	 */
	std::unique_ptr<ARTripletImpl> clone() const noexcept;

	/**
	 * \brief Equality
	 *
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE if \c rhs is equal to the instance, otherwise FALSE
	 */
	bool equals(const ARTripletImpl &rhs) const noexcept;

	/**
	 * \brief Swap
	 *
	 * \param[in] rhs Right hand side to swap
	 */
	void swap(ARTripletImpl &rhs);


protected:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] arcs          The ARCS value of this triplet
	 * \param[in] confidence    The confidence value of this triplet
	 * \param[in] frame450_arcs The ARCS for frame 450 of this triplet
	 */
	ARTripletImpl(const uint32_t arcs, const uint32_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARTripletImpl(const ARTripletImpl &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	ARTripletImpl(ARTripletImpl &&rhs) noexcept;


private:

	/**
	 * \brief The track ARCS in this ARTripletImpl.
	 *
	 * The ARCS may be v1 as well as v2.
	 *
	 * \return ARCS value in this triplet
	 */
	virtual bool do_arcs_valid() const noexcept
	= 0;

	/**
	 * \brief The confidence value in this ARTripletImpl.
	 *
	 * \return Confidence in this triplet
	 */
	virtual bool do_confidence_valid() const noexcept
	= 0;

	/**
	 * \brief The ARCS of frame 450 of the particular track in this ARTripletImpl.
	 *
	 * \return Frame450 ARCS in this triplet
	 */
	virtual bool do_frame450_arcs_valid() const noexcept
	= 0;

	/**
	 * \brief Clone this instance, i.e. create a deep copy
	 *
	 * \return A clone of this instance
	 */
	virtual std::unique_ptr<ARTripletImpl> do_clone() const noexcept
	= 0;

	/**
	 * \brief Implements the part of equals() specific for this class
	 *
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE if \c rhs is equal to the instance, otherwise FALSE
	 */
	virtual bool do_equals(const ARTripletImpl &rhs) const noexcept;

	/**
	 * \brief Implements the part of swap() specific for this class
	 *
	 * \param[in] rhs Right hand side to swap
	 */
	virtual void do_swap(ARTripletImpl &rhs);

	/**
	 * \brief ARCS v1 or v2 of this track
	 */
	uint32_t arcs_;

	/**
	 * \brief Confidence of arcs_
	 */
	uint32_t confidence_;

	/**
	 * \brief ARCS of frame 450
	 */
	uint32_t frame450_arcs_;
};


ARTripletImpl::ARTripletImpl(const uint32_t arcs,
		const uint32_t confidence,
		const uint32_t frame450_arcs)
	: arcs_ { arcs }
	, confidence_ { confidence }
	, frame450_arcs_ { frame450_arcs }
{
	// empty
}


ARTripletImpl::ARTripletImpl(const ARTripletImpl &rhs) = default;


ARTripletImpl::ARTripletImpl(ARTripletImpl &&rhs) noexcept = default;


uint32_t ARTripletImpl::arcs() const noexcept
{
	return arcs_;
}


uint32_t ARTripletImpl::confidence() const noexcept
{
	return confidence_;
}


uint32_t ARTripletImpl::frame450_arcs() const noexcept
{
	return frame450_arcs_;
}


bool ARTripletImpl::arcs_valid() const noexcept
{
	return this->do_arcs_valid();
}


bool ARTripletImpl::confidence_valid() const noexcept
{
	return this->do_confidence_valid();
}


bool ARTripletImpl::frame450_arcs_valid() const noexcept
{
	return this->do_frame450_arcs_valid();
}


// ARTripletImpl::empty() must be defined after AREmptyTripletImpl below!


std::unique_ptr<ARTripletImpl> ARTripletImpl::clone() const noexcept
{
	return this->do_clone();
}


bool ARTripletImpl::equals(const ARTripletImpl &rhs) const noexcept
{
	const bool base_equals = typeid(*this) == typeid(rhs)
					&& this->arcs_    == rhs.arcs_
					&& this->confidence_    == rhs.confidence_
					&& this->frame450_arcs_ == rhs.frame450_arcs_;

	return base_equals && this->do_equals(rhs);
}


void ARTripletImpl::swap(ARTripletImpl &rhs)
{
	if (typeid(*this) != typeid(rhs))
	{
		throw std::domain_error(
			"Cannot swap instances of different ARTripletImpl subclasses");
	}

	using std::swap;

	swap(this->arcs_,          rhs.arcs_);
	swap(this->confidence_,    rhs.confidence_);
	swap(this->frame450_arcs_, rhs.frame450_arcs_);

	this->do_swap(rhs);
}


bool ARTripletImpl::do_equals(const ARTripletImpl & /* rhs */) const noexcept
{
	return true; // Means: no specific additions for equality check
}


void ARTripletImpl::do_swap(ARTripletImpl & /* rhs */)
{
	// empty     // Means: no specific actions to swap
}


/**
 * \brief Implements an incompletely parsed ARTriplet.
 *
 * \see ARTriplet
 */
class ARCompleteTripletImpl final : public ARTripletImpl
{

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] arcs          The ARCS value of this triplet
	 * \param[in] confidence    The confidence value of this triplet
	 * \param[in] frame450_arcs The ARCS for frame 450 of this triplet
	 */
	ARCompleteTripletImpl(const uint32_t arcs, const uint32_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARCompleteTripletImpl(const ARCompleteTripletImpl &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	ARCompleteTripletImpl(ARCompleteTripletImpl &&rhs) noexcept;

	/**
	 * \brief Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARCompleteTripletImpl& operator = (const ARCompleteTripletImpl &rhs)
	= delete;

	/**
	 * \brief Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARCompleteTripletImpl& operator = (ARCompleteTripletImpl &&rhs) noexcept
	= delete;


private:

	bool do_arcs_valid() const noexcept final;

	bool do_confidence_valid() const noexcept final;

	bool do_frame450_arcs_valid() const noexcept final;

	std::unique_ptr<ARTripletImpl> do_clone() const noexcept final;
};


ARCompleteTripletImpl::ARCompleteTripletImpl(const uint32_t arcs,
		const uint32_t confidence,
		const uint32_t frame450_arcs)
	: ARTripletImpl { arcs, confidence, frame450_arcs }
{
	// empty
}


ARCompleteTripletImpl::ARCompleteTripletImpl(const ARCompleteTripletImpl &rhs)
	: ARTripletImpl { rhs }
{
	// empty
}


ARCompleteTripletImpl::ARCompleteTripletImpl(ARCompleteTripletImpl &&rhs)
	noexcept
	: ARTripletImpl { std::move(rhs) }
{
	// empty
}


bool ARCompleteTripletImpl::do_arcs_valid() const noexcept
{
	return true;
}


bool ARCompleteTripletImpl::do_confidence_valid() const noexcept
{
	return true;
}


bool ARCompleteTripletImpl::do_frame450_arcs_valid() const noexcept
{
	return true;
}


std::unique_ptr<ARTripletImpl> ARCompleteTripletImpl::do_clone() const noexcept
{
	return std::make_unique<ARCompleteTripletImpl>(
			this->arcs(), this->confidence(), this->frame450_arcs());
}


/**
 * \brief Implements an incompletely parsed ARTriplet.
 *
 * It carries information about the validity of its parts.
 */
class ARIncompleteTripletImpl final : public ARTripletImpl
{

public:

	/**
	 * \brief Constructor for incomplete ARTriplets.
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

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARIncompleteTripletImpl(const ARIncompleteTripletImpl &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	ARIncompleteTripletImpl(ARIncompleteTripletImpl &&rhs) noexcept;

	/**
	 * \brief Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARIncompleteTripletImpl& operator = (const ARIncompleteTripletImpl &rhs)
	= delete;

	/**
	 * \brief Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	ARIncompleteTripletImpl& operator = (ARIncompleteTripletImpl &&rhs) noexcept
	= delete;


private:

	bool do_arcs_valid() const noexcept override;

	bool do_confidence_valid() const noexcept override;

	bool do_frame450_arcs_valid() const noexcept override;

	std::unique_ptr<ARTripletImpl> do_clone() const noexcept override;

	bool do_equals(const ARTripletImpl &rhs) const noexcept override;

	void do_swap(ARTripletImpl &rhs) override;

	/**
	 * \brief Validity flags
	 */
	uint8_t flags_;
};


ARIncompleteTripletImpl::ARIncompleteTripletImpl(const uint32_t arcs,
		const uint32_t confidence,
		const uint32_t frame450_arcs,
		const bool arcs_valid,
		const bool confidence_valid,
		const bool frame450_arcs_valid)
	: ARTripletImpl { arcs, confidence, frame450_arcs }
	, flags_ { static_cast<uint8_t>(0x00u
			| (arcs_valid          ? 0x01u : 0x00u)
			| (confidence_valid    ? 0x02u : 0x00u)
			| (frame450_arcs_valid ? 0x04u : 0x00u)) }
	// set bits 0,1,2 according to the validity flags
{
	// empty
}


ARIncompleteTripletImpl::ARIncompleteTripletImpl(
		const ARIncompleteTripletImpl &rhs)
	: ARTripletImpl { rhs }
	, flags_ { rhs.flags_ }
{
	// empty
}


ARIncompleteTripletImpl::ARIncompleteTripletImpl(ARIncompleteTripletImpl &&rhs)
	noexcept
	: ARTripletImpl { rhs } // Calls copy ctor!
	, flags_ { rhs.flags_ } // <- Impossible after std::move(rhs) !
{
	// empty // TODO Copy is unnecessary, find better solution
}


bool ARIncompleteTripletImpl::do_arcs_valid() const noexcept
{
	return flags_ & uint8_t { 0x01 };
}


bool ARIncompleteTripletImpl::do_confidence_valid() const noexcept
{
	return flags_ & uint8_t { 0x02 };
}


bool ARIncompleteTripletImpl::do_frame450_arcs_valid() const noexcept
{
	return flags_ & uint8_t { 0x04 };
}


std::unique_ptr<ARTripletImpl> ARIncompleteTripletImpl::do_clone() const
	noexcept
{
	return std::make_unique<ARIncompleteTripletImpl>(this->arcs(),
			this->confidence(), this->frame450_arcs(),
			this->arcs_valid(), this->confidence_valid(),
			this->frame450_arcs_valid());
}


bool ARIncompleteTripletImpl::do_equals(const ARTripletImpl &rhs) const
	noexcept
{
	return this->arcs_valid()          == rhs.arcs_valid()
		&& this->confidence_valid()    == rhs.confidence_valid()
		&& this->frame450_arcs_valid() == rhs.frame450_arcs_valid();
}


void ARIncompleteTripletImpl::do_swap(ARTripletImpl &rhs)
{
	auto casted_rhs { dynamic_cast<ARIncompleteTripletImpl*>(&rhs) };

	if (!casted_rhs)
	{
		throw std::domain_error("Type mismatch detected while swapping");
	}

	using std::swap;

	swap(this->flags_, casted_rhs->flags_);
}


/**
 * \brief Represents an empty ARTriplet.
 */
class AREmptyTripletImpl final : public ARTripletImpl
{
public:

	/**
	 * \brief Default constructor
	 */
	AREmptyTripletImpl();

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	AREmptyTripletImpl(const AREmptyTripletImpl &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Instance to move
	 */
	AREmptyTripletImpl(AREmptyTripletImpl &&rhs) noexcept;

	/**
	 * \brief Copy assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	AREmptyTripletImpl& operator = (const AREmptyTripletImpl &rhs)
	= delete;

	/**
	 * \brief Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	AREmptyTripletImpl& operator = (AREmptyTripletImpl &&rhs) noexcept
	= delete;


private:

	bool do_arcs_valid() const noexcept final;

	bool do_confidence_valid() const noexcept final;

	bool do_frame450_arcs_valid() const noexcept final;

	std::unique_ptr<ARTripletImpl> do_clone() const noexcept final;
};


AREmptyTripletImpl::AREmptyTripletImpl()
	: ARTripletImpl { 0, 0, 0 } // This defines emptyness for all
{
	// empty
}


AREmptyTripletImpl::AREmptyTripletImpl(const AREmptyTripletImpl &rhs)
	: ARTripletImpl { rhs }
{
	// empty
}


AREmptyTripletImpl::AREmptyTripletImpl(AREmptyTripletImpl &&rhs) noexcept
	: ARTripletImpl { std::move(rhs) }
{
	// empty
}


bool AREmptyTripletImpl::do_arcs_valid() const noexcept
{
	return false;
}


bool AREmptyTripletImpl::do_confidence_valid() const noexcept
{
	return false;
}


bool AREmptyTripletImpl::do_frame450_arcs_valid() const noexcept
{
	return false;
}


std::unique_ptr<ARTripletImpl> AREmptyTripletImpl::do_clone() const
	noexcept
{
	return std::make_unique<AREmptyTripletImpl>();
}


// ARTripletImpl::empty()


bool ARTripletImpl::empty() const noexcept
{
	return this->equals(AREmptyTripletImpl{});

	// This entails that only the definition of AREmptyTripletImpl
	// defines when exactly an ARTriplet is empty!
}


// ARTriplet


ARTriplet::ARTriplet()
	: impl_ { std::make_unique<AREmptyTripletImpl>() }
{
	// empty
}


ARTriplet::ARTriplet(const uint32_t arcs,
		const uint32_t confidence,
		const uint32_t frame450_arcs)
	: impl_ { std::make_unique<ARCompleteTripletImpl>( arcs, confidence,
				frame450_arcs) }
{
	// empty
}


ARTriplet::ARTriplet(const uint32_t arcs,
		const uint32_t confidence,
		const uint32_t frame450_arcs,
		const bool arcs_valid,
		const bool confidence_valid,
		const bool frame450_arcs_valid)
	: impl_ { std::make_unique<ARIncompleteTripletImpl>(arcs, confidence,
				frame450_arcs, arcs_valid, confidence_valid,
				frame450_arcs_valid) }
{
	// empty
}


ARTriplet::ARTriplet(const ARTriplet &rhs)
	: impl_ { rhs.impl_->clone() } // deep copy
{
	// empty
}


ARTriplet::ARTriplet(ARTriplet &&rhs) noexcept = default;


ARTriplet::~ARTriplet() noexcept = default;


uint32_t ARTriplet::arcs() const noexcept
{
	return impl_->arcs();
}


uint32_t ARTriplet::confidence() const noexcept
{
	return impl_->confidence();
}


uint32_t ARTriplet::frame450_arcs() const noexcept
{
	return impl_->frame450_arcs();
}


bool ARTriplet::arcs_valid() const noexcept
{
	return impl_->arcs_valid();
}


bool ARTriplet::confidence_valid() const noexcept
{
	return impl_->confidence_valid();
}


bool ARTriplet::frame450_arcs_valid() const noexcept
{
	return impl_->frame450_arcs_valid();
}


bool ARTriplet::empty() const noexcept
{
	return impl_->empty();
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


// ARTriplet friends


bool operator == (const ARTriplet &lhs, const ARTriplet &rhs) noexcept
{
	return lhs.impl_->equals(*rhs.impl_);
}


void swap(ARTriplet &lhs, ARTriplet &rhs) noexcept
{
	lhs.impl_->swap(*rhs.impl_);
}


using ARBlockImplBase = details::AppendableSequence<ARTriplet>;


/**
 * \brief Private implementation of ARBlock.
 *
 * \see ARBlock
 */
class ARBlock::Impl final : public ARBlockImplBase
{
public: /* member functions */

	/**
	 * \brief Constructor.
	 *
	 * \param[in] id ARId of the cd this block describes
	 */
	explicit Impl(const ARId &id);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] id       ARId of this block
	 * \param[in] triplets ARTriplets in block
	 */
	Impl(ARId &&id, std::initializer_list<ARTriplet> triplets);

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs Right hand side for copy
	 *
	 * \return Deep copy of \c rhs
	 */
	Impl(const Impl &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Right hand side for move
	 *
	 * \return Instance moved from \c rhs
	 */
	Impl(Impl &&rhs) noexcept;

	/**
	 * \brief Returns the AccurateRip Id (i.e. the header) of the block.
	 *
	 * \return ARId of this block
	 */
	const ARId& id() const;


private:

	std::unique_ptr<ARBlockImplBase> do_clone() const override;

	std::unique_ptr<ARBlockImplBase> do_create(
			const ARBlockImplBase::size_type size) const override;

	bool do_equals(const ARBlockImplBase &rhs) const noexcept override;

	void do_swap(ARBlockImplBase &rhs) noexcept override;

	/**
	 * \brief Constructor used by do_create()
	 *
	 * \param[in] size Size of the instance
	 */
	Impl(const ARBlockImplBase::size_type size);

	/**
	 * \brief The AccurateRip identifier preceding this block
	 */
	ARId ar_id_;
};


ARBlock::Impl::Impl(const ARBlockImplBase::size_type size)
	: ARBlockImplBase(size)
	, ar_id_ { EmptyARId }
{
	// empty
}


ARBlock::Impl::Impl(const ARId &id)
	: ARBlockImplBase(static_cast<ARBlockImplBase::size_type>(id.track_count()))
	, ar_id_ { id }
{
	// empty
}


ARBlock::Impl::Impl(ARId &&id, std::initializer_list<ARTriplet> triplets)
	: ARBlockImplBase(triplets)
	, ar_id_ { std::move(id) }
{
	// empty
}


ARBlock::Impl::Impl(const Impl &rhs) = default;


ARBlock::Impl::Impl(Impl &&rhs) noexcept = default;


const ARId& ARBlock::Impl::id() const
{
	return ar_id_;
}


std::unique_ptr<ARBlockImplBase> ARBlock::Impl::do_clone() const
{
	auto cloned_base { this->clone_base() };

	auto cloned_impl { dynamic_cast<ARBlock::Impl*>(cloned_base.get()) };
	if (!cloned_impl)
	{
		throw std::runtime_error("Copy operation on ARBlock failed");
	}
	cloned_impl->ar_id_ = this->ar_id_;

	return cloned_base;
}


std::unique_ptr<ARBlockImplBase> ARBlock::Impl::do_create(
		const ARBlockImplBase::size_type size) const
{
	std::unique_ptr<ARBlock::Impl> ptr;
	ptr.reset(new ARBlock::Impl(size));
	return ptr;
}


bool ARBlock::Impl::do_equals(const ARBlockImplBase &rhs) const noexcept
{
	auto downcasted { dynamic_cast<const ARBlock::Impl*>(&rhs) };

	return downcasted != nullptr && this->ar_id_ == downcasted->ar_id_;
}


void ARBlock::Impl::do_swap(ARBlockImplBase &rhs) noexcept
{
	auto downcasted { dynamic_cast<ARBlock::Impl*>(&rhs) };

	using std::swap;

	swap(this->ar_id_, downcasted->ar_id_);
}


// ARBlock


ARBlock::ARBlock(const ARId &id)
	: impl_ { std::make_unique<ARBlock::Impl>(id) }
{
	// empty
}


ARBlock::ARBlock(ARId &&id, std::initializer_list<ARTriplet> triplets)
	: impl_ { std::make_unique<ARBlock::Impl>(std::move(id), triplets) }
{
	// empty
}


ARBlock::ARBlock(std::unique_ptr<ARBlock::Impl> impl) noexcept
	: impl_ { std::move(impl) }
{
	// empty
}


ARBlock::ARBlock(const ARBlock &rhs)
	: impl_ { std::make_unique<ARBlock::Impl>(*rhs.impl_) }
{
	// empty
}


ARBlock::ARBlock(ARBlock &&rhs) noexcept = default;


ARBlock::~ARBlock() noexcept = default;


const ARId& ARBlock::id() const noexcept
{
	return impl_->id();
}


const ARTriplet& ARBlock::at(const ARBlock::size_type index) const
{
	return impl_->at(index);
}


ARBlock::size_type ARBlock::size() const noexcept
{
	return impl_->size();
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


const ARTriplet& ARBlock::operator [](const ARBlock::size_type index) const
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


bool operator == (const ARBlock &lhs, const ARBlock &rhs) noexcept
{
	return lhs.impl_->equals(*rhs.impl_);
}


void swap(const ARBlock &lhs, const ARBlock &rhs)
{
	lhs.impl_->swap(*rhs.impl_);
}


using ARResponseImplBase = details::AppendableSequence<ARBlock>;

/**
 * \brief Private implementation of ARResponse.
 *
 * \see ARResponse
 */
class ARResponse::Impl final : public ARResponseImplBase
{
public: /* member functions */

	/**
	 * \brief Constructor.
	 *
	 * At the start of the parsing process we do not know the number of blocks
	 * to be encountered, hence ARResponse must be constructible without size.
	 */
	Impl();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] blocks ARBlocks in response
	 */
	Impl(std::initializer_list<ARBlock> blocks);

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs Right hand side for copy
	 *
	 * \return Deep copy of \c rhs
	 */
	Impl(const Impl &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Right hand side for move
	 *
	 * \return Instance moved from \c rhs
	 */
	Impl(Impl &&rhs) noexcept;

	/**
	 * \brief Implements ARResponse::tracks_per_block() const
	 *
	 * \return Number of tracks per block
	 */
	int tracks_per_block() const noexcept;


private:

	std::unique_ptr<ARResponseImplBase> do_clone() const override;

	std::unique_ptr<ARResponseImplBase> do_create(
			const ARResponseImplBase::size_type size) const override;

	/**
	 * \brief Constructor used by do_create()
	 *
	 * \param[in] size Size of the instance
	 */
	Impl(const ARResponseImplBase::size_type size);
};


ARResponse::Impl::Impl()
	: ARResponseImplBase {}
{
	// empty
}


ARResponse::Impl::Impl(const ARResponseImplBase::size_type size)
	: ARResponseImplBase(size)
{
	// empty
}


ARResponse::Impl::Impl(std::initializer_list<ARBlock> blocks)
	: ARResponseImplBase(blocks)
{
	// empty
}


ARResponse::Impl::Impl(const Impl &rhs) = default;


ARResponse::Impl::Impl(Impl &&rhs) noexcept = default;


int ARResponse::Impl::tracks_per_block() const noexcept
{
	return static_cast<int>(this->size() ? this->operator[](0).size() : 0);
}


std::unique_ptr<ARResponseImplBase> ARResponse::Impl::do_clone() const
{
	return this->clone_base();
}


std::unique_ptr<ARResponseImplBase> ARResponse::Impl::do_create(
		const ARResponseImplBase::size_type size) const
{
	std::unique_ptr<ARResponse::Impl> ptr;
	ptr.reset(new ARResponse::Impl(size));
	return ptr;
}


// ARResponse


ARResponse::ARResponse()
	: impl_ { std::make_unique<ARResponse::Impl>() }
{
	// empty
}


ARResponse::ARResponse(std::initializer_list<ARBlock> blocks)
	: impl_ { std::make_unique<ARResponse::Impl>(blocks) }
{
	// empty
}


ARResponse::ARResponse(const ARResponse &rhs)
	: impl_ { std::make_unique<ARResponse::Impl>(*rhs.impl_) }
{
	// empty
}


ARResponse::ARResponse(ARResponse &&rhs) noexcept = default;


ARResponse::~ARResponse() noexcept = default;


int ARResponse::tracks_per_block() const noexcept
{
	return impl_->tracks_per_block();
}


ARResponse::size_type ARResponse::size() const noexcept
{
	return impl_->size();
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


const ARBlock& ARResponse::at(const ARResponse::size_type index) const
{
	return impl_->at(index);
}


const ARBlock& ARResponse::operator [](const ARResponse::size_type index) const
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


void ARResponse::reimplement(std::unique_ptr<ARResponse::Impl> impl) noexcept
{
	impl_ = std::move(impl);
}


bool operator == (const ARResponse &lhs, const ARResponse &rhs) noexcept
{
	return lhs.impl_->equals(*rhs.impl_);
}


void swap(const ARResponse &lhs, const ARResponse &rhs)
{
	lhs.impl_->swap(*rhs.impl_);
}


// ContentHandler


ContentHandler::~ContentHandler() noexcept = default;


void ContentHandler::start_input()
{
	this->do_start_input();
}


void ContentHandler::start_block()
{
	this->do_start_block();
}


void ContentHandler::id(const uint8_t track_count,
		const uint32_t id1,
		const uint32_t id2,
		const uint32_t cddb_id)
{
	this->do_id(track_count, id1, id2, cddb_id);
}


void ContentHandler::triplet(const uint32_t arcs,
		const uint8_t confidence,
		const uint32_t frame450_arcs)
{
	this->do_triplet(arcs, confidence, frame450_arcs);
}


void ContentHandler::triplet(const uint32_t arcs,
		const uint8_t confidence,
		const uint32_t frame450_arcs,
		const bool arcs_valid,
		const bool confidence_valid,
		const bool frame450_arcs_valid)
{
	this->do_triplet(arcs, confidence, frame450_arcs, arcs_valid,
			confidence_valid, frame450_arcs_valid);
}


void ContentHandler::end_block()
{
	this->do_end_block();
}


void ContentHandler::end_input()
{
	this->do_end_input();
}


/**
 * \brief Private implementation of DefaultContentHandler.
 *
 * \see DefaultContentHandler
 */
class DefaultContentHandler::Impl final
{

public:

	/**
	 * \brief Default constructor.
	 */
	Impl();

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	Impl(Impl &&rhs) noexcept;

	/**
	 * \brief Default destructor
	 */
	~Impl() noexcept;

	/**
	 * \brief Implements DefaultContentHandler::set_object()
	 *
	 * \param[in] object The object to construct from the parsed content
	 */
	void set_object(ARResponse &object);

	/**
	 * \brief Implements DefaultContentHandler::object()
	 *
	 * \return The object to construct from the parsed content
	 */
	const ARResponse& object() const;

	/**
	 * \brief Implements DefaultContentHandler::start_input()
	 */
	void start_input();

	/**
	 * \brief Implements DefaultContentHandler::start_block()
	 */
	void start_block();

	/**
	 * \brief Implements DefaultContentHandler::id()
	 */
	void id(const uint8_t track_count,
			const uint32_t id1,
			const uint32_t id2,
			const uint32_t cddb_id);

	/**
	 * \brief Implements DefaultContentHandler::triplet(const uint32_t arcs, const uint8_t confidence, const uint32_t frame450_arcs)
	 */
	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * \brief Implements DefaultContentHandler::triplet(const uint32_t arcs, const uint8_t confidence, const uint32_t frame450_arcs, const bool arcs_valid, const bool confidence_valid, const bool frame450_arcs_valid)
	 */
	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs,
			const bool arcs_valid,
			const bool confidence_valid,
			const bool frame450_arcs_valid);

	/**
	 * \brief Implements DefaultContentHandler::end_block()
	 */
	void end_block();

	/**
	 * \brief Implements DefaultContentHandler::end_input().
	 */
	void end_input();

	/**
	 * \brief Move assignment operator.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The resulting left hand side after the assigment
	 */
	Impl& operator = (Impl &&rhs) noexcept;

	// Non-copyable class
	Impl(const Impl &rhs) = delete;
	Impl& operator = (const Impl &rhs) = delete;


private:

	/**
	 * \brief Internal representation of the current block.
	 */
	std::unique_ptr<ARBlock::Impl> current_block_;

	/**
	 * \brief Internal representation of previously parsed blocks.
	 */
	std::unique_ptr<ARResponse::Impl> blocks_;

	/**
	 * \brief Non-owning pointer to the instance aggregating the parsed blocks.
	 */
	ARResponse *response_;
};


DefaultContentHandler::Impl::Impl()
	: current_block_ { nullptr }
	, blocks_        { std::make_unique<ARResponse::Impl>() }
	, response_      { nullptr }
{
	// empty
}


DefaultContentHandler::Impl::Impl(DefaultContentHandler::Impl &&rhs) noexcept
= default;


DefaultContentHandler::Impl::~Impl() noexcept = default;


void DefaultContentHandler::Impl::set_object(ARResponse &object)
{
	response_ = &object;
}


const ARResponse& DefaultContentHandler::Impl::object() const
{
	return *response_;
}


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
		std::make_unique<ARBlock::Impl>(ARId{ track_count, id1, id2, cddb_id });
}


void DefaultContentHandler::Impl::triplet(const uint32_t arcs,
		const uint8_t confidence,
		const uint32_t frame450_arcs)
{
	current_block_->append(ARTriplet{ arcs, confidence, frame450_arcs });
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
		current_block_->append(ARTriplet{ arcs, confidence, frame450_arcs });
	} else
	{
		current_block_->append(ARTriplet{ arcs, confidence,
					frame450_arcs, arcs_valid, confidence_valid,
					frame450_arcs_valid });
	}
}


void DefaultContentHandler::Impl::end_block()
{
	blocks_->append(ARBlock{ std::move(current_block_) });

	current_block_.reset();
}


void DefaultContentHandler::Impl::end_input()
{
	response_->reimplement(std::move(blocks_));

	blocks_.reset();
}


DefaultContentHandler::Impl& DefaultContentHandler::Impl::operator = (
		DefaultContentHandler::Impl &&rhs) noexcept
= default;


// DefaultContentHandler


DefaultContentHandler::DefaultContentHandler()
	: impl_ { std::make_unique<DefaultContentHandler::Impl>() }
{
	// empty
}


DefaultContentHandler::DefaultContentHandler(DefaultContentHandler &&rhs)
	noexcept
= default;


DefaultContentHandler::~DefaultContentHandler() noexcept = default;


void DefaultContentHandler::set_object(ARResponse &object)
{
	impl_->set_object(object);
}


const ARResponse& DefaultContentHandler::object() const
{
	return impl_->object();
}


DefaultContentHandler& DefaultContentHandler::operator = (
		DefaultContentHandler &&rhs) noexcept
= default;


void DefaultContentHandler::do_start_input()
{
	impl_->start_input();
}


void DefaultContentHandler::do_start_block()
{
	impl_->start_block();
}


void DefaultContentHandler::do_id(const uint8_t track_count,
		const uint32_t id1,
		const uint32_t id2,
		const uint32_t cddb_id)
{
	impl_->id(track_count, id1, id2, cddb_id);
}


void DefaultContentHandler::do_triplet(const uint32_t arcs,
		const uint8_t confidence,
		const uint32_t frame450_arcs)
{
	impl_->triplet(arcs, confidence, frame450_arcs);
}


void DefaultContentHandler::do_triplet(const uint32_t arcs,
		const uint8_t confidence,
		const uint32_t frame450_arcs,
		const bool arcs_valid,
		const bool confidence_valid,
		const bool frame450_arcs_valid)
{
	impl_->triplet(arcs, confidence, frame450_arcs, arcs_valid,
			confidence_valid, frame450_arcs_valid);
}


void DefaultContentHandler::do_end_block()
{
	impl_->end_block();
}


void DefaultContentHandler::do_end_input()
{
	impl_->end_input();
}


// ErrorHandler


ErrorHandler::~ErrorHandler() noexcept = default;


void ErrorHandler::error(const uint32_t byte_pos, const uint32_t block,
			const uint32_t block_byte_pos)
{
	this->do_error(byte_pos, block, block_byte_pos);
}


// DefaultErrorHandler


DefaultErrorHandler::DefaultErrorHandler() = default;


DefaultErrorHandler::DefaultErrorHandler(DefaultErrorHandler &&rhs) noexcept
= default;


DefaultErrorHandler& DefaultErrorHandler::operator = (
		DefaultErrorHandler &&rhs) noexcept
= default;


void DefaultErrorHandler::do_error(const uint32_t byte_pos,
		const uint32_t block, const uint32_t block_byte_pos)
{
	static const auto SIZE_OF_CHAR = sizeof(char);

	constexpr int BHB { 13 }; // number of block header bytes
	constexpr int BPT {  9 }; // number of bytes in a triplet

	auto cause = std::stringstream {};

	if (byte_pos > block_byte_pos) // This actually must be the case
	{
		const auto bytes_per_block {
			(byte_pos - block_byte_pos) / (block - 1) };

		cause << "Current block ended after "     << block_byte_pos
			<< " bytes but was expected to have " << bytes_per_block
			<< " bytes ("
			<< (bytes_per_block - block_byte_pos) << " bytes missing).";
	} else
	{
		cause << "Unknown.";
	}

	auto logical_pos = std::stringstream {};
	auto logical_err = std::stringstream {};

	logical_pos << "Block " << block;

	if (block_byte_pos == 0) // error at block start (expected more blocks?)
	{
		logical_pos << ", begin of block " << block;
	}
	else if (block_byte_pos > BHB * SIZE_OF_CHAR) // error in track information
	{
		const auto triplet_byte_pos { block_byte_pos - BHB * SIZE_OF_CHAR };

		logical_pos << ", track "
			<< (triplet_byte_pos / BPT * SIZE_OF_CHAR + 1);

		const auto track_byte_pos { triplet_byte_pos % (BPT * SIZE_OF_CHAR) };

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

		if (block_byte_pos < 1 * SIZE_OF_CHAR)
		{
			logical_pos << "track count expected but missing";
			logical_err << "Expected header but none present";
		} else
		if (block_byte_pos == 1 * SIZE_OF_CHAR) // track count ok
		{
			logical_pos << "disc id 1 expected but missing";
			logical_err << "Missing Disc id 1";
		} else
		if (block_byte_pos < 5 * SIZE_OF_CHAR) // Disc Id 1 incomplete
		{
			logical_pos << "disc id 1"
					<< " is only " << (block_byte_pos - 1)
					<< " bytes wide instead of 4";
			logical_err << "Disc id 1 incomplete";
		} else
		if (block_byte_pos == 5 * SIZE_OF_CHAR) // Disc Id 2 missing
		{
			logical_pos << "disc id 2 expected but missing";
			logical_err << "Missing Disc id 2";
		} else
		if (block_byte_pos < 9 * SIZE_OF_CHAR) // Disc Id 2 incomplete
		{
			logical_pos << "disc id 2"
					<< " is only " << (block_byte_pos - 5)
					<< " bytes wide instead of 4";
			logical_err << "Disc id 2 incomplete";
		} else
		if (block_byte_pos == 9 * SIZE_OF_CHAR) // CDDB Id missing
		{
			logical_pos << "cddb id expected but missing";
			logical_err << "Missing CDDB id";
		} else
		if (block_byte_pos < BHB * SIZE_OF_CHAR) // CDDB Id incomplete
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

	auto message = std::stringstream {};
	message << "Read error after byte " << byte_pos
		<< ", Logical error position: " << logical_pos.str()
		<< ", Problem: " << logical_err.str()
		<< ", Cause: " << cause.str();
	ARCS_LOG_ERROR << message.str();

	throw StreamReadException(byte_pos, block, block_byte_pos, message.str());
}


// StreamReadException


StreamReadException::StreamReadException(const uint32_t byte_pos,
		const uint32_t block,
		const uint32_t block_byte_pos,
		const std::string &what_arg)
	: std::runtime_error { what_arg }
	, byte_pos_ { byte_pos }
	, block_ { block }
	, block_byte_pos_ { block_byte_pos }
{
	// empty
}


StreamReadException::StreamReadException(const uint32_t byte_pos,
		const uint32_t block,
		const uint32_t block_byte_pos,
		const char *what_arg)
	: std::runtime_error { what_arg }
	, byte_pos_ { byte_pos }
	, block_ { block }
	, block_byte_pos_ { block_byte_pos }
{
	// empty
}


uint32_t StreamReadException::byte_position() const noexcept
{
	return byte_pos_;
}


uint32_t StreamReadException::block() const noexcept
{
	return block_;
}


uint32_t StreamReadException::block_byte_position() const noexcept
{
	return block_byte_pos_;
}


/**
 * \brief Private implementation of ARStreamParser.
 *
 * \see ARStreamParser
 */
class ARStreamParser::Impl final
{

public:

	/**
	 * \brief Size in bytes of the block header containing disc_id_1, disc_id_2
	 * and cddb_id.
	 */
	static constexpr int BLOCK_HEADER_BYTES { 13 };

	/**
	 * \brief Size in bytes of a triplet containing the ARCS for a single track,
	 * the confidence of this ARCS, and the ARCS of frame 450 of this track.
	 */
	static constexpr int TRIPLET_BYTES      {  9 };

	/**
	 * \brief Default constructor.
	 */
	explicit Impl(const ARStreamParser *parser);

	/**
	 * \brief Default destructor.
	 */
	~Impl() noexcept = default;

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
	 * \brief Parses the byte stream of an AccurateRip response.
	 *
	 * \param[in] in_stream The stream to be parsed
	 *
	 * \return Number of bytes parsed from \c in_stream
	 *
	 * \throw StreamReadException If reading of the stream fails
	 */
	uint32_t parse_stream(std::istream &in_stream);

	/**
	 * \brief Move assignment
	 *
	 * \param[in] rhs The instance to assign
	 */
	Impl& operator = (Impl &&rhs) noexcept;

	/**
	 * \brief Swap
	 *
	 * \param[in] rhs Right hand side to swap
	 */
	void swap(Impl &rhs);


	// non-copyable class
	Impl(const Impl &rhs) = delete;
	Impl& operator = (const Impl &rhs) = delete;


private:

	/**
	 * \brief React on parse errors.
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
	 * \brief Worker method for parsing an input stream.
	 *
	 * \param[in] in_stream The stream to be parsed
	 *
	 * \throw StreamReadException If reading of the stream fails
	 *
	 * \return Number of parsed bytes
	 *
	 * \todo This implementation silently relies on a little endian plattform.
	 */
	uint32_t parse_stream_worker(std::istream &in_stream);

	/**
	 * \brief Service method: Interpret 4 bytes as a 32 bit unsigned integer
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
	 * \brief Internal event handler instance.
	 */
	std::unique_ptr<ContentHandler> content_handler_;

	/**
	 * \brief Internal error handler instance.
	 */
	std::unique_ptr<ErrorHandler> error_handler_;

	/**
	 * \brief Back pointer to ARStreamParser instance.
	 */
	const ARStreamParser *parser_;
};


// ARStreamParser::Impl


ARStreamParser::Impl::Impl(const ARStreamParser *parser)
	: content_handler_ { nullptr }
	, error_handler_ { nullptr }
	, parser_ { parser }
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


uint32_t ARStreamParser::Impl::parse_stream(std::istream &in_stream)
{
	auto bytes = uint32_t { 0 };
	try
	{
		bytes = this->parse_stream_worker(in_stream);
	}
	catch (const StreamReadException& sre)
	{
		parser_->on_catched_exception(in_stream, sre);

		throw;
	}
	catch (const std::exception& e)
	{
		parser_->on_catched_exception(in_stream, e);

		throw std::runtime_error(
			std::string("Failed to parse input stream, got ") +
			typeid(e).name() + std::string(", message: ") + e.what());
	}

	return bytes;
}


void ARStreamParser::Impl::on_error(const uint32_t byte_pos,
		const uint32_t block, const uint32_t block_byte_pos)
{
	if (error_handler_)
	{
		error_handler_->error(byte_pos, block, block_byte_pos);
	} else
	{
		auto ss = std::stringstream {};
		ss << "Error on input byte " << byte_pos << " (block " << block
			<< ", byte " << block_byte_pos << ")";

		throw StreamReadException(byte_pos, block, block_byte_pos, ss.str());
	}
}


uint32_t ARStreamParser::Impl::parse_stream_worker(std::istream &in)
{
	if (not content_handler_)
	{
		ARCS_LOG_WARNING
			<< "Parser has no content handler attached, skip parsing";
		return 0;
	}

	std::vector<char> id(BLOCK_HEADER_BYTES * sizeof(char));
	std::vector<char> triplet(TRIPLET_BYTES * sizeof(char));

	auto track_count = TrackNo { 0 };
	auto discId1 = uint32_t { 0 };
	auto discId2 = uint32_t { 0 };
	auto cddbId = uint32_t { 0 };
	auto confidence = unsigned { 0 };
	auto trk_arcs = uint32_t { 0 };
	auto frame450_arcs = uint32_t { 0 };

	auto bytes_read = unsigned { 0 };

	auto byte_counter = unsigned { 0 };
	auto block_counter = unsigned { 0 };
	auto block_byte_counter = unsigned { 0 };

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

		for (auto trk = uint8_t { 0 }; trk < track_count; ++trk)
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
		ARStreamParser::Impl &&rhs) noexcept
{
	this->swap(rhs);
	return *this;
}


void ARStreamParser::Impl::swap(ARStreamParser::Impl &rhs)
{
	using std::swap;

	swap(this->content_handler_, rhs.content_handler_);
	swap(this->error_handler_,   rhs.error_handler_);
	// Leave parser_ untouched!
}


// ARStreamParser


ARStreamParser::ARStreamParser()
	: impl_ { std::make_unique<ARStreamParser::Impl>(this) }
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


uint32_t ARStreamParser::parse()
{
	return this->do_parse();
}


uint32_t ARStreamParser::parse_stream(std::istream &in_stream)
{
	return impl_->parse_stream(in_stream);
}


void ARStreamParser::swap(ARStreamParser &rhs)
{
	if (typeid(*this) != typeid(rhs))
	{
		throw std::domain_error("Refuse to swap instances of different types");
	}

	this->impl_->swap(*rhs.impl_);
	this->do_swap(rhs);
}


void ARStreamParser::do_swap(ARStreamParser &/* rhs */)
{
	// empty
}


// ARFileParser


ARFileParser::ARFileParser()
	: filename_ {}
{
	// empty
}


ARFileParser::ARFileParser(ARFileParser &&rhs) noexcept
	: filename_ { /* empty */ }
{
	this->swap(rhs);
}


ARFileParser::ARFileParser(const std::string &filename)
	: filename_ { filename }
{
	// empty
}


void ARFileParser::set_file(const std::string &filename)
{
	filename_ = filename;
}


std::string ARFileParser::file() const noexcept
{
	return filename_;
}


uint32_t ARFileParser::do_parse()
{
	ARCS_LOG_DEBUG << "Open file: " << this->file();

	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		file.open(this->file(), std::ifstream::in | std::ifstream::binary);
	}
	catch (const std::ifstream::failure& f)
	{
		throw std::runtime_error(
			std::string("Failed to open file '") + this->file() +
			std::string("', got: ") + typeid(f).name() +
			std::string(", message: ") + f.what());
	}

	return ARStreamParser::parse_stream(file);
}


void ARFileParser::on_catched_exception(std::istream &istream,
		const std::exception & /* e */) const
{
	auto *filestream { dynamic_cast<std::ifstream*>(&istream) };

	if (filestream)
	{
		filestream->close();
		return;
	}

	ARCS_LOG_WARNING << "Could not close filestream";
}


ARFileParser& ARFileParser::operator = (ARFileParser &&rhs) noexcept
{
	this->swap(rhs);
	return *this;
}


void ARFileParser::do_swap(ARStreamParser &rhs)
{
	auto casted_rhs { dynamic_cast<ARFileParser*>(&rhs) };

	if (!casted_rhs)
	{
		throw std::domain_error("Type mismatch detected while swapping");
	}

	using std::swap;

	swap(this->filename_, casted_rhs->filename_);
}


// ARStdinParser


ARStdinParser::ARStdinParser() = default;


ARStdinParser::ARStdinParser(ARStdinParser &&rhs) noexcept
{
	this->swap(rhs);
}


uint32_t ARStdinParser::do_parse()
{
	auto response_data { StdIn(1024).bytes() };

	VectorIStream<char> response_data_w(response_data);
	std::istream stream(&response_data_w);

	return ARStreamParser::parse_stream(stream);
}


ARStdinParser& ARStdinParser::operator = (ARStdinParser &&rhs) noexcept
{
	this->swap(rhs);
	return *this;
}


void ARStdinParser::on_catched_exception(std::istream & /* istream */,
			const std::exception &/* e */) const
{
	// empty
}

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

