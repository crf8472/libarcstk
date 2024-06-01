#ifndef __LIBARCSTK_CHECKSUM_HPP___
#define __LIBARCSTK_CHECKSUM_HPP___

/**
 * \file
 *
 * \brief
 */

#include <array>    // for array
#include <cstdint>  // for uint32_t, int32_t
#include <unordered_map> // for unordered_map
#include <memory>   // for unique_ptr
#include <set>      // for set
#include <vector>   // for vector

#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"
#endif

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"  // for lba_count_t
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

/** \addtogroup calc */
/** @{ */

/**
 * \brief Everything regarding operation with Checksums.
 */
namespace checksum
{

/**
 * \brief Pre-defined checksum types.
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

} // namespace checksum


/**
 * \brief An AccurateRip checksum for a single file or track.
 *
 * A Checksum has a value_type. This is its numeric representation. It is an
 * unsigned integer of 32 bit length.
 *
 * A Checksum can be represented by its numeric value() which is of type
 * value_type. A Checksum can be compared for equality with instances of its
 * value_type using operator ==.
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
 *
 * As a technical convenience, a Checksum may be empty() which means: it carries
 * no value. Calling value() on an empty() Checksum may lead any result. Two
 * empty Checksum instances qualify as equal when compared using operator ==.
 */
class Checksum final : public Comparable<Checksum>
{
public:

	/**
	 * \brief Numerical base type of checksums: a 32-bit wide unsigned integer.
	 */
	using value_type = uint32_t;

	/**
	 * \brief Constructor.
	 *
	 * Creates an empty Checksum.
	 */
	Checksum();

	/**
	 * \brief Converting constructor.
	 *
	 * \param[in] value Actual checksum value
	 */
	Checksum(const value_type value);

	/**
	 * \brief Numeric value of the checksum.
	 *
	 * \return Numeric value of the checksum
	 */
	value_type value() const noexcept;

	/**
	 * \brief Return \c TRUE iff this Checksum is empty, otherwise \c FALSE.
	 *
	 * A Checksum is empty if it contains no valid value. Note that this
	 * does not guarantee <tt>value() == 0</tt>.
	 *
	 * \return Return \c TRUE iff this Checksum is empty, otherwise \c FALSE.
	 */
	bool empty() const noexcept;

	explicit operator bool() const noexcept;

	// Assignment operator for value_type instances
	Checksum& operator = (const value_type rhs);

	friend bool operator == (const Checksum& lhs, const Checksum& rhs)
		noexcept;

	friend void swap(Checksum& lhs, Checksum& rhs) noexcept;

private:

	/**
	 * \brief Actual checksum value;
	 */
	value_type value_;
};


/**
 * \brief Provide a string representation of a Checksum.
 *
 * \param[in] c Checksum to represent as string
 *
 * \return String representation of the checksum
 */
std::string to_string(const Checksum& c);


/**
 * \internal
 * \brief Overload operator << for outputting Checksums.
 *
 * Note: This is the default layout for printing ARCSs:
 * - hexadecimal representation
 * - without the '0x' base indicator
 * - uppercase letters
 * - leading zeros filling the width up to 8 digits.
 *
 * \param[in] out Stream to print to
 * \param[in] c   Checksum to print
 *
 * \return Reference to the stream
 */
std::ostream& operator << (std::ostream& out, const Checksum& c);


/**
 * \brief A set of Checksum instances of different types for a single track.
 *
 * The ChecksumSet represents the calculation result for a single track.
 */
class ChecksumSet final : public Comparable<ChecksumSet>
{
	using storage_type = std::unordered_map<checksum::type, Checksum>;

	/**
	 * \internal
	 * \brief Internal storage of the ChecksumSet.
	 */
	storage_type set_;

	/**
	 * \internal
	 * \brief Track length.
	 */
	lba_count_t length_;

public:

	using value_type     = storage_type::value_type;
	using iterator       = storage_type::iterator;
	using const_iterator = storage_type::const_iterator;
	using size_type      = storage_type::size_type;

	/**
	 * \brief Constructor for a track with unknown length (will be 0)
	 */
	ChecksumSet();

	/**
	 * \brief Constructor
	 *
	 * \param[in] length Length in LBA frames of the track
	 */
	explicit ChecksumSet(const lba_count_t length);

	/**
	 * \brief Constructor.
	 *
	 * Override the length of an existing ChecksumSet.
	 *
	 * \param[in] length Track length
	 * \param[in] rhs    Existing set
	 */
	ChecksumSet(const lba_count_t length, ChecksumSet&& rhs);

	/**
	 * \brief Constructor
	 *
	 * This constructor is intended for testing purposes.
	 *
	 * \param[in] length Track length
	 * \param[in] sums   Sequence of checksums
	 */
	ChecksumSet(const lba_count_t length,
			std::initializer_list<value_type> sums);

	/**
	 * \brief Length (in LBA frames) of this track.
	 *
	 * \return Length of this track in LBA frames
	 */
	lba_count_t length() const noexcept;

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
	 * \brief Return the \ref Checksum for the specified \c type
	 *
	 * If there is no Checksum represented for the \c type, the Checksum
	 * returned will be \link Checksum::empty() empty()\endlink.
	 *
	 * \param[in] type The checksum::type to return the value
	 *
	 * \return The checksum for the specified type
	 */
	Checksum get(const checksum::type type) const;

	/**
	 * \brief Return the checksum types present in this ChecksumSet
	 *
	 * \return The checksum types in this ChecksumSet
	 */
	std::set<checksum::type> types() const;

	/**
	 * \brief Inserts a new <type,Checksum> pair to the instance.
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
	void merge(const ChecksumSet& rhs);

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

	friend bool operator == (const ChecksumSet& lhs, const ChecksumSet& rhs)
		noexcept;

	friend void swap(ChecksumSet& lhs, ChecksumSet& rhs) noexcept;
};


/**
 * \brief The result of a Calculation, an iterable list of
 * \link ChecksumSet ChecksumSets \endlink.
 *
 * A Checksums instance represents all calculated checksums of an input, i.e. an
 * album or a track list.
 */
class Checksums final : public Comparable<Checksums>
{
	using storage_type = std::vector<ChecksumSet>;

	storage_type sets_;

public:

	using value_type     = storage_type::value_type;
	using iterator       = storage_type::iterator;
	using const_iterator = storage_type::const_iterator;
	using size_type      = storage_type::size_type;

	/**
	 * \brief Default constructor.
	 */
	Checksums();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] size Number of elements
	 */
	explicit Checksums(size_type size);

	/**
	 * \brief Constructor
	 *
	 * This constructor is intended for testing purposes only.
	 *
	 * \param[in] tracks Sequence of track checksums
	 */
	Checksums(std::initializer_list<ChecksumSet> tracks);

	/**
	 * \brief Return the total number of elements.
	 *
	 * \return Total number of elements
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
	 * \brief The ChecksumSet with the specified 0-based index \c index.
	 *
	 * \details
	 *
	 * Bounds checking is performed. If \c index is illegal, an exception is
	 * thrown.
	 *
	 * \see \link Checksums::operator [](const size_type index) const
	 * operator[]\endlink
	 *
	 * \param[in] index Index of the ChecksumSet to read
	 *
	 * \return ChecksumSet at index \c index.
	 *
	 * \throws std::out_of_range Iff \c index >= Checksums::size()
	 */
	const ChecksumSet& at(const size_type index) const;

	/**
	 * \brief The ChecksumSet with the specified \c index.
	 *
	 * No bounds checking is performed. For index based access with bounds
	 * checking, see
	 * \link Checksums::at(const size_type index) const at()\endlink.
	 *
	 * \param[in] index The 0-based index of the ChecksumSet to return
	 *
	 * \return ChecksumSet at the specified index
	 */
	const ChecksumSet& operator [] (const size_type index) const;

	/**
	 * \brief Append a track's checksums
	 *
	 * \param[in] checksums The checksums of a track
	 */
	void append(const ChecksumSet& checksums);

	/**
	 * \brief Append a track's checksums
	 *
	 * \param[in] checksums The checksums of a track
	 */
	void append(ChecksumSet&& checksums);

	/**
	 * \brief Obtain a const_iterator pointing to first ChecksumSet.
	 *
	 * \return const_iterator pointing to first ChecksumSet
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Obtain a const_iterator pointing behind last ChecksumSet.
	 *
	 * \return const_iterator pointing behind last ChecksumSet
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
	 * \brief Obtain a pointer to the first ChecksumSet.
	 *
	 * \return Pointer to the first ChecksumSet
	 */
	iterator begin();

	/**
	 * \brief Obtain a pointer pointing behind the last ChecksumSet.
	 *
	 * \return Pointer pointing behind the last ChecksumSet
	 */
	iterator end();


	friend bool operator == (const Checksums& lhs, const Checksums& rhs)
		noexcept;

	friend void swap(Checksums& lhs, Checksums& rhs) noexcept;
};


/**
 * \brief Global instance of an empty Checksum.
 *
 * This is for convenience since in most cases, the creation of an empty
 * Checksum can be avoided when a reference instance is at hand.
 *
 * This instance defines emptyness for checksums since Checksum::empty()
 * just compares the instance with this instance.
 */
extern const Checksum EmptyChecksum;

/** @} */

} // namespace v_1_0_0
} // namespace arcstk

#endif

