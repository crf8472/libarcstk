#ifndef __LIBARCSTK_CALC_CHECKSUMMAP_HPP__
#define __LIBARCSTK_CALC_CHECKSUMMAP_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Internal API: ChecksumMap
 */

#include <algorithm>     // for transform
#include <map>
#include <set>
#include <utility>

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

/**
 * \internal
 *
 * \brief Generic Map for \link Checksum Checksums \endlink and some key type.
 *
 * This is a generic container for ChecksumMaps adaptable to different
 * checksum types and different keys.
 *
 * \tparam K The key type of this instance
 */
template <typename K>
class ChecksumMap : public Comparable<ChecksumMap<K>>
{

public:

	/**
	 * \brief Iterator
	 */
	using iterator = ChecksumMapIterator<K, false>;

	/**
	 * \brief Const Iterator
	 */
	using const_iterator = ChecksumMapIterator<K, true>;

	/**
	 * \brief Size type
	 */
	using size_type = std::size_t;

	/**
	 * \brief Constructor
	 */
	ChecksumMap()
		: map_ {}
	{
		// empty
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	ChecksumMap(const ChecksumMap &rhs)
	= default;

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs The instance to move
	 */
	ChecksumMap(ChecksumMap &&rhs) noexcept
	= default;

	/**
	 * \brief Virtual default destructor
	 */
	virtual ~ChecksumMap() noexcept
	= default;

	/**
	 * \brief Returns a ChecksumMap::const_iterator to the beginning
	 *
	 * \return ChecksumMap::const_iterator to the beginning
	 */
	const_iterator cbegin() const
	{
		return const_iterator(this->map_.cbegin());
	}

	/**
	 * \brief Returns a ChecksumMap::const_iterator to the end
	 *
	 * \return ChecksumMap::const_iterator to the end
	 */
	const_iterator cend() const
	{
		return const_iterator(this->map_.cend());
	}

	/**
	 * \brief Returns a ChecksumMap::const_iterator to the beginning
	 *
	 * \return ChecksumMap::const_iterator to the beginning
	 */
	const_iterator begin() const
	{
		return this->cbegin();
	}

	/**
	 * \brief Returns a ChecksumMap::const_iterator to the end
	 *
	 * \return ChecksumMap::const_iterator to the end
	 */
	const_iterator end() const
	{
		return this->cend();
	}

	/**
	 * \brief Returns a ChecksumMap::iterator to the beginning
	 *
	 * \return ChecksumMap::iterator to the beginning
	 */
	iterator begin()
	{
		return iterator(this->map_.begin());
	}

	/**
	 * \brief Returns a ChecksumMap::iterator to the end
	 *
	 * \return ChecksumMap::iterator to the end
	 */
	iterator end()
	{
		return iterator(this->map_.end());
	}

	/**
	 * \brief Returns the number of elements contained in the instance.
	 *
	 * \return Number of elements contained in the instance.
	 */
	size_type size() const
	{
		return this->map_.size();
	}

	/**
	 * \brief Returns TRUE iff the instance contains no elements, otherwise
	 * FALSE.
	 *
	 * \return TRUE iff instance contains no elements, otherwise FALSE
	 */
	bool empty() const
	{
		return this->map_.empty();
	}

	/**
	 * \brief Returns TRUE iff the instance contains the key \c key .
	 *
	 * \param[in] key The key to lookup
	 *
	 * \return TRUE iff \c key is present in the instance, otherwise FALSE
	 */
	bool contains(const K &key) const
	{
		return map_.find(key) != map_.end();
		// TODO C++20 can do: return this->map_.contains(key);
	}

	/**
	 * \brief Returns the set of all keys contained in the instance.
	 *
	 * \return Set of keys used in this instance
	 */
	std::set<K> keys() const
	{
		std::set<K> keys;

		std::transform(this->map_.begin(), this->map_.end(),
			std::inserter(keys, keys.begin()),
			[](const map_value_type &pair)
			{
				return pair.first;
			}
		);

		return keys;
	}

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
	const_iterator find(const K &key) const
	{
		return const_iterator(this->map_.find(key));
	}

	/**
	 * \copydoc find(const K &key) const
	 */
	iterator find(const K &key)
	{
		return iterator(this->map_.find(key));
	}

	/**
	 * \brief Inserts a new key-value-pair to the instance.
	 *
	 * If the key is already present in the instance, the existing checksum will
	 * be left unmodified.
	 *
	 * The pair returned contains an iterator to the inserted Checksum and a
	 * bool that is TRUE iff the insertion was successful. If the insertion was
	 * not successful, the value FALSE is returned for the bool and the
	 * iterator will point to the element that prevented the insertion.
	 *
	 * \param[in] key The key to use
	 * \param[in] checksum The checksum for the given key
	 *
	 * \return Pair with an iterator to the inserted value and a status flag
	 */
	std::pair<iterator, bool> insert(const K &key, const Checksum &checksum)
	{
		auto result { this->map_.insert(std::make_pair(key, checksum)) };
		return std::make_pair(iterator(result.first), result.second);
	}

	/**
	 * \brief Inserts a new key-value-pair to the instance.
	 *
	 * If the key is already present in the instance, the existing checksum will
	 * be overwritten with \checksum.
	 *
	 * The pair returned contains an iterator to the inserted Checksum and a
	 * bool that is TRUE iff the insertion took place or FALSE if an existing
	 * value was overwritten.
	 *
	 * \param[in] key The key to use
	 * \param[in] checksum The checksum for the given key
	 *
	 * \return Pair with an iterator to the value and a status flag
	 */
	std::pair<iterator, bool> insert_overwrite(const K &key,
			const Checksum &checksum)
	{
	#if __cplusplus >= 201703L
		auto result {
			this->map_.insert_or_assign(std::make_pair(key, checksum)) };
	#else
		auto result { this->map_.insert(std::make_pair(key, checksum)) };
		if (!result.second)
		{
			(*result.first).second = checksum;
		}
	#endif
		return std::make_pair(iterator(result.first), result.second);
	}

	/**
	 * \brief Merge the elements of another instance into this instance.
	 *
	 * If a key in the other instance is already present in this instance, the
	 * corresponding element will be left unmodified.
	 *
	 * \param[in] rhs The list to be merged into the instance
	 */
	void merge(const ChecksumMap<K> &rhs)
	{
	#if __cplusplus >= 201703L
		this->map_.merge(rhs.map_);
	#else
		this->map_.insert(rhs.map_.begin(), rhs.map_.end());
	#endif
	}

	/**
	 * \brief Merge the elements of another instance into this instance.
	 *
	 * If a key in the other instance is already present in this instance, the
	 * corresponding element will be overwritten with the element from the other
	 * instance.
	 *
	 * \param[in] rhs The list to be merged into the instance
	 */
	void merge_overwrite(const ChecksumMap<K> &rhs)
	{
	#if __cplusplus >= 201703L
		this->map_.insert_or_assign(rhs.map_.begin(), rhs.map_.end());
	#else
		for(auto& it : rhs.map_)
		{
			this->map_[it.first] = it.second;
		}
	#endif
	}

	/**
	 * \brief Erases the element with the given key.
	 *
	 * Does nothing if the given key is not contained in the instance.
	 *
	 * \param[in] key The key to erase
	 */
	void erase(const K &key)
	{
		this->map_.erase(key);
	}

	/**
	 * \brief Erases all elements contained in the instance.
	 *
	 * After clear() the size of the container will be \c 0 .
	 */
	void clear()
	{
		this->map_.clear();
	}


	ChecksumMap<K>& operator = (const ChecksumMap<K> &rhs)
	{
		this->map_ = rhs.map_;
		return *this;
	}


	ChecksumMap<K>& operator = (ChecksumMap<K> &&rhs) noexcept
	= default;


	friend bool operator == (const ChecksumMap &lhs,
			const ChecksumMap &rhs) noexcept
	{
		return lhs.map_ == rhs.map_;
	}

private:

	/**
	 * \brief Internal representation.
	 */
	std::map<K, Checksum> map_;

	/**
	 * \brief Value type of the internal map.
	 *
	 * This is usually std::pair<K, Checksum>.
	 */
	using map_value_type = typename std::map<K, Checksum>::value_type;
};

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif

