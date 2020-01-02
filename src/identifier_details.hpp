#include "identifier.hpp"

#ifndef __LIBARCSTK_IDENTIFIER_DETAILS_HPP__
#define __LIBARCSTK_IDENTIFIER_DETAILS_HPP__

#include <initializer_list>
#include <type_traits>

/**
 * \internal
 *
 * \file
 *
 * \brief Internal APIs for AccurateRip id calculation
 */

namespace arcstk
{
inline namespace v_1_0_0
{


/**
 * \internal
 * \ingroup id
 *
 * \brief Constructs ARId instances from TOC data.
 *
 * Constructs @link ARId ARIds @endlink either from a TOC or from the triplet of
 * track count, list of offsets and leadout frame.
 *
 * ARIdBuilder validates its input and will refuse to construct invalid ARIds
 * from any data. Hence, if an ARId is returned, it is guaranteed to be correct.
 */
class ARIdBuilder final
{

public:

	/**
	 * \brief Build an ARId object from the specified information.
	 *
	 * This method is intended for easy testing the class.
	 *
	 * \param[in] track_count Track count
	 * \param[in] offsets     Offsets
	 * \param[in] leadout     Leadout frame
	 *
	 * \return An ARId object representing the specified information
	 *
	 * \throw InvalidMetadataException If the parameters form no valid ARId
	 */
	std::unique_ptr<ARId> build(const TrackNo &track_count,
		const std::vector<int32_t> &offsets, const uint32_t leadout) const;

	/**
	 * \brief Build an ARId object from the specified TOC and leadout.
	 *
	 * Actual parameters \c toc and \c leadout are validated against each other.
	 *
	 * \param[in] toc     TOC to build ARId from
	 * \param[in] leadout Leadout frame
	 *
	 * \return An ARId object representing the specified information
	 *
	 * \throw InvalidMetadataException If the TOC forms no valid ARId
	 */
	std::unique_ptr<ARId> build(const TOC &toc, const uint32_t leadout) const;

	/**
	 * \brief Build an ARId object from the specified TOC.
	 *
	 * \param[in] toc TOC to build ARId from
	 *
	 * \return An ARId object representing the specified information
	 *
	 * \throw InvalidMetadataException If the TOC forms no valid ARId
	 */
	std::unique_ptr<ARId> build(const TOC &toc) const;

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
	std::unique_ptr<ARId> build_empty_id() const noexcept;


private:

	/**
	 * \brief Perform the actual build process.
	 *
	 * \param[in] track_count Track count
	 * \param[in] offsets     Offsets
	 * \param[in] leadout     Leadout frame
	 *
	 * \return An ARId object representing the specified information
	 *
	 * \throw InvalidMetadataException If the parameters form no valid ARId
	 */
	std::unique_ptr<ARId> build_worker(const TOC &toc, const uint32_t leadout)
		const;

	/**
	 * \brief Service method: Compute the disc id 1 from offsets and leadout.
	 *
	 * \param[in] offsets Offsets (in LBA frames) of each track
	 * \param[in] leadout Leadout LBA frame
	 */
	uint32_t disc_id_1(const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * \brief Service method: Compute the disc id 2 from offsets and leadout.
	 *
	 * \param[in] offsets Offsets (in LBA frames) of each track
	 * \param[in] leadout Leadout LBA frame
	 */
	uint32_t disc_id_2(const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * \brief Service method: Compute the CDDB id from offsets and leadout.
	 *
	 * The CDDB id is a 32bit unsigned integer, formed of a concatenation of
	 * the following 3 numbers:
	 * first chunk (8 bits):   checksum (sum of digit sums of offset secs + 2)
	 * second chunk (16 bits): total seconds count
	 * third chunk (8 bits):   number of tracks
	 *
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 * \param[in] leadout     Leadout LBA frame
	 */
	uint32_t cddb_id(const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * \deprecated
	 *
	 * \brief Service method: Compute the disc id 2 from offsets and leadout.
	 *
	 * \param[in] track_count   Number of tracks in this medium
	 * \param[in] offsets       Offsets (in CDDA frames) of each track
	 * \param[in] leadout Leadout CDDA frame
	 */
	uint32_t disc_id_2(const TrackNo track_count,
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * \deprecated
	 *
	 * \brief Service method: Compute the CDDB disc id from offsets and leadout.
	 *
	 * Vector offsets contains the frame offsets as parsed from the CUE sheet
	 * with the leadout frame added as an additional element on the back
	 * position.
	 *
	 * The CDDB id is a 32bit unsigned integer, formed of a concatenation of
	 * the following 3 numbers:
	 * first chunk (8 bits):   checksum (sum of digit sums of offset secs + 2)
	 * second chunk (16 bits): total seconds count
	 * third chunk (8 bits):   number of tracks
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 * \param[in] leadout     Leadout LBA frame
	 */
	uint32_t cddb_id(const TrackNo track_count,
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * \brief Service method: sum up the digits of the number passed
	 *
	 * \param[in] number An unsigned integer number
	 *
	 * \return The sum of the digits of the number
	 */
	static uint64_t sum_digits(const uint32_t number);
};


/**
 * \internal
 * \ingroup id
 *
 * \brief Constructs a TOC from metadata of the compact disc.
 *
 * TOCBuilder will refuse to construct invalid TOCs from any data and throw
 * an InvalidMetadataException instead. Hence, if no exception was thrown
 * and an actual TOC is returned, this TOC is guaranteed to be at
 * least minimally consistent.
 *
 * The only allowed relaxation is that the TOC may be incomplete, which
 * means that the leadout may be unknown. This relaxation is allowed because the
 * length of the last track or the leadout cannot be deduced from certain
 * TOC file formats. However, it is required to construct TOC from
 * files in any TOC file format, e.g. CUEsheets.
 */
class TOCBuilder final
{

public:

	/**
	 * \brief Build a TOC object from the specified information.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) for each track
	 * \param[in] leadout     Leadout frame
	 * \param[in] files       File name of each track
	 *
	 * \return A TOC object representing the specified information
	 *
	 * \throw InvalidMetadataException If the input data forms no valid TOC
	 */
	std::unique_ptr<TOC> build(const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const uint32_t leadout,
			const std::vector<std::string> &files) const;

	/**
	 * \brief Build a TOC object from the specified information.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 * \param[in] lengths     Lengths (in LBA frames) of each track
	 * \param[in] files       File name of each track
	 *
	 * \return A TOC object representing the specified information
	 *
	 * \throw InvalidMetadataException If the input data forms no valid TOC
	 */
	std::unique_ptr<TOC> build(const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const std::vector<int32_t> &lengths,
			const std::vector<std::string> &files) const;

	/**
	 * \brief Update a non-complete TOC object with a missing leadout.
	 *
	 * If \c toc is already complete, it will not be altered.
	 *
	 * \param[in] toc     The TOC to make complete
	 * \param[in] leadout The leadout to update the TOC with
	 *
	 * \throw InvalidMetadataException If the input data forms no valid TOC
	 */
	std::unique_ptr<TOC> merge(const TOC &toc, const uint32_t leadout) const;


private:

	/**
	 * \brief Service method: Builds a track count for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] track_count Number of tracks
	 *
	 * \return The intercepted track_count
	 *
	 * \throw InvalidMetadataException If the track count is not valid
	 */
	TrackNo build_track_count(const TrackNo track_count) const;

	/**
	 * \brief Service method: Builds validated offsets for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] offsets     Offsets to be validated
	 * \param[in] track_count Number of tracks
	 * \param[in] leadout     Leadout frame
	 *
	 * \return A representation of the validated offsets as unsigned integers
	 *
	 * \throw InvalidMetadataException If the offsets are not valid
	 */
	std::vector<uint32_t> build_offsets(const std::vector<int32_t> &offsets,
			const TrackNo track_count, const uint32_t leadout) const;

	/**
	 * \brief Service method: Builds validated offsets for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] offsets     Offsets to be validated
	 * \param[in] track_count Number of tracks
	 * \param[in] lengths     Lengths to be validated
	 *
	 * \return A representation of the validated offsets as unsigned integers
	 *
	 * \throw InvalidMetadataException If the offsets are not valid
	 */
	std::vector<uint32_t> build_offsets(const std::vector<int32_t> &offsets,
			const TrackNo track_count,
			const std::vector<int32_t> &lengths) const;

	/**
	 * \brief Service method: Builds validated lengths for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] sv Vector of lengths as signed integers to be validated
	 * \param[in] track_count Number of tracks
	 *
	 * \return A representation of the validated lengths as unsigned integers
	 *
	 * \throw InvalidMetadataException If the lengths are not valid
	 */
	std::vector<uint32_t> build_lengths(const std::vector<int32_t> &sv,
			const TrackNo track_count) const;

	/**
	 * \brief Service method: Builds validated leadout for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] leadout Leadout to be validated
	 *
	 * \return A representation of the validated leadout
	 *
	 * \throw InvalidMetadataException If the leadout is not valid
	 */
	uint32_t build_leadout(const uint32_t leadout) const;

	/**
	 * \brief Service method: Builds validated audio file list for a TOC object.
	 *
	 * Used by TOCBuilder::build().
	 *
	 * \param[in] files File list to be validated
	 *
	 * \return A representation of the validated file list
	 *
	 * \throw InvalidMetadataException If the file list is not valid
	 */
	std::vector<std::string> build_files(std::vector<std::string> files) const;

	/**
	 * \brief Validator instance
	 */
	TOCValidator validator_;
};


/**
 * \internal
 * \ingroup id
 *
 * \brief Worker to calculate the leadout
 *
 * \param[in] track_count Number of tracks in this medium
 * \param[in] offsets     Offsets (in CDDA frames) of each track
 * \param[in] lengths     Lengths (in CDDA frames) of each track
 */
//uint32_t calculate_leadout(const TrackNo track_count,
//	const std::vector<uint32_t> &offsets,
//	const std::vector<uint32_t> &lengths);

/**
 * \internal
 * \ingroup id
 *
 * \brief Worker to calculate the leadout
 *
 * \param[in] offsets Offsets (in CDDA frames) of each track
 * \param[in] lengths Lengths (in CDDA frames) of each track
 */
uint32_t leadout(const std::vector<uint32_t> &offsets,
		const std::vector<uint32_t> &lengths);

/**
 * \internal
 * \ingroup id
 *
 * \brief Worker to calculate the leadout
 *
 * \param[in] lengths Lengths (in CDDA frames) of each track
 */
uint32_t leadout(const std::vector<uint32_t> &lengths);

} // namespace v_1_0_0

} // namespace arcstk

#endif
