#ifndef __LIBARCSTK_METADATA_HPP__
#error "Do not include metadata_details.hpp, include metadata.hpp instead"
#endif

#ifndef __LIBARCSTK_METADATA_DETAILS_HPP__
#define __LIBARCSTK_METADATA_DETAILS_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Implementation details for metadata.hpp.
 */

#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"
#endif

#include <cstdint>        // for int32_t

namespace arcstk
{
inline namespace v_1_0_0
{

namespace details
{

/**
 * \internal
 *
 * \brief Convert \c value to the corrsponding number of bytes.
 *
 * \param[in] value Value to convert
 * \param[in] unit  Unit of the value
 *
 * \return The equivalent number of bytes.
 */
int32_t convert_to_bytes(const int32_t value, const UNIT unit) noexcept;

/**
 * \internal
 *
 * \brief Validations for ToCData.
 */
namespace validate
{

/**
 * \internal
 *
 * \brief Maximal valid offset value for a non-redbook 90 min CD (in LBA
 * frames).
 *
 * Non-redbook 90-min CD has 89:59.74 which is equivalent to 405.000 frames.
 */
static constexpr int32_t MAX_OFFSET_90 { (89 * 60 + 59) * 75 + 74 };

/**
 * \internal
 *
 * \brief Maximal valid offset value for a non-redbook 99 min CD (in LBA
 * frames).
 *
 * Non-redbook 99-min CD has 98:59.74 which is equivalent to 445.500 frames.
 */
static constexpr int32_t MAX_OFFSET_99 { (98 * 60 + 59) * 75 + 74 };

/**
 * \internal
 *
 * \brief Worker to validate LBA frame offset for being in legal range.
 *
 * \param[in] frames LBA frame amount to validate
 *
 * \throws std::invalid_argument
 */
void is_legal_offset(const int32_t offset);

/**
 * \internal
 *
 * \brief Worker to validate track length in frames for being of legal size.
 *
 * \param[in] length Track length in LBA frames to validate
 *
 * \throws std::invalid_argument
 */
void is_legal_length(const int32_t length);

/**
 * \internal
 *
 * \brief Validate leadout.
 *
 * \param[in] toc_data ToCData to validate
 */
void validate_leadout(const ToCData& toc_data);

/**
 * \internal
 *
 * \brief Validate all offsets.
 *
 * \param[in] toc_data ToCData to validate
 */
void validate_offsets(const ToCData& toc_data);

/**
 * \internal
 *
 * \brief Validate all lengths.
 *
 * \param[in] toc_data ToCData to validate
 */
void validate_lengths(const ToCData& toc_data);

/**
 * \internal
 *
 * \brief Worker to throw when ToCData validation fails.
 *
 * \param[in] msg Error message
 *
 * \throws std::invalid_argument
 */
void throw_on_invalid_tocdata(const std::string& msg);

} // namespace validate
} // namespace details


class ToC::Impl final
{
public:

	Impl(const ToCData& toc_data, const std::vector<std::string>& filenames);

	Impl(const Impl& rhs);
	Impl& operator = (const Impl& rhs);

	Impl(Impl&& rhs) noexcept;
	Impl& operator = (Impl&& rhs) noexcept;

	~Impl() noexcept;

	int total_tracks() const noexcept;

	void set_leadout(const AudioSize leadout) noexcept;
	AudioSize leadout() const noexcept;

	std::vector<AudioSize>   offsets() const;
	std::vector<std::string> filenames() const;

	bool has_filenames() const noexcept;
	bool is_single_file() const noexcept;

	bool complete() const noexcept;

	friend void swap(Impl& lhs, Impl& rhs) noexcept
	{
		using std::swap;

		swap(lhs.toc_,       rhs.toc_);
		swap(lhs.filenames_, rhs.filenames_);
	}

	friend bool operator == (const Impl& lhs, const Impl& rhs) noexcept
	{
		return lhs.toc_ == rhs.toc_ && lhs.filenames_ == rhs.filenames_;
	}

private:

	ToCData toc_;
	std::vector<std::string> filenames_;
};

} // namespace v_1_0_0
} // namespace arcstk

#endif

