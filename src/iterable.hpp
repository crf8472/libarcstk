#ifndef __LIBARCSTK_ITERABLE_HPP__
#define __LIBARCSTK_ITERABLE_HPP__

/**
 * \file
 *
 * \brief Private API for iterable aggregate.
 */

#include <algorithm> // for copy
#include <initializer_list>
#include <cstddef>   // for size_t
#include <memory>
#include <numeric>   // for accumulate
#include <sstream>
#include <stdexcept> // for out_of_range


namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

/**
 * \brief Base class template for an iterable sequence container.
 */
template <typename T>
class IterableSequence
{

public:

	/**
	 * \brief Value type of this instance
	 */
	using value_type = T;

	/**
	 * \brief Iterator of this instance
	 */
	using iterator = T*;

	/**
	 * \brief Const iterator of this instance
	 */
	using const_iterator = const T*;

	/**
	 * \brief Number of elements of this object
	 */
	using size_type = std::size_t;

	/**
	 * \brief Constructor
	 *
	 * \param[in] size Number of elements in this instance
	 */
	explicit IterableSequence(const size_type size)
		: elements_ { std::make_unique<T[]>(size) }
		, size_     { size }
	{
		// empty
	}

	/**
	 * \brief Constructor
	 *
	 * \param[in] list Initializer list
	 */
	IterableSequence(std::initializer_list<T> list)
		: IterableSequence(list.size())
	{
		std::copy_n(std::begin(list), list.size(), elements_.get());
	}

	/**
	 * \brief Virtual default destructor
	 */
	virtual ~IterableSequence() noexcept = default;

	/**
	 * \brief Number of elements in this instance
	 *
	 * \return Number of elements in this instance
	 */
	size_type size() const noexcept
	{
		return size_;
	}

	/**
	 * \brief Start of the sequence
	 *
	 * \return First element
	 */
	iterator begin()
	{
		return const_cast<iterator>(
			(static_cast<const decltype(this)>(this))->begin());
	}

	/**
	 * \brief End of the sequence
	 *
	 * \return Behind last element
	 */
	iterator end()
	{
		return const_cast<iterator>(
			(static_cast<const decltype(this)>(this))->end());
	}

	/**
	 * \brief Start of the sequence
	 *
	 * \return First element
	 */
	const_iterator begin() const
	{
		return this->cbegin();
	}

	/**
	 * \brief End of the sequence
	 *
	 * \return Behind last element
	 */
	const_iterator end() const
	{
		return this->cend();
	}

	/**
	 * \brief Start of the sequence
	 *
	 * \return First element
	 */
	const_iterator cbegin() const
	{
		return elements_.get();
	}

	/**
	 * \brief End of the sequence
	 *
	 * \return Behind last element
	 */
	const_iterator cend() const
	{
		return this->cbegin() + this->size();
	}

	/**
	 * \brief Access element per index with bounds-check
	 *
	 * \param[in] index Element index to access
	 *
	 * \return Element at index
	 *
	 * \throw std::out_of_range If index is bigger than size() - 1
	 */
	T& at(const size_type index)
	{
		return const_cast<T&>(
			(static_cast<const IterableSequence<T>*>(this))->at(index));
	}

	/**
	 * \brief Access element per index with bounds-check
	 *
	 * \param[in] index Element index to access
	 *
	 * \return Element at index
	 *
	 * \throw std::out_of_range If index is bigger than size() - 1
	 */
	const T& at(const size_type index) const
	{
		if (index >= this->size())
		{
			auto ss { std::stringstream {} };
			ss << "Index too big: " << index
				<< " where size is only " << this->size();

			throw std::out_of_range(ss.str());
		}

		return this->operator[](index);
	}

	/**
	 * \brief Access element per index
	 *
	 * \param[in] index Element index to access
	 *
	 * \return Element at index
	 */
	const T& operator [] (const size_type index) const
	{
		return elements_[index];
	}

	/**
	 * \brief Access element per index
	 *
	 * \param[in] index Element index to access
	 *
	 * \return Element at index
	 */
	T& operator [] (const size_type index)
	{
		return const_cast<T&>(
			(static_cast<const IterableSequence<T>*>(this))->operator[](index));
	}

	/**
	 * \brief Clone this object
	 *
	 * \return A deep copy of the instance
	 */
	std::unique_ptr<IterableSequence> clone() const
	{
		return this->do_clone();
	}


protected:

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs Right hand side of the copy operation
	 */
	IterableSequence(const IterableSequence &rhs)
		: IterableSequence(rhs.size())
	{
		this->copy_elements(rhs);
	}

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Right hand side of the move operation
	 */
	IterableSequence(IterableSequence &&rhs) noexcept = default;

	/**
	 * \brief Equality
	 *
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE if the instances are equal, otherwise false
	 */
	bool equals(const IterableSequence &rhs) const noexcept
	{
		return size_ == rhs.size_
			and std::equal(elements_.get(), elements_.get() + size_,
				rhs.elements_.get());
	}

	/**
	 * \brief Swap
	 *
	 * \param[in] rhs Right hand side to swap
	 */
	void swap(IterableSequence &rhs) noexcept
	{
		using std::swap;

		swap(this->elements_, rhs.elements_);
		swap(this->size_,     rhs.size_);
	}

	/**
	 * \brief Copy assignment
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The assigned instance
	 */
	IterableSequence& operator = (const IterableSequence &rhs)
	{
		// Self-assignment check left out intentionally

		auto clone { rhs.clone() };
		this->swap(*clone);
		return *this;
	}

	/**
	 * \brief Move assignment
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The assigned instance
	 */
	IterableSequence& operator = (IterableSequence &&rhs) noexcept = default;


protected:

	/**
	 * \brief Initialize the instance as a deep copy of another instance
	 *
	 * This function presupposes that the \c elements_ array has already been
	 * allocated and that \c size_ has been set.
	 *
	 * \param[in] other Instance to clone
	 */
	void copy_elements(const IterableSequence &rhs)
	{
		std::copy_n(rhs.elements_.get(), rhs.size(), this->elements_.get());
	}

	/**
	 * \brief Clone this instance.
	 *
	 * This is for use in do_clone() implementations of subclasses.
	 *
	 * \return Deep copy of this instance.
	 */
	std::unique_ptr<IterableSequence> clone_base() const
	{
		auto cloned { this->do_create(this->size()) };
		cloned->copy_elements(*this);
		return cloned;
	}


private:

	/**
	 * \brief Implements clone()
	 *
	 * \return A deep copy of the instance
	 */
	virtual std::unique_ptr<IterableSequence> do_clone() const
	{
		return this->clone_base();
	}

	/**
	 * \brief Create an instance polymorphically
	 *
	 * \param[in] size The constructor parameter to construct the instance
	 *
	 * \return An empty instance of this type
	 */
	virtual std::unique_ptr<IterableSequence> do_create(const size_type size)
		const
	= 0;
	// This pure virtual function also prevents instantiating

	/**
	 * \brief Actual aggregate elements
	 */
	std::unique_ptr<T[]> elements_;

	/**
	 * \brief Number of aggregate elements
	 */
	size_type size_;
};

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif
