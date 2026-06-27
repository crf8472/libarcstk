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
#include <type_traits>      // for is_convertible_v

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
// NOLINTBEGIN(bugprone-crtp-constructor-accessibility)
// deactivated due to over-warning: protected constructor + no virtual functions

/**
 * \internal
 *
 * \brief Adds a non-member friend swap() calling a member swap() noexcept.
 */
template <typename T>
struct Swap
{
	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(const T& lhs, const T& rhs) noexcept
	{
		return lhs.swap(rhs);
	}

protected:

	Swap() = default;
	~Swap() noexcept = default;
};


/**
 * \internal
 *
 * \brief Adds a non-member friend operator==() calling a member equals()
 * noexcept.
 */
template <typename T>
struct Equality
{
	/**
	 * \copydoc SNPT_nf_equality
	 */
	friend bool operator == (const T& lhs, const T& rhs) noexcept
	{
		static_assert(has_equals<T>::value,
			"Derived class must implement: bool equals(const T&) const");

		return lhs.equals(rhs);
	}

protected:

	Equality() = default;
	~Equality() noexcept = default;

private:

		template <typename U, typename = void>
		struct has_equals : std::false_type {};


#if defined(LIBARCSTK_MACOS_BUILD) && !defined(__cpp_lib_void_t)
		// Workaround for old libc++ auf macOS
		template<typename...>
		using void_type = void;
#else
		template<typename... Args>
		using void_type = std::void_t<Args...>;
#endif

		template <typename U>
		struct has_equals<U, void_type<
			decltype(std::declval<const U>().equals(std::declval<const U&>()))
		>> : std::true_type {};
};


/**
 * \internal
 *
 * \brief Adds inequality to classes defining equality operator==.
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
 * (less-than).
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

protected:

	TotallyOrdered() = default;
	~TotallyOrdered() noexcept = default;
};


/**
 * \internal
 *
 * \brief Adds a non-member friend to_string() calling a member to_string().
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

