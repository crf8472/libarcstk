#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Appender.
 */

#include <ctime>                // for time
#include <filesystem>           // for temp_directory_path, exists, remove
#include <string>               // for to_string

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"     // for Copy, Move
#endif


/**
 * \brief A simple temporary file with a name hint.
 *
 * The file will be persisted under the system temp path.
 */
class TemporaryFile final
{
	/**
	 * \brief Internal file path.
	 */
	std::filesystem::path path_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] name_hint Hint for the filename
	 */
	explicit TemporaryFile(const std::string& name_hint)
	{
		const auto temp_dir = std::filesystem::temp_directory_path();

		path_ = temp_dir / (name_hint + "_" +
				std::to_string(std::time(nullptr)) + ".txt");
	}

	/**
	 * \brief Destructor.
	 */
	~TemporaryFile() noexcept
	{
		if (std::filesystem::exists(path_))
		{
			std::filesystem::remove(path_);
		}
	}

	/**
	 * \brief Actual file path.
	 *
	 * \return File path
	 */
	const std::filesystem::path& path() const
	{
		return path_;
	}

	/**
	 * \brief String representation of the file path.
	 *
	 * \return File path as a string
	 */
	std::string string() const
	{
		return path_.string();
	}

	// Non-copyable
	TemporaryFile(const TemporaryFile&)            = delete;
	TemporaryFile& operator=(const TemporaryFile&) = delete;

	// Moveable
	TemporaryFile(TemporaryFile&&)                 = default;
	TemporaryFile& operator=(TemporaryFile&&)      = default;
};


TEST_CASE ( "Appender", "[appender] [logging]" )
{
	//using arcstk::testing::Copy; // no copy possible
	using arcstk::testing::Move;

	using arcstk::Appender;

	const auto tempfile_prefix = std::string { "LIBARCSTK-TEST-UNIT-Appender" };

    TemporaryFile tempfile { tempfile_prefix };

	auto instance = Appender { tempfile.string() };


	SECTION ("Parametized construction is correct")
	{
		CHECK ( ! instance.name().empty() );
	}

	SECTION ("Move construction is correct")
	{
		const auto path = instance.name();

		// Test: is the name_hint part of the actual name?

		REQUIRE ( path.find(tempfile_prefix) != std::string::npos );

		const Appender moved { Move<Appender>::construct(instance) };

        CHECK ( moved.name().find(tempfile_prefix) != std::string::npos );
	}

	SECTION ("Move assignment is correct")
	{
		const auto path = instance.name();

		// Test: is the name_hint part of the actual name?

		REQUIRE ( path.find(tempfile_prefix) != std::string::npos );

		auto moved = Appender { "any_string_would_do_here" };
		Move<Appender>::assign(moved, instance);

        CHECK ( moved.name().find(tempfile_prefix) != std::string::npos );
	}
}


TEST_CASE ( "Appender property", "[appender] [logging]" )
{
	using arcstk::Appender;

	SECTION ("swap() is correct")
	{
		const auto file1_prefix = std::string {
			"LIBARCSTK-TEST-UNIT-Appender-52307562316"
		};

		const auto file2_prefix = std::string {
			"LIBARCSTK-TEST-UNIT-Appender-41296451205"
		};

		TemporaryFile tempfile1 { file1_prefix };
		TemporaryFile tempfile2 { file2_prefix };

		auto ap1 = Appender { tempfile1.string() };
		auto ap2 = Appender { tempfile2.string() };

		REQUIRE ( ap1.name() == tempfile1.string() );
		REQUIRE ( ap2.name() == tempfile2.string() );

		using std::swap;
		swap(ap1, ap2);

		CHECK ( ap1.name() == tempfile2.string() );
		CHECK ( ap2.name() == tempfile1.string() );
	}
}


TEST_CASE ( "Appender functions", "[appender] [logging]" )
{
	using arcstk::Appender;

	SECTION ("append() is correct")
	{
		const auto file_prefix = std::string {
			"LIBARCSTK-TEST-UNIT-Appender-63418673427"
		};

		TemporaryFile tempfile { file_prefix };

		auto apdr = Appender { tempfile.string() };

		CHECK_NOTHROW ( apdr.append("This does not cause an exception") );
	}

	SECTION ("name() is correct")
	{
		const auto file_prefix = std::string {
			"LIBARCSTK-TEST-UNIT-Appender-74529784538"
		};

		TemporaryFile tempfile { file_prefix };

		auto apdr = Appender { tempfile.string() };


		// Test: is the name_hint part of the actual name?

		CHECK ( apdr.name().find(file_prefix) != std::string::npos );
	}
}

