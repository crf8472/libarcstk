#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#define LIBARCSTK_CONSTRUCTION_HPP_

/**
 * \file
 *
 * \brief Test helpers for unit tests: construction.
 */

namespace arcstk
{
namespace testing
{

/**
 * \brief Force use of copy constructor of copy assignment operator.
 */
template <typename T>
class Copy
{
public:

	static T construct(const T& instance)
	{
		return T { instance };
	}

	static T& assign(T& lhs, const T& rhs)
    {
        return lhs = rhs;
    }
};


/**
 * \brief Force use of move constructor of move assignment operator.
 */
template <typename T>
class Move
{
public:

	static T construct(T&& instance)
	{
		return T { std::move(instance) };
	}

	static T& assign(T& lhs, T& rhs)
    {
        return lhs = std::move(rhs);
    }
};

} // namespace testing
} // namespace arcstk

#endif

