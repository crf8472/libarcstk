/**
 * \internal
 *
 * \file
 *
 * \brief Implementations of bytes.hpp.
 */

#ifndef LIBARCSTK_BYTES_HPP_
#include "bytes.hpp"
#endif

#include <cstddef>      // for byte
#include <cstdint>      // for uint32_t

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

uint32_t combine(const std::byte b1, const std::byte b2,
		const std::byte b3, const std::byte b4)
{
	using cpp17::endian;

	if constexpr (endian::native == endian::little)
	{
		return little::combine(b1, b2, b3, b4);
	}

	if constexpr (endian::native == endian::big)
	{
		return big::combine(b1, b2, b3, b4);
	}

	return 0;
}

namespace little
{

uint32_t combine(const std::byte b1, const std::byte b2,
		const std::byte b3, const std::byte b4)
{
	return  static_cast<uint32_t>(b4) << 24 |
            static_cast<uint32_t>(b3) << 16 |
            static_cast<uint32_t>(b2) <<  8 |
            static_cast<uint32_t>(b1);
}

} // namespace little

namespace big
{

uint32_t combine(const std::byte b1, const std::byte b2,
		const std::byte b3, const std::byte b4)
{
	return  static_cast<uint32_t>(b1) << 24 |
            static_cast<uint32_t>(b2) << 16 |
            static_cast<uint32_t>(b3) <<  8 |
            static_cast<uint32_t>(b4);
}

} // namespace big

} // namespace details
} // namespace v_1_0_0
} // namespace arcstk

