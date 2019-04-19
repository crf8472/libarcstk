#ifndef __LIBARCSTK_LOGGING_HPP__
#define __LIBARCSTK_LOGGING_HPP__


/**
 * \file
 *
 * \brief Public Logging API for clients.
 *
 * Provides a thread-safe, type-safe and portable logging interface in form
 * of the logging macros along with a global Logging instance.
 *
 * The code in this file is based on ideas by Petru Marginean published in:
 * Marginean, P: "Logging in C++", http://www.ddj.com/cpp/201804215
 * and
 * Marginean, P: "Logging in C++: Part 2", http://www.ddj.com/cpp/221900468
 */


#include <cstdio>

#include <chrono>
#include <ctime>         // for localtime
#include <iomanip>       // for put_time
#include <memory>
#include <mutex>         // for lock_guard, mutex
#include <sstream>
#include <stdexcept>     // for runtime_error
#include <string>
#include <type_traits>   // for underlying_type
#include <unordered_set>



namespace arcstk
{

/// \defgroup logging Logging API for use by clients
/// @{

inline namespace v_1_0_0
{

/**
 * Range of log levels
 */
enum class LOGLEVEL : int16_t
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


// Appender


/**
 * \brief A named logging output channel.
 *
 * An Appender can be constructed for either a <tt>std::string</tt> representing
 * a filename or a <tt>FILE*</tt> along with a name.
 *
 * An Appender always has a name. If the Appender is constructed with just a
 * filename, this filename becomes its name. If it is constructed with a
 * <tt>FILE*</tt> and a name, the name is arbitrary.
 */
class Appender final {

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
	Appender(const Appender&) = delete;

	/**
	 * Move constructor
	 *
	 * \param[in] rhs The instance to move
	 */
	Appender(Appender &&rhs) noexcept;

	/**
	 * Destructor
	 */
	~Appender() noexcept;

	/**
	 * Append the specified message
	 *
	 * \param[in] msg The message to append
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
	Appender& operator = (const Appender&) = delete;

	/**
	 * Move assignment
	 *
	 * \param[in] rhs The instance to move
	 */
	Appender& operator = (Appender &&rhs) noexcept;


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


inline Appender::Appender(const std::string &filename)
	: name_(filename)
	, stream_(std::fopen(name_.c_str(), "a"))
{
	if (!stream_)
	{
		std::stringstream ss;
		ss << "File " << name_.c_str() << " could not be opened";
		throw std::runtime_error(ss.str());
	}
}


inline Appender::Appender(const std::string &name, FILE* stream)
	: name_(name)
	, stream_(stream)
{
	if (!stream)
	{
		std::stringstream ss;
		ss << "Appender " << name_.c_str() << " has no stream to append to";
		throw std::runtime_error(ss.str());
	}
}


inline Appender::Appender(Appender&& rhs) noexcept = default;


inline Appender::~Appender() noexcept
{
	if (stream_)
	{
		std::fclose(stream_);
	}
}


inline void Appender::append(const std::string& msg) const
{
	if (!stream_)
	{
		return;
	}

	std::fprintf(stream_, "%s", msg.c_str());
	std::fflush(stream_);
	// Note: According to
	// http://www.gnu.org/software/libc/manual/html_node/Streams-and-Threads.html
	// all stream operations are thread safe, ergo using fprintf buys us
	// thread-safety in principle. This at least ensures that no lines are
	// scrambled.
}


inline std::string Appender::name() const
{
	return name_;
}


inline Appender& Appender::operator = (Appender&& rhs) noexcept = default;


// Logger


/**
 * \brief Logs a message to its registered @link Appender Appenders @endlink.
 *
 * A Logger associates a configuration with a set of
 * @link Appender Appenders @endlink. Can be configured to log timestamps. The
 * default is <tt>true</tt>.
 */
class Logger final
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
	 * Default move constructor
	 *
	 * \param[in] rhs The instance to move
	 */
	Logger(Logger&& rhs) noexcept;

	/**
	 * Default destructor
	 */
	~Logger() noexcept;

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
	bool has_timestamps() const;

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
	void remove_appender(const Appender *appender);

	/**
	 * Log the given message to all @link Appender Appenders @endlink
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
	 * Internal set of @link Appender Appenders @endlink
	 */
	std::unordered_set<std::unique_ptr<Appender>> appenders_;

	/**
	 * Flag to activate/deactivate the logging of timestamps
	 */
	bool log_timestamps_;
};


inline Logger::Logger()
	: appenders_()
	//, level_(LOGLEVEL::WARNING)
	, log_timestamps_(true)
{
	// empty
}


inline Logger::Logger(Logger&& logger) noexcept = default;


inline Logger::~Logger() noexcept = default;


inline void Logger::set_timestamps(const bool &on_or_off)
{
	log_timestamps_ = on_or_off;
}


inline bool Logger::has_timestamps() const
{
	return log_timestamps_;
}


inline void Logger::add_appender(std::unique_ptr<Appender> appender)
{
	appenders_.emplace(std::move(appender));
}


inline void Logger::remove_appender(const Appender *appender)
{
	for (auto& app : appenders_)
	{
		if (app.get() == appender)
		{
			appenders_.erase(app);
		}
	}
}


inline void Logger::log(const std::string &msg) const
{
	for (auto& appender : appenders_)
	{
		appender->append(msg);
	}
}


inline Logger& Logger::operator = (Logger&& rhs) noexcept = default;


// now_time


/**
 * \brief Returns the current time in the format <tt>'YYYY-MM-DD hh:mm:ss.lll'</tt>.
 *
 * Returns the current time as a string containing year, month, day, hours,
 * minutes, seconds and milliseconds in the format 'YYYY-MM-DD hh:mm:ss.lll'.
 *
 * \return The current time as a string
 */
std::string now_time();


inline std::string now_time()
{
	auto now = std::chrono::system_clock::now();
	std::stringstream ss;

	// Print year, month, day, hour, minute, second

	{
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %X");
	}

	// Print milliseconds

	{
		auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
		auto m = now - seconds;
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(m);
		ss << "." << millis.count();
	}

	return ss.str();
}


// Log


/**
 * \brief A single logging operation of a Logger using a specified LOGLEVEL.
 *
 * A (More or less) thread-safe, type-safe, portable logging interface for
 * concrete @link Logger Loggers @endlink.
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
	Log(const Logger &logger, LOGLEVEL msg_level);

	/**
	 * Class is non-copyable
	 */
	Log(const Log&) = delete;

	/**
	 * Class is non-movable
	 */
	Log(Log&&) = delete;

	/**
	 * Destructor
	 */
	~Log() noexcept;

	/**
	 * Get the output stream to write to
	 *
	 * \return Get the output stream to write to
	 */
	std::ostringstream& get();

	/**
	 * Turns a LOGLEVEL instance into a string representation
	 *
	 * \param[in] level The log level to turn to a string
	 *
	 * \return A string representation of the log level
	 */
	static std::string to_string(LOGLEVEL level);

	/**
	 * Turns a string representation of the log level to a LOGLEVEL instance
	 *
	 * \param[in] level The name of the log level to create
	 *
	 * \return The log level represented by the string or the default log level
	 */
	static LOGLEVEL from_string(const std::string& level);

	/**
	 * Class is non-copyable
	 */
	Log& operator = (const Log&) = delete;

	/**
	 * Class is non-movable
	 */
	Log& operator = (Log&&) noexcept = delete;


private:

	/**
	 * Internal output stream representation
	 */
	std::ostringstream os_;

	/**
	 * Internal Logger to use
	 */
	const Logger *logger_;

	/**
	 * Loglevel of the message to log
	 */
	LOGLEVEL msg_level_;
};


inline Log::Log(const Logger &logger, LOGLEVEL msg_level)
	: os_()
	, logger_(&logger)
	, msg_level_(msg_level)
{
	// empty
}


inline Log::~Log() noexcept
{
	os_ << std::endl;

	if (logger_)
	{
		logger_->log(os_.str());
	}
}


inline std::ostringstream& Log::get()
{
	// Timestamp

	if (logger_->has_timestamps())
	{
		os_ << "- " << now_time() << " ";
	}

	// Loglevel string

	os_ << Log::to_string(msg_level_) << ": ";

	// Indent messages with level DEBUG and higher

	os_ << std::string(
		msg_level_ > LOGLEVEL::DEBUG
			? 2 * (static_cast<typename
					std::underlying_type<LOGLEVEL>::type>(msg_level_)
					-
					static_cast<typename
					std::underlying_type<LOGLEVEL>::type>(LOGLEVEL::DEBUG)
				)
			: 0, ' ');

	return os_;
}


inline std::string Log::to_string(LOGLEVEL level)
{
	static const char* const buffer[] =
	{
		"NONE",
		" ERROR",
		"  WARN",
		"  INFO",
		" DEBUG",
		"DEBUG1",
		"DEBUG2",
		"DEBUG3",
		"DEBUG4"
	};

	return buffer[
		static_cast<typename std::underlying_type<LOGLEVEL>::type>(level)
	];
}


inline LOGLEVEL Log::from_string(const std::string& level)
{
	if (level == "NONE")
		{ return LOGLEVEL::NONE; }

	if (level == "ERROR")
		{ return LOGLEVEL::ERROR; }

	if (level == "WARNING")
		{ return LOGLEVEL::WARNING; }

	if (level == "INFO")
		{ return LOGLEVEL::INFO; }

	if (level == "DEBUG")
		{ return LOGLEVEL::DEBUG; }

	if (level == "DEBUG1")
		{ return LOGLEVEL::DEBUG1; }

	if (level == "DEBUG2")
		{ return LOGLEVEL::DEBUG2; }

	if (level == "DEBUG3")
		{ return LOGLEVEL::DEBUG3; }

	if (level == "DEBUG4")
		{ return LOGLEVEL::DEBUG4; }

	return LOGLEVEL::NONE;
}


/**
 * \brief A singleton interface used by all Log instances.
 *
 * A singleton manager and thread safe interface class for all
 * @link Log Logs @endlink of the entire component.
 */
class Logging final
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
	const Logger& logger();

	/**
	 * Returns the current log level.
	 *
	 * \return Current log level
	 */
	LOGLEVEL level() const;

	/**
	 * Set the log level.
	 *
	 * \param[in] level Set the log level
	 */
	void set_level(LOGLEVEL level);

	/**
	 * Return TRUE iff the global log level is greater or equal than \c level.
	 *
	 * \param[in] level The level to check for
	 *
	 * \return TRUE iff Logger has at least the level passed
	 */
	bool has_level(LOGLEVEL level);

	/**
	 * Activates or deactivates the output of timestamps.
	 *
	 * \param[in] activate TRUE activates logging of timestamps
	 */
	void set_timestamps(const bool &activate);

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
	Logging& operator = (Logging& rhs) = delete;

	/**
	 * Class is non-movable
	 */
	Logging& operator = (Logging&& rhs) noexcept = delete;


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
	 * Internal log level
	 */
	LOGLEVEL level_;

	/**
	 * Class is singleton
	 */
	Logging();
};


inline Logging::Logging()
	: mutex_()
	, level_(LOGLEVEL::WARNING)
{
	// empty
}


inline Logging::~Logging() noexcept = default;


inline const Logger& Logging::logger()
{
	return logger_;
}


inline Logging& Logging::instance()
{
	// This should not introduce any memory leaks and is thread-safe when
	// compiled with a C++11 conforming compiler

	static Logging logging;

	return logging;
}


inline LOGLEVEL Logging::level() const
{
	return level_;
}


inline void Logging::set_level(LOGLEVEL level)
{
	std::lock_guard<std::mutex> lock(mutex_);
	level_ = level;
}


inline bool Logging::has_level(LOGLEVEL level)
{
	return level_ >= level;
}


inline void Logging::set_timestamps(const bool &on_or_off)
{
	std::lock_guard<std::mutex> lock(mutex_);
	logger_.set_timestamps(on_or_off);
}


inline bool Logging::has_timestamps()
{
	return logger_.has_timestamps();
}


inline void Logging::add_appender(std::unique_ptr<Appender> appender)
{
	std::lock_guard<std::mutex> lock(mutex_);
	logger_.add_appender(std::move(appender));
}


inline void Logging::remove_appender(Appender *a)
{
	std::lock_guard<std::mutex> lock(mutex_);
	logger_.remove_appender(a);
}

} // namespace v_1_0_0

/// @}

} // namespace arcstk


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
#    define CLIP_LOGGING_LEVEL arcstk::LOGLEVEL::DEBUG4
#endif

/**
 * Send error message to the logger libarcstk uses internally
 */
#define ARCS_LOG_ERROR \
    if (arcstk::LOGLEVEL::ERROR > CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::ERROR > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::ERROR).get()

/**
 * Send warning message to the logger libarcstk uses internally
 */
#define ARCS_LOG_WARNING \
    if (arcstk::LOGLEVEL::WARNING > CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::WARNING > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::WARNING).get()

/**
 * Send info message to the logger libarcstk uses internally
 */
#define ARCS_LOG_INFO \
    if (arcstk::LOGLEVEL::INFO > CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::INFO > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::INFO).get()

/**
 * Send debug message to the logger libarcstk uses internally
 */
#define ARCS_LOG_DEBUG \
    if (arcstk::LOGLEVEL::DEBUG > CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::DEBUG > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::DEBUG).get()

/**
 * Send log message with specified log level to the logger libarcstk uses
 * internally.
 *
 * Specify the loglevel without the prefix <tt>LOGLEVEL::</tt>.
 *
 * This is useful for custom log levels beyond DEBUG
 */
#define ARCS_LOG(loglevel) \
    if (arcstk::LOGLEVEL::loglevel > CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::loglevel > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::loglevel).get()


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

