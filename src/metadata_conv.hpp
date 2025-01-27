#ifndef __LIBARCSTK_METADATA_CONV_HPP__
#define __LIBARCSTK_METADATA_CONV_HPP__
/**
 * \file
 *
 * \brief Conversions for metadata.
 */

#include <algorithm>   // for transform
#include <cstdint>     // for uint32_t, int32_t
#include <memory>      // for unique_ptr
#include <type_traits> // for underlying_type
#include <vector>      // for vector

namespace arcstk
{
inline namespace v_1_0_0
{

class AudioSize;
enum class UNIT;

namespace details
{

/**
 * \brief Return the numeric value of a enum class value.
 *
 * \tparam E The type to convert
 *
 * \return The integral value of an enum
 */
template <typename E>
constexpr auto as_integral_value(const E& value)
	-> typename std::underlying_type<E>::type
{
	return static_cast<typename std::underlying_type<E>::type>(value);
}


namespace conv
{

// determine total number of passed units per frame
template <typename E>
constexpr auto per_frame(const E& value)
	-> typename std::underlying_type<E>::type
{
	return as_integral_value(value);
}


// determine factor to multiply or divide by
template <enum UNIT F, enum UNIT T>
constexpr auto factor() -> int
{
	return per_frame(F) == 1 || per_frame(T) == 1  // one type is FRAME (1)?
		? std::max(per_frame(F),per_frame(T))      // => bigger type
		: std::max(per_frame(F),per_frame(T)) /    // otherwise
			std::min(per_frame(F),per_frame(T));   // divide bigger by smaller
}


// determine arithmetic operation to perform
template <bool>
constexpr auto op(const int32_t value, const int32_t factor) -> int32_t;

template <>
constexpr auto op<true>(const int32_t value, const int32_t factor) -> int32_t
{
	return value * factor;
};

template <>
constexpr auto op<false>(const int32_t value, const int32_t factor) -> int32_t
{
	return value / factor;
};

} // namespace


/**
 * \brief Convert from F to T.
 */
template <enum UNIT F, enum UNIT T>
constexpr auto convert(const int32_t from) -> int32_t
{
	return conv::op<conv::per_frame(F) < conv::per_frame(T)>(from,
			conv::factor<F,T>());
}


/**
 * \brief Convert an AudioSize to the specified UNIT.
 *
 * \param[in] v The object to convert
 *
 * \tparam U The UNIT to convert to
 *
 * \return Value of the specified unit
 */
template <enum UNIT U>
int32_t convert_to(const AudioSize& v);

// full specializations

template <>
inline int32_t convert_to<UNIT::FRAMES>(const AudioSize& v)
{
	return v.total_frames();
}

template <>
inline int32_t convert_to<UNIT::SAMPLES>(const AudioSize& v)
{
	return v.total_samples();
}

template <>
inline int32_t convert_to<UNIT::BYTES>(const AudioSize& v)
{
	return v.total_pcm_bytes();
}


/**
 * \brief Convert a vector of AudioSize instances to the specified UNIT.
 *
 * \param[in] values The values to convert
 *
 * \tparam U The UNIT to convert to
 *
 * \return Converted values of the specified unit
 */
template <enum UNIT U>
std::vector<int32_t> convert(const std::vector<AudioSize>& values)
{
	auto integers { std::vector<int32_t>(values.size()) };

	using std::cbegin;
	using std::cend;
	using std::begin;

	std::transform(cbegin(values), cend(values), begin(integers),
			[](const AudioSize& a) -> int32_t
			{
				return convert_to<U>(a);
			});

	return integers;
}

} //namespace details
} // namespace v_1_0_0
} // namespace arcstk

#endif

