#ifndef LIBARCSTK_METADATA_HPP_
#error "Do not include metadata_details.hpp, include metadata.hpp instead"
#endif

#ifndef LIBARCSTK_METADATA_DETAILS_HPP_
#define LIBARCSTK_METADATA_DETAILS_HPP_

/**
 * \internal
 *
 * \file
 *
 * \brief Implementation details for metadata.hpp.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"
#endif

#include <cstdint>        // for int32_t

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

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
 * \brief Worker: implement leadout checks performed on every validation.
 *
 * Implements the common part of checking with or without completeness.
 *
 * \param[in] frames LBA frame amount to validate
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_leadout_impl(const int32_t leadout);

/**
 * \internal
 *
 * \brief Worker: implement checks for offsets and filenames.
 *
 * \param[in] toc_data  ToC data to validate
 * \param[in] filenames Audio filenames
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_filenames_impl(const ToCData& toc_data,
		const std::vector<std::string>& filenames);

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
 * \throws InvalidMetadataException If validation fails
 */
void is_legal_offset(const int32_t offset);

/**
 * \internal
 *
 * \brief Worker to validate track length in frames for being of legal size.
 *
 * \param[in] length Track length in LBA frames to validate
 *
 * \throws InvalidMetadataException If validation fails
 */
void is_legal_length(const int32_t length);

/**
 * \internal
 *
 * \brief Validate leadout.
 *
 * A leadout of 0 validates.
 *
 * \param[in] toc_data ToCData to validate
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_leadout(const ToCData& toc_data);

/**
 * \internal
 *
 * \brief Validate a leadout.
 *
 * A leadout of 0 fails to validate.
 *
 * \param[in] toc_data ToCData to validate
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_nonzero_leadout(const ToCData& toc_data);

/**
 * \internal
 *
 * \brief Validate all offsets.
 *
 * \param[in] toc_data ToCData to validate
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_offsets(const ToCData& toc_data);

/**
 * \internal
 *
 * \brief Validate all lengths.
 *
 * \param[in] toc_data ToCData to validate
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_lengths(const ToCData& toc_data);

/**
 * \internal
 *
 * \brief Worker to call when ToCData validation fails.
 *
 * Throws an InvalidMetadataException.
 *
 * \param[in] msg Error message
 *
 * \throws InvalidMetadataException On every call
 */
void on_invalid_tocdata(const std::string& msg);

/**
 * \internal
 *
 * \brief Worker to call when ToCData validation finds nonstandard data.
 *
 * Current implementation does nothing.
 *
 * \param[in] msg Message
 */
void on_nonstandard_tocdata(const std::string& msg);

} // namespace validate
} // namespace details


class ToC::Impl final
{
public:

	Impl(const ToCData& toc_data, const std::vector<std::string>& filenames);

	unsigned total_tracks() const noexcept;

	void set_leadout(const AudioSize& leadout) noexcept;
	AudioSize leadout() const noexcept;

	std::vector<AudioSize>   offsets() const;
	std::vector<std::string> filenames() const;

	bool has_filenames() const noexcept;
	bool is_single_file() const noexcept;

	void validate() const;

	bool complete() const noexcept;

	void print(std::ostream& out);

	bool empty() const noexcept;

	void swap(Impl& rhs) noexcept;

	bool equals(const Impl& rhs) const noexcept;

	std::string to_string() const;

private:

	ToCData toc_;
	std::vector<std::string> filenames_;
};

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

