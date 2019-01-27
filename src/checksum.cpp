#ifndef __LIBARCS_CHECKSUM_HPP__
#include "checksum.hpp"
#endif

#include <cmath>          // for log2
#include <iomanip>        // for uppercase, nouppercase, setw, setfill
                          // showbase, noshowbase
#include <sstream>        // for stringstream


namespace arcs
{
/// \cond IMPL_ONLY
/// \internal \addtogroup calcImpl
/// @{
inline namespace v_1_0_0
{


// Checksum


Checksum::Checksum()
	: value_(0)
{
	// empty
}


Checksum::Checksum(const uint32_t &value)
	: value_(value)
{
	// empty
}


uint32_t Checksum::value() const
{
	return value_;
}


bool Checksum::empty() const
{
	return 0 == value_;
}


bool Checksum::operator == (const Checksum &rhs) const
{
	return (value_ == rhs.value_);
}


bool Checksum::operator != (const Checksum &rhs) const
{
	return not (*this == rhs);
}


//Checksum& Checksum::operator = (const Checksum &rhs)
//= default;


//Checksum& Checksum::operator = (Checksum &&rhs) noexcept
//= default;


// ChecksumSet


ChecksumSet::ChecksumSet()
	: length_(0)
{
	// empty
}


ChecksumSet::ChecksumSet(const uint32_t length)
	: length_(length)
{
	// empty
}


uint32_t ChecksumSet::length() const
{
	return length_;
}


namespace checksum
{

/**
 * Implementation details of namespace checksum
 */
namespace details
{

	/**
	 * Return the numeric value of a >=C++11 enum class value
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


std::string to_hex_str(const Checksum &checksum, bool upper, bool base)
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
	return names.at(static_cast<int>(std::log2(details::as_integral_value(t))));
}


} // namespace checksum

} // namespace v_1_0_0

/// @}
/// \endcond
// IMPL_ONLY

} // namespace arcs

