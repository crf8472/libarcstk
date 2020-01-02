// vim:ft=cpp
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#error "Do not include builders.tpp directly, include identifier.hpp instead"
#endif

#ifndef __LIBARCSTK_BUILDERS_TPP__
#define __LIBARCSTK_BUILDERS_TPP__

/**
 * \file
 *
 * \internal
 *
 * \brief Builders for ARIds and TOCs.
 */


#include <cstdint>
#include <numeric>   // for accumulate
#include <sstream>
#include <vector>

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{
//namespace details
//{

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
	inline std::unique_ptr<ARId> build(const TrackNo &track_count,
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
	inline std::unique_ptr<ARId> build(const TOC &toc, const uint32_t leadout)
		const;

	/**
	 * \brief Build an ARId object from the specified TOC.
	 *
	 * \param[in] toc TOC to build ARId from
	 *
	 * \return An ARId object representing the specified information
	 *
	 * \throw InvalidMetadataException If the TOC forms no valid ARId
	 */
	inline std::unique_ptr<ARId> build(const TOC &toc) const;

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
	inline std::unique_ptr<ARId> build_empty_id() const noexcept;


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
	inline std::unique_ptr<ARId> build_worker(const TOC &toc, const uint32_t leadout)
		const;

	/**
	 * \brief Service method: Compute the disc id 1 from offsets and leadout.
	 *
	 * \param[in] offsets Offsets (in LBA frames) of each track
	 * \param[in] leadout Leadout LBA frame
	 */
	inline uint32_t disc_id_1(const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * \brief Service method: Compute the disc id 2 from offsets and leadout.
	 *
	 * \param[in] offsets Offsets (in LBA frames) of each track
	 * \param[in] leadout Leadout LBA frame
	 */
	inline uint32_t disc_id_2(const std::vector<uint32_t> &offsets,
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
	inline uint32_t cddb_id(const std::vector<uint32_t> &offsets,
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
	inline uint32_t disc_id_2(const TrackNo track_count,
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
	inline uint32_t cddb_id(const TrackNo track_count,
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout) const;

	/**
	 * \brief Service method: sum up the digits of the number passed
	 *
	 * \param[in] number An unsigned integer number
	 *
	 * \return The sum of the digits of the number
	 */
	inline static uint64_t sum_digits(const uint32_t number);
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
	inline std::unique_ptr<TOC> build(const TrackNo track_count,
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
	inline std::unique_ptr<TOC> build(const TrackNo track_count,
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
	inline std::unique_ptr<TOC> merge(const TOC &toc, const uint32_t leadout) const;


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
	inline TrackNo build_track_count(const TrackNo track_count) const;

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
	inline std::vector<uint32_t> build_offsets(const std::vector<int32_t> &offsets,
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
	inline std::vector<uint32_t> build_offsets(const std::vector<int32_t> &offsets,
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
	inline std::vector<uint32_t> build_lengths(const std::vector<int32_t> &sv,
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
	inline uint32_t build_leadout(const uint32_t leadout) const;

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
	inline std::vector<std::string> build_files(std::vector<std::string> files) const;

	/**
	 * \brief Validator instance
	 */
	TOCValidator validator_;
};



//uint32_t calculate_leadout(const TrackNo track_count,
//		const std::vector<uint32_t> &offsets,
//		const std::vector<uint32_t> &lengths)
//{
//	if (track_count < 1 or track_count > CDDA.MAX_TRACKCOUNT)
//	{
//		return 0;
//	}
//	auto tc = static_cast<std::vector<uint32_t>::size_type>(track_count) - 1u;
//	return (lengths.at(tc) > 0) ? offsets.at(tc) + lengths.at(tc) : 0;
//}


/**
 * \internal
 * \ingroup id
 *
 * \brief Worker to calculate the leadout
 *
 * \param[in] offsets Offsets (in LBA frames) of each track
 * \param[in] lengths Lengths (in LBA frames) of each track
 */
inline uint32_t leadout(const std::vector<uint32_t> &offsets,
		const std::vector<uint32_t> &lengths);

// Implementation
uint32_t leadout(const std::vector<uint32_t> &offsets,
		const std::vector<uint32_t> &lengths)
{
	return lengths.back() == 0 ? 0 : offsets.back() + lengths.back();
}

/**
 * \internal
 * \ingroup id
 *
 * \brief Worker to calculate the leadout
 *
 * \param[in] lengths Lengths (in LBA frames) of each track
 */
inline uint32_t leadout(const std::vector<uint32_t> &lengths);

// Implementation
uint32_t leadout(const std::vector<uint32_t> &lengths)
{
	if (lengths.back() == 0)
	{
		return 0;
	}

	auto sum { std::accumulate(lengths.begin(), lengths.end(), 0) };
	auto max {
		static_cast<decltype(sum)>(std::numeric_limits<uint32_t>::max()) };

	if (sum > max)
	{
		return 0; // TODO
	}

	return static_cast<uint32_t>(sum);
}


/**
 * \brief Uniform access to a container by track
 *
 * Instead of using at() that uses a 0-based index, we need a uniform method
 * to access a container by using a 1-based index and we want to range check it.
 *
 * Type Container is required to yield its number of elements by member function
 * size() and to allow access via operator[].
 *
 * \tparam Container Container type with size() and []
 * \param c Actual container
 * \param t Number of the track to access
 *
 * \return The value for track \c t in the container \c
 */
template <typename Container>
decltype(auto) get_track(Container&& c, const TrackNo t)
{
	auto container_size = std::forward<Container>(c).size();

	// Do the range check
	if (t < 1 or static_cast<decltype(container_size)>(t) > container_size)
	{
		std::stringstream message;
		message << "Track " << t << " is out of range (yields index "
			<< (t - 1) << " but size is " << container_size << ")";

		throw std::out_of_range(message.str());
	}

	return std::forward<Container>(c)[
		static_cast<decltype(container_size)>(t - 1)];
}


// ARIdBuilder


std::unique_ptr<ARId> ARIdBuilder::build(const TrackNo &track_count,
		const std::vector<int32_t> &offsets, const uint32_t leadout) const
{
	TOCBuilder builder;
	auto toc = builder.build(track_count, offsets, leadout, {/* no files */});

	return build_worker(*toc, 0);
}


std::unique_ptr<ARId> ARIdBuilder::build(const TOC &toc, const uint32_t leadout)
	const
{
	return build_worker(toc, leadout);
}


std::unique_ptr<ARId> ARIdBuilder::build(const TOC &toc) const
{
	return build_worker(toc, 0);
}


std::unique_ptr<ARId> ARIdBuilder::build_empty_id() const noexcept
{
	try
	{
		return std::make_unique<ARId>(0, 0, 0, 0);

	} catch (const std::exception& e)
	{
		ARCS_LOG_ERROR << "Exception while creating empty ARId: " << e.what();
	}

	return nullptr;
}


std::unique_ptr<ARId> ARIdBuilder::build_worker(const TOC &toc,
		const uint32_t leadout) const
{
	// Override TOC leadout with optional non-null extra leadout

	uint32_t leadout_val { leadout };

	if (leadout_val > 0)
	{
		TOCValidator validator;
		validator.validate(toc, leadout_val);
	} else
	{
		leadout_val = toc.leadout();
	}

	auto offsets = toc::get_offsets(toc);

	return std::make_unique<ARId>(
			toc.track_count(),
			this->disc_id_1(offsets, leadout_val),
			this->disc_id_2(offsets, leadout_val),
			this->cddb_id  (offsets, leadout_val)
	);
}


uint32_t ARIdBuilder::disc_id_1(const std::vector<uint32_t> &offsets,
		const uint32_t leadout) const
{
	// disc id 1 is just the sum off all offsets + the leadout frame

	uint32_t sum_offsets = 0;

	for (const auto &o : offsets) { sum_offsets += o; }

	return sum_offsets + leadout;
}


uint32_t ARIdBuilder::disc_id_2(const std::vector<uint32_t> &offsets,
		const uint32_t leadout) const
{
	// disc id 2 is the sum of the products of offsets and the corresponding
	// 1-based track number while normalizing offsets to be >= 1

	uint32_t accum = 0;

	uint16_t track = 1;
	for (const auto &o : offsets) { accum += (o > 0 ? o : 1) * track; track++; }

	return accum + leadout /* must be > 0*/ * track;
}


uint32_t ARIdBuilder::cddb_id(const std::vector<uint32_t> &offsets,
		const uint32_t leadout) const
{
	const auto fps = static_cast<uint32_t>(CDDA.FRAMES_PER_SEC);
	uint32_t accum = 0;

	for (const auto &o : offsets)
	{
		accum += sum_digits(o / fps + 2u);
	}
	accum %= 255; // normalize to 1 byte

	const uint32_t     total_seconds = leadout / fps - offsets[0] / fps;
	const unsigned int track_count   = offsets.size();

	return (accum << 24u) | (total_seconds << 8u) | track_count;
}


uint32_t ARIdBuilder::disc_id_2(const TrackNo track_count,
		const std::vector<uint32_t> &offsets, const uint32_t leadout) const
{
	// disc id 2 is the sum of the products of offsets and the corresponding
	// 1-based track number while normalizing offsets to be >= 1

	uint32_t sum_offsets = 0;

	for (std::size_t i = 0; i < static_cast<std::size_t>(track_count); ++i)
	{
		// This will throw if offsets.size() < track_count
		sum_offsets += (offsets[i] > 0 ? offsets[i] : 1) * (i + 1);
	}

	return sum_offsets + leadout *
		(static_cast<unsigned int>(track_count) + 1u);
}


uint32_t ARIdBuilder::cddb_id(const TrackNo track_count,
		const std::vector<uint32_t> &offsets, const uint32_t leadout) const
{
	// The CDDB id is a 32bit unsigned integer, formed of a concatenation of
	// the following 3 numbers:
	// first chunk (8 bits):   checksum (sum of digit sums of offset secs + 2)
	// second chunk (16 bits): total seconds count
	// third chunk (8 bits):   number of tracks

	// Calculate first part: checksum

	uint32_t checksum = 0;
	auto frames_per_sec = static_cast<uint32_t>(CDDA.FRAMES_PER_SEC);

	for (std::size_t i = 0; i < static_cast<std::size_t>(track_count); ++i)
	{
		// This will throw if offsets.size() < track_count
		checksum += sum_digits(offsets[i] / frames_per_sec + 2u);
	}
	checksum %= 255; // normalize to 1 byte

	// Calculate second part: seconds count

	const uint32_t seconds_count = leadout / frames_per_sec
		- offsets[0] / frames_per_sec;

	return (checksum << 24u) | (seconds_count << 8u) |
		static_cast<unsigned int>(track_count);
}


uint64_t ARIdBuilder::sum_digits(const uint32_t number)
{
	return (number < 10) ? number : (number % 10) + sum_digits(number / 10);
}


/**
 * \internal
 * \ingroup id
 *
 * \brief Private implementation of TOC.
 *
 * \see TOC
 */
class TOC::Impl final
{
	// TOCBuilder::build() methods are friends of TOC::Impl
	// since they construct TOC::Impls exclusively

	friend std::unique_ptr<TOC> TOCBuilder::build(
			const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const uint32_t leadout,
			const std::vector<std::string> &files) const;

	friend std::unique_ptr<TOC> TOCBuilder::build(
			const TrackNo track_count,
			const std::vector<int32_t> &offsets,
			const std::vector<int32_t> &lengths,
			const std::vector<std::string> &files) const;

	friend std::unique_ptr<TOC> TOCBuilder::merge(
			const TOC &toc, const uint32_t leadout) const;

public:

	/**
	 * \brief Implements TOC::track_count()
	 */
	inline TrackNo track_count() const;

	/**
	 * \brief Implements TOC::offset(const uint8_t) const
	 */
	inline uint32_t offset(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::parsed_length(const uint8_t) const
	 */
	inline uint32_t parsed_length(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::filename(const TrackNo idx) const
	 */
	inline std::string filename(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::leadout()
	 */
	inline uint32_t leadout() const;

	/**
	 * \brief Implements TOC::complete()
	 */
	inline bool complete() const;

	/**
	 * \brief Implements TOC::operator==()
	 */
	inline bool operator == (const TOC::Impl &rhs) const;


private:

	/**
	 * \brief Implements private constructor of TOC.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 * \param[in] leadout     Leadout frame
	 * \param[in] files       File name of each track
	 */
	inline Impl(const TrackNo track_count,
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout,
			const std::vector<std::string> &files);

	/**
	 * \brief Implements private constructor of TOC.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 * \param[in] lengths     Lengths (in CDDA frames) of each track
	 * \param[in] files       File name of each track
	 */
	inline Impl(const TrackNo track_count,
			const std::vector<uint32_t> &offsets,
			const std::vector<uint32_t> &lengths,
			const std::vector<std::string> &files);

	/**
	 * \brief Number of tracks
	 */
	TrackNo track_count_;

	/**
	 * \brief Track offsets (in frames)
	 */
	std::vector<uint32_t> offsets_;

	/**
	 * \brief Track lengths (in frames)
	 */
	std::vector<uint32_t> lengths_;

	/**
	 * \brief Leadout frame
	 */
	uint32_t leadout_;

	/**
	 * \brief Audio file names
	 */
	std::vector<std::string> files_;
};

/// \cond UNDOC_FUNCTION_BODIES

TOC::Impl::Impl(const TrackNo track_count,
		const std::vector<uint32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files)
	: track_count_(track_count)
	, offsets_(offsets)
	, lengths_()
	, leadout_(leadout)
	, files_(files)
{
	// empty
}


TOC::Impl::Impl(const TrackNo track_count,
		const std::vector<uint32_t> &offsets,
		const std::vector<uint32_t> &lengths,
		const std::vector<std::string> &files)
	: track_count_(track_count)
	, offsets_(offsets)
	, lengths_(lengths)
	, leadout_(arcstk::leadout(offsets, lengths))
	, files_(files)
{
	// empty
}


TrackNo TOC::Impl::track_count() const
{
	return track_count_;
}


uint32_t TOC::Impl::offset(const TrackNo track) const
{
	return get_track(offsets_, track);
}


uint32_t TOC::Impl::parsed_length(const TrackNo track) const
{
	return get_track(lengths_, track);
}


std::string TOC::Impl::filename(const TrackNo track) const
{
	return get_track(files_, track);
}


uint32_t TOC::Impl::leadout() const
{
	return leadout_;
}


bool TOC::Impl::complete() const
{
	return leadout_ != 0;
}


bool TOC::Impl::operator == (const TOC::Impl &rhs) const
{
	if (this == &rhs)
	{
		return true;
	}

	return track_count_ == rhs.track_count_
		and offsets_    == rhs.offsets_
		and lengths_    == rhs.lengths_
		and leadout_    == rhs.leadout_
		and files_      == rhs.files_;
}


// TOCBuilder


std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files) const
{
	auto impl = std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(offsets, track_count, leadout),
		build_leadout(leadout),
		build_files(files))
	);

	return std::make_unique<TOC>(std::move(impl));
}


std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		const std::vector<int32_t> &offsets,
		const std::vector<int32_t> &lengths,
		const std::vector<std::string> &files) const
{
	auto impl = std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(offsets, track_count, lengths),
		build_lengths(lengths, track_count),
		build_files(files))
	);

	return std::make_unique<TOC>(std::move(impl));
}


std::unique_ptr<TOC> TOCBuilder::merge(const TOC &toc, const uint32_t leadout)
	const
{
	if (toc.complete())
	{
		return std::make_unique<TOC>(toc);
	}

	validator_.validate(toc, leadout);

	// add length of last track
	// TODO Erh....
	std::vector<uint32_t> merged_lengths;
	auto size =
		static_cast<std::vector<uint32_t>::size_type>(toc.track_count());
	merged_lengths.reserve(size);
	merged_lengths = toc::get_parsed_lengths(toc);
	merged_lengths.push_back(leadout - toc.offset(toc.track_count()));

	std::unique_ptr<TOC::Impl> impl = std::make_unique<TOC::Impl>(TOC::Impl(
		toc.track_count(),
		toc::get_offsets(toc),
		merged_lengths,
		toc::get_filenames(toc)
	));

	impl->leadout_ = leadout;

	return std::make_unique<TOC>(std::move(impl));
}


TrackNo TOCBuilder::build_track_count(const TrackNo track_count) const
{
	validator_.validate_trackcount(track_count);

	return track_count;
}


std::vector<uint32_t> TOCBuilder::build_offsets(
		const std::vector<int32_t> &offsets, const TrackNo track_count,
		const uint32_t leadout) const
{
	validator_.validate(track_count, offsets, leadout);

	// Convert offsets to uints

	return std::vector<uint32_t>(offsets.begin(), offsets.end());
}


std::vector<uint32_t> TOCBuilder::build_offsets(
		const std::vector<int32_t> &offsets, const TrackNo track_count,
		const std::vector<int32_t> &lengths) const
{
	// Valid number of lengths ?

	if (offsets.size() != static_cast<std::size_t>(track_count))
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(track_count) << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	if (offsets.size() != lengths.size())
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(offsets.size()) << " offsets";

		throw InvalidMetadataException(ss.str());
	}

	validator_.validate_lengths(lengths);

	validator_.validate_offsets(track_count, offsets);

	// Convert offsets to uints

	return std::vector<uint32_t>(offsets.begin(), offsets.end());
}


std::vector<uint32_t> TOCBuilder::build_lengths(
		const std::vector<int32_t> &lengths, const TrackNo track_count) const
{
	// Valid number of lengths ?

	if (lengths.size() != static_cast<std::size_t>(track_count))
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(track_count) << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// If params make sense, use TOCValidator

	validator_.validate_lengths(lengths);

	// Convert ints to uints while normalizing the last length to 0

	std::vector<uint32_t> uv(lengths.begin(), lengths.end() - 1);

	auto last_length = lengths.back() < 0 ? 0 : lengths.back();
	uv.push_back(static_cast<uint32_t>(last_length));

	return uv;
}


uint32_t TOCBuilder::build_leadout(const uint32_t leadout) const
{
	validator_.validate_leadout(leadout);

	return leadout;
}


std::vector<std::string> TOCBuilder::build_files(std::vector<std::string> files)
	const
{
	// no validation for now

	return files;
}


/// endcond

} // namespace v_1_0_0

//} // namespace details

} // namespace arcstk

#endif

