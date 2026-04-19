#ifndef LIBARCSTK_LOGGING_HPP_
#define LIBARCSTK_LOGGING_HPP_

/**
 * \file
 *
 * \brief Public \link logging Logging API for clients\endlink.
 *
 * \details
 *
 * Provides a thread-safe, type-safe and portable logging interface in form
 * of the logging macros along with a global Logging instance.
 *
 * The code in this file is based on ideas by Petru Marginean published in:
 * Marginean, P: "Logging in C++: A typesafe, threadsafe, portable logging
 * mechanism", 10/2007, http://www.ddj.com/cpp/201804215
 * and
 * Marginean, P: "Logging in C++: Part 2", 11/20/2009, http://www.ddj.com/cpp/221900468
 */


#ifndef LIBARCSTK_LOGLEVEL_HPP_
#include "loglevel.hpp"
#endif

#include <atomic>         // for atomic
#include <chrono>         // for milliseconds, seconds, duration_cast, opera...
#include <cstdio>         // for fclose, fflush, FILE, fopen, fprintf
#include <ctime>          // for localtime, time_t
#include <iomanip>        // for operator<<, put_time
#include <memory>         // for unique_ptr, hash, operator==
#include <mutex>          // for mutex, lock_guard
#include <sstream>        // for operator<<, basic_ostream, ostringstream
#include <stdexcept>      // for runtime_error
#include <string>         // for string, operator==, char_traits, operator<<
#include <type_traits>    // for __underlying_type_impl<>::type, underlying_...
#include <unordered_set>  // for unordered_set
#include <utility>        // for move

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

/**
 * \defgroup logging Logging API
 *
 * \brief Log operations
 *
 * @{
 */

/**
 * \brief Returns the current time in the format
 * <tt>'YYYY-MM-DD hh:mm:ss.lll'</tt>.
 *
 * Returns the current time as a string containing year, month, day, hours,
 * minutes, seconds and milliseconds in the format 'YYYY-MM-DD hh:mm:ss.lll'.
 *
 * \return The current time as a string
 */
std::string now_time();

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
class Appender final
{
	/**
	 * \brief Internal name of the Appender
	 */
	std::string name_;

	/**
	 * \brief Internal stream to append to
	 */
	FILE* stream_;

public:

	/**
	 * \brief Constructs an Appender for appending to the given <tt>FILE</tt>.
	 *
	 * The \c stream may also be <tt>stdout</tt>, <tt>stderr</tt> etc.
	 *
	 * \param[in] name   Name of the Appender
	 * \param[in] stream The <tt>FILE</tt> to append to
	 */
	Appender(const std::string& name, FILE* stream)
		: name_   { name }
		, stream_ { stream }
	{
		if (!stream)
		{
			auto ss = std::ostringstream{};
			ss << "Appender " << name_.c_str() << " has no stream to append to";

			throw std::runtime_error(ss.str());
		}
	}

	/**
	 * \brief Constructs an Appender for appending to the given file.
	 *
	 * The \c filename becomes the name of the Appender.
	 *
	 * \param[in] filename File to append to and name of the Appender
	 */
	explicit Appender(const std::string& filename)
		: Appender { filename, std::fopen(filename.c_str(), "a") }
	{
		// empty
	}

	/**
	 * \copydoc SNPT_sm_non_copyable
	 */
	Appender(const Appender&) = delete;

	/**
	 * \copydoc SNPT_sm_non_copyable
	 */
	Appender& operator = (const Appender&) = delete;

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	Appender(Appender&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	Appender& operator = (Appender&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~Appender() noexcept
	{
		if (stream_)
		{
			// NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
			std::fclose(stream_);
		}
	}

	/**
	 * \brief Append the specified message
	 *
	 * \param[in] msg The message to append
	 */
	void append(const std::string& msg) const
	{
		if (!stream_)
		{
			return;
		}

		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		std::fprintf(stream_, "%s", msg.c_str());
		std::fflush(stream_);
		// Note: According to
		// http://www.gnu.org/software/libc/manual/html_node/Streams-and-Threads.html
		// all stream operations are thread safe, ergo using fprintf buys us
		// thread-safety in principle. This at least ensures that no lines are
		// scrambled.
	}

	/**
	 * \brief Name of the Appender
	 *
	 * \return Name of the Appender
	 */
	std::string name() const noexcept
	{
		return name_;
	}
};


/**
 * \brief Logs a message to its registered \link Appender Appenders \endlink.
 *
 * A Logger associates a configuration with a set of
 * \link Appender Appenders \endlink. Can be configured to log timestamps. The
 * default is <tt>true</tt>.
 */
class Logger final
{
	/**
	 * \brief Internal set of \link Appender Appenders \endlink
	 */
	std::unordered_set<std::unique_ptr<Appender>> appenders_;

	/**
	 * \brief Flag to activate/deactivate the logging of timestamps
	 */
	bool log_timestamps_;

public:

	/**
	 * \brief Constructor
	 */
	Logger()
		: appenders_      { /* empty */ }
		, log_timestamps_ { true }
	{
		// empty
	}

	/**
	 * \copydoc SNPT_sm_non_copyable
	 */
	Logger(const Logger&) = delete;

	/**
	 * \copydoc SNPT_sm_non_copyable
	 */
	Logger& operator = (const Logger&) = delete;

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	Logger(Logger&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	Logger& operator = (Logger&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~Logger() noexcept = default;

	/**
	 * \brief Activates or deactivates the output of timestamps.
	 *
	 * \param[in] on_or_off TRUE activates the logging of timestamps
	 */
	void set_timestamps(const bool& on_or_off) noexcept
	{
		log_timestamps_ = on_or_off;
	}

	/**
	 * \brief Returns TRUE iff this instance is configured to log timestamps.
	 *
	 * \return TRUE iff this instance will log timestamps.
	 */
	bool has_timestamps() const noexcept
	{
		return log_timestamps_;
	}

	/**
	 * \brief Add an Appender to this Logger
	 *
	 * \param[in] appender An Appender to use
	 */
	void add_appender(std::unique_ptr<Appender> appender)
	{
		appenders_.emplace(std::move(appender));
	}

	/**
	 * \brief Remove the given Appender from this Logger.
	 *
	 * \param[in] appender An Appender to remove
	 */
	void remove_appender(const Appender *appender)
	{
		using std::begin;
		using std::cend;

		auto it = begin(appenders_);

		while (it != cend(appenders_))
		{
			if (it->get() == appender)
			{
				it = appenders_.erase(it);
			} else
			{
				++it;
			}
		}
	}

	/**
	 * \brief Log the given message to all \link Appender Appenders \endlink.
	 *
	 * \param[in] msg The message to log
	 */
	void log(const std::string& msg) const
	{
		for (auto& appender : appenders_)
		{
			appender->append(msg);
		}
	}
};


/**
 * \brief A single logging operation of a Logger using a specified LOGLEVEL.
 *
 * A (more or less) thread-safe, type-safe, portable logging interface for
 * concrete \link Logger Loggers \endlink.
 */
class Log final
{
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

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] logger    Logger to use
	 * \param[in] msg_level Loglevel of the message to log
	 */
	Log(const Logger& logger, LOGLEVEL msg_level)
		: os_        {}
		, logger_    { &logger   }
		, msg_level_ { msg_level }
	{
		// empty
	}

	/**
	 * \copydoc SNPT_sm_non_copyable
	 */
	Log(const Log&) = delete;

	/**
	 * \copydoc SNPT_sm_non_copyable
	 */
	Log& operator = (const Log&) = delete;

	/**
	 * \copydoc SNPT_sm_non_moveable
	 */
	Log(Log&&) noexcept = delete;

	/**
	 * \copydoc SNPT_sm_non_moveable
	 */
	Log& operator = (Log&&) noexcept = delete;

	/**
	 * \brief Destructor
	 *
	 * Add newline, immediately flush stream and log the resulting string.
	 */
	~Log() noexcept
	{
		// NOLINTNEXTLINE(performance-avoid-endl)
		os_ << std::endl; // We intend to flush here, endl is ok

		if (logger_)
		{
			logger_->log(os_.str());
		}
	}

	/**
	 * \brief Produce indent according to message level.
	 *
	 * \return Indent string (whitespace)
	 */
	std::string indent() const
	{
		using loglevel_type = typename std::underlying_type<LOGLEVEL>::type;

		static constexpr auto step            = int { 2 };
		static constexpr auto threshold_level = LOGLEVEL::DEBUG;
		static constexpr auto whitespace      = char { ' ' };

		// Indent messages with level 'threshold_level' and higher

		// Produces 2, 4, 6, 8 for levels 5, 6, 7, 8
		return std::string(
			static_cast<std::string::size_type>(
				msg_level_ > threshold_level
				? step * (static_cast<loglevel_type>(msg_level_)
						- static_cast<loglevel_type>(threshold_level)
					)
				: 0),
			whitespace
		);
	}

	/**
	 * \brief Get the output stream to write to.
	 *
	 * \return Get the output stream to write to
	 */
	std::ostringstream& get()
	{
		using std::to_string;


		if (logger_->has_timestamps())
		{
			os_ << "- " << now_time() << ' ';
		}

		os_ << to_string(msg_level_) << ": " << indent();

		return os_;
	}
};


/**
 * \brief A singleton interface used by all Log instances.
 *
 * A singleton manager and thread safe interface class for all
 * \link Log Logs \endlink of the entire component.
 */
class Logging final
{
	/**
	 * \brief Mutex for thread-safe access to internal Logger instance.
	 */
	std::mutex mutex_;

	/**
	 * \brief Internal log level.
	 */
	std::atomic<LOGLEVEL> level_;

	/**
	 * \brief Class is singleton.
	 */
	Logging()
		: mutex_ { /* default */ }
		, level_ { LOGLEVEL::WARNING }
	{
		// empty
	}

	/**
	 * \brief Non-const access to the internal Logger.
	 *
	 * \return The internal logger object
	 */
	Logger& on_logger_do()
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		return const_cast<Logger&>(logger());
	}

public:

	/**
	 * \copydoc SNPT_sm_non_copyable
	 */
	Logging(const Logging&) = delete;

	/**
	 * \copydoc SNPT_sm_non_copyable
	 */
	Logging& operator = (Logging& rhs) = delete;

	/**
	 * \copydoc SNPT_sm_non_moveable
	 */
	Logging(Logging&&) noexcept = delete;

	/**
	 * \copydoc SNPT_sm_non_moveable
	 */
	Logging& operator = (Logging&& rhs) noexcept = delete;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~Logging() noexcept = default;

	/**
	 * \brief Handle for the singleton.
	 *
	 * \return This instance
	 */
	static Logging& instance()
	{
		static Logging logging{};

		return logging;
	}

	/**
	 * \brief Access the internal Logger.
	 *
	 * \return The internal logger object
	 */
	const Logger& logger() const
	{
		static Logger logger{};

		return logger;
	}

	/**
	 * \brief Returns the current log level.
	 *
	 * \return Current log level
	 */
	LOGLEVEL level() const
	{
		return level_;
	}

	/**
	 * \brief Set the log level.
	 *
	 * \param[in] level Set the log level
	 */
	void set_level(LOGLEVEL level)
	{
		const auto lock = std::lock_guard { mutex_ };
		level_ = level;
	}

	/**
	 * \brief Return TRUE iff the global log level is greater or equal than
	 * \c level.
	 *
	 * \param[in] level The level to check for
	 *
	 * \return TRUE iff Logger has at least the level passed
	 */
	bool has_level(LOGLEVEL level) noexcept
	{
		return level_ >= level;
	}

	/**
	 * \brief Activates or deactivates the output of timestamps.
	 *
	 * \param[in] on_or_off TRUE activates logging of timestamps
	 */
	void set_timestamps(const bool& on_or_off)
	{
		const auto lock = std::lock_guard { mutex_ };
		on_logger_do().set_timestamps(on_or_off);
	}

	/**
	 * \brief Returns TRUE iff output of timestamps is activated, otherwise
	 * FALSE.
	 *
	 * \return TRUE iff timestamps are logged, otherwise FALSE.
	 */
	bool has_timestamps() const noexcept
	{
		return logger().has_timestamps();
	}

	/**
	 * \brief Add an appender to the internal Logger.
	 *
	 * \param[in] appender The Appender to add
	 */
	void add_appender(std::unique_ptr<Appender> appender)
	{
		const auto lock = std::lock_guard { mutex_ };
		on_logger_do().add_appender(std::move(appender));
	}

	/**
	 * \brief Remove given appender from the internal Logger.
	 *
	 * \param[in] appender The Appender to remove
	 */
	void remove_appender(Appender *appender)
	{
		const auto lock = std::lock_guard { mutex_ };
		on_logger_do().remove_appender(appender);
	}
};

/** @} */


inline std::string now_time()
{
	const auto now { std::chrono::system_clock::now() };
	auto ss = std::ostringstream {};

	// Print year, month, day, hour, minute, second

	{
		const auto now_time = std::time_t {
			std::chrono::system_clock::to_time_t(now) };
		ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %X");
	}

	// Print milliseconds

	{
		const auto secs {
			std::chrono::time_point_cast<std::chrono::seconds>(now) };
		const auto m { now - secs };
		const auto millis {
			std::chrono::duration_cast<std::chrono::milliseconds>(m) };
		ss << "." << millis.count();
	}

	return ss.str();
}

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk


// Macros


/// \addtogroup logging
/// @{

//NOLINTBEGIN(cppcoreguidelines-macro-usage)

// CLIP_LOGGING_LEVEL is commented but not yet removed
///**
// * \brief Clipping for the log level.
// *
// * Every message whose level is GREATER than CLIP_LOGGING_LEVEL is eliminated
// * entirely by the compiler. This enables zero-overhead debug logging in
// * production builds.
// *
// * Example:
// *   - Development: CLIP_LOGGING_LEVEL = DEBUG4 -> All statements remain
// *   - Production:  CLIP_LOGGING_LEVEL = INFO   -> All DEBUG* statements removed
// *
// * This is a compile-time constant, so the optimizer recognizes the comparison
// * and eliminates code via dead-code elimination.
// */
//#ifndef CLIP_LOGGING_LEVEL
//#    define CLIP_LOGGING_LEVEL arcstk::LOGLEVEL::DEBUG4
//#endif

/**
 * \brief Send error message to log.
 */
#define ARCS_LOG_ERROR \
    if (arcstk::LOGLEVEL::ERROR > arcstk::CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::ERROR > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::ERROR).get()

/**
 * \brief Send warning message to log.
 */
#define ARCS_LOG_WARNING \
    if (arcstk::LOGLEVEL::WARNING > arcstk::CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::WARNING > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::WARNING).get()

/**
 * \brief Send info message to log.
 */
#define ARCS_LOG_INFO \
    if (arcstk::LOGLEVEL::INFO > arcstk::CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::INFO > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::INFO).get()

/**
 * \brief Send debug message to log.
 */
#define ARCS_LOG_DEBUG \
    if (arcstk::LOGLEVEL::DEBUG > arcstk::CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::DEBUG > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::DEBUG).get()

/**
 * \brief Send log message with specified log level to log.
 *
 * The loglevel must be specified without the prefix <tt>LOGLEVEL::</tt>.
 *
 * This is useful for custom log levels beyond DEBUG
 *
 * \param[in] loglevel The LOGLEVEL to use
 */
#define ARCS_LOG(loglevel) \
    if (arcstk::LOGLEVEL::loglevel > arcstk::CLIP_LOGGING_LEVEL) ; \
    else if (arcstk::LOGLEVEL::loglevel > arcstk::Logging::instance().level()) ; \
    else arcstk::Log(arcstk::Logging::instance().logger(), arcstk::LOGLEVEL::loglevel).get()

//NOLINTEND(cppcoreguidelines-macro-usage)

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

/** @} */

#endif

