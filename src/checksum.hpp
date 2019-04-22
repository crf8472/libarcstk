#ifndef __LIBARCSTK_CHECKSUM_HPP__
#define __LIBARCSTK_CHECKSUM_HPP__

/**
 * \file
 *
 * \brief Public API for management of checksums.
 *
 * Represent metadata of a given compact disc image, the computed checksums of
 * an audio file and the Checksums calculating class along with its
 * configuration and state.
 *
 * Basic data representation classes include TOC, Checksum and
 * ChecksumSet.
 *
 * TOC represents the toc information of a compact disc along with a
 * function <tt>make_toc()</tt> that guarantees to provide only valid
 * <tt>TOC</tt>s or to throw an exception.
 *
 * Checksum represents a single checksum and a ChecksumSet the
 * <tt>Checksum</tt>s for a single track.
 *
 * Checksums is an aggregation of the <tt>Checksum</tt>s of an audio input.
 */

#include <array>
#include <cstdint>
#include <cstddef>     // for size_t
#include <memory>
#include <map>         // (ChecksumMap)
#include <set>         // (ChecksumMap)
#include <string>
#include <type_traits> // for conditional
#include <utility>     // for pair

namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \addtogroup calc
 * @{
 */

/**
 * \brief Everything regarding operation with Checksums.
 */
namespace checksum
{
	// Note: 'names' array must have same size as 'type' class and the name
	// strings must occurr in exact the same order as in 'type'. Otherwise,
	// function 'type_name()' will fail.

	/**
	 * \brief Type IDs of the pre-defined checksum types.
	 */
	enum class type : uint32_t
	{
		ARCS1   = 1,
		ARCS2   = 2
		//THIRD_TYPE  = 4,
		//FOURTH_TYPE = 8 ...
	};

	/**
	 * \brief Checksum type names.
	 */
	static const std::array<std::string,4> names {
		"ARCSv1",
		"ARCSv2",
		// "THIRD_TYPE" ,
		// "FOURTH_TYPE" ...
	};

	/**
	 * \brief Iterable sequence of all predefined checksum types.
	 */
	static const type types[] = {
		type::ARCS1,
		type::ARCS2
		// type::THIRD_TYPE,
		// type::FOURTH_TYPE ...
	};

	/**
	 * \brief Return the name of a checksum::type.
	 *
	 * \param[in] t Type to get name of
	 *
	 * \return Name of type
	 */
	std::string type_name(type t);

} // namespace checksum


/**
 * \brief A 32-bit wide checksum for a single file or track.
 */
class Checksum final
{

public:

	/**
	 * \brief Constructor.
	 *
	 * Creates an empty Checksum.
	 */
	Checksum();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] value Actual checksum value
	 */
	explicit Checksum(const uint32_t &value);

	//Checksum(const Checksum &rhs);

	//Checksum(Checksum &&rhs) noexcept;

	//virtual ~Checksum() noexcept = default;

	/**
	 * \brief Numeric value of the checksum.
	 *
	 * \return Numeric value of the checksum
	 */
	uint32_t value() const;

	/**
	 * \brief Return TRUE iff this Checksum is empty, otherwise FALSE.
	 *
	 * A Checksum is empty if it contains no valid value. Note that this
	 * does not entail <tt>value() == 0</tt> in all cases. Nonetheless, for most
	 * checksum types, the converse holds, they are <tt>empty()</tt> if they are
	 * \c 0 .
	 *
	 * \return Return TRUE iff this Checksum is empty, otherwise FALSE.
	 */
	bool empty() const;

	/**
	 * \brief Equality.
	 *
	 * \param[in] rhs The instance to check for equality
	 */
	bool operator == (const Checksum &rhs) const;

	/**
	 * \brief Inequality.
	 *
	 * \param[in] rhs The instance to check for inequality
	 */
	bool operator != (const Checksum &rhs) const;

	//Checksum& operator = (const Checksum &rhs);

	//Checksum& operator = (Checksum &&rhs) noexcept;


private:

	/**
	 * \brief Actual checksum value;
	 */
	uint32_t value_;
};


/**
 * \brief Checksums related tools
 */
namespace checksum
{

	/**
	 * \internal
	 *
	 * \brief Creates a hexadecimal string representation of a 32bit checksum.
	 *
	 * \param[in] checksum The Checksum to represent
	 * \param[in] upper    TRUE indicates to print digits A-F in uppercase
	 * \param[in] base     TRUE indicates to print base '0x'
	 *
	 * \return A hexadecimal representation of the \c checksum as a string
	 */
	std::string to_hex_str(const Checksum &checksum, bool upper, bool base);

} // namespace checksum


/**
 * \brief Implementation details of namespace arcstk
 */
namespace details
{

// forward declaration: ChecksumMapIterator needs this
template <typename K>
class ChecksumMap;


/**
 * \brief Iterator for @link ChecksumMap ChecksumMap<>s @endlink.
 */
template <typename K, bool is_const = false>
class ChecksumMapIterator
{
	// Befriend the converse version of the type: const_iterator can access
	// private members of iterator (and vice versa)
	friend ChecksumMapIterator<K, not is_const>;

	// ChecksumMap shall exclusively construct iterators by their private
	// constructor
	friend ChecksumMap<K>;


public: /* types */

	using value_type        = Checksum;

	using difference_type   = Checksum;

	using pointer           = typename std::conditional<is_const,
			const Checksum*, Checksum*>::type;

	using reference         = typename std::conditional<is_const,
			const Checksum&, Checksum&>::type;


private: /* types */

	// Type of the container to iterate
	using IteratedContainerType = typename std::map<K, value_type>;

	// Type of the container's iterator to wrap
	using WrappedIteratorType = typename std::conditional<is_const,
			typename IteratedContainerType::const_iterator,
			typename IteratedContainerType::iterator
		>::type;


public: /* types */

	using iterator_category = typename WrappedIteratorType::iterator_category;


public: /* methods */

	/**
	 * \brief Construct const_iterator from iterator
	 *
	 * \param[in] rhs The iterator to construct a const_iterator
	 */
	ChecksumMapIterator(const ChecksumMapIterator<K, false> &rhs);

	/**
	 * \brief Dereference operator
	 *
	 * \return A Checksum
	 */
	reference operator * ();

	/**
	 * \brief Preincrement operator
	 */
	ChecksumMapIterator& operator ++ ();

	/**
	 * \brief Decrement operator
	 */
	ChecksumMapIterator& operator -- ();

	/**
	 * \brief Equality
	 *
	 * \param[in] lhs Left hand side of the operation
	 * \param[in] rhs Right hand side of the operation
	 *
	 * \return TRUE if lhs equals rhs, otherwise FALSE
	 */
	friend bool operator == (const ChecksumMapIterator &lhs,
			const ChecksumMapIterator &rhs) /* const */
	{
		return lhs.it_ == rhs.it_;
	}

	/**
	 * \brief Inequality
	 *
	 * \param[in] lhs Left hand side of the operation
	 * \param[in] rhs Right hand side of the operation
	 *
	 * \return TRUE if lhs equals rhs, otherwise FALSE
	 */
	friend bool operator != (const ChecksumMapIterator &lhs,
			const ChecksumMapIterator &rhs) /* const */
	{
		return not(lhs == rhs);
	}


private:

	/**
	 * \brief Private Constructor.
	 *
	 * Constructs a ChecksumMapIterator from the iterator of the
	 * wrapped type.
	 *
	 * This constructor is private since ChecksumMap<> instantiates
	 * its iterators exclusively.
	 *
	 * \param[in] it iterator of the wrapped type
	 */
	explicit ChecksumMapIterator(const WrappedIteratorType &it);

	/**
	 * \brief Wrapped iterator of the class implementing ChecksumMap
	 */
	WrappedIteratorType it_;
};


/**
 * \brief Generic implementation of a ChecksumMap.
 *
 * This is a generic container for ChecksumMaps adaptable to different
 * checksum types and different keys.
 */
template <typename K>
class ChecksumMap
{

public: /* types */


	using iterator = ChecksumMapIterator<K>;

	using const_iterator = ChecksumMapIterator<K, true>;


public: /* methods */

	/**
	 * \brief Constructor
	 */
	ChecksumMap();

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	ChecksumMap(const ChecksumMap &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs The instance to move
	 */
	ChecksumMap(ChecksumMap &&rhs) noexcept;

	/**
	 * \brief Virtual default destructor
	 */
	~ChecksumMap() noexcept;


// Access


	/**
	 * \brief Returns a ChecksumMap::const_iterator to the beginning
	 *
	 * \return ChecksumMap::const_iterator to the beginning
	 */
	const_iterator begin() const;

	/**
	 * \brief Returns a ChecksumMap::const_iterator to the beginning
	 *
	 * \return ChecksumMap::const_iterator to the beginning
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Returns a ChecksumMap::const_iterator to the end
	 *
	 * \return ChecksumMap::const_iterator to the end
	 */
	const_iterator end() const;

	/**
	 * \brief Returns a ChecksumMap::const_iterator to the end
	 *
	 * \return ChecksumMap::const_iterator to the end
	 */
	const_iterator cend() const;

	/**
	 * \brief Finds an element in the instance by its key.
	 *
	 * If there is no element for the given key, the returned iterator will be
	 * equal to end().
	 *
	 * The element can not be modified via the returned iterator.
	 *
	 * \param[in] key The key to lookup
	 *
	 * \return ChecksumMap::const_iterator to the element or to end()
	 */
	const_iterator find(const K &key) const;

	/**
	 * \brief Returns TRUE iff the instance contains the key \c key .
	 *
	 * \param[in] key The key to lookup
	 *
	 * \return TRUE iff \c key is present in the instance, otherwise FALSE
	 */
	bool contains(const K &key) const;

	/**
	 * \brief Returns the number of elements contained in the instance.
	 *
	 * \return Number of elements contained in the instance.
	 */
	std::size_t size() const;

	/**
	 * \brief Returns TRUE iff the instance contains no elements, i.e. iff
	 * <tt>size() == 0</tt>, otherwise FALSE.
	 *
	 * \return TRUE iff <tt>size() == 0</tt>, otherwise FALSE
	 */
	bool empty() const;

	/**
	 * \brief Returns the set of all keys contained in the instance.
	 *
	 * \return Set of keys used in this instance
	 */
	std::set<K> keys() const;

	/**
	 * \brief Equality.
	 *
	 * \param[in] rhs The instance to check for equality
	 */
	bool operator == (const ChecksumMap<K> &rhs) const;

	/**
	 * \brief Inequality.
	 *
	 * \param[in] rhs The instance to check for inequality
	 */
	bool operator != (const ChecksumMap<K> &rhs) const;


// Modify


	/**
	 * \brief Returns a ChecksumMap::iterator to the beginning
	 *
	 * \return ChecksumMap::iterator to the beginning
	 */
	iterator begin();

	/**
	 * \brief Returns a ChecksumMap::iterator to the end
	 *
	 * \return ChecksumMap::iterator to the end
	 */
	iterator end();

	/**
	 * \brief Finds an element in the instance by its key.
	 *
	 * If there is no element for the given key, the returned iterator will be
	 * equal to end().
	 *
	 * \param[in] key The key to lookup
	 *
	 * \return ChecksumMap::const_iterator to the element or to end()
	 */
	iterator find(const K &key);

	/**
	 * \brief Inserts a new key-value-pair to the instance.
	 *
	 * If the key is already present in the instance, the existing checksum will
	 * be overwritten with \c checksum.
	 *
	 * The pair returned contains an iterator to the inserted value and a bool
	 * that is TRUE iff the insertion was successful. If the insertion was not
	 * successful, the value FALSE is returned for the bool and end() for
	 * the iterator.
	 *
	 * \param[in] key The key to use
	 * \param[in] checksum The checksum for the given key
	 *
	 * \return Pair with an iterator to the inserted value and a status flag
	 */
	std::pair<iterator, bool> insert(const K &key, const Checksum &checksum);

	/**
	 * \brief Merge the elements of another instance into this instance.
	 *
	 * If a key in the other instance is already present in this instance, the
	 * corresponding element will be left unmodified.
	 *
	 * \param[in] rhs The list to be merged into the instance
	 */
	void merge(const ChecksumMap<K> &rhs);

	/**
	 * \brief Erases the element with the given key.
	 *
	 * Does nothing if the given key is not contained in the instance.
	 *
	 * \param[in] key The key to erase
	 */
	void erase(const K &key);

	/**
	 * \brief Erases all elements contained in the instance.
	 *
	 * After clear() the size of the container will be \c 0 .
	 */
	void clear();

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	ChecksumMap<K>& operator = (const ChecksumMap<K> &rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	ChecksumMap<K>& operator = (ChecksumMap<K> &&rhs) noexcept;


private: // TODO Hide this!

	/**
	 * \brief Internal representation.
	 *
	 * This is not intended as part of the interface and should be ignored.
	 */
	std::map<K, Checksum> map_;
};


#ifndef __LIBARCSTK_CHECKSUM_TPP__
#include "details/checksum.tpp"
#endif

} // namespace arcstk::details


/**
 * \brief A set of Checksum instances of different types for a single track.
 */
class ChecksumSet final : public details::ChecksumMap<checksum::type>
{

public:

	/**
	 * \brief Constructor for a track with unknown length (will be 0)
	 */
	ChecksumSet();

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	ChecksumSet(const ChecksumSet &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs The instance to move
	 */
	ChecksumSet(ChecksumSet &&rhs) noexcept;

	/**
	 * \brief Constructor
	 *
	 * \param[in] length Length in LBA frames of the track
	 */
	explicit ChecksumSet(const uint32_t length);

	/**
	 * \brief Default destructor
	 */
	~ChecksumSet() noexcept;

	/**
	 * \brief Length (in LBA frames) of this track.
	 *
	 * \return Length of this track in LBA frames
	 */
	uint32_t length() const;

	/**
	 * \brief Return the \ref Checksum for the specified \c type
	 *
	 * \param[in] type The checksum::type to return the value
	 *
	 * \return The checksum for the specified type
	 */
	Checksum get(checksum::type type) const;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	ChecksumSet& operator = (const ChecksumSet &rhs);


private:

	// forward declaration for ChecksumSet::Impl
	class Impl;

	/**
	 * \brief Private implementation of ChecksumSet
	 */
	std::unique_ptr<Impl> impl_;
};

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

