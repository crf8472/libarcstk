// vim:ft=cpp
#ifndef __LIBARCS_CALCULATE_HPP__
#error "Do not include sampleblockiterator.tpp directly, include calculate.hpp instead"
#endif

#ifndef __LIBARCS_SAMPLEBLOCKITERATOR_TPP__
#define __LIBARCS_SAMPLEBLOCKITERATOR_TPP__


template <bool is_const>
SampleBlockIterator<is_const>::SampleBlockIterator(
		const WrappedIteratorType &it)
	: it_(it)
{
	// empty
}


template <bool is_const>
SampleBlockIterator<is_const>::SampleBlockIterator(
		const SampleBlockIterator<false> &rhs)
	: it_(rhs.it_) // works due to friendship
{
	// empty
}


template <bool is_const>
typename SampleBlockIterator<is_const>::reference
	SampleBlockIterator<is_const>::operator * ()
{
	return *it_;
}


template <bool is_const>
SampleBlockIterator<is_const>& SampleBlockIterator<is_const>::operator ++ ()
{
	++it_;
	return *this;
}


template <bool is_const>
SampleBlockIterator<is_const> SampleBlockIterator<is_const>::operator ++ (int)
{
	SampleBlockIterator prev_val(*this);
	this->operator++();
	return prev_val;
}


template <bool is_const>
SampleBlockIterator<is_const>& SampleBlockIterator<is_const>::operator -- ()
{
	--it_;
	return *this;
}


template <bool is_const>
SampleBlockIterator<is_const> SampleBlockIterator<is_const>::operator -- (int)
{
	SampleBlockIterator prev_val(*this);
	this->operator--();
	return prev_val;
}


template <bool is_const>
SampleBlockIterator<is_const> SampleBlockIterator<is_const>::operator + (
		const uint32_t value)
{
	return SampleBlockIterator(it_ + value);
}


template <bool is_const>
SampleBlockIterator<is_const> SampleBlockIterator<is_const>::operator - (
		const uint32_t value)
{
	return SampleBlockIterator(it_ - value);
}


template <bool is_const>
SampleBlockIterator<is_const>& SampleBlockIterator<is_const>::operator += (
		const uint32_t value)
{
	it_ += value;
	return *this;
}


template <bool is_const>
SampleBlockIterator<is_const>& SampleBlockIterator<is_const>::operator -= (
		const uint32_t value)
{
	it_ -= value;
	return *this;
}

#endif

