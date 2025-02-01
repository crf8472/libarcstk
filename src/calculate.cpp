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

#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"      // for AudioSize, ToC, CDDA
#endif
#ifndef __LIBARCSTK_METADATA_CONV_HPP__
#include "metadata_conv.hpp" // for convert
#endif

#include <algorithm>   // for max, min, transform
#include <cstdint>     // for int32_t
#include <iostream>

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

	const auto start_of_track = b == 0 ? 0 : points[b - 1];
	const auto end_of_track   = points[b] - 1;

	// start of the first (and maybe only) partition
	const auto p0_lower { real_lower };

	// end of the first (and maybe only) partition
	const auto p0_upper { b < points.size()  // if not last track
		? std::min(end_of_track, real_upper)
		: real_upper
	};

	ARCS_LOG(DEBUG1) << "  Create front partition, b: " << b << ", " << p0_lower
		<< " - " << p0_upper;

	auto partitions = std::vector<Partition> {
		Partition { p0_lower, p0_upper,
			p0_lower == start_of_track || p0_lower == legal.lower(),
			p0_upper == end_of_track   || p0_upper == legal.upper(),
			static_cast<TrackNo>(b)
		}
	};

	// If the interval does not span over multiple tracks, we are done now.
	if (b == e) { return partitions; }

	// Add further partitions, this is just from point i to point i + 1.
	for (auto i { b }; i < e - 1; ++i)
	{
		ARCS_LOG(DEBUG1) << "  Create partition " << points[i] << " - "
			<< points[ i + 1 ];
		partitions.emplace_back(points[i], points[i + 1] - 1, true, true, i + 1);
	}

	const auto pN_lower { points[e - 1] };

	const auto pN_upper { e < points.size()
		? std::min(points[e] - 1, real_upper)
		: real_upper
	};

	ARCS_LOG(DEBUG1) << "  Create back partition, e: " << e << ", " <<
		pN_lower << " - " << pN_upper;

	// Add last partition: from the beginning of the track that contains the
	// upper bound to the real upper bound.
	partitions.emplace_back(pN_lower, pN_upper,
		true, //starts track?
		//(e == points.size() || pN_upper == points[e]), //ends track?
		pN_upper == points[e] - 1 || pN_upper == legal.upper(),
		static_cast<TrackNo>(e)
	);

	return partitions;
}


Partitioning get_partitioning(const SampleRange& interval,
		const SampleRange& legal)
{
	// Create a single partition spanning the entire block of samples,
	// but respect skipping samples at front or back.

	const auto partition_start { interval.contains(legal.lower())
		? legal.lower()
		: interval.lower()
	};

	const auto partition_end { interval.contains(legal.upper())
		? legal.upper()
		: interval.upper()
	};

	const auto begin_offset { partition_start - interval.lower()     };
	const auto end_offset   { partition_end   - interval.lower() + 1 };
	// XXX The +1 seems like an error

	ARCS_LOG(DEBUG1) << "  Convert interval to partition: "
		<< begin_offset << " - " << end_offset;

	return { Partition {
		{ begin_offset },
		{ end_offset   },
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
		/*first phys. sample in block*/ offset,
		/*last  phys. sample in block*/ offset + am2ind(total_samples_in_block)
		// -1 because the amount total_samples_in_block has to be converted to
		// the sample index of the last sample in the interval.
	};

	// If the sample block does not contain any relevant samples,
	// just return an empty partitioning.

	if (current_interval.upper() < legal_range().lower() ||
			current_interval.lower() > legal_range().upper())
	{
		ARCS_LOG(DEBUG1) << "  No relevant samples in this block, skip";
		return Partitioning {};
	}

	if (points_.empty())
	{
		return do_create_partitioning(current_interval, legal_range());
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


Partitioning TrackPartitioner::do_create_partitioning(
			const SampleRange& interval,
			const SampleRange& legal) const
{
	return get_partitioning(interval, legal);
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
	: current_offset_    { 0 }
	, samples_processed_ { 0 }
	, track_samples_processed_ { 0 }
	, tracks_processed_  { 0 }
	, proc_time_elapsed_ { std::chrono::duration<int64_t, std::milli>(0) }
	, algorithm_         { algorithm }
{
	// empty
}


CalculationState::CalculationState(const CalculationState& rhs)
	: current_offset_    { rhs.current_offset_    }
	, samples_processed_ { rhs.samples_processed_ }
	, track_samples_processed_ { rhs.track_samples_processed_ }
	, tracks_processed_  { rhs.tracks_processed_ }
	, proc_time_elapsed_ { rhs.proc_time_elapsed_ }
	, algorithm_         { rhs.algorithm_         }
{
	// empty
}


CalculationState::CalculationState(CalculationState&& rhs) noexcept
	: current_offset_    { std::move(rhs.current_offset_)    }
	, samples_processed_ { std::move(rhs.samples_processed_) }
	, track_samples_processed_ { std::move(rhs.track_samples_processed_) }
	, tracks_processed_  { std::move(rhs.tracks_processed_)  }
	, proc_time_elapsed_ { std::move(rhs.proc_time_elapsed_) }
	, algorithm_         { std::move(rhs.algorithm_)         }
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


std::chrono::milliseconds CalculationState::proc_time_elapsed() const noexcept
{
	return proc_time_elapsed_.value();
}


void CalculationState::increment_proc_time_elapsed(
		const std::chrono::milliseconds amount)
{
	proc_time_elapsed_.increment(amount);
}


void CalculationState::update(SampleInputIterator start,
		SampleInputIterator stop)
{
	const auto amount { std::distance(start, stop) };

	do_update(start, stop); // TODO try and update counter in catch

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
	swap(this->current_offset_,    rhs.current_offset_    );
	swap(this->samples_processed_, rhs.samples_processed_ );
	swap(this->track_samples_processed_, rhs.track_samples_processed_ );
	swap(this->tracks_processed_,  rhs.tracks_processed_  );
	swap(this->proc_time_elapsed_, rhs.proc_time_elapsed_ );
	swap(this->algorithm_,         rhs.algorithm_         );
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
	auto tmp { CalculationStateImpl(rhs) };

	using std::swap;
	swap(*this, tmp);
	return *this;
}


CalculationStateImpl& CalculationStateImpl::operator = (
		CalculationStateImpl&& rhs) noexcept
{
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


void perform_update(SampleInputIterator start, SampleInputIterator stop,
		const Partitioner& partitioner,
		CalculationState&  state,
		Checksums&         result_buffer)
{
	const auto start_pos        { state.current_offset() };
	const auto samples_in_block { std::distance(start, stop) };

	const auto last_sample_in_block { start_pos + am2ind(samples_in_block) };

	ARCS_LOG_DEBUG << "  Offset:  " << state.current_offset() << " samples";
	ARCS_LOG_DEBUG << "  Size:    " << samples_in_block       << " samples";
	ARCS_LOG_DEBUG << "  Indices: " << start_pos << " - " << last_sample_in_block;

	// Create a partitioning following the track bounds in this block

	const auto partitioning { partitioner.create_partitioning(
			start_pos, samples_in_block) };

	if (partitioning.empty())
	{
		ARCS_LOG_DEBUG << "  Skip block (" << samples_in_block << " samples).";
		state.advance(samples_in_block);
		return;
	} else
	{
		// If we skipped some samples at the beginning of the partition, advance
		// the state by this amount so that current_offset() will be correct on
		// subsequent call.
		const auto diff { partitioning.front().begin_offset() - start_pos };
		if (diff > 0)
		{
			ARCS_LOG_DEBUG << "  Skipped " << diff
				<< " samples at front, resync";
			state.advance(diff);
		}
	}

	ARCS_LOG_DEBUG << "  Partitions: " << partitioning.size();

	const bool is_last_relevant_block {
		SampleRange(start_pos, last_sample_in_block)
			.contains(partitioner.legal_range().upper()/* last rel. sample */)
	};

	// Update the state with each partition in this partitioning

	auto partition_counter = uint16_t { 0 };
	auto relevant_samples_counter = std::size_t { 0 };

	auto offset_first { 0 };
	auto offset_last  { 0 };

	const auto start_time { std::chrono::steady_clock::now() };

	for (const auto& partition : partitioning)
	{
		++partition_counter;
		relevant_samples_counter += partition.size();

		ARCS_LOG_DEBUG << "  PARTITION " << partition_counter << "/" <<
			partitioning.size();

		offset_first = partition.begin_offset() - start_pos;
		offset_last  = partition.end_offset()   - start_pos;

		ARCS_LOG_DEBUG << "    Samples " << start_pos + offset_first << " - "
			<< start_pos + offset_last
			<< " (Track " << partition.track() << ", "
			<< (partition.starts_track()
					? (partition.ends_track() ? "complete"  : "first part")
					: (partition.ends_track() ? "last part" : "mid part"))
			<< ")";
		ARCS_LOG_DEBUG << "    Samples total: " << partition.size();

		state.update(start + offset_first, start + offset_last + 1);
		// +1 because the "stop" sample will not be processed. As an
		// end()-iterator, the intended stop point has to be shifted behind the
		// last sample to pass.

		// If the current partition ends a track, save the ARCSs for this track
		if (partition.ends_track())
		{
			ARCS_LOG_DEBUG << "    Completed track: "
				<< std::to_string(partition.track());

			state.track_finished();
			//result_buffer.append(state.current_subtotal());
			result_buffer.push_back(state.current_subtotal());
		}
	}

	const auto block_time_elapsed {
		std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - start_time)
	};

	state.increment_proc_time_elapsed(block_time_elapsed);

	ARCS_LOG_DEBUG << "  Number of relevant samples in this block: "
			<< relevant_samples_counter;
	ARCS_LOG_DEBUG << "  Milliseconds elapsed by processing this block: "
			<<	state.proc_time_elapsed().count();

	if (is_last_relevant_block)
	{
		ARCS_LOG(DEBUG1) << "Calculation complete.";

		ARCS_LOG(DEBUG1) << "Total samples processed: " <<
			state.samples_processed() <<
			" (from " << partitioner.legal_range().lower() <<
			" to "    << partitioner.legal_range().upper() << ")";
		ARCS_LOG(DEBUG1) << "Total samples declared:  " <<
			partitioner.total_samples();
		ARCS_LOG(DEBUG1) << "Milliseconds elapsed by calculating ARCSs: " <<
			state.proc_time_elapsed().count();
	}
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
	, result_buffer_ { std::move(init_buffer())                    }
	, algorithm_     { std::move(algorithm)                        }
	, state_         { std::move(init_state(algorithm_.get()))     }
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
	this->set_settings(s); // also sets up Algorithm

	const auto interval { details::SampleRange {
		algorithm_->range(size, points)
	}};

	ARCS_LOG_DEBUG << "Calculation interval is [" << interval.lower() << "," <<
		interval.upper() << "]";

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


std::chrono::milliseconds Calculation::Impl::proc_time_elapsed() const noexcept
{
	return state_->proc_time_elapsed();
}


bool Calculation::Impl::complete() const noexcept
{
	return state_->current_offset() >= partitioner_->legal_range().upper();
}


void Calculation::Impl::update(SampleInputIterator start,
		SampleInputIterator stop)
{
	ARCS_LOG_DEBUG << "PROCESS BLOCK";

	perform_update(start, stop, *partitioner_, *state_, *result_buffer_);

	ARCS_LOG_DEBUG << "END BLOCK";
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


std::chrono::milliseconds Calculation::proc_time_elapsed() const noexcept
{
	return impl_->proc_time_elapsed();
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

	return std::make_unique<Calculation>(Context::ALBUM,
		std::move(algorithm),
		leadout, toc.offsets());
}


} // namespace v_1_0_0
} // namespace arcstk

