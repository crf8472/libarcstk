#ifndef LIBARCSTK_LIBARCSTK_HPP__
#define LIBARCSTK_LIBARCSTK_HPP__


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

#ifndef LIBARCSTK_LOGGING_HPP__
#include <arcstk/logging.hpp>
#endif


/* Policies widely used by libarcstk */

#ifndef LIBARCSTK_POLICIES_HPP__
#include <arcstk/policies.hpp>
#endif


/* Input: interfaces for toc metadata */

#ifndef LIBARCSTK_METADATA_HPP__
#include <arcstk/metadata.hpp>
#endif


/* Input: interfaces for audio samples */

#ifndef LIBARCSTK_SAMPLES_HPP__
#include <arcstk/samples.hpp>
#endif


/* Identifiers */

#ifndef LIBARCSTK_IDENTIFIER_HPP__
#include <arcstk/identifier.hpp>
#endif


/* Checksum types */

#ifndef LIBARCSTK_CHECKSUM_HPP__
#include <arcstk/checksum.hpp>
#endif


/* AccurateRip algorithms */

#ifndef LIBARCSTK_ALGORITHMS_HPP__
#include <arcstk/algorithms.hpp>
#endif


/* Calculation interface */

#ifndef LIBARCSTK_CALCULATE_HPP__
#include <arcstk/calculate.hpp>
#endif


/* DBAR parsing */

#ifndef LIBARCSTK_DBAR_HPP__
#include <arcstk/dbar.hpp>
#endif


/* Checksum verification */

#ifndef LIBARCSTK_VERIFY_HPP__
#include <arcstk/verify.hpp>
#endif


/* Version, Name and Release info */

#ifndef LIBARCSTK_VERSION_HPP__
#include <arcstk/version.hpp>
#endif

#endif

