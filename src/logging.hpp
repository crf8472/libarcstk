#ifndef __LIBARCSTK_LOGGING_HPP__
#define __LIBARCSTK_LOGGING_HPP__


/**
 * \file logging.hpp A thread-safe, type-safe and portable logging interface
 *
 * Provides the logging macros along with a global Logging instance
 *
 * The code in this file is based on ideas by Petru Marginean published in:
 * Marginean, P: "Logging in C++", http://www.ddj.com/cpp/201804215
 * and
 * Marginean, P: "Logging in C++: Part 2", http://www.ddj.com/cpp/221900468
 */


#include <cstdio>

#include <memory>
#include <mutex>         // for mutex
#include <sstream>
#include <string>
#include <unordered_set>


namespace arcs
{

/// \defgroup logging Logging API for use by clients
/// @{

inline namespace v_1_0_0
{

/**
 * Range of log levels
 */
enum LOGLEVEL_t : uint8_t
{
	LOG_NONE     = 0,
	//
	LOG_ERROR    = 1,
	LOG_WARNING  = 2,
	LOG_INFO     = 3,
	LOG_DEBUG    = 4,
	LOG_DEBUG1   = 5,
	LOG_DEBUG2   = 6,
	LOG_DEBUG3   = 7,
	LOG_DEBUG4   = 8
};


// Appender


/**
 * \brief A named logging output channel.
 *
 * An Appender can be constructed for either a <tt>FILE*</tt> or a std::string
 * representing a filename. An always has a name. If the Appender is
 * constructed with just a filename, this string becomes its name. If it is
 * constructed with a <tt>FILE*</tt> and a name, the name is arbitrary.
 */
class Appender final
{

public:

	/**
	 * Constructs an Appender for appending to the given filename,
	 * whereby the filename becomes the name of the Appender.
	 *
	 * \param[in] filename File to append to and name of the Appender
	 */
	explicit Appender(const std::string &filename);

	/**
	 * Constructs an Appender for appending to the given <tt>FILE</tt>
	 * (may also be <tt>stdout</tt>, <tt>stderr</tt> etc.)
	 *
	 * \param[in] name Name of the Appender
	 * \param[in] stream The <tt>FILE</tt> to append to
	 */
	Appender(const std::string &name, FILE* stream);

	/**
	 * Appender is non-copyable
	 */
	Appender(const Appender &a) = delete;

	/**
	 * Appender is non-copyable
	 */
	Appender(Appender &&a) noexcept;

	/**
	 * Virtual default destructor
	 */
	~Appender() noexcept;

	/**
	 * Append the specified message to the output stream
	 *
	 * \param[in] msg The message to output
	 */
	void append(const std::string& msg) const;

	/**
	 * Name of the Appender
	 *
	 * \return Name of the Appender
	 */
	std::string name() const;

	/**
	 * Appender is non-copyable
	 */
	Appender& operator = (const Appender &a) = delete;

	/**
	 * Move assignment
	 */
	Appender& operator = (Appender &&a) noexcept;


private:

	/**
	 * Internal name of the Appender
	 */
	std::string name_;

	/**
	 * Internal stream to append to
	 */
	FILE* stream_;
};


// Logger


/**
 * \brief Logs a message to its registered @link Appender Appenders @endlink.
 *
 * A Logger has a configurable log level and logs messages to all of
 * its appenders.
 *
 * The default log level is <tt>LOG_WARNING</tt>.
 */
class Logger
{

public:

	/**
	 * Constructor
	 */
	Logger();

	/**
	 * Logger is non-copyable
	 */
	Logger(const Logger&) = delete;

	/**
	 * Move constructor
	 */
	Logger(Logger&& logger) noexcept;

	/**
	 * Destructor
	 */
	~Logger() noexcept;

	/**
	 * Log level of this instance.
	 *
	 * \return The log level of this instance
	 */
	LOGLEVEL_t level() const;

	/**
	 * Set the log level for this instance.
	 *
	 * \param[in] level The log level for this instance
	 */
	void set_level(LOGLEVEL_t level);

	/**
	 * Return TRUE iff the log level of this Logger is greater or equal
	 * than the level passed.
	 *
	 * \param[in] level The level to check for
	 *
	 * \return TRUE iff Logger has at least the level passed
	 */
	bool has_level(LOGLEVEL_t level);

	/**
	 * Activates or deactivates the output of timestamps
	 *
	 * \param[in] onoff TRUE activates the logging of timestamps for this logger
	 */
	void set_timestamps(const bool &onoff);

	/**
	 * Returns TRUE iff this instance is configured to log timestamps.
	 *
	 * \return TRUE iff this instance will log timestamps.
	 */
	bool has_timestamps();

	/**
	 * Add an Appender to this Logger
	 *
	 * \param[in] appender An Appender to use
	 */
	void add_appender(std::unique_ptr<Appender> appender);

	/**
	 * Remove the given Appender from this Logger
	 *
	 * \param[in] appender An Appender to remove
	 */
	void remove_appender(Appender *appender);

	/**
	 * Log the given message to all <tt>Appender</tt>s
	 *
	 * \param[in] msg The message to log
	 */
	void log(const std::string &msg) const;

	/**
	 * Logger is non-copyable
	 */
	Logger& operator = (const Logger&) = delete;

	/**
	 * Move assignment
	 */
	Logger& operator = (Logger&& logger) noexcept;


private:

	/**
	 * Internal set of <tt>Appender</tt>s
	 */
	std::unordered_set<std::unique_ptr<Appender>> appenders_;

	/**
	 * Internal log level
	 */
	LOGLEVEL_t level_;

	/**
	 * Flag to activate/deactivate the logging of timestamps
	 */
	bool log_timestamps_;
};


// now_time


/**
 * \brief Returns the current time in the format 'YYYY-MM-DD hh:mm:ss.lll'.
 *
 * Returns the current time as a string containing year, month, day, hours,
 * minutes, seconds and milliseconds in the format 'YYYY-MM-DD hh:mm:ss.lll'.
 *
 * \return The current time as a string
 */
std::string now_time();


// Log


/**
 * \brief Logs a message using a Logger and a log level
 *
 * (More or less) thread-safe, type-safe, portable logging interface for
 * concrete <tt>Logger</tt>s.
 */
class Log final
{

public:

	/**
	 * Empty constructor
	 *
	 * \param[in] logger    Logger to use
	 * \param[in] msg_level Loglevel of the message to log
	 */
	Log(Logger *logger, LOGLEVEL_t msg_level);

	/**
	 * Class is non-copyable
	 */
	Log(const Log&) = delete;

	/**
	 * Class is non-copyable
	 */
	Log(Log&& log) noexcept;

	/**
	 * Default destructor
	 */
	~Log() noexcept;

	/**
	 * Get the output stream to write to
	 *
	 * \return Get the output stream to write to
	 */
	std::ostringstream& get();

	/**
	 * Turns a LOGLEVEL_t instance into a string representation
	 *
	 * \param[in] level The log level to turn to a string
	 *
	 * \return A string representation of the log level
	 */
	static std::string to_string(LOGLEVEL_t level);

	/**
	 * Turns a string representation of the log level to a LOGLEVEL_t instance
	 *
	 * \param[in] level The name of the log level to create
	 *
	 * \return The log level represented by the string or the default log level
	 */
	static LOGLEVEL_t from_string(const std::string& level);

	/**
	 * Class is non-copyable
	 */
	Log& operator = (const Log&) = delete;

	/**
	 * Class is non-copyable
	 */
	Log& operator = (Log&& log) noexcept;


private:

	/**
	 * Internal output stream representation
	 */
	std::ostringstream os_;

	/**
	 * Internal Logger to use
	 */
	Logger *logger_;

	/**
	 * Loglevel of the message to log
	 */
	LOGLEVEL_t msg_level_;
};


/**
 * \brief A singleton interface used by all Logger instances.
 *
 * A singleton manager and thread safe interface class for all Logger
 * instances of the entire component.
 */
class Logging
{

public:

	/**
	 * Class is non-copyable
	 */
	Logging(const Logging&) = delete;

	/**
	 * Class is non-movable
	 */
	Logging(Logging&&) noexcept = delete;

	/**
	 * Standard destructor
	 */
	virtual ~Logging() noexcept;

	/**
	 * Handle for the singleton.
	 *
	 * \return This instance
	 */
	static Logging& instance();

	/**
	 * Access the internal Logger.
	 *
	 * \return The internal logger object
	 */
	Logger* logger();

	/**
	 * Returns the current log level.
	 *
	 * \return Current log level
	 */
	LOGLEVEL_t level() const;

	/**
	 * Set the log level.
	 *
	 * \param[in] level Set the log level
	 */
	void set_level(LOGLEVEL_t level);

	/**
	 * Return TRUE iff the global log level is greater or equal than the level
	 * passed.
	 *
	 * \param[in] level The level to check for
	 *
	 * \return TRUE iff Logger has at least the level passed
	 */
	bool has_level(LOGLEVEL_t level);

	/**
	 * Activates or deactivates the output of timestamps.
	 *
	 * \param[in] onoff TRUE activates logging of timestamps is activated
	 */
	void set_timestamps(const bool &onoff);

	/**
	 * Returns TRUE iff output of timestamps is activated, otherwise FALSE.
	 *
	 * \return TRUE iff timestamps are logged, otherwise FALSE.
	 */
	bool has_timestamps();

	/**
	 * Add an appender to the internal Logger.
	 *
	 * \param[in] appender The Appender to add
	 */
	void add_appender(std::unique_ptr<Appender> appender);

	/**
	 * Remove given appender from the internal Logger.
	 *
	 * \param[in] appender The Appender to remove
	 */
	void remove_appender(Appender *appender);

	/**
	 * Class is non-copyable
	 */
	Logging& operator = (Logging& logging) = delete;

	/**
	 * Class is non-movable
	 */
	Logging& operator = (Logging&& logging) noexcept = delete;


private:

	/**
	 * Internal Logger instance
	 */
	static thread_local Logger logger_;

	/**
	 * Mutex for thread-safe access to internal Logger instance
	 */
	std::mutex mutex_;

	/**
	 * Class is singleton
	 */
	Logging();
};

} // namespace v_1_0_0

/// @}

} // namespace arcs


/// \addtogroup logging
/// @{

// Macros


// Define a clipping for the log level: every message that has not at least the
// clipping level is immediatly discarded.
// Since this is a compile-time constant, the optimizer will recognize any
// comparison loglevel == LOG_NONE and remove the statement in question
// entirely. (See the definition of the ARCS_LOG_* macros to inspect how the
// clipping level is checked.)
#ifndef CLIP_LOGGING_LEVEL
#    define CLIP_LOGGING_LEVEL arcs::LOG_DEBUG4
#endif

/**
 * Send error message to the logger libarcstk uses internally
 */
#define ARCS_LOG_ERROR \
    if (arcs::LOG_ERROR > CLIP_LOGGING_LEVEL) ; \
    else if (arcs::LOG_ERROR > arcs::Logging::instance().level()) ; \
    else arcs::Log(arcs::Logging::instance().logger(), arcs::LOG_ERROR).get()

/**
 * Send warning message to the logger libarcstk uses internally
 */
#define ARCS_LOG_WARNING \
    if (arcs::LOG_WARNING > CLIP_LOGGING_LEVEL) ; \
    else if (arcs::LOG_WARNING > arcs::Logging::instance().level()) ; \
    else arcs::Log(arcs::Logging::instance().logger(), arcs::LOG_WARNING).get()

/**
 * Send info message to the logger libarcstk uses internally
 */
#define ARCS_LOG_INFO \
    if (arcs::LOG_INFO > CLIP_LOGGING_LEVEL) ; \
    else if (arcs::LOG_INFO > arcs::Logging::instance().level()) ; \
    else arcs::Log(arcs::Logging::instance().logger(), arcs::LOG_INFO).get()

/**
 * Send debug message to the logger libarcstk uses internally
 */
#define ARCS_LOG_DEBUG \
    if (arcs::LOG_DEBUG > CLIP_LOGGING_LEVEL) ; \
    else if (arcs::LOG_DEBUG > arcs::Logging::instance().level()) ; \
    else arcs::Log(arcs::Logging::instance().logger(), arcs::LOG_DEBUG).get()

/**
 * Send log message with specified log level to the logger libarcstk uses
 * internally.
 *
 * This is useful for custom log levels beyond LOG_DEBUG
 */
#define ARCS_LOG(loglevel) \
    if (loglevel > CLIP_LOGGING_LEVEL) ; \
    else if (loglevel > arcs::Logging::instance().level()) ; \
    else arcs::Log(arcs::Logging::instance().logger(), loglevel).get()


// The ARCS_LOG* macros ensure a reduction of logging costs as follows:
//
// 1) Note that CLIP_LOGGING_LEVEL is a compile time constant. The comparison of
// loglevel and CLIP_LOGGING_LEVEL comes out as a comparison between two compile
// time constants. This indicates the optimizer to eliminate all logging
// statements with a level bigger than CLIP_LOGGING_LEVEL effectively from the
// code.
//
// 2) The comparison of loglevel and Logging::level() ensures that all
// logging costs beyond this comparison are only generated for actual logging
// operations. It thereby avoids a lot of string manipulation for operations
// that may be in fact discarded due to a wrong log level.

/// @}

#endif

