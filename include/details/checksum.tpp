// vim:ft=cpp
#ifndef __LIBARCSTK_CHECKSUM_HPP__
#error "Do not include checksum.tpp directly, include checksum.hpp instead"
#endif

#ifndef __LIBARCSTK_CHECKSUM_TPP__
#define __LIBARCSTK_CHECKSUM_TPP__

/**
 * \file
 *
 * \brief Implementation of ChecksumMapIterator and ChecksumMap
 */


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
ChecksumMapIterator<K, is_const>& ChecksumMapIterator<K, is_const>::operator ++ ()
{
	++it_;
	return *this;
}


template <typename K, bool is_const>
ChecksumMapIterator<K, is_const>& ChecksumMapIterator<K, is_const>::operator -- ()
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
std::size_t ChecksumMap<K>::size() const
{
	return this->map_.size();
}


template <typename K>
bool ChecksumMap<K>::empty() const
{
	return this->map_.empty();
}


template <typename K>
bool ChecksumMap<K>::operator == (const ChecksumMap<K> &rhs) const
{
	return this->map_ == rhs.map_;
}


template <typename K>
bool ChecksumMap<K>::operator != (const ChecksumMap<K> &rhs) const
{
	return not (*this == rhs);
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

#endif

