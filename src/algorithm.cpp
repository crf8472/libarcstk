/**
 * \internal
 *
 * \file
 *
 * \brief Implementation of the algorithm interface.
 */

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"
#endif

#include <algorithm>   // for min, max
#include <cstddef>     // for size_t
#include <cstdint>     // for int32_t, uint16_t
#include <iomanip>     // for setw
#include <ios>         // for right
#include <memory>      // for make_unique, unique_ptr
#include <utility>     // for move, pair
#include <vector>      // for vector

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"
#endif
#ifndef LIBARCSTK_LOGLEVEL_HPP_
#include "loglevel.hpp"
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"        // for AudioSize, CDDA
#endif


namespace arcstk
{
inline namespace v_1_0_0
{

// Partition


Partition::Partition(
		const int32_t begin_offset,
		const int32_t end_offset,
		const bool    starts_track,
		const bool    ends_track,
		const TrackNo track
	)
	: begin_offset_ { begin_offset }
	, end_offset_   { end_offset   }
	, starts_track_ { starts_track }
	, ends_track_   { ends_track   }
	, track_        { track        }
{
	// empty
}


int32_t Partition::begin_offset() const
{
	return begin_offset_;
}


int32_t Partition::end_offset() const
{
	return end_offset_;
}


bool Partition::starts_track() const
{
	return starts_track_;
}


bool Partition::ends_track() const
{
	return ends_track_;
}


int Partition::track() const
{
	return track_;
}


std::size_t Partition::size() const
{
	return static_cast<std::size_t>(end_offset() - begin_offset());
}


// Partitioner


Partitioner::Partitioner(Points points, AudioSize total_samples,
		SampleRange legal)
	: points_        { std::move(points) }
	, total_samples_ { total_samples     }
	, legal_         { legal             }
{
	// empty
}


Partitioning Partitioner::create_partitioning(
		const int32_t offset,
		const int32_t total_samples_in_block) const
{
	ARCS_LOG(DEBUG2) << "Create partitioning for offset " << offset;

	const SampleRange current_interval {
		offset, offset + details::am2ind(total_samples_in_block)
	};

	// If the sample block does not contain any relevant samples,
	// just return an empty partitioning.

	if (current_interval.upper() < legal_range().lower() ||
			current_interval.lower() > legal_range().upper())
	{
		ARCS_LOG(DEBUG2) <<
			"No relevant samples in interval, provide no partitions";
		return Partitioning {};
	}

	return do_create_partitioning(current_interval, legal_range(), points_);
}


AudioSize Partitioner::total_samples() const noexcept
{
	return total_samples_;
}


void Partitioner::set_total_samples(const AudioSize& total_samples) noexcept
{
	total_samples_ = total_samples;
}


SampleRange Partitioner::legal_range() const noexcept
{
	return legal_;
}


AudioSize Partitioner::length(const TrackNo track_no) const
{
	const auto t = static_cast<std::size_t>(track_no);
	const auto next = (t == points_.size()) ? total_samples_ : points_[t];

	return { next.bytes() - points_[(t - 1)].bytes(), UNIT::BYTES };
}


AudioSize Partitioner::offset(const TrackNo t) const
{
	return points_[static_cast<std::size_t>(t - 1)];
}


Points Partitioner::points() const noexcept
{
	return points_;
}


std::unique_ptr<Partitioner> Partitioner::clone() const
{
	return do_clone();
}


namespace details
{

// TrackPartitioner


TrackPartitioner::TrackPartitioner(const Points& points,
		const AudioSize&   total_samples,
		const SampleRange& legal)
	: Partitioner(points, total_samples, legal)
{
	// empty
}


Partitioning TrackPartitioner::do_create_partitioning(
		const SampleRange& interval,     /* block of samples */
		const SampleRange& legal,        /* legal range of samples */
		const Points&      points) const /* track points */
{
	return get_partitioning(interval, legal, points);
}


std::unique_ptr<Partitioner> TrackPartitioner::do_clone() const
{
	return std::make_unique<TrackPartitioner>(*this);
}


// get_partitioning


Partitioning get_partitioning(const SampleRange& interval,
		const SampleRange& legal,
		const Points&      opoints)
{
	if (opoints.empty())
	{
		return get_partitioning(interval, legal);
	}

	const auto real_lower { std::max(legal.lower(), interval.lower()) };
	const auto real_upper { std::min(legal.upper(), interval.upper()) };
	const auto points     { convert<UNIT::SAMPLES>(opoints)  };

	// Both, real_lower and real_upper lie in segments between two of points[].
	// Identify those segments.

	auto b = std::size_t { 0 };
	auto e = std::size_t { 0 };
	for (const auto& p : points)
	{
		if (real_lower >= p) { ++b; };
		if (real_upper >= p) { ++e; } else { break; }
	}

	// Now, b-1 and e-1 are the indices of the tracks/segments in which the
	// bounds lie. All segments between these two, i.e. in the interval
	// [b+1,e-2] can be just read off of points[].

	// Add first partition: from real lower to the start of the subsequent
	// track or the upper bound. May or may not end the first track.

	// Note: The interval may be smaller than a track. In this case, only one
	// partition will be returned and it may be the partition that ends the last
	// track. In this case, b will be bigger than the index.

	auto partitions = std::vector<Partition>{};

	// front

	auto track { b };
	{
		const auto start_of_track = track == 0 ? 0 : points[track - 1];
		const auto end_of_track { track < points.size()
			? points[track] - 1
			: real_upper
		};

		// start of the first (and maybe only) partition
		const auto p0_lower { real_lower };

		// end of the first (and maybe only) partition
		const auto p0_upper { track < points.size()  // if not last track
			? std::min(end_of_track, real_upper)
			: real_upper
		};

		ARCS_LOG(DEBUG3) << "Create front partition, "
			<< "track " << std::setw(2) << std::right << track << ": "
			<< std::setw(9) << std::right << p0_lower
			<< " - "
			<< std::setw(9) << std::right << p0_upper;

		partitions.emplace_back(
			p0_lower, p0_upper,
			p0_lower == start_of_track || p0_lower == legal.lower(),
			p0_upper == end_of_track   || p0_upper == legal.upper(),
			static_cast<TrackNo>(track)
		);
	} // front

	// If the interval does not span over multiple tracks, we are done now.
	if (b == e) { return partitions; }

	// mid (if any)

	// Add further partitions, this is just from point i to point i + 1.
	// Will be entirely skipped if the partition does span 2 tracks or less.
	for (auto i { b }; i < e - 1; ++i)
	{
		track = i + 1;

		ARCS_LOG(DEBUG3) << "Create mid. partition,  "
			<< "track " << std::setw(2) << std::right << track << ": "
			<< std::setw(9) << std::right << points[i]
			<< " - "
			<< std::setw(9) << std::right << points[track];

		partitions.emplace_back(
				points[i], points[track] - 1, true, true,
				static_cast<TrackNo>(track));
	}

	// back

	track = e;
	{
		const auto pN_lower { points[track - 1] };

		const auto pN_upper { track < points.size()
			? std::min(points[track] - 1, real_upper)
			: real_upper
		};

		ARCS_LOG(DEBUG3) << "Create back partition,  "
			<< "track " << std::setw(2) << std::right << e << ": "
			<< std::setw(9) << std::right << pN_lower
			<< " - "
			<< std::setw(9) << std::right << pN_upper;

		// Add last partition: from the beginning of the track that contains the
		// upper bound to the real upper bound.
		partitions.emplace_back(pN_lower, pN_upper,
			/* a previous partition ending with previous track is guaranteed */
			true,
			/* does this partition end with last track or another track? */
			pN_upper == legal.upper()
				|| (track < points.size() && pN_upper == points[track] - 1),
			static_cast<TrackNo>(track)
		);
	} // back

	return partitions;
}


Partitioning get_partitioning(const SampleRange& interval,
		const SampleRange& legal)
{
	// Create a single partition spanning the entire block of samples.
	// Respect legal range.

	const auto partition_start { interval.contains(legal.lower())
		? legal.lower()
		: interval.lower()
	};

	const auto partition_end { interval.contains(legal.upper())
		? legal.upper()
		: interval.upper()
	};

	ARCS_LOG(DEBUG3) << "Create partition from interval: " << partition_start
		<< " - " << partition_end;

	return { Partition { partition_start, partition_end,
		( partition_start == legal.lower() )/* starts track ? */,
		( partition_end   == legal.upper() )/* ends track ? */,
		0/* invalid track */
	}};
}


// ind2am()


int32_t ind2am(const int32_t index)
{
	return index + 1;
}


// am2ind()


int32_t am2ind(const int32_t amount)
{
	return amount - 1;
}

} // namespace details
} // namespace v_1_0_0
} // namespace arcstk

