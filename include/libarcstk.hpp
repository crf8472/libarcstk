#ifndef LIBARCSTK_LIBARCSTK_HPP_
#define LIBARCSTK_LIBARCSTK_HPP_


/**
 * \file
 *
 * \brief libarcstk API.
 *
 * \details
 *
 * Include the public API of libarcstk.
 */


/* Input: interfaces for toc metadata */

#ifndef LIBARCSTK_METADATA_HPP_
#include <arcstk/metadata.hpp>
#endif


/* Identifier interface */

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include <arcstk/identifier.hpp>
#endif


/* Checksum interface and types */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include <arcstk/checksum.hpp>
#endif


/* Input: interfaces for audio samples */

#ifndef LIBARCSTK_SAMPLES_HPP_
#include <arcstk/samples.hpp>
#endif


/* Algorithm interface */

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include <arcstk/algorithm.hpp>
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


// The following 5 public headers are intentionally not included:
// accuraterip.hpp
// bytes.hpp
// logging.hpp
// loglevel.hpp
// mixins.hpp

#endif

