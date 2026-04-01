#ifndef LIBARCSTK_IDENTIFIER_HPP_
#error "Do not include identifier_details.hpp, include identifier.hpp instead"
#endif

#ifndef LIBARCSTK_IDENTIFIER_DETAILS_HPP_
#define LIBARCSTK_IDENTIFIER_DETAILS_HPP_

/**
 * \internal
 *
 * \file
 *
 * \brief Implementation details for identifier.hpp.
 */

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"
#endif

#include <cstdint>  // for uint32_t, int32_t
#include <memory>   // for unique_ptr
#include <string>   // for string
#include <vector>   // for vector

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

namespace arid // TODO These are not details but tools
{

/**
 * \brief Service method: Compute the disc id 1 from offsets and leadout.
 *
 * \param[in] offsets Offsets (in LBA frames) of each track
 * \param[in] leadout Leadout LBA frame
 */
uint32_t disc_id_1(const std::vector<int32_t>& offsets, const int32_t leadout)
	noexcept;

/**
 * \brief Service method: Compute the disc id 2 from offsets and leadout.
 *
 * \param[in] offsets Offsets (in LBA frames) of each track
 * \param[in] leadout Leadout LBA frame
 */
uint32_t disc_id_2(const std::vector<int32_t>& offsets, const int32_t leadout)
	noexcept;

/**
 * \brief Service method: Compute the CDDB id from offsets and leadout.
 *
 * The CDDB id is a 32bit unsigned integer, formed of a concatenation of
 * the following 3 numbers:
 * first chunk (8 bits):   checksum (sum of digit sums of offset secs + 2)
 * second chunk (16 bits): total seconds count
 * third chunk (8 bits):   total number of tracks
 *
 * \param[in] offsets     Offsets (in LBA frames) of each track
 * \param[in] leadout     Leadout LBA frame
 */
uint32_t cddb_id(const std::vector<int32_t>& offsets, const int32_t leadout);

/**
 * \brief Service method: sum up the digits of the number passed
 *
 * \param[in] number An unsigned integer number
 *
 * \return The sum of the digits of the number
 */
uint64_t sum_digits(const uint32_t number) noexcept;

/**
 * \brief Normalize total number of tracks to a legal unsigned value.
 *
 * \param[in] total_tracks Total number of tracks
 *
 * \return Total number of tracks
 */
unsigned normalize_total_tracks(const std::size_t total_tracks) noexcept;

/**
 * \brief Service method: Compute the AccurateRip response filename
 *
 * Used by ARId::Impl::filename().
 *
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 *
 * \return AccurateRip response filename
 */
std::string construct_filename(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept;

/**
 * \brief Service method: Compute the AccurateRip request URL
 *
 * The URL is constructed using current_request_url_prefix().
 *
 * Used by ARId::Impl::url().
 *
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 *
 * \return AccurateRip request URL
 */
std::string construct_url(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept;

/**
 * \brief Service method: Compute the AccurateRip request URL
 *
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 * \param[in] prefix        URL prefix
 *
 * \return AccurateRip request URL
 */
std::string construct_url(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id,
		const std::string& prefix) noexcept;

/**
 * \brief Service method: Compute the AccurateRip request ID
 *
 * Used by ARId::Impl::to_string().
 *
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 *
 * \return AccurateRip request URL
 */
std::string construct_id(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept;

/**
 * \brief Worker: Print a sequence of ids.
 *
 * \param[in] out           Stream to print to
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 * \param[in] delim         Delimiter
 */
void print(std::ostream& out, const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id, const std::string& delim);

/**
 * \brief Worker: Print an ARId by its ids.
 *
 * \param[in] out           Stream to print to
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 */
void print(std::ostream& out, const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id);

/**
 * \brief Worker: Print an ARId by its ids.
 *
 * \param[in] out Stream to print to
 * \param[in] id  ARId to print
 */
void print(std::ostream& out, const ARId& id);

/**
 * \brief Worker: Create an ARId by offsets and leadout.
 *
 * The input is unvalidated.
 *
 * \param[in] offsets Offsets (in LBA frames)
 * \param[in] leadout Leadout (in LBA frames)
 *
 * \return ARId
 */
ARId make(const std::vector<int32_t>& offsets, const int32_t leadout);

} //namespace arid


/**
 * \internal
 *
 * \ingroup id
 *
 * \brief Private implementation of ARId
 *
 * \see ARId
 */
class ARId::Impl final
{
	/**
	 * \brief Number of tracks
	 */
	unsigned total_tracks_;

	/**
	 * \brief Disc id no. 1
	 */
	uint32_t disc_id1_;

	/**
	 * \brief Disc id no. 2
	 */
	uint32_t disc_id2_;

	/**
	 * \brief CDDB disc id
	 */
	uint32_t cddb_id_;

public:

	Impl(const unsigned total_tracks,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id) noexcept;

	std::string url() const noexcept;

	std::string filename() const noexcept;

	unsigned total_tracks() const noexcept;

	uint32_t disc_id_1() const noexcept;

	uint32_t disc_id_2() const noexcept;

	uint32_t cddb_id() const noexcept;

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

