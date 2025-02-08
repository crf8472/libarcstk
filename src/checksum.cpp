/**
 * \file
 *
 * \brief Implementation of the checksum calculation API
 */

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"
#endif
#ifndef __LIBARCSTK_CHECKSUM_DETAILS_HPP__
#include "checksum_details.hpp"
#endif

#include <algorithm>        // for transform
#include <array>            // for array
#include <cmath>            // for log2
#include <cstdint>          // for int32_t
#include <initializer_list> // for initializer_list
#include <iomanip>          // for setfill, setw
#include <iterator>         // for begin, end, inserter
#include <set>              // for set
#include <sstream>          // for ostringstream
#include <stdexcept>        // for domain_error
#include <string>           // for string
#include <type_traits>      // for underlying_type
#include <utility>          // for pair, swap


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


bool operator == (const Checksum& lhs, const Checksum& rhs) noexcept
{
	return lhs.value() == rhs.value();
}


void swap(Checksum& lhs, Checksum& rhs) noexcept
{
	using std::swap;
	swap(lhs.value_, rhs.value_);
}


//


std::string to_string(const Checksum& c)
{
	auto stream = std::ostringstream {};
	stream << c;
	return stream.str();
}


std::ostream& operator << (std::ostream& out, const Checksum& c)
{
	auto prev_settings = std::ios_base::fmtflags { out.flags() };

	checksum::details::print_hex(c, true, false, out);

	out.flags(prev_settings);
	return out;
}


//


const Checksum EmptyChecksum { 0 }; // defines emptyness for Checksum

const ChecksumSet EmptyChecksumSet { ChecksumSet{/* empty */} };

const Checksums EmptyChecksums { Checksums{/* empty */} };


namespace checksum
{

/// \internal \addtogroup calc
/// @{

/**
 * \internal
 * \brief Implementation details of namespace checksum.
 */
namespace details
{

/**
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

/**
 * \brief Worker for printing a Checksum to an output stream.
 *
 * \param[in] checksum	Checksum to print
 * \param[in] upper		Flag: iff TRUE, print letters uppercase
 * \param[in] base		Flag: iff TRUE, print base 0x
 * \param[in] out		Output stream to print to
 */
void print_hex(const Checksum& checksum, const bool upper,
		const bool base, std::ostream& out)
{
	out << std::hex
		<< (base  ? std::showbase  : std::noshowbase  )
		<< (upper ? std::uppercase : std::nouppercase )
		<< std::setw(8) << std::setfill('0')
		<< checksum.value();
}

} // namespace checksum::details

/** @} */

/**
 * \brief Return the name of a checksum::type.
 *
 * \param[in] t Checksum type to get the name for
 *
 * \return Name of checksum::type \c t
 */
std::string type_name(const type t)
{
	return details::names.at(std::log2(
		static_cast<typename std::underlying_type<checksum::type>::type>(t)));
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


bool ChecksumSet::contains(const checksum::type& type) const
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
		[](const storage_type::value_type& pair)
		{
			return pair.first;
		}
	);

	return keys;
}


std::pair<ChecksumSet::iterator, bool> ChecksumSet::insert(
		const checksum::type type, const Checksum& checksum)
{
	return set_.insert({ type, checksum });
}


void ChecksumSet::merge(ChecksumSet& rhs)
{
	if (this->length() != 0 && rhs.length() != 0)
	{
		// Non-zero lengths with different value indicates different tracks.
		if (this->length() != rhs.length())
		{
			throw std::domain_error(
					"Refuse to merge checksums of different track");
		}

		// Sets with zero length may be merged without constraint
	}

	#if __cplusplus < 201703L
		// pre-C++17 implementation of merge()
		using std::begin;
		using std::end;
		set_.insert(begin(rhs.set_), end(rhs.set_));
	#else
		set_.merge(rhs.set_);
	#endif
}


void ChecksumSet::erase(const checksum::type& type)
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


bool operator == (const ChecksumSet& lhs, const ChecksumSet& rhs) noexcept
{
	return lhs.length_ == rhs.length_ and lhs.set_ == rhs.set_;
}


void swap(ChecksumSet& lhs, ChecksumSet& rhs) noexcept
{
	using std::swap;
	swap(lhs.length_, rhs.length_);
	swap(lhs.set_,    rhs.set_);
}

} // namespace v_1_0_0
} // namespace arcstk

