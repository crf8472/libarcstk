/**
 * \internal
 *
 * \file
 *
 * \brief Implementing a thread-safe, type-safe and portable logging interface
 */

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \brief The Logger for a single thread
 */
thread_local Logger Logging::logger_;

} // namespace v_1_0_0
} // namespace arcstk

