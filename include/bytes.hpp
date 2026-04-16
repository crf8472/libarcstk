#ifndef LIBARCSTK_BYTES_HPP_
#define LIBARCSTK_BYTES_HPP_

/**
 * \file
 *
 * \brief Byte operations.
 */

#include <cstdint>       // for int16_t, int32_t, uint8_t, uint32_t,...
#include <cstddef>       // for byte
#include <type_traits>   // for is_same, enable_if_t


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

	template <typename T>
	uint32_t combine(const T higher, const T lower)
	{
		return (static_cast<uint32_t>(higher) << 16) |
			(static_cast<uint32_t>(lower) & 0xFFFF);

		// NOTE: This works because T cannot be anything but only signed or
		// unsigned integers of either 32 or 64 bit length. Those variants can
		// all be handled correctly by just casting them to sample_t.
	}

} // namespace little

namespace big
{
	// b1 becomes the most significant bit
	uint32_t combine(const std::byte b1, const std::byte b2,
		const std::byte b3, const std::byte b4);

	template <typename T>
	uint32_t combine(const T higher, const T lower)
	{
		// TODO Implement big endian variant of combine()
		return 0;
	}
} // namespace big

/**
 * \brief Worker template to mimic a disjunction.
 *
 * Intended for static_asserts.
 */
template <typename T, typename... Types>
constexpr bool is_one_of_v = (std::is_same_v<T, Types> || ...);

/**
 * \brief Worker template to mimic a disjunction.
 *
 * Intended for SFINAE.
 */
template <typename T, typename... Types>
using is_one_of = std::enable_if_t<is_one_of_v<T, Types...>>;

/**
 * \brief TRUE iff T is one of the supported sample types, otherwise FALSE.
 *
 * \tparam T Type to check
 */
template <typename T, typename... Types>
constexpr bool supported_sample_type =
	is_one_of_v<T, int16_t, int32_t, uint16_t, uint32_t>;

/**
 * \brief Defined iff T is a legal sample type, an integral type of 16 or 32 bit
 */
template <typename T>
using is_sample_type = std::enable_if_t<supported_sample_type<T>>;

/**
 * \brief Combine 2 16-bit samples to a 32-bit value.
 *
 * Output format is:
 *
 * - Bits 31-24: Right Channel MSB
 * - Bits 23-16: Right Channel LSB
 * - Bits 15-09: Left  Channel MSB
 * - Bits 08-00: Left  Channel LSB
 *
 * \param[in] higher The bits to become the higher bits in output
 * \param[in] lower  The bits to become the lower bits in output
 *
 * \return 32-bit value representing 2 16-bit stereo samples
 */
template <typename T, typename = is_sample_type<T>>
uint32_t combine(const T higher, const T lower)
{
	using cpp17::endian;

	if constexpr (endian::native == endian::little)
	{
		return little::combine(higher, lower);
	}

	if constexpr (endian::native == endian::big)
	{
		return big::combine(higher, lower);
	}

	return 0;
}

} // namespace details
                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

