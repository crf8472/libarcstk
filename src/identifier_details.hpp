#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#error "Do not include identifier_details.hpp, include identifier.hpp instead"
#endif

#ifndef __LIBARCSTK_IDENTIFIER_DETAILS_HPP__
#define __LIBARCSTK_IDENTIFIER_DETAILS_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Implementation details for identifier.hpp.
 */

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif

#include <cstdint>  // for uint32_t, int32_t
#include <memory>   // for unique_ptr
#include <string>   // for string
#include <vector>   // for vector

namespace arcstk
{
inline namespace v_1_0_0
{

namespace details
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
 * \brief Service method: Compute the AccurateRip response filename
 *
 * Used by ARId::Impl::filename().
 *
 * \param[in] track_count   Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 *
 * \return AccurateRip response filename
 */
std::string construct_filename(const int track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept;

/**
 * \brief Service method: Compute the AccurateRip request URL
 *
 * Used by ARId::Impl::url().
 *
 * \param[in] track_count   Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 *
 * \return AccurateRip request URL
 */
std::string construct_url(const int track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept;

/**
 * \brief Service method: Compute the AccurateRip request ID
 *
 * Used by ARId::Impl::url().
 *
 * \param[in] track_count   Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 *
 * \return AccurateRip request URL
 */
std::string construct_id(const int track_count,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept;

/**
 * \brief Create an ARId from the toc data.
 *
 * \param[in] offsets Offsets (in LBA frames)
 * \param[in] leadout Leadout (in LBA frames)
 *
 * \return ARId
 */
std::unique_ptr<ARId> make_arid(const std::vector<int32_t>& offsets,
		const int32_t leadout);

} //namespace details


/**
 * \internal
 * \ingroup id
 *
 * \brief Private implementation of ARId
 *
 * \see ARId
 */
class ARId::Impl final
{

public:

	/**
	 * \brief URL prefix for accessing AccurateRip
	 */

	/**
	 * \brief Implements ARId::ARId().
	 */
	Impl(const int track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id);

	/**
	 * \brief Implements ARId::url()
	 */
	std::string url() const noexcept;

	/**
	 * \brief Implements ARId::filename()
	 */
	std::string filename() const noexcept;

	/**
	 * \brief Implements ARId::track_count()
	 */
	int track_count() const noexcept;

	/**
	 * \brief Implements ARId::disc_id_1()
	 */
	uint32_t disc_id_1() const noexcept;

	/**
	 * \brief Implements ARId::disc_id_2()
	 */
	uint32_t disc_id_2() const noexcept;

	/**
	 * \brief Implements ARId::cddb_id()
	 */
	uint32_t cddb_id() const noexcept;

	/**
	 * \brief Implements ARId::empty()
	 */
	bool empty() const noexcept;

	void swap(Impl& rhs) noexcept;

	bool equals(const Impl& rhs) const noexcept;

	std::string to_string() const noexcept;

private:

	/**
	 * \brief Number of tracks
	 */
	int track_count_;

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
};

} // namespace v_1_0_0
} // namespace arcstk
#endif

