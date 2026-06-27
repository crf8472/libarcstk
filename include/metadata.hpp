#ifndef LIBARCSTK_METADATA_HPP_
#define LIBARCSTK_METADATA_HPP_

#if __cplusplus < 201703L
	#error "C++17 is required to compile libarcstk"
#endif

/**
 * \file
 *
 * \brief Classes and functions for \link meta metadata of a compact
 * disc\endlink.
 *
 * \details
 *
 * Part of the API for \link meta calculating AccurateRip checksums\endlink.
 */

#ifndef LIBARCSTK_MIXINS_HPP_
#include "mixins.hpp"     // for Comparable, TotallyOrdered
#endif

#include <algorithm>      // for transform
#include <array>          // for array
#include <cstdint>        // for uint16_t, int32_t
#include <memory>         // for unique_ptr
#include <sstream>        // for ostringstream
#include <stdexcept>      // for runtime_error
#include <string>         // for string
#include <type_traits>    // for underlying_type
#include <vector>         // for vector

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

/**
 * \defgroup meta Compact Disc and Audio Metadata
 *
 * \brief ToC metadata and input size of the audio files
 *
 * \details
 *
 * A ToC is the table-of-content information from a compact disc. It contains
 * the track offsets and optionally the leadout of the compact disc. ToCs that
 * contain not only the offsets but also the leadout are called
 * \e complete. A ToC may or may not contain filenames corresponding to the
 * audio files.
 *
 * ToCData is the data part of a ToC: an aggregate that contains the
 * leadout at index 0 and on the subsequent index positions 1..n the offsets of
 * the respective tracks. Hence, ToCData contains all and only the data that is
 * required to calculate AccurateRip checksums and AccurateRip ids.
 *
 * ToCData can be constructed by toc::construct. ToC instances can be
 * constructed by make_toc().
 *
 * ToCData and also ToC instances are called \e valid iff the contained data
 * conforms to the redbook standard, i.e.
 *
 * - the total number of tracks is positive and not bigger than
 *   CDDA::MAX_TRACKCOUNT,
 * - no offset is negative or bigger than CDDA::MAX_OFFSET,
 * - each two offset have at least positive distance
 *   CDDA::MIN_TRACK_OFFSET_DIST, and
 * - each track length is at least CDDA::MIN_TRACK_LEN_FRAMES.
 *
 * ToCData can be validated with or without the requirement for being complete
 * by either toc::validate_with_completeness() or
 * toc::validate_without_completeness(). Any ToC instance can be validated by
 * member function ToC::valid().
 *
 * Validated ToCs can be constructed by using functions validated_toc().
 *
 * An InvalidMetadataException indicates that no valid ToC can be constructed
 * from the input provided.
 *
 * AudioSize is an abstract representation of an amount of audio information
 * that can be evaluated as a total number of frames, samples or bytes. Passing
 * AudioSize instances helps to avoid accidentally calculating with the wrong
 * UNIT, e.g. w/ samples when frames are required.
 *
 * CDDA provides a set of constants related to the CDDA standard. They are used
 * on validating and parsing audio information.
 *
 * @{
 */

/**
 * \brief Constants related to the CDDA format.
 */
struct CDDA final
{
	/**
	 * \brief CDDA: sampling rate of 44100 samples per second.
	 */
	constexpr static int SAMPLES_PER_SECOND { 44100 };

	/**
	 * \brief CDDA: 16 bits per sample.
	 */
	constexpr static int BITS_PER_SAMPLE    { 16 };

	/**
	 * \brief CDDA: stereo involves 2 channels.
	 */
	constexpr static int NUMBER_OF_CHANNELS { 2 };

	/**
	 * \brief Total number of frames per second is 75.
	 */
	constexpr static int FRAMES_PER_SEC     { 75 };

	/**
	 * \brief Total number of bytes per sample is 4.
	 *
	 * This follows from CDDA where
	 * 1 sample == 16 bit/sample * 2 channels / 8 bits/byte
	 */
	constexpr static int BYTES_PER_SAMPLE   { 4 };

	/**
	 * \brief Total number of samples per frame is 588.
	 *
	 * This follows from CDDA where 1 frame == 44100 samples/sec / 75 frames/sec
	 */
	constexpr static int SAMPLES_PER_FRAME  { 588 };

	/**
	 * \brief Total number of bytes per frame is 2352.
	 *
	 * This follows from CDDA where 1 frame == 588 samples * 4 bytes/sample
	 */
	constexpr static int BYTES_PER_FRAME    { 2352 };

	/**
	 * \brief Maximal valid track count is 99.
	 */
	constexpr static int MAX_TRACKCOUNT { 99 };

	/**
	 * \brief Redbook maximal value for a valid LBA frame index is 449.999.
	 *
	 * Redbook defines 99:59.74 (MSF) as maximal valid block adress. This is
	 * equivalent to 449.999 frames.
	 */
	constexpr static int32_t MAX_BLOCK_ADDRESS { ( 99 * 60 + 59 ) * 75 + 74 };

	/**
	 * \brief Redbook maximal valid offset value is 359.999 LBA frames.
	 *
	 * Redbook defines 79:59.74 (MSF) (+leadin+leadout) as maximal play
	 * duration. This is equivalent to 360.000 frames, thus the maximal valid
	 * offset is LBA frame index 359.999.
	 */
	constexpr static int32_t MAX_OFFSET { ( 79 * 60 + 59 ) * 75 + 74 };

	/**
	 * \brief Two subsequenct offsets must have a distance of at least 300 LBA
	 * frames.
	 *
	 * The CDDA conforming minimal track length is 4 seconcs including 2 seconds
	 * pause, thus 4 sec * 75 frames/sec == 300 frames.
	 */
	constexpr static int32_t MIN_TRACK_OFFSET_DIST { 300 };

	/**
	 * \brief Minimal number of LBA frames a track contains is 150.
	 *
	 * The CDDA conforming minmal track length is 4 seconds including 2 seconds
	 * pause but the pause does not contribute to the track lengths, thus
	 * 2 sec * 75 frames/sec == 150 frames.
	 */
	constexpr static int32_t MIN_TRACK_LEN_FRAMES { 150 };
};


/**
 * \brief Represents an audio unit.
 *
 * Units are frames, samples or bytes.
 */
enum class UNIT : uint16_t
{
	FRAMES  = 1,
	SAMPLES = CDDA::SAMPLES_PER_FRAME,
	BYTES   = CDDA::BYTES_PER_FRAME
};


namespace details
{

/**
 * \brief Maximum value for the specified UNIT according to CDDA.
 *
 * \return Maximum CDDA compatible value for \c U
 */
template <enum UNIT U>
constexpr int32_t cdda_max_value() noexcept;

// full specializations

template <>
inline constexpr int32_t cdda_max_value<UNIT::FRAMES>() noexcept
{
	return CDDA::MAX_BLOCK_ADDRESS;
}

template <>
inline constexpr int32_t cdda_max_value<UNIT::SAMPLES>() noexcept
{
	return cdda_max_value<UNIT::FRAMES>() * CDDA::SAMPLES_PER_FRAME;
}

template <>
inline constexpr int32_t cdda_max_value<UNIT::BYTES>() noexcept
{
	return cdda_max_value<UNIT::FRAMES>() * CDDA::BYTES_PER_FRAME;
}

} // namespace details


/**
 * \brief Maximum value for the specified UNIT according to CDDA.
 *
 * \tparam U The UNIT the determine the maximum legal CDDA value of
 */
template <enum UNIT U>
constexpr int32_t cdda_max { details::cdda_max_value<U>() };


/**
 * \internal
 *
 * \brief Conversion operations
 */
namespace conv
{

/**
 * \brief Return the numeric value of a enum class value.
 *
 * \param[in] value Value to convert
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


// Implement the conversion of different UNITs as follows:

// FRAMES  -> SAMPLES: x * SAMPLES   multiply by bigger type iff one type is 1
// FRAMES  -> BYTES  : x * BYTES     multiply by bigger type iff one type is 1
// SAMPLES -> FRAMES : x \ SAMPLES   divide by bigger type iff one type is 1
// SAMPLES -> BYTES  : x * (BYTES \ SAMPLES)
// BYTES   -> FRAMES : x \ BYTES     divide by bigger type iff one type is 1
// BYTES   -> SAMPLES: x \ (BYTES \ SAMPLES)

// We therefore require:
// - UNITS per frame: per_frame<>()
// - select factor: (1) bigger type or (2) division of bigger by smaller type
// - determine whether at least one of the two UNITS is FRAMES

/**
 * \brief Determine total number of units per frame.
 *
 * \tparam E Unit
 *
 * \param[in] value Total number of units \c E in a single LBA frame
 *
 * \return Total number of units of type \c E in a single LBA frame
 */
template <typename E>
constexpr auto per_frame(const E& value)
	-> typename std::underlying_type<E>::type
{
	return as_integral_value(value);
}


/**
 * \brief Implement factor selection for conversion.
 *
 * \tparam F The UNIT to convert from
 * \tparam T The UNIT to convert to
 * \tparam B Determine which factor implementation to select
 */
template <enum UNIT F, enum UNIT T, bool B>
struct factor_impl
{
	// empty
};

// partial specializations

template <enum UNIT F, enum UNIT T>
struct factor_impl<F, T, true>
{
	// if true: use "bigger" type as factor
	static constexpr int value()
	{
		return std::max(per_frame(F), per_frame(T));
	}
};

template <enum UNIT F, enum UNIT T>
struct factor_impl<F, T, false>
{
	static constexpr int value()
	{
		// if false: use "bigger" type divided by "smaller" type as factor
		return std::max(per_frame(F), per_frame(T)) /
			std::min(per_frame(F), per_frame(T));
	}
};


/**
 * \brief Determine factor to multiply or divide by when converting F to T.
 *
 * \tparam F The UNIT to convert from
 * \tparam T The UNIT to convert to
 *
 * \return Factor to multiply or divide by on conversion
 */
template <enum UNIT F, enum UNIT T>
constexpr auto factor() -> int
{
	return factor_impl<F, T, per_frame(F) == 1 || per_frame(T) == 1>::value();
}


/**
 * \brief Determine whether to multiply or divide when converting.
 *
 * \tparam B Iff TRUE perform multiplication, otherwise perform division
 *
 * \param[in] value  Value to apply factor on
 * \param[in] factor Factor to be applied
 *
 * \return Result of applying \c factor to \c value
 */
template <bool>
constexpr auto op(const int32_t value, const int32_t factor) -> int32_t;

// full specialization

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


/**
 * \brief Convert from UNIT F to UNIT T.
 *
 * Convert an amount auf UNIT F to the equivalent amount of UNIT T.
 * It is not checked whether F and T are identical.
 *
 * \param[in] amount The amount to convert
 *
 * \tparam F The UNIT of \c amount to be converted
 * \tparam T The UNIT to convert to
 *
 * \return The equivalent amount in UNIT T
 */
template <enum UNIT F, enum UNIT T>
constexpr auto convert_impl(const int32_t amount) -> int32_t
{
	return op<per_frame(F) < per_frame(T)>(amount, factor<F, T>());
};

} // namespace conv

/**
 * \brief Convert from UNIT F to UNIT T.
 *
 * Convert an amount auf UNIT F to the equivalent amount of UNIT T.
 *
 * \param[in] amount The amount to convert
 *
 * \tparam F The UNIT of \c amount to be converted
 * \tparam T The UNIT to convert to
 *
 * \return The equivalent amount in UNIT T
 */
template <enum UNIT F, enum UNIT T>
constexpr auto convert(const int32_t amount) -> int32_t
{
	if constexpr (F == T)
	{
		return amount;
	} else
	{
		return conv::convert_impl<F, T>(amount);
	}

	return 0; // unreachable
};

/**
 * \brief Convert a value of a UNIT to the specified UNIT.
 *
 * \param[in] value The value to convert
 * \param[in] unit  The unit to convert from
 *
 * \tparam U The UNIT to convert to
 *
 * \return Value of the specified unit
 */
template <UNIT U>
auto convert_to(int32_t value, UNIT unit) noexcept -> int32_t
{
	// Note: this type must be unsigned!
	using unit_type  = typename std::underlying_type<UNIT>::type;

	if (U == unit)
	{
		return value;
	} else
	{
		using int_func   = int32_t(*)(const int32_t);
		using array_type = typename std::array<int_func, 3>;
		using size_type  = typename array_type::size_type;

		const array_type converters =
		{
			+[](const int32_t v) noexcept {
					return conv::convert_impl<UNIT::FRAMES,  U>(v); },
			+[](const int32_t v) noexcept {
					return conv::convert_impl<UNIT::SAMPLES, U>(v); },
			+[](const int32_t v) noexcept {
					return conv::convert_impl<UNIT::BYTES,   U>(v); },
		};

		// We convert the UNIT of parameter unit to UNIT U, hence we must pick
		// func 0 for value 1, func 1 for value 588 and func 2 for value 2352.
		// Since 1 is zero trailing 0s, 588 has 2 trailing 0s, 2352 has 4
		// trailing zeros, we can just divide the number of trailing 0s by 2 and
		// have a proper mapping function at hand.

		// Determine correct conversion function

		static constexpr auto count_trailing_zeros =
			[](unit_type v) -> unit_type
			{
				auto count = unsigned { 0 };

				// How many shifts to the right until an 1 occurrs on rightmost
				// pos? (Would be cooler to do this by a de Brujin table.)
				while ((v & 1) == 0)
				{
					++count;
					v >>= 1;
				}

				return count;
			};

		static constexpr auto map_to_idx =
			[](const unit_type v) -> size_type
			{
				return count_trailing_zeros(v) / 2;
			};


		const auto idx = map_to_idx(static_cast<unit_type>(unit));

		if (idx < converters.size())
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
			return converters[idx](value);
		}

		return value; // fallback
	}

	return 0; // unreachable
}


class AudioSize; // forward declaration

// ensure to put declaration in this namespace
std::ostream& operator << (std::ostream& out, const AudioSize& i);

/**
 * \brief Uniform access to the size of the input audio information.
 *
 * Some decoders provide the number of frames, others the number of samples and
 * maybe in some situations just the number of bytes of the sample stream is
 * known. To avoid implementing the appropriate conversion for each decoder,
 * AudioSize provides an interface for uniform representation to this
 * information. Any of the informations provided will determine all of the
 * others.
 *
 * An AudioSize converts to TRUE if it is greater than 0. An AudioSize of 0
 * converts to FALSE.
 */
class AudioSize final : Equality<AudioSize>, TotallyOrdered<AudioSize>,
						Swap<AudioSize>, ToString<AudioSize>
{
	/**
	 * \brief Data: Total number of bytes.
	 */
	int32_t bytes_ {};

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] value Size value
	 * \param[in] unit  Unit for \c value
	 */
	AudioSize(const int32_t value, const UNIT unit) noexcept
		: bytes_ { convert_to<UNIT::BYTES>(value, unit) }
	{
		// empty
	}

	/**
	 * \brief Constructor.
	 *
	 * Constructs an AudioSize of zero().
	 */
	AudioSize() noexcept
		: AudioSize { 0, UNIT::BYTES }
	{
		// empty
	}

	/**
	 * \brief Size in LBA frames.
	 *
	 * \return The size in LBA frames
	 */
	int32_t frames() const noexcept
	{
		return convert<UNIT::BYTES, UNIT::FRAMES>(bytes_);
	}

	/**
	 * \brief Update this size by an amount of LBA frames
	 *
	 * \param[in] frames Updated size to set as an amount of LBA frames
	 */
	void set_frames(const int32_t frames) noexcept
	{
		bytes_ = convert<UNIT::FRAMES, UNIT::BYTES>(frames);
	}

	/**
	 * \brief Size in stereo PCM samples.
	 *
	 * \return The size in stereo PCM samples
	 */
	int32_t samples() const noexcept
	{
		return convert<UNIT::BYTES, UNIT::SAMPLES>(bytes_);
	}

	/**
	 * \brief Update this size by an amount of stereo PCM samples.
	 *
	 * \param[in] samples Updated size to set as an amount of stereo PCM samples
	 */
	void set_samples(const int32_t samples) noexcept
	{
		bytes_ = convert<UNIT::SAMPLES, UNIT::BYTES>(samples);
	}

	/**
	 * \brief Size in bytes.
	 *
	 * \return The size in bytes
	 */
	int32_t bytes() const noexcept
	{
		return bytes_;
	}

	/**
	 * \brief Update this size by an amount of bytes.
	 *
	 * \param[in] bytes Updated size to set as an amount of bytes
	 */
	void set_bytes(const int32_t bytes) noexcept
	{
		bytes_ = bytes;
	}

	/**
	 * \copydoc SNPT_mf_zero
	 */
	bool zero() const noexcept
	{
		return 0 == bytes_;
	}

	/**
	 * \copydoc SNPT_mf_op_bool_if_zero
	 */
	explicit operator bool() const noexcept
	{
		return !zero();
	}

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(AudioSize& rhs) noexcept
	{
		using std::swap;

		swap(this->bytes_, rhs.bytes_);
	}

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const AudioSize& rhs) const noexcept
	{
		return this->bytes_ == rhs.bytes_;
	}

	/**
	 * \copydoc SNPT_mf_to_string
	 */
	std::string to_string() const
	{
		auto ss = std::ostringstream {};
		ss << *this;
		return ss.str();
	}

	/**
	 * \copydoc SNPT_nf_stream_in
	 */
	friend std::ostream& operator << (std::ostream& out, const AudioSize& i)
	{
		if (!out.good())
		{
			// Maybe set badbit: out.setstate(std::ios_base::badbit);
			return out;
		}

		out << i.frames() << " LBA frames";
		return out;
	}

	/**
	 * \copydoc SNPT_nf_less
	 */
	friend bool operator < (const AudioSize& lhs, const AudioSize& rhs) noexcept
	{
		return lhs.bytes_ < rhs.bytes_;
	}
};


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
 * \brief Convert a vector of AudioSize instances to a vector of the specified
 * UNIT.
 *
 * \param[in] values The values to convert
 *
 * \tparam U The UNIT to convert to
 *
 * \return Converted values of the specified unit
 */
template <enum UNIT U>
inline std::vector<int32_t> convert(const std::vector<AudioSize>& values)
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


/**
 * \brief ToC data from a file, e.g. offsets and leadout.
 *
 * ToCData is an iterable container accessible by operator [].
 *
 * Leadout is on index 0, followed by the sequence of offsets. Offset indices
 * therefore correspond to track numbers, i.e. index 7 is offset of track 7.
 */
using ToCData = std::vector<AudioSize>; // also defined in calculate.hpp


/**
 * \brief Functions for managing ToCData instances.
 *
 * This is intended to be used when implementing metadata parsers.
 */
namespace toc
{

/**
 * \brief Construct ToCData from leadout and offsets.
 *
 * All <tt>int32_t</tt> data types denote amounts of LBA frames.
 *
 * \param[in] leadout Leadout frame
 * \param[in] offsets Offset frames in order of tracks
 *
 * \return ToCData object
 */
ToCData construct(const int32_t leadout, const std::vector<int32_t>& offsets);

/**
 * \brief Construct ToCData from leadout and offsets.
 *
 * \param[in] leadout Leadout
 * \param[in] offsets Offset values in order of tracks
 *
 * \return ToCData object
 */
ToCData construct(const AudioSize& leadout,
		const std::vector<AudioSize>& offsets);

/**
 * \brief Total tracks.
 *
 * \param[in] data ToCData to read from
 *
 * \return Total number of tracks
 */
unsigned total_tracks(const ToCData& data);

/**
 * \brief Offsets of a ToC object.
 *
 * \param[in] data ToCData to read from
 *
 * \return Offsets of a ToC object
 */
std::vector<AudioSize> offsets(const ToCData& data);

/**
 * \brief Leadout of a ToC object.
 *
 * \param[in] data ToCData to read from
 *
 * \return Leadout of a ToC object
 */
AudioSize leadout(const ToCData& data);

/**
 * \brief Offset of track \c track.
 *
 * \param[in] track Track number
 * \param[in] data  ToCData to read from
 *
 * \return Offset of track \c track
 */
AudioSize offset(const ToCData::size_type track, const ToCData& data);

/**
 * \brief Length of track \c track.
 *
 * \param[in] track Track number
 * \param[in] data  ToCData to read from
 *
 * \return Length of track \c track
 */
AudioSize length(const ToCData::size_type track, const ToCData& data);

/**
 * \brief Set the leadout of a ToCData object.
 *
 * \param[in] leadout Leadout to set
 * \param[in] data    ToCData to update
 */
void set_leadout(const AudioSize& leadout, ToCData& data);

/**
 * \brief Set the offset of track \c track of a ToCData object.
 *
 * \param[in] track  Track number
 * \param[in] offset Offset to set
 * \param[in] data   ToCData to update
 */
void set_offset(const ToCData::size_type track, const int32_t offset,
		ToCData& data);

/**
 * \brief Set the offsets for this ToCData.
 *
 * \param[in] offsets Offsets, interpreted as LBA frames
 * \param[in] data    ToCData to update
 */
void set_offsets(const std::vector<int32_t>& offsets, ToCData& data);

/**
 * \brief Set the offsets for this ToCData.
 *
 * \param[in] offsets Offsets, interpreted as LBA frames
 * \param[in] data    ToCData to update
 */
void set_offsets(const std::vector<AudioSize>& offsets, ToCData& data);

/**
 * \brief Lengths of tracks.
 *
 * If the leadout is zero, the length of the last track is unknown and the
 * size of the returned container will be smaller than total_tracks().
 *
 * \param[in] data ToCData to read from
 *
 * \return Track lengths
 */
std::vector<AudioSize> lengths(const ToCData& data);

/**
 * \brief TRUE iff non-zero leadout and non-empty sequence of offsets are
 * specified.
 *
 * This does not entail a validation of the specified values.
 *
 * \param[in] data TocData instance to check for completeness
 *
 * \return TRUE iff non-zero leadout and non-empty offsets exist
 */
bool complete(const ToCData& data);

/**
 * \brief Validate ToCData object.
 *
 * The leadout is allowed to be 0 but validated if it is non-zero.
 *
 * \param[in] toc_data ToCData object to be validated
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate(const ToCData& toc_data);

/**
 * \brief Validate ToCData object requiring completeness.
 *
 * The leadout is always validated and required to be non-zero.
 *
 * \param[in] toc_data ToCData object to be validated
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_with_completeness(const ToCData& toc_data);

/**
 * \copydoc SNPT_mf_empty
 *
 * \param[in] toc_data ToCData object to be checked
 */
bool is_empty(const ToCData& toc_data);

/**
 * \brief Create a string representation of this instance.
 *
 * \param[in] toc_data ToCData object to be converted
 *
 * \return String representation
 */
std::string to_string(const ToCData& toc_data);

} // namespace toc


class ToC; // forward declaration

// ensure to put declaration in this namespace
std::ostream& operator << (std::ostream& out, const ToC& i);

/**
 * \brief Table of contents of a compact disc.
 */
class ToC final : Equality<ToC>, Comparable<ToC>, Swap<ToC>, ToString<ToC>
{
	class Impl;

	// intentionally undocumented
	std::unique_ptr<Impl> impl_ {};

public:

	/**
	 * \brief Constructs an empty ToC.
	 */
	ToC();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] toc_data   The ToC data to construct a ToC from
	 * \param[in] filenames  Audio filenames
	 */
	ToC(const ToCData& toc_data, const std::vector<std::string>& filenames);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] toc_data The ToC data to construct a ToC from
	 */
	explicit ToC(const ToCData& toc_data);

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	ToC(const ToC& rhs);

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	ToC& operator = (const ToC& rhs);

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	ToC(ToC&& rhs) noexcept;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	ToC& operator = (ToC&& rhs) noexcept;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~ToC() noexcept;

	/**
	 * \brief Total tracks in this ToC.
	 *
	 * \return Total tracks
	 */
	unsigned total_tracks() const noexcept;

	/**
	 * \brief Offsets of this ToC.
	 *
	 * \return Offsets
	 */
	std::vector<AudioSize> offsets() const;

	/**
	 * \brief Leadout LBA frame of this ToC.
	 *
	 * If the leadout is unknown, the AudioSize returned is <tt>zero()</tt>.
	 *
	 * \return Leadout LBA frame
	 */
	AudioSize leadout() const noexcept;

	/**
	 * \brief Filenames of this ToC.
	 *
	 * \return Filenames
	 */
	std::vector<std::string> filenames() const;

	/**
	 * \brief Set the offsets.
	 *
	 * \param[in] offsets Offsets to set
	 */
	void set_offsets(const std::vector<AudioSize>& offsets);

	/**
	 * \brief Set the leadout LBA frame of this ToC.
	 *
	 * \param[in] leadout The leadout frame to set
	 */
	void set_leadout(const AudioSize& leadout) noexcept;

	/**
	 * \brief Set the filenames.
	 *
	 * \param[in] filenames Filenames to set
	 */
	void set_filenames(const std::vector<std::string>& filenames);

	/**
	 * \brief TRUE iff the ToC specifies exactly one audiofile, otherwise FALSE.
	 *
	 * However, the <tt>filenames()</tt> returned may be more than 1, but they
	 * will be all identical. This is possible when the parsed metadata source
	 * specifies a file for each track.
	 *
	 * This will be FALSE in case where multiple non-identical filenames were
	 * specified by the metadata source.
	 *
	 * \return TRUE iff the ToC has exactly one audio file, otherwise FALSE
	 */
	bool is_single_file() const noexcept;

	/**
	 * \brief Validate the instance.
	 *
	 * The ToC instance is not required to be complete validation success. If
	 * the instance is complete, the leadout will be validated.
	 *
	 * \throws InvalidMetadataException If validation fails
	 */
	void validate() const;

	/**
	 * \brief TRUE iff the ToC instance is valid.
	 *
	 * Calls validate() and returns TRUE iff no exception was thrown.
	 *
	 * \return TRUE iff ToC contains only valid data.
	 */
	bool valid() const noexcept;

	/**
	 * \brief TRUE iff offsets and non-zero leadout are present, otherwise
	 * FALSE.
	 *
	 * \return TRUE iff ToC contains complete toc information otherwise FALSE
	 */
	bool complete() const noexcept;

	/**
	 * \copydoc SNPT_mf_empty
	 *
	 * \details There may or may not be filenames present, but no ToCData.
	 */
	bool empty() const noexcept;

	/**
	 * \class SNPT_mf_op_bool_if_empty
	 */
	explicit operator bool() const noexcept;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(const ToC& rhs) const noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const ToC& rhs) const noexcept;

	/**
	 * \copydoc SNPT_mf_to_string
	 */
	std::string to_string() const;

	/**
	 * \copydoc SNPT_nf_stream_in
	 */
	friend std::ostream& operator << (std::ostream& out, const ToC& i);
};

/**
 * \brief Create a ToC from leadout, offsets and filenames.
 *
 * \param[in] leadout   Leadout frame
 * \param[in] offsets   Offset frames
 * \param[in] filenames Audio filenames
 *
 * \return ToC created from leadout, offsets and filenames
 */
ToC make_toc(const int32_t leadout, const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames);

/**
 * \brief Create a ToC from leadout and offsets.
 *
 * \param[in] leadout   Leadout frame
 * \param[in] offsets   Offset frames
 *
 * \return ToC created from leadout and offsets
 */
ToC make_toc(const int32_t leadout, const std::vector<int32_t>& offsets);

/**
 * \brief Create a ToC from offsets and filenames.
 *
 * \param[in] offsets   Offset frames
 * \param[in] filenames Audio filenames
 *
 * \return ToC created from offsets and filenames
 */
ToC make_toc(const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames);

/**
 * \brief Create a ToC from offsets.
 *
 * \param[in] offsets   Offset frames
 *
 * \return ToC created from offsets.
 */
ToC make_toc(const std::vector<int32_t>& offsets);

/**
 * \brief Create a validated ToC from leadout, offsets and filenames.
 *
 * \param[in] leadout   Leadout frame
 * \param[in] offsets   Offset frames
 * \param[in] filenames Audio filenames
 *
 * \return ToC created from leadout, offsets and filenames
 *
 * \throws InvalidMetadataException If validation fails
 */
ToC validated_toc(const int32_t leadout, const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames);

/**
 * \brief Create a validated ToC from leadout and offsets.
 *
 * \param[in] leadout   Leadout frame
 * \param[in] offsets   Offset frames
 *
 * \return ToC created from leadout and offsets
 *
 * \throws InvalidMetadataException If validation fails
 */
ToC validated_toc(const int32_t leadout, const std::vector<int32_t>& offsets);

/**
 * \brief Create a validated ToC from offsets and filenames.
 *
 * \param[in] offsets   Offset frames
 * \param[in] filenames Audio filenames
 *
 * \return ToC created from offsets and filenames
 *
 * \throws InvalidMetadataException If validation fails
 */
ToC validated_toc(const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames);

/**
 * \brief Create a validated ToC from offsets.
 *
 * \param[in] offsets   Offset frames
 *
 * \return ToC created from offsets.
 *
 * \throws InvalidMetadataException If validation fails
 */
ToC validated_toc(const std::vector<int32_t>& offsets);

/**
 * \brief Requirements for metadata validity.
 */
enum class MetadataRequirement : uint8_t
{
	// mandatory for each ToC data
	OFFSETS_ARE_NONNEGATIVE,
	OFFSETS_ARE_NOT_GREATER_THAN_MAX,
	OFFSETS_ARE_STRICTLY_WELLORDERED,
	OFFSETS_HAVE_MIN_DIST,
	OFFSETS_PRODUCE_MIN_LENGTHS,
	TOTAL_TRACKS_IS_POSITIVE,
	TOTAL_TRACKS_IS_NOT_GREATER_THAN_MAX,
	// optional
	LEADOUT_IS_NONNEGATIVE,
	TOTAL_FILENAMES_MATCH_TOTAL_TRACKS
};

/**
 * \brief Reports invalid metadata for constructing a ToC.
 */
class InvalidMetadataException final : public std::runtime_error
{
	/**
	 * \brief Requirement that was violated.
	 */
	MetadataRequirement req_ {};

	/**
	 * \brief Value that violated the requirement.
	 */
	int32_t value_ {};

	/**
	 * \brief Index position where the violation occurred.
	 */
	ToCData::size_type index_ {};

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] what_arg What argument
	 * \param[in] r        Requirement violated
	 * \param[in] v        Value that violated the requirement
	 * \param[in] i        Index position that violated the requirement
	 */
	InvalidMetadataException(const std::string& what_arg,
		const MetadataRequirement r, const int32_t v,
		const ToCData::size_type i);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] r Requirement violated
	 * \param[in] v Value that violated the requirement
	 * \param[in] i Index position that violated the requirement
	 */
	InvalidMetadataException(const MetadataRequirement r,
		const int32_t v, const ToCData::size_type i);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidMetadataException(const std::string& what_arg);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidMetadataException(const char* what_arg);

	/**
	 * \brief Requirement that has been violated.
	 *
	 * \return Requirement having been violated
	 */
	MetadataRequirement requirement() const;

	/**
	 * \brief Value that has violated the requirement.
	 *
	 * \return Value that caused the violation
	 */
	int32_t violating_value() const;

	/**
	 * \brief Index position where the violation occurred.
	 *
	 * A value greater than CDDA::MAX_TRACKCOUNT indicates that the violation
	 * did not occurr on a specific index position. This is the case for
	 * illegal or mismatiching values for the total number of tracks.
	 *
	 * \return Index position for a track offset, 0 for leadout
	 */
	ToCData::size_type pos() const;
};

/** @} */

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

