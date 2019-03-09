#ifndef __LIBARCS_CHECKSUM_HPP__
#define __LIBARCS_CHECKSUM_HPP__


/**
 * \file checksum.hpp Construction and management of checksums
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
#include <map>         // (ChecksumList)
#include <set>         // (ChecksumList)
#include <string>
#include <type_traits> // for conditional
#include <utility>     // for pair


namespace arcs
{
/// \addtogroup calc
/// @{
inline namespace v_1_0_0
{


/**
 * Everything regarding operation with <tt>Checksum</tt>s.
 */
namespace checksum
{
	// Note: 'names' array must have same size as 'type' class and the name
	// strings must occurr in exact the same order as in 'type'. Otherwise,
	// function 'type_name()' will fail.

	/**
	 * Type IDs of the pre-defined checksum types.
	 */
	enum class type : uint32_t
	{
		ARCS1   = 1,
		ARCS2   = 2
		//THIRD_TYPE  = 4,
		//FOURTH_TYPE = 8 ...
	};

	/**
	 * Checksum type names.
	 */
	static const std::array<std::string,4> names {
		"ARCSv1",
		"ARCSv2",
		// "THIRD_TYPE" ,
		// "FOURTH_TYPE" ...
	};

	/**
	 * Iterable sequence of all predefined checksum types.
	 */
	static const type types[] = {
		type::ARCS1,
		type::ARCS2
		// type::THIRD_TYPE,
		// type::FOURTH_TYPE ...
	};

	/**
	 * Return the name of a checksum::type
	 *
	 * \param[in] t Type to get name of
	 *
	 * \return Name of type
	 */
	std::string type_name(type t);

} // namespace checksum


/**
 * A 32-bit wide checksum for a single file or track.
 */
class Checksum final
{

public:

	/**
	 * Constructor.
	 *
	 * Creates an empty Checksum.
	 */
	Checksum();

	/**
	 * Constructor.
	 *
	 * \param[in] value Actual checksum value
	 */
	explicit Checksum(const uint32_t &value);

	//Checksum(const Checksum &rhs);

	//Checksum(Checksum &&rhs) noexcept;

	//virtual ~Checksum() noexcept = default;

	/**
	 * Numeric value of the checksum.
	 *
	 * \return Numeric value of the checksum
	 */
	uint32_t value() const;

	/**
	 * Return TRUE iff this Checksum is empty, otherwise FALSE.
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
	 * Equality.
	 *
	 * \param[in] rhs The instance to check for equality
	 */
	bool operator == (const Checksum &rhs) const;

	/**
	 * Inequality.
	 *
	 * \param[in] rhs The instance to check for inequality
	 */
	bool operator != (const Checksum &rhs) const;

	//Checksum& operator = (const Checksum &rhs);

	//Checksum& operator = (Checksum &&rhs) noexcept;


private:

	/**
	 * Actual checksum value;
	 */
	uint32_t value_;
};


namespace checksum
{

	/**
	 * Creates a hexadecimal representation of a 32bit checksum as a std::string
	 *
	 * \param[in] checksum The Checksum to represent
	 * \param[in] upper    TRUE indicates to print digits A-F in uppercase
	 * \param[in] base     TRUE indicates to print base '0x'
	 *
	 * \return A hexadecimal representation of the \c checksum as a string
	 *
	 * \internal
	 */
	std::string to_hex_str(const Checksum &checksum, bool upper, bool base);

} // namespace checksum


namespace details
{

// ChecksumListIterator needs this
template <typename K>
class ChecksumList;


/**
 * Iterator for <tt>ChecksumList<></tt>s.
 */
template <typename K, bool is_const = false>
class ChecksumListIterator
{
	// Befriend the converse version of the type: const_iterator can access
	// private members of iterator (and vice versa)
	friend ChecksumListIterator<K, not is_const>;

	// ChecksumList shall exclusively construct iterators by their private
	// constructor
	friend ChecksumList<K>;


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
	 * Construct const_iterator from iterator
	 *
	 * \param[in] rhs The iterator to construct a const_iterator
	 */
	ChecksumListIterator(const ChecksumListIterator<K, false> &rhs);

	/**
	 * Dereference operator
	 *
	 * \return A Checksum
	 */
	reference operator * ();

	/**
	 * Increment operator
	 */
	ChecksumListIterator& operator ++ ();

	/**
	 * Decrement operator
	 */
	ChecksumListIterator& operator -- ();

	/**
	 * Equality
	 *
	 * \param[in] lhs Left hand side of the operation
	 * \param[in] rhs Right hand side of the operation
	 *
	 * \return TRUE if lhs equals rhs, otherwise FALSE
	 */
	friend bool operator == (const ChecksumListIterator &lhs,
			const ChecksumListIterator &rhs) /* const */
	{
		return lhs.it_ == rhs.it_;
	}

	/**
	 * Inequality
	 *
	 * \param[in] lhs Left hand side of the operation
	 * \param[in] rhs Right hand side of the operation
	 *
	 * \return TRUE if lhs equals rhs, otherwise FALSE
	 */
	friend bool operator != (const ChecksumListIterator &lhs,
			const ChecksumListIterator &rhs) /* const */
	{
		return not(lhs == rhs);
	}


private:

	/**
	 * Private Constructor.
	 *
	 * Constructs a ChecksumListIterator from the iterator of the
	 * wrapped type.
	 *
	 * This constructor is private since ChecksumList<> instantiates
	 * its iterators exclusively.
	 *
	 * \param[in] i iterator of the wrapped type
	 */
	explicit ChecksumListIterator(const WrappedIteratorType &it);

	/**
	 * Wrapped iterator of the class implementing ChecksumList
	 */
	WrappedIteratorType it_;
};


/**
 * Generic implementation of a ChecksumList.
 *
 * This is a generic container for ChecksumLists adaptable to different
 * checksum types and different keys.
 */
template <typename K>
class ChecksumList
{

public: /* types */


	using iterator = ChecksumListIterator<K>;

	using const_iterator = ChecksumListIterator<K, true>;


public: /* methods */

	/**
	 * Constructor
	 */
	ChecksumList();

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	ChecksumList(const ChecksumList &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs The instance to move
	 */
	ChecksumList(ChecksumList &&rhs) noexcept;

	/**
	 * Virtual default destructor
	 */
	~ChecksumList() noexcept;


// Access


	/**
	 * Returns a ChecksumList::const_iterator to the beginning
	 *
	 * \return ChecksumList::const_iterator to the beginning
	 */
	const_iterator begin() const;

	/**
	 * Returns a ChecksumList::const_iterator to the beginning
	 *
	 * \return ChecksumList::const_iterator to the beginning
	 */
	const_iterator cbegin() const;

	/**
	 * Returns a ChecksumList::const_iterator to the end
	 *
	 * \return ChecksumList::const_iterator to the end
	 */
	const_iterator end() const;

	/**
	 * Returns a ChecksumList::const_iterator to the end
	 *
	 * \return ChecksumList::const_iterator to the end
	 */
	const_iterator cend() const;

	/**
	 * Finds an element in the instance by its key.
	 *
	 * If there is no element for the given key, the returned iterator will be
	 * equal to end().
	 *
	 * The element can not be modified via the returned iterator.
	 *
	 * \param[in] key The key to lookup
	 *
	 * \return ChecksumList::const_iterator to the element or to end()
	 */
	const_iterator find(const K &key) const;

	/**
	 * Returns TRUE iff the instance contains the key \c key .
	 *
	 * \param[in] key The key to lookup
	 *
	 * \return TRUE iff \c key is present in the instance, otherwise FALSE
	 */
	bool contains(const K &key) const;

	/**
	 * Returns the number of elements contained in the instance.
	 *
	 * \return Number of elements contained in the instance.
	 */
	std::size_t size() const;

	/**
	 * Returns TRUE iff the instance contains no elements, i.e. iff
	 * <tt>size() == 0</tt>, otherwise FALSE.
	 *
	 * \return TRUE iff <tt>size() == 0</tt>, otherwise FALSE
	 */
	bool empty() const;

	/**
	 * Returns the set of all keys contained in the instance.
	 *
	 * \return Set of keys used in this instance
	 */
	std::set<K> keys() const;

	/**
	 * Equality.
	 *
	 * \param[in] rhs The instance to check for equality
	 */
	bool operator == (const ChecksumList<K> &rhs) const;

	/**
	 * Inequality.
	 *
	 * \param[in] rhs The instance to check for inequality
	 */
	bool operator != (const ChecksumList<K> &rhs) const;


// Modify


	/**
	 * Returns a ChecksumList::iterator to the beginning
	 *
	 * \return ChecksumList::iterator to the beginning
	 */
	iterator begin();

	/**
	 * Returns a ChecksumList::iterator to the end
	 *
	 * \return ChecksumList::iterator to the end
	 */
	iterator end();

	/**
	 * Finds an element in the instance by its key.
	 *
	 * If there is no element for the given key, the returned iterator will be
	 * equal to end().
	 *
	 * \param[in] key The key to lookup
	 *
	 * \return ChecksumList::const_iterator to the element or to end()
	 */
	iterator find(const K &key);

	/**
	 * Inserts a new key-value-pair to the instance.
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
	 * Merge the elements of another instance into this instance.
	 *
	 * If a key in the other instance is already present in this instance, the
	 * corresponding element will be left unmodified.
	 *
	 * \param[in] rhs The list to be merged into the instance
	 */
	void merge(const ChecksumList<K> &rhs);

	/**
	 * Erases the element with the given key.
	 *
	 * Does nothing if the given key is not contained in the instance.
	 *
	 * \param[in] key The key to erase
	 */
	void erase(const K &key);

	/**
	 * Erases all elements contained in the instance.
	 *
	 * After clear() the size of the container will be \c 0 .
	 */
	void clear();

	/**
	 * Copy assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	ChecksumList<K>& operator = (const ChecksumList<K> &rhs);

	/**
	 * Move assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	ChecksumList<K>& operator = (ChecksumList<K> &&rhs) noexcept;


private: // TODO Hide this!

	/**
	 * Internal representation.
	 *
	 * This is not intended as part of the interface and should be ignored.
	 */
	std::map<K, Checksum> map_;
};


#ifndef __LIBARCS_CHECKSUM_TPP__
#include "details/checksum.tpp"
#endif

} // namespace arcs::details


/**
 * A set of <tt>Checksum</tt>s of different types for a single track.
 */
class ChecksumSet final : public details::ChecksumList<checksum::type>
{

public:

	/**
	 * Constructor for a track with unknown length (will be 0)
	 */
	ChecksumSet();

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	ChecksumSet(const ChecksumSet &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs The instance to move
	 */
	ChecksumSet(ChecksumSet &&rhs) noexcept;

	/**
	 * Constructor
	 *
	 * \param[in] length Length in LBA frames of the track
	 */
	explicit ChecksumSet(const uint32_t length);

	/**
	 * Default destructor
	 */
	~ChecksumSet() noexcept;

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
	ChecksumSet& operator = (const ChecksumSet &rhs);


private:

	// forward declaration for ChecksumSet::Impl
	class Impl;

	/**
	 * Private implementation of ChecksumSet
	 */
	std::unique_ptr<Impl> impl_;
};


} // namespace v_1_0_0

/// @}

} // namespace arcs

#endif

