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
 * \brief Internal APIs for AccurateRip id calculation
 */

namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \internal \defgroup idInternal Internal APIs
 * \ingroup id
 * @{
 */

/**
 * \internal
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
	 * \brief Default constructor.
	 */
	ARIdBuilder();

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	ARIdBuilder(const ARIdBuilder &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	ARIdBuilder(ARIdBuilder &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~ARIdBuilder() noexcept;

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
	std::unique_ptr<ARId> build_empty_id() noexcept;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	ARIdBuilder& operator = (const ARIdBuilder &rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	ARIdBuilder& operator = (ARIdBuilder &&rhs) noexcept;


private:

	// Forward declaration for private implementation
	class Impl;

	/**
	 * \brief Private implementation of ARIdBuilder.
	 */
	std::unique_ptr<ARIdBuilder::Impl> impl_;
};


/**
 * \internal
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

	// Forward declaration for TOCBuilder::Impl
	class Impl;

	/**
	 * \brief Constructor.
	 */
	TOCBuilder();

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	TOCBuilder(const TOCBuilder &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	TOCBuilder(TOCBuilder &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~TOCBuilder() noexcept;

	/**
	 * \brief Build a TOC object from the specified information.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) for each track
	 * \param[in] leadout     Leadout frame
	 *
	 * \return A TOC object representing the specified information
	 *
	 * \throw InvalidMetadataException If the input data forms no valid TOC
	 */
	std::unique_ptr<TOC> build(const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const uint32_t leadout);

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
			const std::vector<std::string> &files);

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

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TOCBuilder& operator = (const TOCBuilder &rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs The right hand side of the assignment
	 *
	 * \return The right hand side of the assigment
	 */
	TOCBuilder& operator = (TOCBuilder &&rhs) noexcept;


private:

	/**
	 * \brief Private implementation of TOCBuilder.
	 */
	std::unique_ptr<TOCBuilder::Impl> impl_;
};


/**
 * \internal
 *
 * \brief Validates offsets, leadout and track count of a compact disc toc.
 *
 * TOCBuilder and ARIdBuilder use TOCValidator to validate
 * the input for building their respective instances.
 *
 * This class is considered an implementation detail. Its declaration resides in
 * a header to ensure its testability.
 */
class TOCValidator final
{

public:

	/**
	 * \brief Validate TOC information.
	 *
	 * It is ensured that the number of offsets matches the track count, that
	 * the offsets are consistent and the leadout frame is consistent with the
	 * offsets.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 * \param[in] leadout     Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	void validate(const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * \brief Validate TOC information and leadout.
	 *
	 * It is ensured that the leadout frame is consistent with the offsets.
	 *
	 * \param[in] toc     TOC information
	 * \param[in] leadout Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	void validate(const TOC &toc, const uint32_t leadout) const;

	/**
	 * \brief Validate offsets and track count.
	 *
	 * It is ensured that the number of offsets matches the track count and that
	 * the offsets are consistent.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	void validate_offsets(const TrackNo track_count,
			const std::vector<int32_t> &offsets) const;

	/**
	 * \brief Validate offsets.
	 *
	 * It is ensured that the offsets are consistent, which means they all are
	 * within a CDDA conforming range, ordered in ascending order with a legal
	 * distance between any two subsequent offsets and their number is a valid
	 * track count.
	 *
	 * \param[in] offsets Offsets (in CDDA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	void validate_offsets(const std::vector<int32_t> &offsets) const;

	/**
	 * \brief Validate lengths.
	 *
	 * It is ensured that the lengths are consistent, which means they all have
	 * have a CDDA conforming minimal lengths, their sum is within a CDDA
	 * conforming range and their number is a valid track count. An
	 * InvalidMetadataException is thrown if the validation fails.
	 *
	 * \param[in] lengths Lengths (in CDDA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	void validate_lengths(const std::vector<int32_t> &lengths) const;

	/**
	 * \brief Validate leadout frame.
	 *
	 * It is ensured that the leadout is within a CDDA conforming range. An
	 * InvalidMetadataException is thrown if the validation fails.
	 *
	 * \param[in] leadout Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	void validate_leadout(const uint32_t leadout) const;

	/**
	 * \brief Validate track count.
	 *
	 * It is ensured that the track count is within a CDDA conforming range. An
	 * InvalidMetadataException is thrown if the validation fails.
	 *
	 * \param[in] track_count   Number of tracks in this medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	void validate_trackcount(const TrackNo track_count) const;


protected:

	/**
	 * \brief Ensure that \c prev_track and \c next_track offsets have at least minimal
	 * distance
	 *
	 * \param[in] prev_track Previous track offset
	 * \param[in] next_track Next track offset
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	void have_min_dist(const uint32_t prev_track, const uint32_t next_track)
		const;

	/**
	 * \brief Maximal valid offset value for a non-redbook 90 min CD (in CDDA frames).
	 *
	 * Non-redbook 90-min CD has 89:59.74 which is equivalent to 405.000 frames.
	 */
	static constexpr uint32_t MAX_OFFSET_90 = (89 * 60 + 59) * 75 + 74;

	/**
	 * \brief Maximal valid offset value for a non-redbook 99 min CD (in CDDA frames).
	 *
	 * Non-redbook 99-min CD has 98:59.74 which is equivalent to 445.500 frames.
	 */
	static constexpr uint32_t MAX_OFFSET_99 = (98 * 60 + 59) * 75 + 74;
};


/**
 * \brief Worker to calculate the leadout
 *
 * \param[in] track_count Number of tracks in this medium
 * \param[in] offsets     Offsets (in CDDA frames) of each track
 * \param[in] lengths     Lengths (in CDDA frames) of each track
 */
uint32_t calculate_leadout(const TrackNo track_count,
	const std::vector<uint32_t> &offsets,
	const std::vector<uint32_t> &lengths);


/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

