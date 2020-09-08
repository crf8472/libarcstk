#ifndef __LIBARCSTK_POLICIES_NONPUBLIC_HPP__
#define __LIBARCSTK_POLICIES_NONPUBLIC_HPP__

/**
 * \file
 *
 * \brief AppendableSequence is a common implementation for some containers.
 */

#include <algorithm>         // for equal
#include <initializer_list>
#include <stdexcept>         // for domain_error
#include <memory>
#include <vector>

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

/**
 * \brief Base class template for a sequence container that can be appended to,
 * but provides only read iterators.
 *
 * ARBlock and ARResponse are implemented in terms of this. The reason is to
 * hide the iterator types of the STL-container used for implementation to the
 * outside world.
 *
 * The sequence must be appendable since ARBlock and ARResponse are objects
 * constructed by parsing. There is no strong guarantee that the expected number
 * of elements will be parsed in the input. The class has to be robust against
 * this kind of problem. Hence the sequence must allow to append more elements
 * than expected. On the other hand, since the content of this instance are
 * parsed values, it is not required to provide write access via iterators.
 *
 * One could consider a type erasing iterator container, like it is used
 * in module 'calculate' but it seems to be less effort to just use raw
 * pointers to the base type as iterators.
 */
template <typename T>
class AppendableSequence
{

	/**
	 * \brief Container implementation for actual elements
	 */
	std::vector<T> elements_; // surprise!


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
	using size_type = typename decltype(elements_)::size_type;

	/**
	 * \brief Constructor for appending
	 *
	 * This constructor reserves memory for the specified number of elements
	 * but does not instantiate them. Therefore, an assignment to particular
	 * index positions will fail.
	 *
	 * \param[in] capacity Number of elements in this instance
	 */
	explicit AppendableSequence(const size_type capacity)
		: elements_ {}
	{
		elements_.reserve(capacity);
	}

	/**
	 * \brief Constructor
	 *
	 * \param[in] list Initializer list
	 */
	AppendableSequence(std::initializer_list<T> list)
		: elements_(list)
	{
		/* empty */
	}

	/**
	 * \brief Virtual default destructor
	 */
	virtual ~AppendableSequence() noexcept = default;

	/**
	 * \brief Number of elements in this instance
	 *
	 * \return Number of elements in this instance
	 */
	size_type size() const noexcept
	{
		return elements_.size();
	}

	/**
	 * \brief Number of elements that could be stored in this instance
	 *
	 * Calls to append() may change the capacity. You may use capacity() to
	 * check whether append() will invalidate all current iterators or the
	 * past-the-end iterator only.
	 *
	 * \return Number of elements that could be stored in this instance
	 */
	size_type capacity() const noexcept
	{
		return elements_.capacity();
	}

	/**
	 * \brief Start of the sequence
	 *
	 * \return First element
	 */
	iterator begin()
	{
		return const_cast<iterator>(
			//(static_cast<const decltype(this)>(this))->begin());
			static_cast<const AppendableSequence&>(*this).begin());
	}

	/**
	 * \brief End of the sequence
	 *
	 * \return Behind last element
	 */
	iterator end()
	{
		return const_cast<iterator>(
			//(static_cast<const decltype(this)>(this))->end());
			static_cast<const AppendableSequence&>(*this).end());
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
		return &(*elements_.begin());
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
	const T& at(const size_type index) const
	{
		return elements_.at(index);
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
			static_cast<const AppendableSequence&>(*this).at(index));
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
			static_cast<const AppendableSequence&>(*this)[index]);
	}

	/**
	 * \brief Append an element to the sequence.
	 *
	 * Invalidates the past-the-end-iterator.
	 *
	 * If the new size is greater than the capacity, then all iterators are
	 * invalidated.
	 *
	 * \param[in] element The element to be appended
	 *
	 * \return The index position of the new element
	 */
	int append(const T &element)
	{
		elements_.push_back(element);
		return this->size() - 1;
	}

	/**
	 * \copydoc append(const T&)
	 */
	int append(T &&element)
	{
		elements_.push_back(std::move(element));
		return this->size() - 1;
	}

	/**
	 * \brief Clone this object
	 *
	 * \return A deep copy of the instance
	 */
	std::unique_ptr<AppendableSequence> clone() const
	{
		return this->do_clone();
	}

	/**
	 * \brief Equality
	 *
	 * This function can be used polymorphically.
	 *
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE if the instances are equal, otherwise false
	 */
	bool equals(const AppendableSequence &rhs) const noexcept
	{
		return typeid(*this) == typeid(rhs)
			and this->size() == rhs.size()
			and std::equal(elements_.data(), elements_.data() + this->size(),
				rhs.elements_.data())
			and this->do_equals(rhs);
	}
	// typeid check is necessary since equals() is public

	/**
	 * \brief Swap
	 *
	 * This function will throw iff \c rhs is not of identical type with
	 * \c this.
	 *
	 * \param[in] rhs Right hand side to swap
	 *
	 * \throw std::domain_error If \c rhs is not of same type as \c this
	 */
	void swap(AppendableSequence &rhs)
	{
		if (typeid(*this) != typeid(rhs))
		{
			throw std::domain_error(
				"Refuse to swap AppendableSequences of different typeid");
		}

		using std::swap;

		swap(this->elements_, rhs.elements_);

		this->do_swap(rhs);
	}
	// typeid check is necessary since swap() is public


protected:

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs Right hand side of the copy operation
	 */
	AppendableSequence(const AppendableSequence &rhs)
		: elements_(rhs.elements_)
	{
		// empty   (Cleaner on behalf of API was this->copy_elements(rhs).)
	}

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Right hand side of the move operation
	 */
	AppendableSequence(AppendableSequence &&rhs) noexcept = default;

	/**
	 * \brief Copy assignment
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The assigned instance
	 */
	AppendableSequence& operator = (const AppendableSequence &rhs)
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
	AppendableSequence& operator = (AppendableSequence &&rhs) noexcept = default;


protected:

	/**
	 * \brief Initialize the instance as a deep copy of another instance
	 *
	 * This function presupposes that the \c elements_ array has already been
	 * allocated and that \c size_ has been set.
	 *
	 * \param[in] other Instance to clone
	 */
	void copy_elements(const AppendableSequence &rhs)
	{
		this->elements_ = rhs.elements_;
	}

	/**
	 * \brief Clone this instance.
	 *
	 * This is for use in do_clone() implementations of subclasses.
	 *
	 * \return Deep copy of this instance.
	 */
	std::unique_ptr<AppendableSequence> clone_base() const
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
	virtual std::unique_ptr<AppendableSequence> do_clone() const
	{
		return this->clone_base(); // XXX This should be pure virtual
	}

	/**
	 * \brief Create an instance polymorphically
	 *
	 * \param[in] size The constructor parameter to construct the instance
	 *
	 * \return An empty instance of this type
	 */
	virtual std::unique_ptr<AppendableSequence> do_create(const size_type size)
		const
	= 0;

	/**
	 * \brief Implements equals()
	 *
	 * \param[in] rhs The right hand side of the comparison
	 *
	 * \return TRUE iff \c rhs equals \c this, otherwise FALSE
	 */
	virtual bool do_equals(const AppendableSequence &/* rhs */) const noexcept
	{
		return true;
	}

	/**
	 * \brief Implements swap()
	 *
	 * \param[in] rhs The right hand side to swap
	 */
	virtual void do_swap(AppendableSequence &/* rhs */) noexcept
	{
		// empty;
	}

	// This pure virtual function also prevents instantiating

};

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk


// Commented out: tried to implement std::begin + std::end for
// AppendableSequences but did not understand that entirely.
//namespace std
//{
//
//template <class T> auto begin(arcstk::details::AppendableSequence<T>& c)
//	-> decltype(c.cbegin())
//{
//	return (const_cast<const decltype(c)>(c))->cbegin();
//}
//
////template <class C> auto begin(const C& c) -> decltype(c.begin());
//
//template <class T> auto end(arcstk::details::AppendableSequence<T>& c)
//	-> decltype(c.cend())
//	-> decltype(c.cend())
//{
//	return (const_cast<const decltype(c)>(c))->cend();
//}
//
//} // namespace std

#endif


