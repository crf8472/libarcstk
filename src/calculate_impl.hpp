#ifndef __LIBARCSTK_CALCULATE_IMPL_HPP__
#define __LIBARCSTK_CALCULATE_IMPL_HPP__
/**
 * \file
 *
 * \brief Implementations of calculate.hpp that depend on the public header.
 */

#include <cstdint>       // for uint32_t, int32_t
#include <memory>        // for unique_ptr
#include <unordered_map> // for unordered_map
#include <string>        // for string

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"  // for TOC
#endif

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{


/**
 * \brief Check whether \c value is within the bounds of \c unit..
 *
 * \param[in] value Value to convert
 * \param[in] unit  Unit of the value
 *
 * \throw std::underflow_error If value is negative
 * \throw std::overflow_error  If value is bigger than the unit maximum
 *
 * \return TRUE iff <tt>0 <= value <= AudioSize::max(unit)</tt>
 */
bool within_bounds(const int32_t value, const AudioSize::UNIT unit);

/**
 * \brief Convert \c value to the corrsponding number of bytes.
 *
 * \param[in] value Value to convert
 * \param[in] unit  Unit of the value
 *
 * \return The equivalent number of bytes.
 */
int32_t to_bytes(const int32_t value, const AudioSize::UNIT unit) noexcept;

/**
 * \brief Convert \c bytes to the specified \c unit.
 *
 * \param[in] bytes Amount of bytes
 * \param[in] unit  Unit to convert to
 *
 * \return The equivalent value in the specified unit
 */
int32_t from_bytes(const int32_t bytes, const AudioSize::UNIT unit) noexcept;

/**
 * \internal
 * \ingroup calc
 *
 * \brief Base class for CalcContext implementations.
 *
 * Provides the properties AudioSize and filename and implements
 * CalcContext::first_relevant_sample() as well as
 * CalcContext::last_relevant_sample().
 */
class CalcContextImplBase : virtual public CalcContext
{
	/**
	 * \brief Internal representation of the AudioSize of the current audiofile
	 */
	AudioSize audiosize_;

	/**
	 * \brief Internal representation of the audiofilename
	 */
	std::string filename_;

	/**
	 * \brief Number of samples to skip at beginning of first track if requested
	 */
	int32_t num_skip_front_;

	/**
	 * \brief Number of samples to skip at end of last track if requested
	 */
	int32_t num_skip_back_;


	// CalcContext

	std::string do_filename() const noexcept final;

	void do_set_filename(const std::string& filename) noexcept final;

	const AudioSize& do_audio_size() const noexcept final;

	void do_set_audio_size(const AudioSize& audio_size) final;

	int32_t do_first_relevant_sample(const TrackNo) const noexcept override;

	int32_t do_first_relevant_sample_no_parms() const noexcept override;

	int32_t do_last_relevant_sample(const TrackNo track) const noexcept
		override;

	int32_t do_last_relevant_sample_no_parms() const noexcept override;

	// do_skips_front()
	// do_skips_back()

	int32_t do_num_skip_front() const noexcept override;

	int32_t do_num_skip_back() const noexcept override;

	// do_is_multi_track()

	void do_notify_skips(const int32_t num_skip_front,
			const int32_t num_skip_back) noexcept override;

	// do_total_tracks()
	// do_track(const int32_t)
	// do_offset(const int)
	// do_length(const int)
	// do_id()
	// do_clone()
	// do_equals()


	// CalcContextImplBase

	/**
	 * \brief Hook called after set_audio_size() is finished.
	 */
	virtual void do_hook_post_set_audio_size();

protected:

	/**
	 * \brief Default destructor.
	 *
	 * This class is not intended to be used for polymorphical deletion.
	 */
	~CalcContextImplBase() noexcept override = default;

	/**
	 * \brief Base implementation for operator == in subclasses.
	 *
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE iff \c *this == \c rhs, otherwise FALSE
	 */
	bool base_equals(const CalcContextImplBase& rhs) const noexcept;

	/**
	 * \brief Base implementation for swap in subclasses.
	 *
	 * \param[in] rhs Right hand side of the comparison
	 */
	void base_swap(CalcContextImplBase& rhs) noexcept;

public:

	/**
	 * \brief Construct with skip configuration.
	 *
	 * \param[in] filename       The audio file to process
	 * \param[in] num_skip_front Amount of samples to skip at the beginning
	 * \param[in] num_skip_back  Amount of samples to skip at the end
	 */
	CalcContextImplBase(const std::string& filename,
			const int32_t num_skip_front,
			const int32_t num_skip_back);
};


/**
 * \internal
 * \ingroup calc
 *
 * \brief CalcContext for singletrack mode.
 *
 * A SingletrackCalcContext is a CalcContext derived from an actual filename
 * representing a single track.
 */
class SingletrackCalcContext final : public CalcContextImplBase
								   , public Comparable<SingletrackCalcContext>
{
	/**
	 * \brief State: indicates whether to skip the front samples
	 */
	bool skip_front_;

	/**
	 * \brief State: indicates whether to skip the back samples
	 */
	bool skip_back_;


	// CalcContext

	int32_t do_first_relevant_sample(const TrackNo track) const noexcept
		final;

	int32_t do_last_relevant_sample(const TrackNo track) const noexcept
		final;

	bool do_skips_front() const noexcept final;

	bool do_skips_back() const noexcept final;

	bool do_is_multi_track() const noexcept final;

	int do_total_tracks() const noexcept final;

	int do_track(const int32_t smpl) const noexcept final;

	lba_count_t do_offset(const int track) const noexcept final;

	lba_count_t do_length(const int track) const noexcept final;

	ARId do_id() const final;

	std::unique_ptr<CalcContext> do_clone() const noexcept final;

	bool do_equals(const CalcContext& rhs) const noexcept final;

public:

	/**
	 * \brief Constructor.
	 *
	 * No samples are skipped.
	 *
	 * \param[in] filename Name of the audio file
	 */
	explicit SingletrackCalcContext(const std::string& filename);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] filename   Name of the audio file
	 * \param[in] skip_front Indicate whether to skip samples at the beginning
	 * \param[in] skip_back  Indicate whether to skip samples at the end
	 */
	SingletrackCalcContext(const std::string& filename,
			const bool skip_front, const bool skip_back);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] filename   Name of the audio file
	 * \param[in] skip_front Indicate whether to skip samples at the beginning
	 * \param[in] num_skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Indicate whether to skip samples at the end
	 * \param[in] num_skip_back Amount of samples to skip at the end
	 */
	SingletrackCalcContext(const std::string& filename,
			const bool skip_front, const int32_t num_skip_front,
			const bool skip_back,  const int32_t num_skip_back);

	/**
	 * \brief Activate skipping of the first 2939 samples of the first track.
	 *
	 * \param[in] skip TRUE skips the first 2939 samples of the first track
	 */
	void set_skip_front(const bool skip) noexcept;

	/**
	 * \brief Activate skipping of the last 2940 samples of the last track.
	 *
	 * \param[in] skip TRUE skips the last 2940 samples of the last track
	 */
	void set_skip_back(const bool skip) noexcept;


	friend bool operator == (const SingletrackCalcContext& lhs,
			const SingletrackCalcContext& rhs) noexcept;

	friend void swap(SingletrackCalcContext& lhs,
			SingletrackCalcContext& rhs) noexcept;
};


/**
 * \internal
 * \ingroup calc
 *
 * \brief CalcContext for multitrack mode.
 *
 * A MultitrackCalcContext is a CalcContext derived from a TOC and an optional
 * actual filename. It always skips the front and back samples.
 */
class MultitrackCalcContext final : public CalcContextImplBase
								  , public Comparable<MultitrackCalcContext>
{
	/**
	 * \brief TOC representation
	 */
	TOC toc_;

	// CalcContext

	int32_t do_first_relevant_sample(const TrackNo track) const noexcept final;

	int32_t do_last_relevant_sample(const TrackNo track) const noexcept final;

	bool do_skips_front() const noexcept final;

	bool do_skips_back() const noexcept final;

	bool do_is_multi_track() const noexcept final;

	int do_total_tracks() const noexcept final;

	int do_track(const int32_t smpl) const noexcept final;

	lba_count_t do_offset(const int track) const noexcept final;

	lba_count_t do_length(const int track) const noexcept final;

	ARId do_id() const final;

	std::unique_ptr<CalcContext> do_clone() const noexcept final;

	bool do_equals(const CalcContext& rhs) const noexcept final;

	// CalcContextImplBase

	void do_hook_post_set_audio_size() final;

public:

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename if
	 * \c filename is not empty. No samples are skipped.
	 *
	 * \param[in] toc      Name of the TOC
	 * \param[in] filename Name of the file
	 */
	MultitrackCalcContext(const std::unique_ptr<TOC>& toc,
			const std::string& filename = details::EmptyString);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename if
	 * \c filename is not empty. No samples are skipped.
	 *
	 * \param[in] toc      Name of the TOC
	 * \param[in] filename Name of the file
	 */
	MultitrackCalcContext(const TOC& toc,
			const std::string& filename = details::EmptyString);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename if
	 * \c filename is not empty.
	 *
	 * \param[in] toc        Name of the TOC
	 * \param[in] skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Amount of samples to skip at the end
	 * \param[in] filename   Name of the file
	 */
	MultitrackCalcContext(const std::unique_ptr<TOC>& toc,
			const int32_t skip_front, const int32_t skip_back,
			const std::string& filename = details::EmptyString);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename if
	 * \c filename is not empty.
	 *
	 * \param[in] toc        Name of the TOC
	 * \param[in] skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Amount of samples to skip at the end
	 * \param[in] filename   Name of the file
	 */
	MultitrackCalcContext(const TOC& toc, const int32_t skip_front,
			const int32_t skip_back,
			const std::string& filename = details::EmptyString);

	/**
	 * \brief The TOC of the audio input file.
	 *
	 * \return The TOC information to use for the audio input
	 */
	const TOC& toc() const noexcept;

	/**
	 * \brief Set the TOC for the audio input.
	 *
	 * \param[in] toc The TOC information to use for the audio input
	 */
	void set_toc(const TOC& toc);


	friend bool operator == (const MultitrackCalcContext& lhs,
		const MultitrackCalcContext& rhs) noexcept;

	friend void swap(MultitrackCalcContext& lhs,
			MultitrackCalcContext& rhs) noexcept;
};


using ChecksumBuffer = std::unordered_map<TrackNo, ChecksumSet>;


/**
 * \brief Provides chunks of a block.
 */
class PartitionProvider
{
	const CalcContext* context_;
	const Partitioner* partitioner_;

public:

	PartitionProvider(const CalcContext& context,
			const Partitioner& partitioner)
		: context_     { &context }
		, partitioner_ { &partitioner }
	{
		// empty
	}

	/**
	 * \brief Provide a partitioning.
	 */
	Partitioning operator()(const int32_t s_offset, const int32_t s_total) const
	{
		return partitioner_->create_partitioning(s_offset, s_total,
			{
				context_->first_relevant_sample(),
				context_->last_relevant_sample()
			}
		);
	}
};


/**
 * \brief Updates a calculation.
 *
 * \tparam B Type of the iterator pointing to the start position
 * \tparam E Type of the iterator pointing to the stop position
 *
 * \param[in]     start         Iterator pointing to first sample in block
 * \param[in]     stop          Iterator pointing to last sample in block
 * \param[in]     last_sample   Index of the leadout or last sample expected
 * \param[in,out] state         Current calculation state
 * \param[in]     partitioner   Partition provider
 * \param[in,out] result_buffer Collect the results
 */
template<class B, class E>
void calc_update(B& start, E& stop, const int32_t last_sample,
		CalculationState& state,
		ChecksumBuffer& result_buffer,
		const PartitionProvider& partitioner)

{
	const auto samples_in_block     { std::distance(start, stop) };
	const auto last_sample_in_block {
		state.sample_offset() + samples_in_block - 1 };

	ARCS_LOG_DEBUG << "  Offset:  " << state.sample_offset() << " samples";
	ARCS_LOG_DEBUG << "  Size:    " << samples_in_block << " samples";
	ARCS_LOG_DEBUG << "  Indices: " <<
		state.sample_offset() << " - " << last_sample_in_block;

	// Create a partitioning following the track bounds in this block

	auto partitioning { partitioner(state.sample_offset(), samples_in_block) };

	ARCS_LOG_DEBUG << "  Partitions:  " << partitioning.size();

	const bool is_last_relevant_block {
		Interval<int32_t>(state.sample_offset(), last_sample_in_block).contains(
				last_sample)
	};


	// Update the CalcState with each partition in this partitioning

	auto partition_counter = uint16_t { 0 };
	auto relevant_samples_counter = int32_t { 0 };

	const auto start_time { std::chrono::steady_clock::now() };
	for (const auto& partition : partitioning)
	{
		++partition_counter;
		relevant_samples_counter += partition.size();

		//this->log_partition(partition_counter, partitioning.size(), partition);
		ARCS_LOG_DEBUG << "  Partition " << partition_counter << "/" <<
			partitioning.size();

		// Update the calculation state with the current partition/chunk

		state.update(start + partition.begin_offset(),
				stop + partition.end_offset());

		// If the current partition ends a track, save the ARCSs for this track

		if (partition.ends_track())
		{
			//state->save(partition.track());
			result_buffer.insert(partition.track(),
					{ state.current_value() });

			ARCS_LOG_DEBUG << "    Completed track: "
				<< std::to_string(partition.track());
		}
	}
	//smpl_offset += samples_in_block;
	state.increment_sample_offset(samples_in_block);
	const auto end_time { std::chrono::steady_clock::now() };


	// Do the logging

	ARCS_LOG_DEBUG << "  Number of relevant samples in this block: "
			<< relevant_samples_counter;

	{
		const auto block_time_elapsed {
			std::chrono::duration_cast<std::chrono::milliseconds>
				(end_time - start_time)
		};

		state.increment_proc_time_elapsed(block_time_elapsed);

		ARCS_LOG_DEBUG << "  Milliseconds elapsed by processing this block: "
			<<	block_time_elapsed.count();
	}


	if (is_last_relevant_block)
	{
		ARCS_LOG(DEBUG1) << "Calculation complete.";
		// ARCS_LOG(DEBUG1) << "Total samples counted:  " <<
		// 	this->samples_processed();
		// ARCS_LOG(DEBUG1) << "Total samples declared: " <<
		// 	this->samples_expected();
		ARCS_LOG(DEBUG1) << "Milliseconds elapsed by calculating ARCSs: "
			<< state.proc_time_elapsed().count();
	}
}

} // namespace details
} // namespace v_1_0_0
} // namespace arcstk

#endif

