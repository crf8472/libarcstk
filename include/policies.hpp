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

#include <string>           // for string

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

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
		return lhs.equals(rhs);
	}

protected:

	Equality() = default;
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
};

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

