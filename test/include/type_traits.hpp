#ifndef __LIBARCSTK_TYPE_TRAITS_HPP__
#define __LIBARCSTK_TYPE_TRAITS_HPP__

/**
 * \file
 *
 * \brief Tools for types.
 */


#include <string>           // for string
#include <type_traits>      // for true_type, false_type, is_same, bool_constant


namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
namespace meta
{

// ---- equals() and == ----

template<class C, class = void>
struct member_equals_invokeable : std::false_type
{ /* empty */ };

template<class C>
struct member_equals_invokeable <C,
	std::void_t<decltype(
		std::declval<const C>().equals( std::declval<const C&>() )
	)>> : std::true_type
{ /* empty */ };

template <typename T>
constexpr bool has_member_equals_f()
{
	/* symbol is invokeable and const ? */
    if constexpr(! member_equals_invokeable<T>::value)
	{
		return false;
	} else
	{
		using RTYPE = decltype(
				std::declval<const T>().equals(std::declval<const T&>()) );

		/* return type is bool ? */
		if constexpr( ! std::is_same< bool, RTYPE>::value )
		{
			return false;
		}

		constexpr bool is_noexcept =
			noexcept(std::declval<const T>().equals(std::declval<const T&>()));

		/* function is noexcept ? */
		if constexpr( ! is_noexcept )
		{
			return false;
		}
	}

	return true;
};

template <typename T>
struct has_member_equals: std::bool_constant< has_member_equals_f<T>()>
{ /* empty */ };

template <typename T>
constexpr bool has_member_equals_v = has_member_equals<T>::value;



template <typename A, typename B, typename = void>
struct nonmember_equality_invokeable : std::false_type
{ /* empty */ };

template <typename A, typename B>
struct nonmember_equality_invokeable<A, B,
	std::void_t< decltype(
		std::declval<const A&>() == std::declval<const B&>()
	) >> : std::true_type
{ /* empty */ };

template <typename A, typename B = A>
constexpr bool has_nonmember_equality_f()
{
	/* symbol is invokeable and const ? */
    if constexpr(! nonmember_equality_invokeable<A, B>::value)
	{
		return false;
	} else
	{
		using RTYPE = decltype(
				std::declval<const A&>() == std::declval<const B&>() );

		/* return type is bool ? */
		if constexpr( ! std::is_same< bool, RTYPE>::value )
		{
			return false;
		}

		constexpr bool is_noexcept =
			noexcept(std::declval<const A&>() == std::declval<const B&>());

		/* function is noexcept ? */
		if constexpr( ! is_noexcept )
		{
			return false;
		}
	}

	return true;
};

template <typename T>
struct has_nonmember_equality: std::bool_constant<
	has_nonmember_equality_f<T>() >
{ /* empty */ };

template <typename T>
constexpr bool has_nonmember_equality_v = has_nonmember_equality<T>::value;




template <typename A, typename B, typename = void>
struct nonmember_inequality_invokeable : std::false_type
{ /* empty */ };

template <typename A, typename B>
struct nonmember_inequality_invokeable<A, B,
	std::void_t< decltype(
		std::declval<const A&>() != std::declval<const B&>()
	) >> : std::true_type
{ /* empty */ };

template <typename A, typename B = A>
constexpr bool has_nonmember_inequality_f()
{
	/* symbol is invokeable and const ? */
    if constexpr(! nonmember_inequality_invokeable<A, B>::value)
	{
		return false;
	} else
	{
		using RTYPE = decltype(
				std::declval<const A&>() != std::declval<const B&>() );

		/* return type is bool ? */
		if constexpr( ! std::is_same< bool, RTYPE>::value )
		{
			return false;
		}

		constexpr bool is_noexcept =
			noexcept(std::declval<const A&>() != std::declval<const B&>());

		/* function is noexcept ? */
		if constexpr( ! is_noexcept )
		{
			return false;
		}
	}

	return true;
};

template <typename T>
struct has_nonmember_inequality: std::bool_constant<
	has_nonmember_inequality_f<T>() >
{ /* empty */ };

template <typename T>
constexpr bool has_nonmember_inequality_v = has_nonmember_inequality<T>::value;



template <typename T>
struct is_comparable : std::bool_constant<
	has_member_equals_v<T>        &&
	has_nonmember_equality_v<T>   &&
	has_nonmember_inequality_v<T>
>
{ /* empty */ };

template <typename T>
constexpr bool is_comparable_v = is_comparable<T>::value;


// ---- to_string() ----


template<class C, class = void>
struct member_tostring_invokeable : std::false_type
{ /* empty */ };

template<class C>
struct member_tostring_invokeable <C,
	std::void_t<decltype(/* */ std::declval<const C>().to_string() /* */)>> :
		std::true_type
{ /* empty */ };


template <typename T>
constexpr bool has_member_tostring_f()
{
	/* symbol is invokeable and const ? */
    if constexpr(! member_tostring_invokeable<T>::value)
	{
		return false;
	} else
	{
		using RTYPE = decltype( std::declval<const T>().to_string() );

		/* return type is string ? */
		if constexpr( ! std::is_same< std::string, RTYPE>::value )
		{
			return false;
		}
	}

	return true;
};

template <typename T>
struct has_member_tostring : std::bool_constant< has_member_tostring_f<T>() >
{ /* empty */ };

template <typename T>
constexpr bool has_member_tostring_v = has_member_tostring<T>::value;



template <typename T, typename = void>
struct nonmember_tostring_invokeable : std::false_type
{ /* empty */ };

template<typename T>
struct nonmember_tostring_invokeable<T,
	std::void_t< decltype(/* */to_string( std::declval<const T&>() )/* */) >> :
		std::true_type
{ /* empty */ };

template <typename T>
constexpr bool has_nonmember_tostring_f()
{
	/* symbol is invokeable and const ? */
    if constexpr(! nonmember_tostring_invokeable<T>::value)
	{
		return false;
	} else
	{
		using RTYPE = decltype( to_string( std::declval<const T&>() ) );

		/* return type is string ? */
		if constexpr( ! std::is_same< std::string, RTYPE>::value )
		{
			return false;
		}
	}

	return true;
};

template <typename T>
struct has_nonmember_tostring : std::bool_constant<
	has_nonmember_tostring_f<T>() >
{ /* empty */ };

template <typename T>
constexpr bool has_nonmember_tostring_v = has_nonmember_tostring<T>::value;



template <typename T>
struct has_tostring_functionality : std::bool_constant<
	has_member_tostring<T>::value      &&
	has_nonmember_tostring<T>::value
>
{ /* empty */ };

} // namespace meta
                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

