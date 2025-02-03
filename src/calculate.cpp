/**
 * \file
 *
 * \brief Implementation of the new checksum calculation API
 */

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_IMPL_HPP__
#include "calculate_impl.hpp"
#endif

#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif
#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"      // for AudioSize, ToC, CDDA
#endif
#ifndef __LIBARCSTK_METADATA_CONV_HPP__
#include "metadata_conv.hpp" // for convert
#endif

#include <algorithm>   // for min, max
#include <cstdint>     // for int32_t, uint16_t
#include <iomanip>     // for setw, right

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

// calculate_details.hpp


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
	const auto points     { details::convert<UNIT::SAMPLES>(opoints)  };

	// Both, real_lower and real_upper lie in segments between two of points[].
	// Identify those segments.

	auto b = std::size_t { 0 };
	auto e = std::size_t { 0 };
	for (const auto& p : points)
	{
		if (real_lower >= p) { ++b; };
		if (real_upper >= p) { ++e; } else break;
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
		const auto end_of_track   = points[track] - 1;

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
			true,/*since a previous partition is guaranteed that ends on track end*/
			pN_upper == points[track] - 1 || pN_upper == legal.upper(),
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

	return { Partition {
		{ partition_start },
		{ partition_end   },
		{ partition_start == legal.lower() }/* starts track ? */,
		{ partition_end   == legal.upper() }/* ends track ? */,
		0/* invalid track */
	}};
}


// Partitioner


Partitioner::Partitioner(const int32_t total_samples, const Points& points,
		const SampleRange& legal)
	: total_samples_ { total_samples }
	, points_        { points        }
	, legal_         { legal         }
{
	// empty
}


Partitioner::~Partitioner() noexcept = default;


Partitioning Partitioner::create_partitioning(
		const int32_t offset,
		const int32_t total_samples_in_block) const
{
	const SampleRange current_interval {
		offset, offset + am2ind(total_samples_in_block)
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


int32_t Partitioner::total_samples() const
{
	return total_samples_;
}


void Partitioner::set_total_samples(const int32_t total_samples)
{
	total_samples_ = total_samples;
}


SampleRange Partitioner::legal_range() const
{
	return legal_;
}


Points Partitioner::points() const
{
	return points_;
}


std::unique_ptr<Partitioner> Partitioner::clone() const
{
	return do_clone();
}


// make_partitioner


std::unique_ptr<Partitioner> make_partitioner(const AudioSize& size,
		const SampleRange& calc_range) noexcept
{
	return make_partitioner(size, {/* empty */}, calc_range);
}


std::unique_ptr<Partitioner> make_partitioner(const AudioSize& size,
		const Points& points, const SampleRange& calc_range)
		noexcept
{
	// TODO Check calc_range
	// if calc_range.lower() < 1, use 1 as lower
	// if calc_range.upper() > size, use size as upper

	return std::make_unique<TrackPartitioner>(size.samples(), points,
			calc_range);
}


// TrackPartitioner


TrackPartitioner::TrackPartitioner(const int32_t total_samples,
			const Points& points,
			const SampleRange&    legal)
	: Partitioner(total_samples, points, legal)
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


// Partition


Partition::Partition(
		const int32_t& begin_offset,
		const int32_t& end_offset,
		const bool&    starts_track,
		const bool&    ends_track,
		const TrackNo& track
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


// calculate_impl.hpp


int32_t ind2am(const int32_t index)
{
	return index + 1;
}


int32_t am2ind(const int32_t amount)
{
	return amount - 1;
}


// CalculationState


CalculationState::CalculationState(Algorithm* const algorithm)
	: current_offset_      { 0 }
	, samples_processed_   { 0 }
	, track_samples_processed_ { 0 }
	, tracks_processed_    { 0 }
	, algo_time_elapsed_   { 0 }
	, update_time_elapsed_ { 0 }
	, algorithm_           { algorithm }
{
	// empty
}


CalculationState::CalculationState(const CalculationState& rhs)
	: current_offset_      { rhs.current_offset_      }
	, samples_processed_   { rhs.samples_processed_   }
	, track_samples_processed_ { rhs.track_samples_processed_ }
	, tracks_processed_    { rhs.tracks_processed_    }
	, algo_time_elapsed_   { rhs.algo_time_elapsed_   }
	, update_time_elapsed_ { rhs.update_time_elapsed_ }
	, algorithm_           { rhs.algorithm_           }
{
	// empty
}


CalculationState::CalculationState(CalculationState&& rhs) noexcept
	: current_offset_      { std::move(rhs.current_offset_)    }
	, samples_processed_   { std::move(rhs.samples_processed_) }
	, track_samples_processed_ { std::move(rhs.track_samples_processed_) }
	, tracks_processed_    { std::move(rhs.tracks_processed_)  }
	, algo_time_elapsed_   { std::move(rhs.algo_time_elapsed_) }
	, update_time_elapsed_ { std::move(rhs.update_time_elapsed_) }
	, algorithm_           { std::move(rhs.algorithm_)         }
{
	// empty
}


CalculationState::~CalculationState() noexcept = default;


void CalculationState::do_advance(const int32_t /* amount */)
{
	// empty
}


void CalculationState::do_update(SampleInputIterator start,
		SampleInputIterator stop)
{
	algorithm_->update(start, stop);
}


ChecksumSet CalculationState::do_current_subtotal() const
{
	return algorithm_->result();
}


void CalculationState::do_track_finished()
{
	algorithm_->track_finished(tracks_processed_.value(),
			AudioSize { track_samples_processed_.value(), UNIT::SAMPLES });
}


int32_t CalculationState::current_offset() const noexcept
{
	return current_offset_.value();
}


void CalculationState::advance(const int32_t amount)
{
	current_offset_.increment(amount);
	do_advance(amount);
}


int32_t CalculationState::samples_processed() const noexcept
{
	return samples_processed_.value();
}


const Algorithm* CalculationState::algorithm() const noexcept
{
	return algorithm_;
}


std::chrono::duration<float> CalculationState::update_time_elapsed() const
	noexcept
{
	return update_time_elapsed_;
}


void CalculationState::increment_update_time_elapsed(
			const std::chrono::duration<float>& duration)
{
	update_time_elapsed_ += duration;
}


std::chrono::duration<float> CalculationState::algo_time_elapsed() const
	noexcept
{
	return algo_time_elapsed_;
}


void CalculationState::update(SampleInputIterator start,
		SampleInputIterator stop)
{
	using fsec = std::chrono::duration<float>;

	const auto amount { std::distance(start, stop) };

	const auto start_time { std::chrono::steady_clock::now() };

	do_update(start, stop); // TODO try and update counter in catch

	const auto stop_time { std::chrono::steady_clock::now() };
	const fsec dur       { stop_time - start_time };
	algo_time_elapsed_ += dur;

	samples_processed_.increment(amount);
	track_samples_processed_.increment(amount);
	advance(amount);
}


void CalculationState::track_finished()
{
	tracks_processed_.increment(1);
	this->do_track_finished();
	track_samples_processed_.reset();
}


ChecksumSet CalculationState::current_subtotal() const
{
	return do_current_subtotal();
}


std::unique_ptr<CalculationState> CalculationState::clone() const
{
	return do_clone();
}


std::unique_ptr<CalculationState> CalculationState::clone_to(Algorithm* a) const
{
	return do_clone_to(a);
}


void CalculationState::swap_base(CalculationState& rhs)
{
	using std::swap;

	swap(this->current_offset_,      rhs.current_offset_    );
	swap(this->samples_processed_,   rhs.samples_processed_ );
	swap(this->track_samples_processed_, rhs.track_samples_processed_ );
	swap(this->tracks_processed_,    rhs.tracks_processed_  );
	swap(this->algo_time_elapsed_,   rhs.algo_time_elapsed_ );
	swap(this->update_time_elapsed_, rhs.update_time_elapsed_ );
	swap(this->algorithm_,           rhs.algorithm_         );
}


void CalculationState::set_algorithm(Algorithm* const algorithm) noexcept
{
	algorithm_ = algorithm;
}


// CalculationStateImpl


CalculationStateImpl::CalculationStateImpl(Algorithm* const algorithm)
	: CalculationState{ algorithm }
{
	// empty
}


CalculationStateImpl::CalculationStateImpl(const CalculationStateImpl& rhs)
	: CalculationState(rhs)
{
	// empty
}


CalculationStateImpl::CalculationStateImpl(CalculationStateImpl&& rhs) noexcept
	: CalculationState(std::move(rhs))
{
	// empty
}


CalculationStateImpl::~CalculationStateImpl() noexcept = default;


std::unique_ptr<CalculationState> CalculationStateImpl::do_clone() const
{
	return base_clone();
}


std::unique_ptr<CalculationState> CalculationStateImpl::do_clone_to(
		Algorithm* algorithm) const
{
	auto cloned { base_clone() };
	cloned->set_algorithm(algorithm);
	return cloned;
}


std::unique_ptr<CalculationStateImpl> CalculationStateImpl::base_clone() const
{
	return std::make_unique<CalculationStateImpl>(*this);
}


CalculationStateImpl& CalculationStateImpl::operator = (
		const CalculationStateImpl& rhs)
{
	auto tmp { CalculationStateImpl(rhs) }; // TODO pass by value instead?

	using std::swap;
	swap(*this, tmp);
	return *this;
}


CalculationStateImpl& CalculationStateImpl::operator = (
		CalculationStateImpl&& rhs) noexcept
{
	// TODO Could this be done more performant?

	auto tmp { std::move(rhs) };

	using std::swap;
	swap(*this, tmp);
	return *this;
}


void swap(CalculationStateImpl& lhs, CalculationStateImpl& rhs) noexcept
{
	lhs.swap_base(rhs);
}


// perform_update


bool perform_update(SampleInputIterator start, SampleInputIterator stop,
		const Partitioner& partitioner,
		CalculationState&  state,
		Checksums&         result_buffer)
{
	const auto start_pos        { state.current_offset() };
	const auto samples_in_block { std::distance(start, stop) };
	const auto last_pos         { start_pos + am2ind(samples_in_block) };

	ARCS_LOG(DEBUG1) << "Offsets: " << start_pos << " - " << last_pos;
	ARCS_LOG(DEBUG1) << "Size:    " << samples_in_block   << " samples";

	// Create a partitioning following the track bounds in this block

	const auto partitioning { partitioner.create_partitioning(
			start_pos, samples_in_block) };

	if (partitioning.empty())
	{
		ARCS_LOG_DEBUG << "Skip block, advance";
		state.advance(samples_in_block);
		return state.current_offset() >= partitioner.legal_range().upper();
	} else
	{
		// If we skipped some samples at the beginning of the partition, advance
		// the state by this amount so that current_offset() will be correct on
		// subsequent call.
		const auto diff { partitioning.front().begin_offset() - start_pos };
		if (diff > 0)
		{
			ARCS_LOG(DEBUG1) << "Skipped " << diff << " samples, advance";
			state.advance(diff);
		}
	}

	ARCS_LOG(DEBUG1) << "Partitions: " << partitioning.size();

	// Update the state with each partition in this partitioning

	auto partition_counter = uint16_t { 0 };

	auto offset_first { 0 };
	auto offset_last  { 0 };
	auto total        { 0 };

	for (const auto& partition : partitioning)
	{
		++partition_counter;

		ARCS_LOG(DEBUG2) << "PARTITION " << partition_counter << "/" <<
			partitioning.size();

		offset_first = partition.begin_offset() - start_pos;
		offset_last  = partition.end_offset()   - start_pos;
		total        = offset_last + 1 - offset_first;

		ARCS_LOG(DEBUG2) << "Samples "
			<< std::setw(9) << std::right << (start_pos + offset_first)
			<< " - "
			<< std::setw(9) << std::right << (start_pos + offset_last)
			<< " (Track " << partition.track() << ", "
			<< (partition.starts_track()
					? (partition.ends_track() ? "complete"  : "first part")
					: (partition.ends_track() ? "last part" : "mid part"))
			<< ")";
		ARCS_LOG(DEBUG2) << "Samples total: " << total;

		state.update(start + offset_first, start + offset_last + 1);
		// +1 because the "stop" sample will not be processed. As an
		// end()-iterator, the intended stop point has to be shifted behind the
		// last sample to pass.

		// If the current partition ends a track, save the ARCSs for this track
		if (partition.ends_track())
		{
			ARCS_LOG(DEBUG3) << "Completed track:  " << partition.track();

			state.track_finished();
			result_buffer.push_back(state.current_subtotal());
		}
	}

	return SampleRange(start_pos, last_pos).contains(
			partitioner.legal_range().upper()/* last rel. sample */);
}

} // namespace details


// calculate.hpp


// SampleInputIterator


inline SampleInputIterator operator + (const int32_t amount,
		SampleInputIterator rhs) noexcept
{
	return rhs + amount;
}


// Context


bool any(const Context& rhs) noexcept
{
	return rhs != Context::NONE;
}


std::string to_string(const Context& c) noexcept
{
	switch (c)
	{
		case Context::ALBUM:       return "ALBUM";
		case Context::LAST_TRACK:  return "LAST_TRACK";
		case Context::FIRST_TRACK: return "FIRST_TRACK";
		case Context::NONE:        return "NONE";
		default: ;
	}

	return "";
}


// Settings


Settings::Settings()
	: context_ { Context::ALBUM }
{
	// empty
}


Settings::Settings(const Context& c)
	: context_ { c }
{
	// empty
}


void Settings::set_context(const Context c)
{
	context_ = c;
}


Context Settings::context() const
{
	return context_;
}


// Algorithm


Algorithm::Algorithm()
	:settings_ { nullptr }
{
	// empty
}


Algorithm::~Algorithm() noexcept = default;


void Algorithm::set_settings(const Settings* s) noexcept
{
	settings_ = s;

	if (s)
	{
		do_setup(s);
	}
}


const Settings* Algorithm::settings() const noexcept
{
	return settings_;
}


std::pair<int32_t, int32_t> Algorithm::range(const AudioSize& size,
		const Points& points) const
{
	return this->do_range(size, points);
}


void Algorithm::update(SampleInputIterator start, SampleInputIterator stop)
{
	this->do_update(start, stop);
}


void Algorithm::track_finished(const int t, const AudioSize& length)
{
	this->do_track_finished(t, length);
}


ChecksumSet Algorithm::result() const
{
	return this->do_result();
}


ChecksumtypeSet Algorithm::types() const
{
	return this->do_types();
}


std::unique_ptr<Algorithm> Algorithm::clone() const
{
	return this->do_clone();
}


void Algorithm::base_swap(Algorithm& rhs)
{
	using std::swap;

	swap(settings_, rhs.settings_);
}


// Calculation::Impl


Calculation::Impl::Impl(std::unique_ptr<Algorithm> algorithm)
	: settings_      { Context::ALBUM /* just to have a default */ }
	, partitioner_   { nullptr /* requires concrete input data */  }
	, result_buffer_ { init_buffer()                               }
	, algorithm_     { std::move(algorithm)                        }
	, state_         { init_state(algorithm_.get())                }
{
	// empty
}


Calculation::Impl::Impl(const Impl& rhs)
	: settings_      { rhs.settings_                                    }
	, partitioner_   { rhs.partitioner_->clone()                        }
	, result_buffer_ { std::make_unique<Checksums>(*rhs.result_buffer_) }
	, algorithm_     { rhs.algorithm_->clone()                          }
	, state_         { rhs.state_->clone_to(algorithm_.get())           }
{
	// empty
}


Calculation::Impl& Calculation::Impl::operator=(const Impl& rhs)
{
	// FIXME Implement copy assignment without code duplication
	// see: http://www.gotw.ca/gotw/059.htm
	settings_      = rhs.settings_;
	partitioner_   = rhs.partitioner_->clone();
	result_buffer_ = std::make_unique<Checksums>(*rhs.result_buffer_);
	algorithm_     = rhs.algorithm_->clone();
	state_         = rhs.state_->clone_to(algorithm_.get());
	return *this;
}


Calculation::Impl::Impl(Impl&& rhs) noexcept
	: settings_      { std::move(rhs.settings_)      }
	, partitioner_   { std::move(rhs.partitioner_)   }
	, result_buffer_ { std::move(rhs.result_buffer_) }
	, algorithm_     { std::move(rhs.algorithm_)     }
	, state_         { std::move(rhs.state_)         } // FIXME pointer to algo
{
	// empty
}


Calculation::Impl& Calculation::Impl::operator=(Impl&& rhs) noexcept
{
	// FIXME Implement move assignment without code duplication
	// see: http://www.gotw.ca/gotw/059.htm
	settings_      = std::move(rhs.settings_);
	partitioner_   = std::move(rhs.partitioner_);
	result_buffer_ = std::move(rhs.result_buffer_);
	algorithm_     = std::move(rhs.algorithm_);
	state_         = std::move(rhs.state_); // FIXME pointer to algo
	return *this;
}


Calculation::Impl::~Impl() noexcept = default;


void Calculation::Impl::init(const Settings& s, const AudioSize& size,
		const Points& points)
{
	using details::SampleRange;

	this->set_settings(s); // also sets up Algorithm

	const auto interval { SampleRange { algorithm_->range(size, points) }};

	ARCS_LOG(DEBUG1) << "Calculation interval is " << interval.to_string();

	partitioner_ = details::make_partitioner(size, points, interval);
}


std::unique_ptr<details::CalculationStateImpl> Calculation::Impl::init_state(
		Algorithm* const algorithm)
{
	return std::make_unique<details::CalculationStateImpl>(algorithm);
}


std::unique_ptr<Checksums> Calculation::Impl::init_buffer()
{
	return std::make_unique<Checksums>();
}


void Calculation::Impl::set_settings(const Settings& s) noexcept
{
	settings_ = s;
	algorithm_->set_settings(&settings());
}


const Settings& Calculation::Impl::settings() const noexcept
{
	return settings_;
}


void Calculation::Impl::set_algorithm(std::unique_ptr<Algorithm> a) noexcept
{
	algorithm_     = std::move(a);
	state_         = init_state(algorithm_.get());
	result_buffer_ = init_buffer();
}


const Algorithm* Calculation::Impl::algorithm() const noexcept
{
	return algorithm_.get();
}


int32_t Calculation::Impl::samples_expected() const noexcept
{
	// Expected total number of input samples
	return partitioner_->total_samples();
}


int32_t Calculation::Impl::samples_processed() const noexcept
{
	return state_->samples_processed();
}


std::chrono::duration<float> Calculation::Impl::update_time_elapsed() const
	noexcept
{
	return state_->update_time_elapsed();
}


std::chrono::duration<float> Calculation::Impl::algo_time_elapsed() const
	noexcept
{
	return state_->algo_time_elapsed();
}


bool Calculation::Impl::complete() const noexcept
{
	return state_->current_offset() >= partitioner_->legal_range().upper();
}


void Calculation::Impl::update(SampleInputIterator start,
		SampleInputIterator stop)
{
	using fsec = std::chrono::duration<float>;
	using ms   = std::chrono::milliseconds;

	ARCS_LOG(DEBUG1) << "PROCESS BLOCK: START";

	const auto start_time { std::chrono::steady_clock::now() };

	const auto finished = bool {
		perform_update(start, stop, *partitioner_, *state_, *result_buffer_) };

	const auto stop_time  { std::chrono::steady_clock::now() };
	const fsec dur { stop_time - start_time }; // intentionally not auto
	// Type of the subtraction is high_resolution_clock::duration which is
	// not necessarily the same type as duration<float>.
	state_->increment_update_time_elapsed(dur);

	if (finished)
	{
		ARCS_LOG(DEBUG1) << "Last block completed, calculation finished";

		ARCS_LOG(DEBUG1) << "Total samples declared:  " <<
			partitioner_->total_samples();

		ARCS_LOG(DEBUG1) << "Total samples processed: " <<
			state_->samples_processed() <<
			" (== " << partitioner_->legal_range().to_string() << ")";

		const ms update_time =
			std::chrono::duration_cast<ms>(state_->update_time_elapsed());

		ARCS_LOG(DEBUG1) << "Milliseconds elapsed by calculating ARCSs: "
			<< update_time.count();

		const ms algo_time =
			std::chrono::duration_cast<ms>(state_->algo_time_elapsed());

		ARCS_LOG(DEBUG1) << "Milliseconds elapsed by Algorithm: "
			<< algo_time.count();
	}

	ARCS_LOG(DEBUG1) << "PROCESS BLOCK: END";
}


void Calculation::Impl::update(const AudioSize& audiosize)
{
	partitioner_->set_total_samples(audiosize.samples());
}


Checksums Calculation::Impl::result() const noexcept
{
	return *result_buffer_;
}


// Calculation


Calculation::Calculation(const Settings& settings,
		std::unique_ptr<Algorithm> algorithm, const AudioSize& size,
		const Points& points)
	:impl_ { std::make_unique<Impl>(std::move(algorithm)) }
{
	impl_->init(settings, size, points);
}


Calculation::Calculation(const Calculation& rhs)
	:impl_ { std::make_unique<Calculation::Impl>(*rhs.impl_) }
{
	// empty
}


Calculation& Calculation::operator=(const Calculation& rhs)
{
	impl_ = std::make_unique<Calculation::Impl>(*rhs.impl_);
	return *this;
}


Calculation::Calculation(Calculation&& rhs) noexcept
	:impl_ { std::move(rhs.impl_) }
{
	// empty
}


Calculation& Calculation::operator=(Calculation&& rhs) noexcept
{
	impl_ = std::move(rhs.impl_);
	return *this;
}


Calculation::~Calculation() noexcept = default;


void Calculation::set_settings(const Settings& s) noexcept
{
	impl_->set_settings(s);
}


Settings Calculation::settings() const noexcept
{
	return impl_->settings();
}


void Calculation::set_algorithm(std::unique_ptr<Algorithm> algorithm) noexcept
{
	impl_->set_algorithm(std::move(algorithm));
}


const Algorithm* Calculation::algorithm() const noexcept
{
	return impl_->algorithm();
}


ChecksumtypeSet Calculation::types() const noexcept
{
	return algorithm()->types();
}


int32_t Calculation::samples_expected() const noexcept
{
	return impl_->samples_expected();
}


int32_t Calculation::samples_processed() const noexcept
{
	return impl_->samples_processed();
}


int32_t Calculation::samples_todo() const noexcept
{
	return samples_expected() - samples_processed();
}


std::chrono::duration<float> Calculation::update_time_elapsed() const
	noexcept
{
	return impl_->update_time_elapsed();
}


std::chrono::duration<float> Calculation::algo_time_elapsed() const noexcept
{
	return impl_->algo_time_elapsed();
}


bool Calculation::complete() const noexcept
{
	return impl_->complete();
}


void Calculation::update(SampleInputIterator start, SampleInputIterator stop)
{
	impl_->update(start, stop);
}


void Calculation::update(const AudioSize& audiosize)
{
	impl_->update(audiosize);
}


Checksums Calculation::result() const noexcept
{
	return impl_->result();
}


// make_calculation


std::unique_ptr<Calculation> make_calculation(
		std::unique_ptr<Algorithm> algorithm, const ToC& toc)
{
	auto leadout = AudioSize{};

	if (toc.complete())
	{
		leadout = toc.leadout();
	}

	return std::make_unique<Calculation>(Context::ALBUM, std::move(algorithm),
		leadout, toc.offsets());
}


} // namespace v_1_0_0
} // namespace arcstk

