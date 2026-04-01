#ifndef LIBARCSTK_ACCURATERIP_FUNC_HPP_
#define LIBARCSTK_ACCURATERIP_FUNC_HPP_

/**
 * \internal
 *
 * \file
 *
 * \brief Additional support functions for accuraterip.hpp.
 */

#include <cstdint>  // for uint32_t, int32_t
#include <string>   // for string

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

namespace accuraterip
{

namespace id
{

/**
 * \brief Worker: Print a sequence of ids.
 *
 * \param[in] out           Stream to print to
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 * \param[in] delim         Delimiter
 */
void print_impl(std::ostream& out, const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id, const std::string& delim);

} // namespace id


namespace details
{

/**
 * \brief Service method: sum up the digits of the number passed
 *
 * \param[in] number An unsigned integer number
 *
 * \return The sum of the digits of the number
 */
uint64_t sum_digits(const uint32_t number) noexcept;

} // namespace details

} // namespace accuraterip

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

