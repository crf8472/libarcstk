#ifndef __LIBARCSTK_CHECKSUM_HPP__
#error "Do not include checksum_details.hpp, include checksum.hpp instead"
#endif

#ifndef __LIBARCSTK_CHECKSUM_DETAILS_HPP__
#define __LIBARCSTK_CHECKSUM_DETAILS_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Implementation details for checksum.hpp.
 */

#include <ostream>  // for ostream

namespace arcstk
{
inline namespace v_1_0_0
{

class Checksum;

namespace checksum
{
namespace details
{

/**
 * \brief Worker for printing a Checksum to an output stream.
 *
 * \param[in] checksum	Checksum to print
 * \param[in] upper		Flag: iff TRUE, print letters uppercase
 * \param[in] base		Flag: iff TRUE, print base 0x
 * \param[in] out		Output stream to print to
 */
void print_hex(const Checksum& checksum, const bool upper,
		const bool base, std::ostream& out);

} // namespace details
} // namespace checksum

} // namespace v_1_0_0
} // namespace arcstk

#endif

