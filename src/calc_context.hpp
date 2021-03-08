#ifndef __LIBARCSTK_CALC_CONTEXT_HPP__
#define __LIBARCSTK_CALC_CONTEXT_HPP__

/**
 * \internal
 *
 * \file
 *
 * \brief Internal API for SingletrackCalcContexts and MultitrackCalcContexts
 */

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif

#include <string>                 // for string

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

/**
 * \internal
 * \ingroup calc
 * \brief Default argument for empty strings, avoid creating temporary objects
 */
const auto EmptyString = std::string { };

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
class CalcContextBase : virtual public arcstk::v_1_0_0::CalcContext
{
public:

	/**
	 * \brief Construct with skip configuration.
	 *
	 * \param[in] filename       The audio file to process
	 * \param[in] num_skip_front Amount of samples to skip at the beginning
	 * \param[in] num_skip_back  Amount of samples to skip at the end
	 */
	CalcContextBase(const std::string &filename,
			const sample_count_t num_skip_front,
			const sample_count_t num_skip_back);


private:

	void do_set_audio_size(const AudioSize &audio_size) noexcept override;

	const AudioSize& do_audio_size() const noexcept override;

	void do_set_filename(const std::string &filename) noexcept override;

	std::string do_filename() const noexcept override;

	// do_total_tracks()
	// do_is_multi_track()

	sample_count_t do_first_relevant_sample(const TrackNo) const noexcept
		override;

	sample_count_t do_first_relevant_sample_no_parms() const noexcept override;

	sample_count_t do_last_relevant_sample(const TrackNo track) const noexcept
		override;

	sample_count_t do_last_relevant_sample_no_parms() const noexcept override;

	// do_track(const sample_count_t)
	// do_offset(const int)
	// do_length(const int)
	// do_id()
	// do_skips_front()
	// do_skips_back()

	sample_count_t do_num_skip_front() const noexcept override;

	sample_count_t do_num_skip_back() const noexcept override;

	void do_notify_skips(const sample_count_t num_skip_front,
			const sample_count_t num_skip_back) noexcept override;

	// do_clone()

	/**
	 * \brief Hook called after set_audio_size() is finished.
	 */
	virtual void do_hook_post_set_audio_size() noexcept;


protected:

	/**
	 * \brief Default destructor.
	 *
	 * This class is not intended to be used for polymorphical deletion.
	 */
	~CalcContextBase() noexcept override;

	/**
	 * \brief Base implementation for operator == in subclasses.
	 *
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE iff \c *this == \c rhs, otherwise FALSE
	 */
	bool equals(const CalcContextBase &rhs) const noexcept;


private:

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
	sample_count_t num_skip_front_;

	/**
	 * \brief Number of samples to skip at end of last track if requested
	 */
	sample_count_t num_skip_back_;
};


// forward declaration for operator ==
class SingletrackCalcContext;

bool operator == (const SingletrackCalcContext &lhs,
		const SingletrackCalcContext &rhs) noexcept;


/**
 * \internal
 * \ingroup calc
 *
 * \brief CalcContext for singletrack mode.
 *
 * A SingletrackCalcContext is a CalcContext derived from an actual filename
 * representing a single track.
 */
class SingletrackCalcContext final : public CalcContextBase
								   , public Comparable<SingletrackCalcContext>
{

public:

	friend bool operator == (const SingletrackCalcContext &lhs,
		const SingletrackCalcContext &rhs) noexcept;

	/**
	 * \brief Constructor.
	 *
	 * No samples are skipped.
	 *
	 * \param[in] filename Name of the audio file
	 */
	explicit SingletrackCalcContext(const std::string &filename);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] filename   Name of the audio file
	 * \param[in] skip_front Indicate whether to skip samples at the beginning
	 * \param[in] skip_back  Indicate whether to skip samples at the end
	 */
	SingletrackCalcContext(const std::string &filename,
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
	SingletrackCalcContext(const std::string &filename,
			const bool skip_front, const sample_count_t num_skip_front,
			const bool skip_back,  const sample_count_t num_skip_back);

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


private:

	int do_total_tracks() const noexcept final;

	bool do_is_multi_track() const noexcept final;

	sample_count_t do_first_relevant_sample(const TrackNo track) const noexcept
		final;

	// do_first_relevant_sample_no_parms() is implemented in CalcContextBase

	sample_count_t do_last_relevant_sample(const TrackNo track) const noexcept
		final;

	// do_last_relevant_sample_no_parms() is implemented in CalcContextBase

	int do_track(const sample_count_t smpl) const noexcept final;

	lba_count_t do_offset(const int track) const noexcept final;

	lba_count_t do_length(const int track) const noexcept final;

	ARId do_id() const noexcept final;

	bool do_skips_front() const noexcept final;

	bool do_skips_back() const noexcept final;

	std::unique_ptr<CalcContext> do_clone() const noexcept final;

	/**
	 * \brief State: indicates whether to skip the front samples
	 */
	bool skip_front_;

	/**
	 * \brief State: indicates whether to skip the back samples
	 */
	bool skip_back_;
};


// forward declaration for operator ==
class MultitrackCalcContext;

bool operator == (const MultitrackCalcContext &lhs,
		const MultitrackCalcContext &rhs) noexcept;


/**
 * \internal
 * \ingroup calc
 *
 * \brief CalcContext for multitrack mode.
 *
 * A MultitrackCalcContext is a CalcContext derived from a TOC and an optional
 * actual filename. It always skips the front and back samples.
 */
class MultitrackCalcContext final : public CalcContextBase
								  , public Comparable<MultitrackCalcContext>
{
public:

	friend bool operator == (const MultitrackCalcContext &lhs,
		const MultitrackCalcContext &rhs) noexcept;

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename . No samples
	 * are skipped.
	 *
	 * \param[in] toc      Name of the TOC
	 * \param[in] filename Name of the file
	 */
	MultitrackCalcContext(const TOC &toc,
			const std::string &filename = EmptyString);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename . No samples
	 * are skipped.
	 *
	 * \param[in] toc      Name of the TOC
	 * \param[in] filename Name of the file
	 */
	MultitrackCalcContext(const std::unique_ptr<TOC> &toc,
			const std::string &filename = EmptyString);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename .
	 *
	 * \param[in] toc        Name of the TOC
	 * \param[in] skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Amount of samples to skip at the end
	 * \param[in] filename   Name of the file
	 */
	MultitrackCalcContext(const TOC &toc, const sample_count_t skip_front,
			const sample_count_t skip_back,
			const std::string &filename = EmptyString);

	/**
	 * \brief Constructor.
	 *
	 * The filenames in \c toc are ignored in favour of \c filename .
	 *
	 * \param[in] toc        Name of the TOC
	 * \param[in] skip_front Amount of samples to skip at the beginning
	 * \param[in] skip_back  Amount of samples to skip at the end
	 * \param[in] filename   Name of the file
	 */
	MultitrackCalcContext(const std::unique_ptr<TOC> &toc,
			const sample_count_t skip_front, const sample_count_t skip_back,
			const std::string &filename = EmptyString);

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
	void set_toc(const TOC &toc) noexcept;


private:

	void do_hook_post_set_audio_size() noexcept final;

	int do_total_tracks() const noexcept final;

	bool do_is_multi_track() const noexcept final;

	sample_count_t do_first_relevant_sample(const TrackNo track) const noexcept
		final;

	// do_first_relevant_sample_no_parms() is implemented in CalcContextBase

	sample_count_t do_last_relevant_sample(const TrackNo track) const noexcept
		final;

	// do_last_relevant_sample_no_parms() is implemented in CalcContextBase

	int do_track(const sample_count_t smpl) const noexcept final;

	lba_count_t do_offset(const int track) const noexcept final;

	lba_count_t do_length(const int track) const noexcept final;

	ARId do_id() const noexcept final;

	bool do_skips_front() const noexcept final;

	bool do_skips_back() const noexcept final;

	std::unique_ptr<CalcContext> do_clone() const noexcept final;

	/**
	 * \brief TOC representation
	 */
	TOC toc_;
};

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif

