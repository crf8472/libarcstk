#ifndef LIBARCSTK_LIBARCSTK_HPP_
#define LIBARCSTK_LIBARCSTK_HPP_


/**
 * \file
 *
 * \brief libarcstk API.
 *
 * \details
 *
 * Include the entire API of libarcstk.
 */


/* Logging macros */

#ifndef LIBARCSTK_LOGGING_HPP_
#include <arcstk/logging.hpp>
#endif


/* Policies widely used by libarcstk */

#ifndef LIBARCSTK_POLICIES_HPP_
#include <arcstk/policies.hpp>
#endif


/* Input: interfaces for toc metadata */

#ifndef LIBARCSTK_METADATA_HPP_
#include <arcstk/metadata.hpp>
#endif


/* Input: interfaces for audio samples */

#ifndef LIBARCSTK_SAMPLES_HPP_
#include <arcstk/samples.hpp>
#endif


/* Identifiers */

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include <arcstk/identifier.hpp>
#endif


/* Checksum types */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include <arcstk/checksum.hpp>
#endif


/* AccurateRip algorithms */

#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include <arcstk/algorithms.hpp>
#endif


/* Calculation interface */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include <arcstk/calculate.hpp>
#endif


/* DBAR parsing */

#ifndef LIBARCSTK_DBAR_HPP_
#include <arcstk/dbar.hpp>
#endif


/* Checksum verification */

#ifndef LIBARCSTK_VERIFY_HPP_
#include <arcstk/verify.hpp>
#endif


/* Version, Name and Release info */

#ifndef LIBARCSTK_VERSION_HPP_
#include <arcstk/version.hpp>
#endif

#endif

