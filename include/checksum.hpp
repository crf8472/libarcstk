#ifndef LIBARCSTK_CHECKSUM_HPP_
#define LIBARCSTK_CHECKSUM_HPP_

/**
 * \file
 *
 * \brief Representation for \link calc checksums\endlink, their aggregates and
 * their types.
 *
 * \details
 *
 * Part of the API for \link calc calculating AccurateRip checksums\endlink.
 */

#include <array>            // for array
#include <climits>          // for CHAR_BIT
#include <cmath>            // for log2
#include <cstdint>          // for int32_t, uint32_t
#include <initializer_list> // for initializer_list
#include <ostream>          // for ostream
#include <set>              // for set
#include <unordered_map>    // for unordered_map
#include <utility>          // for pair
#include <string>           // for string
#include <vector>           // for vector

#ifndef LIBARCSTK_POLICIES_HPP_
#include "policies.hpp"     // for Comparable
#endif

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
class Checksum; // forward declaration

/** \addtogroup calc */
/** @{ */

/**
 * \brief Operations on checksum types and their names.
 */
namespace checksum
{

/**
 * \brief Pre-defined checksum types.
 *
 * ARCS1 is AccurateRip v1 and ARCS2 is AccurateRip v2.
 */
enum class type : unsigned int
{
	ARCS1   = 1,
	ARCS2   = 2
	//THIRD_TYPE  = 4,
	//FOURTH_TYPE = 8 ...
};


/**
 * \brief Iterable sequence of all predefined checksum types.
 *
 * The order of the types is identical to the total order of numeric values the
 * types have in enum class checksum::type.
 */
static const std::array<type, 2> types = {
	type::ARCS1,
	type::ARCS2
	// type::THIRD_TYPE,
	// type::FOURTH_TYPE ...
};


/**
 * \brief Obtain the name of a checksum::type.
 *
 * \param[in] t Type to get name of
 *
 * \return Name of type \c t
 */
std::string type_name(const type t);


/**
 * \brief Print a checksum value to a stream.
 *
 * This defines how to print a Checksum by default.
 *
 * It provides the default implementation of Checksum::operator << and
 * Checksum::to_string().
 *
 * \param[in] stream   The stream to print to
 * \param[in] checksum Checksum to print
 */
void print(std::ostream& stream, const Checksum& checksum);

} // namespace checksum

/**
 * \brief A checksum for a single file or track.
 *
 * \details
 *
 * A Checksum has a value_type of an unsigned integer of 32 bit length.
 *
 * A Checksum can be represented by its numeric value(). A Checksum can be
 * compared for equality with instances of its value_type using operator ==.
 * As a technical convenience, a Checksum is zero() if its value() is 0. In this
 * case it is converted by operator bool() to FALSE. Any Checksum that is not
 * zero() converts to TRUE.
 *
 * A Checksum has a converting constructor for its value_type, thus every
 * parameter that expects a checksum can be assigned a value of type value_type
 * instead of a Checksum. Some compilers will do the conversion with other
 * numerical types but issue a warning if a conversion from signed to unsigned
 * types is required (e.g. -Wsign-conversion).
 *
 * Operator << is overloaded for printing Checksums to streams. The Checksum
 * will then occurr in its standard layout: as a hexadecimal number without the
 * base '0x', all digits in upper case, and filled with leading zeros up to a
 * width of 8 digits.
 */
class Checksum final : public Comparable<Checksum>
{
public:

	/**
	 * \brief Numerical base type of checksums: a 32-bit wide unsigned integer.
	 */
	using value_type = uint32_t;

	/**
	 * \brief Total number of printed hexadecimal digits of an ARCS.
	 */
	constexpr static std::size_t TOTAL_DIGITS =
		(sizeof(Checksum::value_type) * CHAR_BIT) / std::log2(16);
	// 4 bits are required to represent a single hexadecimal digit
	// (since 2^4 == 16). We express 4 as log_2(16).

	/**
	 * \brief Constructor.
	 *
	 * Creates an empty Checksum.
	 */
	Checksum();

	/**
	 * \brief Converting constructor for value_type instances.
	 *
	 * \param[in] value Actual checksum value
	 */
	Checksum(const value_type value);

	/**
	 * \brief Assignment operator to assign value_type values
	 *
	 * \param[in] rhs Actual checksum value
	 *
	 * \return Reference to instance
	 */
	Checksum& operator = (const value_type rhs);

	/**
	 * \brief Numeric value of the checksum.
	 *
	 * \return Numeric value of the checksum
	 */
	value_type value() const noexcept;

	/**
	 * \brief Return \c TRUE iff this Checksum is 0, otherwise \c FALSE.
	 *
	 * \return Return \c TRUE iff this Checksum is 0, otherwise \c FALSE.
	 */
	bool zero() const noexcept;

	/**
	 * \brief Return \c TRUE iff instance is not zero(), otherwise \c FALSE.
	 *
	 * \return Return \c TRUE iff instance is not zero(), otherwise \c FALSE.
	 */
	explicit operator bool() const noexcept;

	/**
	 * \brief Swap with another instance.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap(Checksum& rhs) noexcept;

	/**
	 * \brief TRUE iff this instance is equal to another instance.
	 *
	 * \param[in] rhs Instance to check for equality
	 *
	 * \return TRUE iff \c rhs == \c this
	 */
	bool equals(const Checksum& rhs) const noexcept;

	/**
	 * \brief Create a string representation of this instance.
	 *
	 * \return String representation
	 */
	std::string to_string() const;


	friend void swap(Checksum& lhs, Checksum& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	friend bool operator == (const Checksum& lhs, const Checksum& rhs) noexcept
	{
		return lhs.equals(rhs);
	}

	friend std::ostream& operator << (std::ostream& out, const Checksum& c)
	{
		checksum::print(out, c);
		return out;
	}

	friend std::string to_string(const Checksum& c)
	{
		return c.to_string();
	}

private:

	/**
	 * \brief Actual checksum value;
	 */
	value_type value_;
};


/**
 * \brief A set of Checksum instances of different types for a single track.
 *
 * The ChecksumSet represents the calculation result for a single track. It also
 * holds optionally the track length as number of LBA frames for convenience.
 * The length may be zero which counts as "unknown".
 */
class ChecksumSet final : public Comparable<ChecksumSet>
{
public:

	/**
	 * \brief Value type of the ChecksumSet.
	 */
	using value_type = Checksum;

private:

	/**
	 * \internal
	 * \brief Track length as number of LBA frames.
	 */
	int32_t length_;

	/**
	 * \internal
	 * \brief Key type of the internal type map.
	 */
	using key_type = checksum::type;

	/**
	 * \internal
	 * \brief Type of the internal storage of the ChecksumSet.
	 */
	using storage_type = std::unordered_map<key_type, value_type>;

	/**
	 * \internal
	 * \brief Internal storage of the ChecksumSet.
	 */
	storage_type set_;

	// intentionally undocumented
	using unspecified_forward_iterator_type = storage_type::iterator;

	// intentionally undocumented
	using unspecified_constant_forward_iterator_type =
		storage_type::const_iterator;

	// intentionally undocumented
	using unspecified_unsigned_size_type = storage_type::size_type;

public:

	/**
	 * \brief Unspecified forward iterator type.
	 */
	using iterator = unspecified_forward_iterator_type;

	/**
	 * \brief Unspecified forward iterator type.
	 */
	using const_iterator = unspecified_constant_forward_iterator_type;

	/**
	 * \brief Size type (unsigned integral type)
	 */
	using size_type = unspecified_unsigned_size_type;

	/**
	 * \brief Constructor for a track with unknown length (will be 0)
	 */
	ChecksumSet();

	/**
	 * \brief Constructor
	 *
	 * \param[in] length Length in LBA frames of the track
	 */
	explicit ChecksumSet(const int32_t length);

	/**
	 * \brief Constructor for a known set of typed Checksums.
	 *
	 * \param[in] length Track length
	 * \param[in] sums   Sequence of checksums represented as type-value pairs
	 */
	ChecksumSet(const int32_t length,
			std::initializer_list<
				std::pair<const checksum::type, value_type>> sums);
	//NOTE We do not expose key_type. If key is not of key_type, it just breaks.

	/**
	 * \brief Length (in LBA frames) of this track.
	 *
	 * For ChecksumSets constructed by an instance of Algorithm, this will be
	 * the length actually used for computing the Checksum. It may or may not be
	 * identical to the parsed length hold in the ToC. The parsed length might
	 * be smaller since it is possible that it does not contain the silence
	 * adjacent to the respective track. Hence, a mismatch between the length()
	 * of a ChecksumSet and the parsed_length() of the ToC used in the
	 * Calculation that created the ChecksumSet is not an error.
	 *
	 * \return Length of this track in LBA frames
	 */
	int32_t length() const noexcept; //TODO AudioSize

	/**
	 * \brief Set the length (in LBA frames) of this track.
	 *
	 * \param[in] length New length for this instance
	 */
	void set_length(const int32_t length) noexcept; //TODO AudioSize

	/**
	 * \brief Returns the number of elements contained in the instance.
	 *
	 * \return Number of elements contained in the instance.
	 */
	size_type size() const noexcept;

	/**
	 * \brief Returns \c TRUE iff the instance contains no elements, otherwise
	 * \c FALSE.
	 *
	 * \return \c TRUE iff instance contains no elements, otherwise \c FALSE
	 */
	bool empty() const noexcept;

	/**
	 * \brief Returns \c TRUE iff the instance contains a Checksum of the type
	 * \c type.
	 *
	 * \param[in] type The type to lookup the Checksum for
	 *
	 * \return \c TRUE iff \c type is present in the instance, otherwise \c FALSE
	 */
	bool contains(const checksum::type& type) const;

	/**
	 * \brief Return the \ref Checksum for the specified \c type and a boolean.
	 *
	 * The boolean is TRUE iff the search was successfull and the Checksum is an
	 * actual search result. If there is no Checksum represented for the
	 * \c type, the Checksum returned will be
	 * \link Checksum::zero() zero()\endlink and the boolean value will be
	 * FALSE.
	 *
	 * \param[in] type The checksum::type to return the value
	 *
	 * \return The checksum for the specified type
	 */
	std::pair<Checksum, bool> get(const checksum::type type) const;

	/**
	 * \brief Return the checksum types present in this ChecksumSet
	 *
	 * \return The checksum types in this ChecksumSet
	 */
	std::set<checksum::type> types() const;

	/**
	 * \brief Inserts a new <type, Checksum> pair to the instance.
	 *
	 * If the key is already present in the instance, the existing checksum will
	 * be left unmodified.
	 *
	 * The pair returned contains an iterator to the inserted Checksum and a
	 * bool that is \c TRUE iff the insertion was successful. If the insertion
	 * was not successful, the value \c FALSE is returned for the bool and the
	 * iterator will point to the element that prevented the insertion.
	 *
	 * \param[in] type     The key to use
	 * \param[in] checksum The checksum for the given key
	 *
	 * \return Pair with an iterator to the inserted value and a status flag
	 */
	std::pair<iterator, bool> insert(const checksum::type type,
			const Checksum& checksum);

	/**
	 * \brief Merge the elements of another instance into this instance.
	 *
	 * If a key in the other instance is already present in this instance, the
	 * corresponding element will be left unmodified.
	 *
	 * Trying to merge a set with non-zero length into an instance with non-zero
	 * length of different value will cause a std::domain_error. If either
	 * \c rhs or \c this has zero length, the merge will succeed.
	 *
	 * \param[in] rhs The list to be merged into the instance
	 *
	 * \throws domain_error If \c rhs.length() != \c length() and both are != 0
	 */
	void merge(ChecksumSet& rhs);

	/**
	 * \brief Erases the Checksum with the given type.
	 *
	 * Iff the given type is not contained in the instance, the call has
	 * no effect.
	 *
	 * \param[in] type The type to erase
	 */
	void erase(const checksum::type& type);

	/**
	 * \brief Erases all \link Checksum Checksums\endlink contained in the
	 * instance.
	 *
	 * After clear() has been called the size of the container will be \c 0 .
	 */
	void clear();

	/**
	 * \brief Obtain a pointer to the first Checksum.
	 *
	 * \return Pointer to the first Checksum
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Obtain a pointer pointing behind the last Checksum.
	 *
	 * \return Pointer pointing behind the last Checksum
	 */
	const_iterator cend() const;

	/**
	 * \copydoc cbegin()
	 */
	const_iterator begin() const;

	/**
	 * \copydoc cend()
	 */
	const_iterator end() const;

	/**
	 * \brief Obtain a pointer to the first Checksum.
	 *
	 * \return Pointer to the first Checksum
	 */
	iterator begin();

	/**
	 * \brief Obtain a pointer pointing behind the last Checksum.
	 *
	 * \return Pointer pointing behind the last Checksum
	 */
	iterator end();

	/**
	 * \brief Return \c TRUE iff instance is not empty(), otherwise \c FALSE.
	 *
	 * \return Return \c TRUE iff instance is not empty(), otherwise \c FALSE.
	 */
	explicit operator bool() const noexcept;

	/**
	 * \brief Swap with another instance.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap(ChecksumSet& rhs) noexcept;

	/**
	 * \brief TRUE iff this instance is equal to another instance.
	 *
	 * \param[in] rhs Instance to check for equality
	 *
	 * \return TRUE iff \c rhs == \c this
	 */
	bool equals(const ChecksumSet& rhs) const noexcept;


	friend void swap(ChecksumSet& lhs, ChecksumSet& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	friend bool operator == (const ChecksumSet& lhs, const ChecksumSet& rhs)
		noexcept
	{
		return lhs.equals(rhs);
	}
};

/**
 * \brief A list of @link arcstk::ChecksumSet ChecksumSets @endlink.
 *
 * Each ChecksumSet represents a track. The order of the ChecksumSets follows
 * the order of tracks on the original compact disc.
 */
using Checksums = std::vector<ChecksumSet>;

/**
 * \brief Global instance of an empty ChecksumSet.
 *
 * Avoid the creation of empty instances whenever possible.
 */
extern const ChecksumSet EmptyChecksumSet;

/**
 * \brief Global instance of empty Checksums.
 *
 * Avoid the creation of empty instances whenever possible.
 */
extern const Checksums EmptyChecksums;

/** @} */ // group calc

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

