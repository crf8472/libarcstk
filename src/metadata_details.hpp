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
#include "metadata.hpp"   // for MetadataRequirement
#endif

#include <cstdint>        // for int32_t

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

namespace toc
{

/**
 * \internal
 *
 * \brief Implementation details of namespace toc.
 */
namespace details
{

/**
 * \brief Worker: Print ToCData to output stream.
 *
 * \param[in] out Stream to print to
 * \param[in] d   ToCData to print
 */
void print(std::ostream& out, const ToCData& d);

/**
 * \brief Validate all offsets.
 *
 * The leadout is allowed to be 0 but validated if it is non-zero.
 *
 * \param[in] toc_data ToCData to validate
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_offsets(const ToCData& toc_data);

/**
 * \brief Validate all offsets and non-zero leadout.
 *
 * The leadout is always validated and required to be non-zero.
 *
 * \param[in] toc_data ToCData to validate
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_offsets_and_leadout(const ToCData& toc_data);

/**
 * \brief Worker: implement checks for offsets and filenames.
 *
 * \param[in] toc_data  ToC data to validate
 * \param[in] filenames Audio filenames
 *
 * \throws InvalidMetadataException If validation fails
 */
void validate_filenames(const ToCData& toc_data,
		const std::vector<std::string>& filenames);

} // namespace details


/**
 * \internal
 *
 * \brief Helpers for ToCData requirements.
 */
namespace req
{

/**
 * \brief Obtain the name of a MetadataRequirement.
 *
 * \param[in] r Requirement to get name of
 *
 * \return Name of type \c r
 */
std::string name(const MetadataRequirement r);

/**
 * \internal
 *
 * \brief Worker to call when ToCData validation fails.
 *
 * Throws an InvalidMetadataException.
 *
 * \param[in] msg Error message
 * \param[in] r   Requirement violated
 * \param[in] v   Value that violated the requirement
 * \param[in] i   Index position that violated the requirement
 *
 * \throws InvalidMetadataException On every call
 */
void on_invalid_tocdata(const std::string& msg, const MetadataRequirement r,
		const int32_t v, const ToCData::size_type i);

/**
 * \internal
 *
 * \brief Worker to call when ToCData validation fails.
 *
 * Throws an InvalidMetadataException.
 *
 * \param[in] r   Requirement violated
 * \param[in] v   Value that violated the requirement
 * \param[in] i   Index position that violated the requirement
 *
 * \throws InvalidMetadataException On every call
 */
void on_invalid_tocdata(const MetadataRequirement r, const int32_t v,
		const ToCData::size_type i);

/**
 * \brief Default error message.
 *
 * \param[in] r Requirement violated
 * \param[in] v Value that violated the requirement
 * \param[in] i Index position that violated the requirement
 *
 * \return Default error message
 */
std::string default_error_message(const MetadataRequirement r,
		const int32_t v, const ToCData::size_type i);

} // namespace req
} // namespace toc


namespace details
{

/**
 * \brief Maximal valid offset value for a non-redbook 90 min CD (in LBA
 * frames).
 *
 * Non-redbook 90-min CD has 89:59.74 which is equivalent to 405.000 frames.
 */
static constexpr int32_t MAX_OFFSET_90 { (89 * 60 + 59) * 75 + 74 };

/**
 * \brief Maximal valid offset value for a non-redbook 99 min CD (in LBA
 * frames).
 *
 * Non-redbook 99-min CD has 98:59.74 which is equivalent to 445.500 frames.
 */
static constexpr int32_t MAX_OFFSET_99 { (98 * 60 + 59) * 75 + 74 };

/**
 * \brief Worker: Check LBA frame offset for exceeding some maximums.
 *
 * A return value of 0 indicates that no redbook standard was exceeded.
 * A return value greater than 0 will be the greatest max value that was
 * exceeded by \c offset. It will be one of the following values:
 * - CDDA::MAX_BLOCK_ADDRESS,
 * - MAX_OFFSET_99,
 * - MAX_OFFSET_90,
 * - CDDA::MAX_OFFSET,
 *
 * \param[in] frames LBA frame amount to check
 *
 * \return Return highest max value that was exceeded, 0 for no exceeding
 */
int32_t exceeds_maximum(const int32_t offset);

} // namespace details


class ToC::Impl final
{
	/**
	 * \internal
	 *
	 * \brief Internal ToCData.
	 */
	ToCData toc_;

	/**
	 * \internal
	 *
	 * \brief Internal filenames.
	 */
	std::vector<std::string> filenames_;

public:

	Impl();

	Impl(const ToCData& toc_data, const std::vector<std::string>& filenames);

	unsigned total_tracks() const noexcept;

	std::vector<AudioSize> offsets() const;

	AudioSize leadout() const noexcept;

	void set_offsets(const std::vector<AudioSize>& offsets);

	void set_leadout(const AudioSize& leadout);

	std::vector<std::string> filenames() const noexcept;

	void set_filenames(const std::vector<std::string>& filenames);

	bool has_filenames()  const noexcept;

	bool is_single_file() const noexcept;

	void validate() const;

	bool complete() const noexcept;

	void print(std::ostream& out); // implements ToC::operator <<

	bool empty() const noexcept;

	void swap(Impl& rhs) noexcept;

	bool equals(const Impl& rhs) const noexcept;

	std::string to_string() const;
};

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

