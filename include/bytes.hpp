#ifndef LIBARCSTK_BYTES_HPP_
#define LIBARCSTK_BYTES_HPP_

/**
 * \file
 *
 * \brief Byte operations.
 */

#include <cstdint>
#include <cstddef>  // for byte


namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
namespace details
{

/**
 * \brief Service method: Interpret 4 bytes as a 32 bit unsigned integer.
 *
 * \param[in] b1 First input byte
 * \param[in] b2 Second input byte
 * \param[in] b3 Third input byte
 * \param[in] b4 Fourth input byte
 *
 * \return The bytes as 32 bit unsigned integer
 */
uint32_t combine(const std::byte b1, const std::byte b2,
		const std::byte b3, const std::byte b4);

/**
 * \brief Things specific for C++ 17.
 */
namespace cpp17
{

/**
 * \brief Provide plattform endianess
 *
 * Recognize plattform endianess at compile time. Mimics C++20's std::endian.
 * Recognizes big endian and little endian plattforms and fails with an error.
 */
enum class endian
{
#if defined(__BYTE_ORDER__)
	#if __BYTE_ORDER__   == __ORDER_LITTLE_ENDIAN__
		little = 1234,
		big    = 4321,
		native = little
	#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		little = 1234,
		big    = 4321,
		native = big
	#else
	#error "Unknown byte order"
	#endif
#elif defined(_WIN32) || defined(_WIN64)
		little = 1234,
		big    = 4321,
		native = little
#elif defined(__APPLE__)
	#include <machine/endian.h>
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		little = 1234,
		big    = 4321,
		native = little
	#elif __BYTE_ORDER == __BIG_ENDIAN
		little = 1234,
		big    = 4321,
		native = big
	#endif
#else
#error "Cannot detect byte order"
#endif
};

} // namespace cpp17

namespace little
{
	// b4 becomes the most significant bit
	uint32_t combine(const std::byte b1, const std::byte b2,
		const std::byte b3, const std::byte b4);
} // namespace little

namespace big
{
	// b1 becomes the most significant bit
	uint32_t combine(const std::byte b1, const std::byte b2,
		const std::byte b3, const std::byte b4);
} // namespace big

} // namespace details
                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

