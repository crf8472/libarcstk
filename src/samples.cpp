/**
 * \file samples.cpp Implementation of SampleSequence and SampleBlock
 */

#ifndef __LIBARCS_SAMPLES_HPP__
#include "samples.hpp"
#endif

namespace arcs
{


/**
 * Private implementation of SampleBlock.
 */
class SampleBlock::Impl final
{

private: /* types */

	using container_type = std::vector<uint32_t>;


public: /* types */

	using iterator = container_type::iterator;

	using const_iterator = container_type::const_iterator;


public: /* methods */

	/**
	 * Constructor.
	 *
	 * Construct the instance with a fixed capacity.
	 *
	 * \param capacity Capacity of the container in number of PCM 32 bit samples
	 */
	explicit Impl(const std::size_t capacity);

	/**
	 * Return iterator pointing to the beginning
	 *
	 * \return iterator pointing to the beginning
	 */
	iterator begin();

	/**
	 * Return iterator pointing to the end
	 *
	 * \return iterator pointing to the end
	 */
	iterator end();

	/**
	 * Return const_iterator pointing to the beginning
	 *
	 * \return const_iterator pointing to the beginning
	 */
	const_iterator cbegin() const;

	/**
	 * Return const_iterator pointing to the end
	 *
	 * \return const_iterator pointing to the end
	 */
	const_iterator cend() const;

	/**
	 * Actual number of elements in the instance.
	 *
	 * \return Actual number of elements in the container
	 */
	std::size_t size() const;

	/**
	 * Capacity of this instance.
	 *
	 * \return Capacity of this instance in number of PCM 32 bit samples
	 */
	std::size_t capacity() const;

	/**
	 * \todo This is due to a current bug and is to be removed
	 */
	void resize(std::size_t num_samples);

	/**
	 * Returns TRUE if the instance holds no elements.
	 *
	 * \return TRUE if the instance holds no elements, otherwise FALSE
	 */
	bool empty() const;

	/**
	 * Pointer to the start of the samples.
	 *
	 * \return Raw pointer to the first of the samples.
	 */
	uint32_t* front();


private: /* members */

	/**
	 * Representation of the samples in the wrapped container
	 */
	container_type container_;
};


SampleBlock::Impl::Impl(const std::size_t capacity)
	: container_(capacity)
{
	// empty
}


SampleBlock::Impl::iterator SampleBlock::Impl::begin()
{
	return container_.begin();
}


SampleBlock::Impl::iterator SampleBlock::Impl::end()
{
	return container_.end();
}


SampleBlock::Impl::const_iterator SampleBlock::Impl::cbegin() const
{
	return container_.cbegin();
}


SampleBlock::Impl::const_iterator SampleBlock::Impl::cend() const
{
	return container_.cend();
}


std::size_t SampleBlock::Impl::size() const
{
	return container_.size();
}


std::size_t SampleBlock::Impl::capacity() const
{
	return container_.capacity();
}


void SampleBlock::Impl::resize(std::size_t num_samples)
{
	container_.resize(num_samples);
}


bool SampleBlock::Impl::empty() const
{
	return container_.empty();
}


uint32_t* SampleBlock::Impl::front()
{
	return &container_.front();
}


// SampleBlock


SampleBlock::SampleBlock(const std::size_t capacity)
	: impl_(std::make_unique<SampleBlock::Impl>(capacity))
{
	// empty
}


SampleBlock::SampleBlock(SampleBlock&& rhs) noexcept
	: impl_(std::move(rhs.impl_))
{
	// empty
}


SampleBlock::~SampleBlock() noexcept = default;


SampleBlock::iterator SampleBlock::begin()
{
	return SampleBlockIterator<false>(impl_->begin());
}


SampleBlock::iterator SampleBlock::end()
{
	return SampleBlockIterator<false>(impl_->end());
}


SampleBlock::const_iterator SampleBlock::begin() const
{
	return SampleBlockIterator<true>(impl_->cbegin());
}


SampleBlock::const_iterator SampleBlock::end() const
{
	return SampleBlockIterator<true>(impl_->cend());
}


SampleBlock::const_iterator SampleBlock::cbegin() const
{
	return SampleBlockIterator<true>(impl_->cbegin());
}


SampleBlock::const_iterator SampleBlock::cend() const
{
	return SampleBlockIterator<true>(impl_->cend());
}


std::size_t SampleBlock::size() const
{
	return impl_->size();
}


std::size_t SampleBlock::capacity() const
{
	return impl_->capacity();
}


void SampleBlock::set_size(std::size_t num_samples)
{
	return impl_->resize(num_samples);
}


bool SampleBlock::empty() const
{
	return impl_->empty();
}


uint32_t* SampleBlock::front()
{
	return impl_->front();
}


SampleBlock& SampleBlock::operator = (SampleBlock&& rhs) noexcept
{
	impl_ = std::move(rhs.impl_);
	return *this;
}


} // namespace arcs


