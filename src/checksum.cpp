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


// ChecksumSet::Impl


/**
 * Private implementation of ChecksumSet.
 */
class ChecksumSet::Impl final
{

public:

	/**
	 * Constructor
	 *
	 * \param[in] length Length (in LBA frames) of this track
	 */
	explicit Impl(const uint32_t length);

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	Impl(const Impl &rhs);

	/**
	 * Length (in LBA frames) of this track.
	 *
	 * \return Length of this track in LBA frames
	 */
	uint32_t length() const;

	/**
	 * Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Impl& operator = (const Impl &rhs);


private:

	/**
	 * Internal representation of the length (in frames)
	 */
	uint32_t length_;
};


ChecksumSet::Impl::Impl(const uint32_t length)
	: length_(length)
{
	// empty
}


ChecksumSet::Impl::Impl(const ChecksumSet::Impl &rhs)
	: length_(rhs.length_)
{
	// empty
}


uint32_t ChecksumSet::Impl::length() const
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


ChecksumSet::ChecksumSet(const uint32_t length)
	: impl_(std::make_unique<ChecksumSet::Impl>(length))
{
	// empty
}


ChecksumSet::ChecksumSet(const ChecksumSet &rhs)
	: ChecksumList<checksum::type>(rhs)
	, impl_(std::make_unique<ChecksumSet::Impl>(*rhs.impl_))
{
	// empty
}


ChecksumSet::~ChecksumSet() noexcept = default;


ChecksumSet::ChecksumSet(ChecksumSet &&rhs) noexcept = default;


uint32_t ChecksumSet::length() const
{
	return impl_->length();
}


Checksum ChecksumSet::get(checksum::type type) const
{
	return *this->find(type);
}


ChecksumSet& ChecksumSet::operator = (const ChecksumSet &rhs)
{
	ChecksumList<checksum::type>::operator=(rhs);
	impl_ = std::make_unique<ChecksumSet::Impl>(rhs.length());
	return *this;
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

