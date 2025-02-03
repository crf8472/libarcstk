#ifndef __LIBARCSTK_METADATA_HPP__
#error "Do not include metadata_conv.hpp, include metadata.hpp instead"
#endif

#ifndef __LIBARCSTK_METADATA_CONV_HPP__
#define __LIBARCSTK_METADATA_CONV_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Conversions for metadata.
 */

#include <algorithm>   // for transform
#include <cstdint>     // for uint32_t, int32_t
#include <iterator>    // for begin, cbegin, cend, end
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
// Implement the conversion of different UNITs as follows:

// F -> S : f * SAMPLES   multiply by bigger type iff one type is 1
// F -> B : f * BYTES     multiply by bigger type iff one type is 1
// S -> F : f \ SAMPLES   divide by bigger type iff one type is 1
// S -> B : f * (BYTES \ SAMPLES)
// B -> F : f \ BYTES     divide by bigger type iff one type is 1
// B -> S : f \ (BYTES \ SAMPLES)


// determine total number of passed units per frame
template <typename E>
constexpr auto per_frame(const E& value)
	-> typename std::underlying_type<E>::type
{
	return as_integral_value(value);
}


// implement factor selection
template <enum UNIT F, enum UNIT T, bool B>
struct factor_impl
{
	// empty
};

template <enum UNIT F, enum UNIT T>
struct factor_impl<F, T, true>
{
	static constexpr int value()
	{
		// use bigger type as factor
		return std::max(per_frame(F), per_frame(T));
	}
};

template <enum UNIT F, enum UNIT T>
struct factor_impl<F, T, false>
{
	static constexpr int value()
	{
		// use bigger type divided by smaller type as factor
		return std::max(per_frame(F), per_frame(T)) /
			std::min(per_frame(F), per_frame(T));
	}
};



// determine factor to multiply or divide by
template <enum UNIT F, enum UNIT T>
constexpr auto factor() -> int
{
	return factor_impl<F, T, per_frame(F) == 1 || per_frame(T) == 1>::value();
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
	return conv::op<conv::per_frame(F) < conv::per_frame(T)>(
			from, conv::factor<F,T>());
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
	return v.frames();
}

template <>
inline int32_t convert_to<UNIT::SAMPLES>(const AudioSize& v)
{
	return v.samples();
}

template <>
inline int32_t convert_to<UNIT::BYTES>(const AudioSize& v)
{
	return v.bytes();
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

