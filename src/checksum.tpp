// vim:ft=cpp
#ifndef __LIBARCS_CHECKSUM_HPP__
#error "Do not include checksum.tpp directly, include checksum.hpp instead"
#endif

#ifndef __LIBARCS_CHECKSUM_TPP__
#define __LIBARCS_CHECKSUM_TPP__


// ChecksumListIterator


template <typename K, bool is_const>
ChecksumListIterator<K, is_const>::ChecksumListIterator(
		const WrappedIteratorType &it)
	: it_(it)
{
	// empty
}


template <typename K, bool is_const>
ChecksumListIterator<K, is_const>::ChecksumListIterator(
		const ChecksumListIterator<K, false> &rhs)
	: it_(rhs.it_)
{
	// empty
}


template <typename K, bool is_const>
typename ChecksumListIterator<K, is_const>::reference
	ChecksumListIterator<K, is_const>::operator * ()
{
	return (*it_).second;
}


template <typename K, bool is_const>
ChecksumListIterator<K, is_const>& ChecksumListIterator<K, is_const>::operator ++ ()
{
	++it_;
	return *this;
}


template <typename K, bool is_const>
ChecksumListIterator<K, is_const>& ChecksumListIterator<K, is_const>::operator -- ()
{
	--it_;
	return *this;
}


// ChecksumList


template <typename K>
	ChecksumList<K>::ChecksumList()
	: map_()
{
	// empty
}


template <typename K>
	ChecksumList<K>::ChecksumList(const ChecksumList &rhs)
= default;


template <typename K>
	ChecksumList<K>::ChecksumList(ChecksumList &&rhs) noexcept
= default;


template <typename K>
	ChecksumList<K>::~ChecksumList() noexcept
= default;


// ChecksumList : Accessors


template <typename K>
typename ChecksumList<K>::const_iterator
	ChecksumList<K>::begin() const
{
	return const_iterator(this->map_.begin());
}


template <typename K>
typename ChecksumList<K>::const_iterator
	ChecksumList<K>::cbegin() const
{
	return const_iterator(this->map_.cbegin());
}


template <typename K>
typename ChecksumList<K>::const_iterator
	ChecksumList<K>::end() const
{
	return const_iterator(this->map_.end());
}


template <typename K>
typename ChecksumList<K>::const_iterator
	ChecksumList<K>::cend() const
{
	return const_iterator(this->map_.cend());
}


template <typename K>
typename ChecksumList<K>::const_iterator
	ChecksumList<K>::find(const K &key) const
{
	return const_iterator(this->map_.find(key));
}


template <typename K>
bool ChecksumList<K>::contains(const K &key) const
{
#if __cplusplus >= 201703L
	return this->map_.contains(key); // C++20
#else
	return map_.find(key) != map_.end();
#endif
}


template <typename K>
std::set<K> ChecksumList<K>::keys() const
{
	std::set<K> keys;

	for (const auto& entry : this->map_)
	{
		keys.insert(keys.end(), entry.first);
	}

	return keys;
}


template <typename K>
std::size_t ChecksumList<K>::size() const
{
	return this->map_.size();
}


template <typename K>
bool ChecksumList<K>::empty() const
{
	return this->map_.empty();
}


template <typename K>
bool ChecksumList<K>::operator == (const ChecksumList<K> &rhs) const
{
	return this->map_ == rhs.map_;
}


template <typename K>
bool ChecksumList<K>::operator != (const ChecksumList<K> &rhs) const
{
	return not (*this == rhs);
}


// ChecksumList : Modifiers


template <typename K>
typename ChecksumList<K>::iterator
	ChecksumList<K>::begin()
{
	return iterator(this->map_.begin());
}


template <typename K>
typename ChecksumList<K>::iterator
	ChecksumList<K>::end()
{
	return iterator(this->map_.end());
}


template <typename K>
typename ChecksumList<K>::iterator
	ChecksumList<K>::find(const K &key)
{
	return iterator(this->map_.find(key));
}


template <typename K>
std::pair<typename ChecksumList<K>::iterator, bool>
	ChecksumList<K>::insert(const K &key, const Checksum &checksum)
{
	auto result = this->map_.insert(std::make_pair(key, checksum));
	return std::make_pair(iterator(result.first), result.second);
}


template <typename K>
void ChecksumList<K>::merge(const ChecksumList<K> &rhs)
{
#if __cplusplus >= 201703L
	this->map_.merge(rhs.map_);
#else
	this->map_.insert(rhs.map_.begin(), rhs.map_.end());
#endif
}

// Note: merge with overwrite would be:
//template <typename K>
//void ChecksumList<K>::merge_overwrite(const ChecksumList<K> &rhs)
//{
//	for(auto& it : rhs.map_)
//	{
//		this->map_[it.first] = it.second;
//	}
//}


template <typename K>
void ChecksumList<K>::erase(const K &key)
{
	this->map_.erase(key);
}


template <typename K>
void ChecksumList<K>::clear()
{
	this->map_.clear();
}


template <typename K>
ChecksumList<K>& ChecksumList<K>::operator =
	(const ChecksumList<K> &rhs)
{
	this->map_ = rhs.map_;
	return *this;
}


template <typename K>
ChecksumList<K>& ChecksumList<K>::operator =
	(ChecksumList<K> &&rhs) noexcept = default;

#endif

