/**
 * \file logging.cpp Implementing a thread-safe, type-safe and portable logging interface
 */


#ifndef __LIBARCS_LOGGING_HPP__
#include "logging.hpp"
#endif

#include <chrono>
#include <ctime>     // for localtime
#include <iomanip>   // for put_time
#include <memory>
#include <mutex>     // for lock_guard, mutex
#include <stdexcept> // for runtime_error

namespace arcs
{

inline namespace v_1_0_0
{

// Log


Appender::Appender(const std::string &filename)
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


Appender::Appender(const std::string &name, FILE* stream)
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


Appender::Appender(Appender&& appender) noexcept = default;


Appender::~Appender() noexcept
{
	if (stream_)
	{
		std::fclose(stream_);
	}
}


void Appender::append(const std::string& msg) const
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


std::string Appender::name() const
{
	return name_;
}


Appender& Appender::operator = (Appender&& appender) noexcept = default;


// Logger


Logger::Logger()
	: appenders_()
	, level_(LOG_WARNING)
	, log_timestamps_(true)
{
	// empty
}


Logger::Logger(Logger&& logger) noexcept = default;


Logger::~Logger() noexcept = default;


LOGLEVEL_t Logger::level() const
{
	return level_;
}


void Logger::set_level(LOGLEVEL_t level)
{
	level_ = level;
}


bool Logger::has_level(LOGLEVEL_t level)
{
	return level_ >= level;
}


void Logger::set_timestamps(const bool &on_or_off)
{
	log_timestamps_ = on_or_off;
}


bool Logger::has_timestamps()
{
	return log_timestamps_;
}


void Logger::add_appender(std::unique_ptr<Appender> appender)
{
	appenders_.emplace(std::move(appender));
}


void Logger::remove_appender(Appender *a)
{
	for (auto& app : appenders_)
	{
		if (app.get() == a)
		{
			appenders_.erase(app);
		}
	}
}


void Logger::log(const std::string &msg) const
{
	// NOTE We could require a log level and check this, but for
	// performance reasons this is not done here. This entails that the
	// Logger does not enforce logging to its log level, its a mere preference.
	// Obeying the actual log level is enforced by the Log instance.

	for (auto& appender : appenders_)
	{
		appender->append(msg);
	}
}


Logger& Logger::operator = (Logger&& logger) noexcept = default;


// Log


Log::Log(Logger *logger, LOGLEVEL_t msg_level)
	: os_()
	, logger_(logger)
	, msg_level_(msg_level)
{
	// empty
}


Log::~Log() noexcept
{
	os_ << std::endl;

	if (logger_)
	{
		logger_->log(os_.str());
	}
}


std::ostringstream& Log::get()
{
	// Timestamp

	if (logger_->has_timestamps())
	{
		os_ << "- " << now_time() << " ";
	}

	// Loglevel string

	os_ << Log::to_string(msg_level_) << ": ";

	// Indent messages with level LOG_DEBUG1 and higher

	os_ << std::string(
		msg_level_ > LOG_DEBUG ? 2 * (msg_level_ - LOG_DEBUG) : 0, ' ');

	return os_;
}


std::string Log::to_string(LOGLEVEL_t level)
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

	return buffer[level];
}


LOGLEVEL_t Log::from_string(const std::string& level)
{
	if (level == "NONE")
		{ return LOG_NONE; }

	if (level == "ERROR")
		{ return LOG_ERROR; }

	if (level == "WARNING")
		{ return LOG_WARNING; }

	if (level == "INFO")
		{ return LOG_INFO; }

	if (level == "DEBUG")
		{ return LOG_DEBUG; }

	if (level == "DEBUG1")
		{ return LOG_DEBUG1; }

	if (level == "DEBUG2")
		{ return LOG_DEBUG2; }

	if (level == "DEBUG3")
		{ return LOG_DEBUG3; }

	if (level == "DEBUG4")
		{ return LOG_DEBUG4; }

	return LOG_NONE;
}


// now_time()


std::string now_time()
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


// Logging


thread_local Logger Logging::logger_;


Logging::Logging()
	: mutex_()
{
	// empty
}


Logging::~Logging() noexcept = default;


Logger* Logging::logger()
{
	return &logger_;
}


Logging& Logging::instance()
{
	// This should not introduce any memory leaks and is thread-safe when
	// compiled with a C++11 conforming compiler

	static Logging logging;

	return logging;
}


LOGLEVEL_t Logging::level() const
{
	return logger_.level();
}


void Logging::set_level(LOGLEVEL_t level)
{
	std::lock_guard<std::mutex> lock(mutex_);
	logger_.set_level(level);
}


bool Logging::has_level(LOGLEVEL_t level)
{
	return logger_.has_level(level);
}


void Logging::set_timestamps(const bool &on_or_off)
{
	std::lock_guard<std::mutex> lock(mutex_);
	logger_.set_timestamps(on_or_off);
}


bool Logging::has_timestamps()
{
	return logger_.has_timestamps();
}


void Logging::add_appender(std::unique_ptr<Appender> appender)
{
	std::lock_guard<std::mutex> lock(mutex_);
	logger_.add_appender(std::move(appender));
}


void Logging::remove_appender(Appender *a)
{
	std::lock_guard<std::mutex> lock(mutex_);
	logger_.remove_appender(a);
}

} // namespace v_1_0_0

} // namespace arcs

