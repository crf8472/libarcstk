/**
 * \file
 *
 * \brief Implementation of the checksum calculation API
 */

#include <algorithm>  // for transform
#include <array>      // for array
#include <cmath>      // for log2
#include <iomanip>    // for setfill, setw
#include <unordered_map> // for unordered_map

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

// Checksum


Checksum::Checksum()
	: value_ { EmptyChecksum.value() }
{
	// empty
}


Checksum::Checksum(const Checksum::value_type value)
	: value_ { value }
{
	// empty
}


Checksum::value_type Checksum::value() const noexcept
{
	return value_;
}


bool Checksum::empty() const noexcept
{
	return value_ == EmptyChecksum.value_;
}


Checksum::operator bool() const noexcept
{
	return !empty();
}


Checksum& Checksum::operator = (const Checksum::value_type rhs)
{
	value_ = rhs;
	return *this;
}


bool operator == (const Checksum &lhs, const Checksum &rhs) noexcept
{
	return lhs.value() == rhs.value();
}


void swap(Checksum& lhs, Checksum& rhs) noexcept
{
	using std::swap;
	swap(lhs.value_, rhs.value_);
}


//


namespace checksum
{

/// \internal \addtogroup calcImpl
/// @{

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
 * \brief Creates a hexadecimal string representation of a 32bit checksum.
 *
 * \param[in] checksum The Checksum to represent
 * \param[in] upper    TRUE indicates to print digits A-F in uppercase
 * \param[in] base     TRUE indicates to print base '0x'
 * \param[in] out      Stream to print to
 *
 * \return A hexadecimal representation of the \c checksum as a string
 */
void print_hex(const Checksum& checksum, const bool upper,
		const bool base, std::ostream& out);

} // namespace checksum::details

/** @} */    /* group calcImpl ends here */

} // namespace checksum


std::ostream& operator << (std::ostream& out, const Checksum &c)
{
	auto prev_settings = std::ios_base::fmtflags { out.flags() };

	checksum::details::print_hex(c, true, false, out);
	//out << std::hex << std::uppercase << std::setw(8) << std::setfill('0')
	//	<< c.value();

	out.flags(prev_settings);
	return out;
}


std::string to_string(const Checksum& c)
{
	auto stream = std::ostringstream {};
	stream << c;
	return stream.str();
}


//


const Checksum EmptyChecksum = 0; // defines emptyness for Checksum


namespace checksum
{
/// \internal \addtogroup calcImpl
/// @{
namespace details
{

/**
 * \internal
 *
 * \brief Checksum type names.
 *
 * The order of names in this aggregate must match the order of types in
 * enum class checksum::type, otherwise function type_name() will fail.
 */
static const std::array<std::string, 2> names {
	"ARCSv1",
	"ARCSv2",
	// "THIRD_TYPE" ,
	// "FOURTH_TYPE" ...
};


void print_hex(const Checksum &checksum, const bool upper,
		const bool base, std::ostream& out)
{
	out << std::hex
		<< (base  ? std::showbase  : std::noshowbase  )
		<< (upper ? std::uppercase : std::nouppercase )
		<< std::setw(8) << std::setfill('0')
		<< checksum.value();
}

/**
 * \internal
 *
 * \brief Return the numeric value of a >=C++11 enum class value
 *
 * \return The numeric constant of an enum class value
 */
template <typename E> // TODO Why not <enum E>?
auto as_integral_value(E const value)
		-> typename std::underlying_type<E>::type
{
	return static_cast<typename std::underlying_type<E>::type>(value);
}


} // namespace checksum::details

/** @} */    /* group calcImpl ends here */

std::string type_name(const type t)
{
	using details::names;

	return names.at(std::log2(details::as_integral_value(t)));
}

} // namespace checksum


// ChecksumSet


ChecksumSet::ChecksumSet()
	: ChecksumSet { 0 }
{
	// empty
}


ChecksumSet::ChecksumSet(const int32_t length)
	: ChecksumSet { length, { /* empty */ } }
{
	// empty
}


ChecksumSet::ChecksumSet(const int32_t length,
		std::initializer_list<
			std::pair<const ChecksumSet::key_type,
			                ChecksumSet::value_type>> checksums)
	: set_     ( checksums )
	, length_  { length    }
{
	// empty
}


int32_t ChecksumSet::length() const noexcept
{
	return length_;
}


void ChecksumSet::set_length(const int32_t l) noexcept
{
	length_ = l;
}


ChecksumSet::size_type ChecksumSet::size() const noexcept
{
	return set_.size();
}


bool ChecksumSet::empty() const noexcept
{
	return set_.empty();
}


bool ChecksumSet::contains(const checksum::type &type) const
{
	using std::end;
	return set_.find(type) != end(set_);
}


Checksum ChecksumSet::get(const checksum::type type) const
{
	const auto rc { set_.find(type) };

	using std::end;

	if (rc == end(set_))
	{
		return EmptyChecksum;
	}

	return rc->second;
}


std::set<checksum::type> ChecksumSet::types() const
{
	auto keys { std::set<checksum::type>{} };

	using std::begin;
	using std::end;

	std::transform(begin(set_), end(set_),
		std::inserter(keys, begin(keys)),
		[](const storage_type::value_type &pair)
		{
			return pair.first;
		}
	);

	return keys;
}


std::pair<ChecksumSet::iterator, bool> ChecksumSet::insert(
		const checksum::type type, const Checksum &checksum)
{
	return set_.insert({ type, checksum });
}


void ChecksumSet::merge(const ChecksumSet& rhs)
{
	if (this->length() != 0 and rhs.length() != 0)
	{
		// Non-zero lengths with different value indicates different tracks.
		if (this->length() != rhs.length())
		{
			throw std::domain_error(
					"Refuse to merge checksums of different track");
		}

		// Sets with zero length may be merged without constraint
	}

	#if __cplusplus >= 201703L
		set_.merge(rhs.set_);
	#else
		using std::begin;
		using std::end;
		set_.insert(begin(rhs.set_), end(rhs.set_));
	#endif
}


void ChecksumSet::erase(const checksum::type &type)
{
	set_.erase(type);
}


void ChecksumSet::clear()
{
	set_.clear();
}


ChecksumSet::const_iterator ChecksumSet::cbegin() const
{
	return set_.cbegin();
}


ChecksumSet::const_iterator ChecksumSet::cend() const
{
	return set_.cend();
}


ChecksumSet::const_iterator ChecksumSet::begin() const
{
	return set_.begin();
}


ChecksumSet::const_iterator ChecksumSet::end() const
{
	return set_.end();
}


ChecksumSet::iterator ChecksumSet::begin()
{
	return set_.begin();
}


ChecksumSet::iterator ChecksumSet::end()
{
	return set_.end();
}


ChecksumSet::operator bool() const noexcept
{
	return !empty();
}


bool operator == (const ChecksumSet &lhs, const ChecksumSet &rhs) noexcept
{
	return lhs.length_ == rhs.length_ and lhs.set_ == rhs.set_;
}


void swap(ChecksumSet& lhs, ChecksumSet& rhs) noexcept
{
	using std::swap;
	swap(lhs.length_, rhs.length_);
	swap(lhs.set_,    rhs.set_);
}


// Checksums


// Checksums::Checksums()
// 	: sets_ { /* empty */ }
// {
// 	sets_.reserve(default_size);
// }
//
//
// Checksums::Checksums(const size_type size)
// 	: sets_ { /* empty */ }
// {
// 	sets_.reserve(size);
// }
//
//
// Checksums::Checksums(std::initializer_list<ChecksumSet> tracks)
// 	: sets_ { tracks }
// {
// 	// empty
// }
//
//
// Checksums::size_type Checksums::size() const noexcept
// {
// 	return sets_.size();
// }
//
//
// bool Checksums::empty() const noexcept
// {
// 	return sets_.empty();
// }
//
//
// const ChecksumSet& Checksums::at(const size_type i) const
// {
// 	return sets_.at(i);
// }
//
//
// const ChecksumSet& Checksums::operator[](const size_type i) const
// {
// 	return sets_[i];
// }
//
//
// void Checksums::append(const ChecksumSet& s)
// {
// 	sets_.push_back(s);
// }
//
//
// void Checksums::append(ChecksumSet&& s)
// {
// 	sets_.emplace_back(std::move(s));
// }
//
//
// Checksums::const_iterator Checksums::cbegin() const
// {
// 	return sets_.cbegin();
// }
//
//
// Checksums::const_iterator Checksums::cend() const
// {
// 	return sets_.cend();
// }
//
//
// Checksums::const_iterator Checksums::begin() const
// {
// 	return sets_.begin();
// }
//
//
// Checksums::const_iterator Checksums::end() const
// {
// 	return sets_.end();
// }
//
//
// Checksums::iterator Checksums::begin()
// {
// 	return sets_.begin();
// }
//
//
// Checksums::iterator Checksums::end()
// {
// 	return sets_.end();
// }
//
//
// bool operator == (const Checksums &lhs, const Checksums &rhs) noexcept
// {
// 	return lhs.sets_ == rhs.sets_;
// }
//
//
// void swap(Checksums& lhs, Checksums& rhs) noexcept
// {
// 	using std::swap;
// 	swap(lhs.sets_, rhs.sets_);
// }

} // namespace v_1_0_0
} // namespace arcstk

