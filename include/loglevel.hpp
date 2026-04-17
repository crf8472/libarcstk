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

#include <cstdint>        // for int16_t
#include <string>         // for string
#include <unordered_map>  // for unordered_map

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
 * \brief Converts a LOGLEVEL instance into its corresponding string
 * representation.
 *
 * \param[in] level The log level to turn to a string
 *
 * \return A string representation of the log level
 */
std::string to_string(LOGLEVEL level);

/**
 * \brief Converts a string representation of a LOGLEVEL to the
 * corresponding LOGLEVEL instance.
 *
 * Any string that is not an actual loglevel is converted to LOGLEVEL::NONE.
 *
 * \param[in] level The name of the log level to create
 *
 * \return The log level represented by the string or the default log level
 */
LOGLEVEL from_string(const std::string& level);

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



inline std::string to_string(LOGLEVEL level)
{
	// NOLINTNEXTLINE (cppcoreguidelines-avoid-c-arrays)
	static const char* const buffer[] =
	{
		"NONE  ",
		"ERROR ",
		"WARN  ",
		"INFO  ",
		"DEBUG ",
		"DEBUG1",
		"DEBUG2",
		"DEBUG3",
		"DEBUG4"
	};

	using loglevel_type = typename std::underlying_type<LOGLEVEL>::type;

	const auto idx = static_cast<loglevel_type>(level);

	if (idx > LOGLEVEL_MAX) // idx < 0 is not possible due to unsigned type
	{
		return "INVALID";
	}

	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
	return buffer[idx];
}


inline LOGLEVEL from_string(const std::string& level)
{
	static const std::unordered_map<std::string, LOGLEVEL> map
	{
		{ to_string(LOGLEVEL::NONE),    LOGLEVEL::NONE    },
		{ to_string(LOGLEVEL::ERROR),   LOGLEVEL::ERROR   },
		{ to_string(LOGLEVEL::WARNING), LOGLEVEL::WARNING },
		{ to_string(LOGLEVEL::INFO),    LOGLEVEL::INFO    },
		{ to_string(LOGLEVEL::DEBUG),   LOGLEVEL::DEBUG   },
		{ to_string(LOGLEVEL::DEBUG1),  LOGLEVEL::DEBUG1  },
		{ to_string(LOGLEVEL::DEBUG2),  LOGLEVEL::DEBUG2  },
		{ to_string(LOGLEVEL::DEBUG3),  LOGLEVEL::DEBUG3  },
		{ to_string(LOGLEVEL::DEBUG4),  LOGLEVEL::DEBUG4  },
	};

	const auto it = map.find(level);

	using std::cend;
	return (it != cend(map)) ? it->second : LOGLEVEL::NONE;
}

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif
