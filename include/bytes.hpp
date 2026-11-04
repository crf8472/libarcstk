#ifndef LIBARCSTK_BYTES_HPP_
#define LIBARCSTK_BYTES_HPP_

/**
 * \file
 *
 * \brief Byte operations.
 */

#include <cstdint>       // for int16_t, int32_t, uint16_t, uint32_t
#include <cstddef>       // for byte
#include <type_traits>   // for is_same, is_same_v, enable_if_t

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
namespace details
{

/**
 * \brief Worker template to mimic a disjunction.
 *
 * \tparam T     First input type
 * \tparam Types Optional subsequent input types
 *
 * Intended for static_asserts.
 */
template <typename T, typename... Types>
constexpr bool is_one_of_v = (std::is_same_v<T, Types> || ...);


/**
 * \brief TRUE iff T is one of the supported sample types, otherwise FALSE.
 *
 * \tparam T Type to check
 */
template <typename T>
constexpr bool supported_sample_type =
	is_one_of_v<T, std::int16_t, std::int32_t, std::uint16_t, std::uint32_t>;


/**
 * \brief Implementation of combine() for all platforms.
 *
 * The caller decides which byte is the most significant one by the order of
 * the arguments.
 *
 * The first byte is shifted 24 bits left, the second 16 bits, the third 8 bits
 * and the fourth is not shifted.
 *
 * \param[in] b1 First input byte
 * \param[in] b2 Second input byte
 * \param[in] b3 Third input byte
 * \param[in] b4 Fourth input byte
 *
 * \return The bytes as 32 bit unsigned integer
 */
inline constexpr std::uint32_t combine_impl(const std::byte b1,
		const std::byte b2, const std::byte b3, const std::byte b4) noexcept
{
	return  static_cast<std::uint32_t>(b1) << 24 |
            static_cast<std::uint32_t>(b2) << 16 |
            static_cast<std::uint32_t>(b3) <<  8 |
            static_cast<std::uint32_t>(b4);
}


/**
 * \brief Things specific for C++ 17.
 */
namespace cpp17
{

/**
 * \brief Provide platform endianness.
 *
 * Mimics C++20's std::endian.
 *
 * Recognizes big endian and little endian platforms. On any other platform,
 * compilation fails with an error.
 */        // NOLINTNEXTLINE(performance-enum-size)
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
#else
	#error "Unknown byte order"
#endif
};

} // namespace cpp17

} // namespace details


/**
 * \brief Defined (as void) iff \c T is one of int16_t, int32_t, uint16_t,
 * uint32_t.
 *
 * \tparam T Input type
 */
template <typename T>
using is_sample_type = std::enable_if_t<details::supported_sample_type<T>>;


/**
 * \brief Service method: Interpret 4 bytes as a 32 bit unsigned integer.
 *
 * Interprets four bytes, given in memory order, as one 32 bit unsigned integer
 * in native byte order.
 *
 * On little endian platforms, \c b4 becomes the most significant byte and
 * \c b1 the least significant one. On big endian platforms, \c b1 becomes the
 * most significant byte and \c b4 the least significant one.
 *
 * \param[in] b1 First input byte to read
 * \param[in] b2 Second input byte to read
 * \param[in] b3 Third input byte to read
 * \param[in] b4 Fourth input byte to read
 *
 * \return The bytes as 32 bit unsigned integer
 */
constexpr std::uint32_t combine(const std::byte b1, const std::byte b2,
		const std::byte b3, const std::byte b4) noexcept
{
	using details::cpp17::endian;

	// Note that the following check is actually redundant since a value other
	// than 'little' or 'big' just won't compile (due to #error above).
	// Kept as a defense against further enum values.
	static_assert(endian::native == endian::little ||
			endian::native == endian::big, "unsupported byte order");

	if constexpr (endian::native == endian::little)
	{
		return details::combine_impl(b4, b3, b2, b1);
	}
	else
	{
		// endian::native == endian::big is guaranteed due to #error above

		return details::combine_impl(b1, b2, b3, b4);
	}
}


/**
 * \brief Combine 2 16-bit samples to a 32-bit value.
 *
 * While the input type \c T is supposed to be a 16-bit type, 32-bit types
 * are accepted as well, in which case only the lower 16 bits of \c higher and
 * \c lower are respected.
 *
 * Output format is:
 *
 * - Bits 31-24: Right Channel MSB
 * - Bits 23-16: Right Channel LSB
 * - Bits 15-08: Left  Channel MSB
 * - Bits 07-00: Left  Channel LSB
 *
 * \tparam T Input type
 *
 * \param[in] higher The bits to become the higher bits in output
 * \param[in] lower  The bits to become the lower bits in output
 *
 * \return 32-bit value representing 2 16-bit stereo samples
 */
template <typename T, typename = is_sample_type<T>>
std::uint32_t constexpr combine(const T higher, const T lower) noexcept
{
	// Mask negative signed types correctly
	auto high = static_cast<std::uint32_t>(static_cast<std::uint16_t>(higher));

	// Only use the lower 16 bits of the input values
	auto low = static_cast<std::uint32_t>(static_cast<std::uint16_t>(lower));

	return (high << 16) | low;
}
                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

