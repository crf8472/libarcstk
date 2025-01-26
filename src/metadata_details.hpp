#ifndef __LIBARCSTK_METADATA_HPP__
#error "Do not include metadata_details.hpp, include metadata.hpp instead"
#endif

#ifndef __LIBARCSTK_METADATA_DETAILS_HPP__
#define __LIBARCSTK_METADATA_DETAILS_HPP__

/**
 * \file
 *
 * \brief Declare implementation details for metadata.hpp.
 */

#include <cstdint>        // for int32_t

#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

namespace details
{

/**
 * \brief Convert amount of frames to the equivalent amount of samples.
 *
 * \param[in] frames Amount of frames to convert
 *
 * \return Amount of samples equivalent to \c frames
 */
int32_t frames2samples(const int32_t frames);

/**
 * \brief Convert amount of samples to the equivalent amount of frames.
 *
 * \param[in] samples Amount of samples to convert
 *
 * \return Amount of frames equivalent to \c samples
 */
int32_t samples2frames(const int32_t samples);

/**
 * \brief Convert amount of frames to the equivalent amount of bytes.
 *
 * \param[in] frames Amount of frames to convert
 *
 * \return Amount of bytes equivalent to \c frames
 */
int32_t frames2bytes(const int32_t frames);

/**
 * \brief Convert amount of bytes to the equivalent amount of frames.
 *
 * \param[in] bytes Amount of bytes to convert
 *
 * \return Amount of frames equivalent to \c bytes
 */
int32_t bytes2frames(const int32_t bytes);

/**
 * \brief Convert amount of samples to the equivalent amount of bytes.
 *
 * \param[in] samples Amount of samples to convert
 *
 * \return Amount of bytes equivalent to \c samples
 */
int32_t samples2bytes(const int32_t samples);

/**
 * \brief Convert amount of bytes to the equivalent amount of samples.
 *
 * \param[in] bytes Amount of bytes to convert
 *
 * \return Amount of samples equivalent to \c bytes
 */
int32_t bytes2samples(const int32_t bytes);


/**
 * \brief Convert \c value to the corrsponding number of bytes.
 *
 * \param[in] value Value to convert
 * \param[in] unit  Unit of the value
 *
 * \return The equivalent number of bytes.
 */
int32_t to_bytes(const int32_t value, const AudioSize::UNIT unit) noexcept;

/**
 * \brief Convert \c bytes to the specified \c unit.
 *
 * \param[in] bytes Amount of bytes
 * \param[in] unit  Unit to convert to
 *
 * \return The equivalent value in the specified unit
 */
int32_t from_bytes(const int32_t bytes, const AudioSize::UNIT unit) noexcept;


namespace validate
{

/**
 * \brief Maximal valid offset value for a non-redbook 90 min CD (in LBA
 * frames).
 *
 * Non-redbook 90-min CD has 89:59.74 which is equivalent to 405.000 frames.
 */
static constexpr int32_t MAX_OFFSET_90 { (89 * 60 + 59) * 75 + 74 };

/**
 * \brief Maximal valid offset value for a non-redbook 99 min CD (in LBA
 * frames).
 *
 * Non-redbook 99-min CD has 98:59.74 which is equivalent to 445.500 frames.
 */
static constexpr int32_t MAX_OFFSET_99 { (98 * 60 + 59) * 75 + 74 };

void legal_leadout_size(const ToCData& toc_data);
void legal_offset_sizes(const ToCData& toc_data);
void legal_total_tracks(const ToCData& toc_data);

void legal_ordering(const ToCData& toc_data);
void legal_minimum_distances(const ToCData& toc_data);

}

} //namespace details
} // namespace v_1_0_0
} // namespace arcstk

#endif

