#ifndef LIBARCSTK_MIXINS_HPP_
#define LIBARCSTK_MIXINS_HPP_

/**
 * \file
 *
 * \brief CRTP-Mixins for enhancing classes.
 *
 * \details
 *
 * You probably should not include this file directly.
 */

#include <string>           // for string
#include <type_traits>      // for false_type, true_type, enable_if_t,
                            // is_convertible_v
#include <utility>          // for declval

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

namespace details
{
	/**
	 * \internal
	 *
	 * \brief Primary template for any type \c T that does not have a member
	 * function swap() that is <tt>noexcept</tt>.
	 *
	 * \tparam T Input type
	 */
	template <typename T, typename = void>
	struct has_swap : std::false_type {};

	/**
	 * \internal
	 *
	 * \brief Partial specialization for any type \c T that has a member
	 * function swap() that is <tt>noexcept</tt>.
	 *
	 * \tparam T Input type
	 */
	template <typename T>
	struct has_swap<T, std::enable_if_t<
		noexcept(std::declval<const T&>().equals(std::declval<const T&>()))
		&&
		std::is_convertible_v<decltype(
			std::declval<const T&>().equals(std::declval<const T&>())), bool>
	>> : std::true_type {};

	/**
	 * \internal
	 *
	 * \brief Primary template for any type \c T that does not have a member
	 * function equals() that is <tt>const noexcept</tt> and returns a type
	 * convertible to \c bool.
	 *
	 * \tparam T Input type
	 */
	template <typename T, typename = void>
	struct has_equals : std::false_type {};

	/**
	 * \internal
	 *
	 * \brief Partial specialization for any type \c T that has a member
	 * function equals() that is <tt>const noexcept</tt> and returns a type
	 * convertible to \c bool.
	 *
	 * \tparam T Input type
	 */
	template <typename T>
	struct has_equals<T, std::enable_if_t<
		noexcept(std::declval<const T&>().equals(std::declval<const T&>()))
		&&
		std::is_convertible_v<decltype(
			std::declval<const T&>().equals(std::declval<const T&>())), bool>
	>> : std::true_type {};

} // namespace details

// NOLINTBEGIN(bugprone-crtp-constructor-accessibility)
// deactivated due to over-warning: protected constructor + no virtual functions

/**
 * \internal
 *
 * \brief Adds a non-member friend swap() calling a member swap() noexcept.
 *
 * Any type \c T must implement a member function <tt>void swap(T&)
 * noexcept</tt>, because the friend swap() is declared noexcept
 * unconditionally and terminates the program if the member function throws.
 *
 * \tparam T Input type
 */
template <typename T>
struct Swap
{
	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(T& lhs, T& rhs) noexcept
	{
		lhs.swap(rhs);
	}

protected:

	Swap() = default;
	~Swap() noexcept = default;
};


/**
 * \internal
 *
 * \brief Adds a non-member friend operator==() calling a member equals()
 * const noexcept whose result type must be convertible to bool.
 *
 * \tparam T Input type
 */
template <typename T>
struct Equality
{
	/**
	 * \copydoc SNPT_nf_equality
	 */
	friend bool operator == (const T& lhs, const T& rhs) noexcept
	{
		static_assert(details::has_equals<T>::value,
			"derived class must implement: "
			"bool equals(const T&) const noexcept");

		return lhs.equals(rhs);
	}

protected:

	Equality() = default;
	~Equality() noexcept = default;
};


/**
 * \internal
 *
 * \brief Adds inequality to classes defining equality operator==.
 *
 * \tparam T Input type
 */
template <typename T>
struct Comparable
{
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

protected:

	Comparable() = default;
	~Comparable() noexcept = default;
};


/**
 * \internal
 *
 * \brief Adds all relational operators to classes defining operator<
 * (less-than) and operator== (is-equal-to).
 *
 * Any type \c T must either only inherit from TotallyOrdered<T> or only from
 * Comparable<T>, never from both which would duplicate operator!=.
 *
 * \tparam T Input type
 */
template <typename T>
struct TotallyOrdered : public Comparable<T>
{
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
		return !(lhs < rhs);
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
		return !(rhs < lhs);
	}

protected:

	TotallyOrdered() = default;
	~TotallyOrdered() noexcept = default;
};


/**
 * \internal
 *
 * \brief Adds a non-member friend to_string() calling a member to_string().
 *
 * \tparam T Input type
 */
template <typename T>
struct ToString
{
	/**
	 * \copydoc SNPT_nf_to_string
	 */
	friend std::string to_string(const T& i)
	{
		return i.to_string();
	}

protected:

	ToString() = default;
	~ToString() noexcept = default;
};
// NOLINTEND(bugprone-crtp-constructor-accessibility)

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

