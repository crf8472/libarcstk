#ifndef LIBARCSTK_LOGLEVEL_HPP_
#define LIBARCSTK_LOGLEVEL_HPP_

/**
 * \internal
 *
 * \file
 *
 * \brief Compile-time log level configuration for the logging system.
 *
 * This file is configured by CMake and configures the default log level
 * clipping based on the build type (Debug vs Release).
 */

#include <cstdint>  // for int16_t

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

/**
 * \brief Range of log levels
 *
 * The loglevels are totally ordered in ascending order of verbosity starting
 * on the value 0 that represents the level where nothing is logged at all.
 */
enum class LOGLEVEL : uint8_t
{
	NONE     = 0,
	//
	ERROR    = 1,
	WARNING  = 2,
	INFO     = 3,
	DEBUG    = 4,
	DEBUG1   = 5,
	DEBUG2   = 6,
	DEBUG3   = 7,
	DEBUG4   = 8
};


/**
 * \brief Numeric representation of the minimal legal loglevel.
 */
constexpr int LOGLEVEL_MIN = 0;


/**
 * \brief Numeric representation of the maximum legal loglevel.
 */
constexpr int LOGLEVEL_MAX = 8;


/**
 * \brief Compile-time logging level clipping.
 *
 * Messages with a level GREATER than this constant are eliminated
 * entirely by the compiler, resulting in zero runtime overhead.
 *
 * - Debug builds:   All levels enabled (DEBUG4)
 * - Release builds: Only ERROR, WARNING, INFO (production logging)
 */
constexpr LOGLEVEL CLIP_LOGGING_LEVEL = arcstk::LOGLEVEL::DEBUG4;

// TODO Make this CMake-configurable:
//constexpr LOGLEVEL CLIP_LOGGING_LEVEL = arcstk::LOGLEVEL::@LIBARCSTK_LOG_LEVEL@;


                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif
