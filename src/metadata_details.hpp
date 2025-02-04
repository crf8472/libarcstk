#ifndef __LIBARCSTK_METADATA_HPP__
#error "Do not include metadata_details.hpp, include metadata.hpp instead"
#endif

#ifndef __LIBARCSTK_METADATA_DETAILS_HPP__
#define __LIBARCSTK_METADATA_DETAILS_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Implementation details for metadata.hpp.
 */

#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"
#endif

#include <cstdint>        // for int32_t

namespace arcstk
{
inline namespace v_1_0_0
{

namespace details
{

/**
 * \brief Convert \c value to the corrsponding number of bytes.
 *
 * \param[in] value Value to convert
 * \param[in] unit  Unit of the value
 *
 * \return The equivalent number of bytes.
 */
int32_t convert_to_bytes(const int32_t value, const UNIT unit) noexcept;

/**
 * \brief Validations for ToCData.
 */
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

/**
 * \brief Worker to validate LBA frame offset for being in legal range.
 *
 * \param[in] frames LBA frame amount to validate
 *
 * \throws std::invalid_argument
 */
void is_legal_offset(const int32_t offset);

/**
 * \brief Worker to validate track length in frames for being of legal size.
 *
 * \param[in] length Track length in LBA frames to validate
 *
 * \throws std::invalid_argument
 */
void is_legal_length(const int32_t length);

/**
 * \brief Validate leadout.
 *
 * \param[in] toc_data ToCData to validate
 */
void validate_leadout(const ToCData& toc_data);

/**
 * \brief Validate all offsets.
 *
 * \param[in] toc_data ToCData to validate
 */
void validate_offsets(const ToCData& toc_data);

/**
 * \brief Validate all lengths.
 *
 * \param[in] toc_data ToCData to validate
 */
void validate_lengths(const ToCData& toc_data);

/**
 * \brief Worker to throw when ToCData validation fails.
 *
 * \param[in] msg Error message
 *
 * \throws std::invalid_argument
 */
void throw_on_invalid_tocdata(const std::string& msg);

} // namespace validate
} // namespace details

} // namespace v_1_0_0
} // namespace arcstk

#endif

