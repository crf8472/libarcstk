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
#include <string>   // for string
#include <vector>   // for vector

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

namespace arid
{

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


namespace details
{

/**
 * \brief Normalize total number of tracks to a legal unsigned value.
 *
 * \param[in] total_tracks Total number of tracks
 *
 * \return Total number of tracks
 */
unsigned normalize_total_tracks(const std::size_t total_tracks) noexcept;

} // namespace details


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

