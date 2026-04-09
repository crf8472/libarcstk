#ifndef LIBARCSTK_POLICIES_HPP_
#define LIBARCSTK_POLICIES_HPP_

/**
 * \file
 *
 * \brief Policies for enhancing classes.
 *
 * \details
 *
 * You probably should not include this file directly.
 */

#include <cstddef>          // for size_t, nullptr
#include <limits>           // for numeric_limits
#include <memory>           // for addressof

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

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
	~TotallyOrdered() override = default;

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
 * \tparam V Some value type
 * \tparam I Some index type, std::ptrdiff_t if omitted
 */
template<typename V, typename I = std::ptrdiff_t>
class IteratorElement final
{
public:

	/**
	 * \brief Value type to represent
	 */
	using value_type = V;

	/**
	 * \brief Index type to represent
	 */
	using index_type = I;

private:

	/**
	 * \brief Internal index position.
	 */
	index_type index_;

	/**
	 * \brief Internal value on index position.
	 */
	value_type element_;

public:

	/**
	 * \brief Constructor.
	 */
	IteratorElement()
		: index_   { std::numeric_limits<decltype( index_ )>::max() }
		, element_ { /* default */ }
	{
		// empty
	} // XXX Max value is not strongly guaranteed to never occurr in real life

	/**
	 * \brief Constructor.
	 *
	 * \param[in] index   Index position
	 * \param[in] element Element value
	 */
	IteratorElement(const index_type index, const value_type& element)
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
	index_type index() const
	{
		return index_;
	}

	/**
	 * \brief Element value.
	 *
	 * \return Element value
	 */
	const value_type& element() const
	{
		return element_;
	}

	/**
	 * \brief Explicit conversion to \c value_type.
	 *
	 * \return Element value
	 */
	explicit operator value_type() const
	{
		return this->element();
	}

	// pointer stuff

	/**
	 * \copydoc SNPT_mf_deref
	 */
	const value_type& operator * () const
	{
		return this->element();
	}

	/**
	 * \copydoc SNPT_mf_arrow
	 */
	const value_type* operator -> () const
	{
		return std::addressof(this->element_);
	}
	// https://stackoverflow.com/a/4923639
	// https://stackoverflow.com/a/64275124
};

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

