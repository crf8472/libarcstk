#ifndef __LIBARCSTK_POLICIES_HPP__
#define __LIBARCSTK_POLICIES_HPP__

/**
 * \file
 *
 * \brief Policies for enhancing classes.
 */

#include <cstddef>          // for size_t, nullptr
#include <limits>           // for numeric_limits
#include <memory>           // for addressof

namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \internal
 *
 * \brief Adds inequality to classes defining equality operator==.
 */
template <typename T>
struct Comparable
{
	virtual ~Comparable() = default;

	/**
	 * \brief Inequality.
	 *
	 * \param[in] lhs Left hand side of the comparison
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE iff not \c lhs == \c rhs, otherwise FALSE
	 */
	friend bool operator != (const T& lhs, const T& rhs) noexcept
	{
		return !(lhs == rhs);
	}
};


/**
 * \internal
 *
 * \brief Adds all relational operators to classes defining operator<
 * (less-than).
 */
template <typename T>
struct TotallyOrdered : public Comparable<T>
{
	virtual ~TotallyOrdered() = default;

	/**
	 * \brief Greater-than.
	 *
	 * \param[in] lhs Left hand side of the comparison
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE iff \c lhs > \c rhs, otherwise FALSE
	 */
	friend bool operator > (const T& lhs, const T& rhs) noexcept
	{
		return rhs < lhs;
	}

	/**
	 * \brief Greater-or-equal-than.
	 *
	 * \param[in] lhs Left hand side of the comparison
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE iff \c lhs >= \c rhs, otherwise FALSE
	 */
	friend bool operator >= (const T& lhs, const T& rhs) noexcept
	{
		return !(rhs > lhs);
	}

	/**
	 * \brief Less-or-equal-than.
	 *
	 * \param[in] lhs Left hand side of the comparison
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE iff \c lhs <= \c rhs, otherwise FALSE
	 */
	friend bool operator <= (const T& lhs, const T& rhs) noexcept
	{
		return !(lhs > rhs);
	}
};


/**
 * \internal
 *
 * \brief Store current element together with its current index.
 *
 * Intended to be used with types without operator ->, e.g. primitive types.
 *
 * \tparam T Some object type
 */
template<typename T>
class IteratorElement final
{
	std::size_t index_;
	T element_;

public:

	/**
	 * \brief Value type to represent
	 */
	using value_type = T;

	/**
	 * \brief Constructor.
	 */
	IteratorElement()
		: index_   { std::numeric_limits<decltype( index_ )>::max() }
		, element_ { /* default */ }
	{
		// empty
	} // TODO Max value is not strongly guaranteed to never occurr in real life

	/**
	 * \brief Constructor.
	 *
	 * \param[in] index   Index position
	 * \param[in] element Element value
	 */
	IteratorElement(const std::size_t index, const T& element)
		: index_   { index   }
		, element_ { element }
	{
		// empty
	}

	/**
	 * \brief Index position of the element.
	 *
	 * \return Index position of the element
	 */
	std::size_t index() const
	{
		return index_;
	}

	/**
	 * \brief Element value.
	 *
	 * \return Element value
	 */
	const T& element() const
	{
		return element_;
	}

	T* operator->()
	{
		return std::addressof(element_);
	}
	// https://stackoverflow.com/questions/64274156/operator-for-an-iterator-that-returns-a-temporary
	// https://stackoverflow.com/a/4923639
};

} // namespace v_1_0_0
} // namespace arcstk

#endif

