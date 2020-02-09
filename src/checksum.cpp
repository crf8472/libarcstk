/**
 * \file
 *
 * \brief Implementation of the checksum calculation API
 */

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"
#endif

#include <cmath>          // for log2
#include <iomanip>        // for uppercase, nouppercase, setw, setfill
                          // showbase, noshowbase
#include <sstream>        // for stringstream

namespace arcstk
{
inline namespace v_1_0_0
{


// Checksum


Checksum::Checksum()
	: value_(0)
{
	// empty
}


Checksum::Checksum(const uint32_t value)
	: value_(value)
{
	// empty
}


uint32_t Checksum::value() const noexcept
{
	return value_;
}


bool Checksum::empty() const noexcept
{
	return 0 == value_;
}


Checksum& Checksum::operator = (const uint32_t rhs)
{
	value_ = rhs;
	return *this;
}


// Checksum operators


bool operator == (const Checksum &lhs, const Checksum &rhs) noexcept
{
	return lhs.value_ == rhs.value_;
}


bool operator != (const Checksum &lhs, const Checksum &rhs) noexcept
{
	return not(lhs == rhs);
}


/// \internal \addtogroup calcImpl
/// @{


// ChecksumSet::Impl


/**
 * \brief Private implementation of ChecksumSet.
 *
 * \see ChecksumSet
 */
class ChecksumSet::Impl final
{

public:

	/**
	 * \brief Constructor
	 *
	 * \param[in] length Length (in LBA frames) of this track
	 */
	explicit Impl(const int64_t length);

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	Impl(const Impl &rhs);

	/**
	 * \brief Length (in LBA frames) of this track.
	 *
	 * \return Length of this track in LBA frames
	 */
	int64_t length() const noexcept;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Impl& operator = (const Impl &rhs);


private:

	/**
	 * \brief Internal representation of the length (in frames)
	 */
	int64_t length_;
};

/** @} */

ChecksumSet::Impl::Impl(const int64_t length)
	: length_(length)
{
	// empty
}


ChecksumSet::Impl::Impl(const ChecksumSet::Impl &rhs)
	: length_(rhs.length_)
{
	// empty
}


int64_t ChecksumSet::Impl::length() const noexcept
{
	return length_;
}


ChecksumSet::Impl& ChecksumSet::Impl::operator = (const ChecksumSet::Impl& rhs)
= default;


// ChecksumSet


ChecksumSet::ChecksumSet()
	: impl_(std::make_unique<ChecksumSet::Impl>(0))
{
	// empty
}


ChecksumSet::ChecksumSet(const int64_t length)
	: impl_(std::make_unique<ChecksumSet::Impl>(length))
{
	// empty
}


ChecksumSet::ChecksumSet(const ChecksumSet &rhs)
	: ChecksumMap<checksum::type>(rhs)
	, impl_(std::make_unique<ChecksumSet::Impl>(*rhs.impl_))
{
	// empty
}


ChecksumSet::~ChecksumSet() noexcept = default;


ChecksumSet::ChecksumSet(ChecksumSet &&rhs) noexcept = default;


int64_t ChecksumSet::length() const noexcept
{
	return impl_->length();
}


Checksum ChecksumSet::get(const checksum::type type) const
{
	return *this->find(type);
}


std::set<checksum::type> ChecksumSet::types() const
{
	return this->keys();
}


ChecksumSet& ChecksumSet::operator = (const ChecksumSet &rhs)
{
	ChecksumMap<checksum::type>::operator=(rhs);
	impl_ = std::make_unique<ChecksumSet::Impl>(rhs.length());
	return *this;
}


namespace checksum
{

/**
 * \internal
 *
 * \brief Implementation details of namespace checksum
 */
namespace details
{

	/**
	 * \internal
	 *
	 * \brief Return the numeric value of a >=C++11 enum class value
	 *
	 * \return The numeric constant of an enum class value
	 */
	template <typename E>
	auto as_integral_value(E const value)
			-> typename std::underlying_type<E>::type
	{
		return static_cast<typename std::underlying_type<E>::type>(value);
	}

} // namespace checksum::details


std::string to_hex_str(const Checksum &checksum, const bool upper,
		const bool base)
{
	std::stringstream ss;
	ss << std::hex
		<< (base  ? std::showbase  : std::noshowbase  )
		<< (upper ? std::uppercase : std::nouppercase )
		<< std::setw(8) << std::setfill('0')
		<< checksum.value();
	return ss.str();
}


std::string type_name(type t)
{
	return names.at(std::log2(details::as_integral_value(t)));
}


} // namespace checksum

} // namespace v_1_0_0

} // namespace arcstk

