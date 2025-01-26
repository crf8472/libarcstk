#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#error "Do not include identifier_details.hpp, include identifier.hpp instead"
#endif

#ifndef __LIBARCSTK_IDENTIFIER_DETAILS_HPP__
#define __LIBARCSTK_IDENTIFIER_DETAILS_HPP__
/**
 * \file
 *
 * \brief Implementation details for identifier.hpp.
 */

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif

#include <cstdint>  // for uint32_t, int32_t
#include <memory>   // for unique_ptr
#include <vector>   // for vector

namespace arcstk
{
inline namespace v_1_0_0
{

// avoid includes
class AudioSize;

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
 * \internal
 * \ingroup id
 *
 * \brief Constructs \link arcstk::v_1_0_0::ARId ARIds \endlink from
 * \link arcstk::v_1_0_0::ToC ToC \endlink data.
 *
 * Constructs ARIds either from a ToC or from the triplet of track count, list
 * of offsets and leadout frame.
 *
 * ARIdBuilder validates its input and will refuse to construct invalid ARIds
 * from any data. Hence, if an ARId is returned, it is guaranteed to be correct.
 *
 * This class is considered an implementation detail.
 */
class ARIdBuilder final
{
public:

	/**
	 * \brief Create an ARId from the specified ToC and leadout.
	 *
	 * Actual parameters \c toc and \c leadout are validated against each other.
	 *
	 * \param[in] toc     ToC to build ARId from
	 * \param[in] leadout Leadout frame
	 *
	 * \return An ARId representing the specified information
	 *
	 * \throw InvalidMetadataException If the ToC forms no valid ARId
	 */
	static std::unique_ptr<ARId> build(const ToC &toc, const AudioSize size);

	/**
	 * \brief Create an ARId from the specified ToC.
	 *
	 * \param[in] toc ToC to build ARId from
	 *
	 * \return An ARId representing the specified information
	 *
	 * \throw InvalidMetadataException If the ToC forms no valid ARId
	 */
	static std::unique_ptr<ARId> build(const ToC &toc);

	/**
	 * \brief Safely construct an empty ARId.
	 *
	 * An empty ARId has the invalid value 0 for the track count and also 0
	 * for disc id 1, disc id 2 and cddb id. An empty ARId is not a valid
	 * description of a CDDA medium.
	 *
	 * Building an empty ARId also provides the possibility to just provide an
	 * ARId on sites where an ARId is required without having to test for null.
	 *
	 * It may help provide an uniforming implementation of cases where
	 * an ARId in fact is expected but cannot be provided due to missing
	 * data, e.g. when processing single tracks without knowing the offset.
	 *
	 * \return An empty ARId
	 */
	static std::unique_ptr<ARId> build_empty_id() noexcept;


private:

	/**
	 * \brief Perform the actual build process.
	 *
	 * \param[in] toc         The ToC to use
	 * \param[in] leadout     Leadout frame
	 *
	 * \return An ARId representing the specified information
	 *
	 * \throw InvalidMetadataException If the parameters form no valid ARId
	 */
	static std::unique_ptr<ARId> build_worker(const ToC &toc,
			const AudioSize leadout);
};

// TODO Implement this as a universal template like toc_get()
std::vector<int32_t> to_frames(const std::vector<AudioSize>& offsets);

} //namespace details

} // namespace v_1_0_0
} // namespace arcstk
#endif

