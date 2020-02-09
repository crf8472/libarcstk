// vim:ft=cpp
#ifndef __LIBARCSTK_CHECKSUM_HPP__
#error "Do not include checksum.tpp directly, include checksum.hpp instead"
#endif

#ifndef __LIBARCSTK_CHECKSUM_TPP__
#define __LIBARCSTK_CHECKSUM_TPP__

/**
 * \file
 *
 * \internal
 *
 * \brief Implementation of ChecksumMapIterator and ChecksumMap
 */

namespace details
{


template <typename K>
class ChecksumMap;    // forward declaration for ChecksumMapIterator


/**
 * \internal
 *
 * \brief Iterator for @link ChecksumMap ChecksumMaps @endlink.
 *
 * \tparam K        The key type of the iterated ChecksumMap
 * \tparam is_const TRUE indicates a const_iterator
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

	using value_type = Checksum;


private: /* types */

	/**
	 * \brief Type of the container to iterate
	 */
	using IteratedContainerType = typename std::map<K, value_type>;

	/**
	 * \brief Type of the container's iterator to wrap
	 */
	using WrappedIteratorType = typename std::conditional<is_const,
			typename IteratedContainerType::const_iterator,
			typename IteratedContainerType::iterator
		>::type;


public: /* types */

	using difference_type   = Checksum;

	using pointer           = typename std::conditional<is_const,
			const Checksum*, Checksum*>::type;

	using reference         = typename std::conditional<is_const,
			const Checksum&, Checksum&>::type;

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
	 *
	 * \return The incremented instance
	 */
	ChecksumMapIterator& operator ++ ();

	/**
	 * \brief Decrement operator
	 *
	 * \return The decremented instance
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
			const ChecksumMapIterator &rhs)
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
			const ChecksumMapIterator &rhs)
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
 * \internal
 *
 * \brief Generic implementation of a ChecksumMap.
 *
 * This is a generic container for ChecksumMaps adaptable to different
 * checksum types and different keys.
 *
 * \tparam K The key type of this instance
 */
template <typename K>
class ChecksumMap
{

public: /* types */


	using iterator = ChecksumMapIterator<K>;

	using const_iterator = ChecksumMapIterator<K, true>;

	using size_type = std::size_t;


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
	size_type size() const;

	/**
	 * \brief Returns TRUE iff the instance contains no elements, otherwise
	 * FALSE.
	 *
	 * \return TRUE iff instance contains no elements, otherwise FALSE
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
	 * \param[in] lhs The left hand side instance to check for equality
	 * \param[in] rhs The right hand side instance to check for equality
	 *
	 * \return TRUE if \c lhs is equal to \c rhs, otherwise FALSE
	 */
	friend bool operator == (const ChecksumMap &lhs,
			const ChecksumMap &rhs) noexcept
	{
		return lhs.map_ == rhs.map_;
	}

	/**
	 * \brief Inequality.
	 *
	 * \param[in] lhs The instance to check for equality
	 * \param[in] rhs The instance to check for inequality
	 *
	 * \return TRUE if \c lhs is not equal to \c rhs, otherwise FALSE
	 */
	friend bool operator != (const ChecksumMap &lhs,
			const ChecksumMap &rhs) noexcept
	{
		return not(lhs == rhs);
	}


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

// ChecksumMapIterator


template <typename K, bool is_const>
ChecksumMapIterator<K, is_const>::ChecksumMapIterator(
		const WrappedIteratorType &it)
	: it_(it)
{
	// empty
}


template <typename K, bool is_const>
ChecksumMapIterator<K, is_const>::ChecksumMapIterator(
		const ChecksumMapIterator<K, false> &rhs)
	: it_(rhs.it_)
{
	// empty
}


template <typename K, bool is_const>
typename ChecksumMapIterator<K, is_const>::reference
	ChecksumMapIterator<K, is_const>::operator * ()
{
	return (*it_).second;
}


template <typename K, bool is_const>
ChecksumMapIterator<K, is_const>&
	ChecksumMapIterator<K, is_const>::operator ++ ()
{
	++it_;
	return *this;
}


template <typename K, bool is_const>
ChecksumMapIterator<K, is_const>&
	ChecksumMapIterator<K, is_const>::operator -- ()
{
	--it_;
	return *this;
}


// ChecksumMap


template <typename K>
	ChecksumMap<K>::ChecksumMap()
	: map_()
{
	// empty
}


template <typename K>
	ChecksumMap<K>::ChecksumMap(const ChecksumMap &rhs)
= default;


// This @relates-statement silences a doxygen 1.8.15 warning that reads
// "no uniquely matching class member found"
/// \relates arcstk::v_1_0_0::details::ChecksumMap(ChecksumMap &&rhs) noexcept
template <typename K>
	ChecksumMap<K>::ChecksumMap(ChecksumMap &&rhs) noexcept
= default;


template <typename K>
	ChecksumMap<K>::~ChecksumMap() noexcept
= default;


// ChecksumMap : Accessors


template <typename K>
typename ChecksumMap<K>::const_iterator
	ChecksumMap<K>::begin() const
{
	return const_iterator(this->map_.begin());
}


template <typename K>
typename ChecksumMap<K>::const_iterator
	ChecksumMap<K>::cbegin() const
{
	return const_iterator(this->map_.cbegin());
}


template <typename K>
typename ChecksumMap<K>::const_iterator
	ChecksumMap<K>::end() const
{
	return const_iterator(this->map_.end());
}


template <typename K>
typename ChecksumMap<K>::const_iterator
	ChecksumMap<K>::cend() const
{
	return const_iterator(this->map_.cend());
}


template <typename K>
typename ChecksumMap<K>::const_iterator
	ChecksumMap<K>::find(const K &key) const
{
	return const_iterator(this->map_.find(key));
}


template <typename K>
bool ChecksumMap<K>::contains(const K &key) const
{
#if __cplusplus >= 201703L
	return this->map_.contains(key); // C++20
#else
	return map_.find(key) != map_.end();
#endif
}


template <typename K>
std::set<K> ChecksumMap<K>::keys() const
{
	std::set<K> keys;

	for (const auto& entry : this->map_)
	{
		keys.insert(keys.end(), entry.first);
	}

	return keys;
}


template <typename K>
typename ChecksumMap<K>::size_type ChecksumMap<K>::size() const
{
	return this->map_.size();
}


template <typename K>
bool ChecksumMap<K>::empty() const
{
	return this->map_.empty();
}


// ChecksumMap : Modifiers


template <typename K>
typename ChecksumMap<K>::iterator
	ChecksumMap<K>::begin()
{
	return iterator(this->map_.begin());
}


template <typename K>
typename ChecksumMap<K>::iterator
	ChecksumMap<K>::end()
{
	return iterator(this->map_.end());
}


template <typename K>
typename ChecksumMap<K>::iterator
	ChecksumMap<K>::find(const K &key)
{
	return iterator(this->map_.find(key));
}


template <typename K>
std::pair<typename ChecksumMap<K>::iterator, bool>
	ChecksumMap<K>::insert(const K &key, const Checksum &checksum)
{
	auto result = this->map_.insert(std::make_pair(key, checksum));
	return std::make_pair(iterator(result.first), result.second);
}


template <typename K>
void ChecksumMap<K>::merge(const ChecksumMap<K> &rhs)
{
#if __cplusplus >= 201703L
	this->map_.merge(rhs.map_);
#else
	this->map_.insert(rhs.map_.begin(), rhs.map_.end());
#endif
}

// Note: merge with overwrite would be:
//template <typename K>
//void ChecksumMap<K>::merge_overwrite(const ChecksumMap<K> &rhs)
//{
//	for(auto& it : rhs.map_)
//	{
//		this->map_[it.first] = it.second;
//	}
//}


template <typename K>
void ChecksumMap<K>::erase(const K &key)
{
	this->map_.erase(key);
}


template <typename K>
void ChecksumMap<K>::clear()
{
	this->map_.clear();
}


template <typename K>
ChecksumMap<K>& ChecksumMap<K>::operator = (const ChecksumMap<K> &rhs)
{
	this->map_ = rhs.map_;
	return *this;
}

// The full path silences a doxygen 1.8.15 warning that reads
// "no uniquely matching class member found"
template <typename K>
ChecksumMap<K>& arcstk::v_1_0_0::details::ChecksumMap<K>::operator = (
		ChecksumMap<K> &&rhs) noexcept = default;

} // namespace details


using ChecksumSetBase = details::ChecksumMap<checksum::type>;


#endif

