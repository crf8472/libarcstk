#ifndef LIBARCSTK_TEMPFILE_HPP_
#define LIBARCSTK_TEMPFILE_HPP_

/**
 * \file
 *
 * \brief Test helpers for unit tests: temporary file.
 */

#include <ctime>                // for time
#include <filesystem>           // for temp_directory_path, exists, remove
#include <string>               // for string


namespace arcstk
{
namespace testing
{

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

} // namespace testing
} // namespace arcstk

#endif

