/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of the checksum calculation API
 */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"
#endif

#include <algorithm>        // for transform
#include <array>            // for array
#include <cmath>            // for log2
#include <initializer_list> // for initializer_list
#include <iomanip>			// for setfill, setw
#include <ios>              // for ios_base
#include <iterator>         // for begin, end, inserter
#include <ostream>          // for ostream
#include <set>              // for set
#include <stdexcept>        // for domain_error
#include <sstream>          // for ostringstream
#include <string>           // for string
#include <type_traits>      // for underlying_type
#include <utility>          // for pair, swap


namespace arcstk
{
inline namespace v_1_0_0
{

// Checksum


Checksum::Checksum(const Checksum::value_type value)
	: value_ { value }
{
	// empty
}


Checksum::value_type Checksum::value() const noexcept
{
	return value_;
}


bool Checksum::zero() const noexcept
{
	return value_ == 0;
}


Checksum::operator bool() const noexcept
{
	return !zero();
}


Checksum::operator Checksum::value_type() const noexcept
{
	return value_;
}


void Checksum::swap(Checksum& rhs) noexcept
{
	using std::swap;

	swap(this->value_, rhs.value_);
}


bool Checksum::equals(const Checksum& rhs) const noexcept
{
	return this->equals_value(rhs.value());
}


bool Checksum::equals_value(const value_type rhs) const noexcept
{
	return this->value_ == rhs;
}


std::string Checksum::to_string() const
{
	auto stream = std::ostringstream {};
	checksum::print(stream, *this);
	return stream.str();
}


Checksum& Checksum::operator = (const Checksum::value_type rhs)
{
	value_ = rhs;
	return *this;
}


namespace checksum
{

std::string name(const type t)
{
	// The order of names in this aggregate must match the order of types in
	// enum class checksum::type, otherwise function type_name() will fail.
	static const std::array<std::string, 2> names {
		"ARCSv1",
		"ARCSv2",
		// "THIRD_TYPE" ,
		// "FOURTH_TYPE" ...
	};

	using index_type = typename std::underlying_type<checksum::type>::type;

	return names.at(static_cast<decltype( names )::size_type>(
				std::log2(static_cast<index_type>(t))));
}


void print(std::ostream& out, const Checksum& c)
{
	if (!out.good())
	{
		return;  // Maybe set badbit: out.setstate(std::ios_base::badbit);
	}

	[[maybe_unused]] const details::StreamFlagsGuard guard { out };

	static const auto hex_flags =
		[](const std::ostream& stream) -> std::ios_base::fmtflags
		{
			auto flags = std::ios_base::fmtflags { stream.flags() };

			flags &= ~stream.adjustfield; // unset 'left' or 'internal'
			flags |= stream.right;        // set 'right' only
			flags &= ~stream.basefield;   // unset 'dec' and 'oct'
			flags |= stream.hex;          // set 'hex' only
			flags |= stream.uppercase;    // set 'uppercase'
			flags &= ~stream.showbase;    // unset 'showbase'

			return flags;
		};

	out.flags(hex_flags(out));

	out << std::setw(static_cast<int>(Checksum::TOTAL_HEX_DIGITS))
		<< std::setfill('0') << c.value();
}

} // namespace checksum


// ChecksumSet


ChecksumSet::ChecksumSet()
	: ChecksumSet { AudioSize{ /* zero */} }
{
	// empty
}


ChecksumSet::ChecksumSet(const AudioSize& length)
	: ChecksumSet { length, { /* empty */ } }
{
	// empty
}


ChecksumSet::ChecksumSet(const AudioSize& length,
		std::initializer_list<
			std::pair<const ChecksumSet::key_type,
							ChecksumSet::value_type>> checksums)
	: length_  { length    }
	, set_     ( checksums )
{
	// empty
}


AudioSize ChecksumSet::length() const noexcept
{
	return length_;
}


void ChecksumSet::set_length(const AudioSize& l) noexcept
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
	using std::cend;

	return set_.find(type) != cend(set_);
}


std::pair<Checksum, bool> ChecksumSet::get(const checksum::type type) const
{
	using std::cend;

	if (const auto result { set_.find(type) }; result != cend(set_))
	{
		return { result->second, true };
	}

	return { Checksum {/*zero*/}, false };
}


std::set<checksum::type> ChecksumSet::types() const
{
	auto keys { std::set<checksum::type>{} };

	using std::begin;
	using std::cbegin;
	using std::cend;

	std::transform(cbegin(set_), cend(set_),
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
	if (!this->length().zero() && !rhs.length().zero())
	{
		// Non-zero lengths with different value indicates different tracks.
		if (this->length() != rhs.length())
		{
			throw std::domain_error(
					"Refuse to merge checksums of different track");
		}

		// Sets with zero length may be merged without constraint
	}

	set_.merge(rhs.set_);
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
	using std::cbegin;

	return cbegin(set_);
}


ChecksumSet::const_iterator ChecksumSet::cend() const
{
	using std::cend;

	return cend(set_);
}


ChecksumSet::const_iterator ChecksumSet::begin() const
{
	return this->cbegin();
}


ChecksumSet::const_iterator ChecksumSet::end() const
{
	return this->cend();
}


ChecksumSet::iterator ChecksumSet::begin()
{
	using std::begin;

	return begin(set_);
}


ChecksumSet::iterator ChecksumSet::end()
{
	using std::end;

	return end(set_);
}


ChecksumSet::operator bool() const noexcept
{
	return !empty();
}


void ChecksumSet::swap(ChecksumSet& rhs) noexcept
{
	using std::swap;

	swap(this->length_, rhs.length_);
	swap(this->set_,    rhs.set_);
}


bool ChecksumSet::equals(const ChecksumSet& rhs) const noexcept
{
	return this->length_ == rhs.length_ && this->set_ == rhs.set_;
}


std::string ChecksumSet::to_string() const
{
	auto ss = std::ostringstream {};
	ss << *this;
	return ss.str();
}


Checksum Checksum::from_fast(fast_type f) noexcept
{
    return Checksum(static_cast<value_type>(f));
}


std::ostream& operator << (std::ostream& out, const ChecksumSet& set)
{
	if (!out.good())
	{
		// Maybe set badbit: out.setstate(std::ios_base::badbit);
		return out;
	}

	using std::cbegin;
	using std::cend;
	using value_t = ChecksumSet::storage_type::value_type;

	std::for_each(cbegin(set.set_), cend(set.set_),
		[&out](const value_t& pair)
		{
			out << checksum::name(pair.first) << ": " << pair.second
				<< ' ';
		}
	);
	out << '(' << set.length() << ')';

	return out;
}

} // namespace v_1_0_0
} // namespace arcstk

