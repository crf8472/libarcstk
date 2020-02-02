#ifndef __LIBARCSTK_CALCULATE_HPP__
#define __LIBARCSTK_CALCULATE_HPP__

/**
 * \file
 *
 * \brief Public API for checksum calculation.
 *
 * An API to calculate different types of checksums for CDDA conforming audio
 * tracks.
 *
 * A Calculation represents a (stateful) concrete checksum calculation that
 * must be configured with a CalcContext specific to the input audio file
 * and a checksum::type that specifies the calculation algorithm. The input
 * of the audio file must be represented as a succession of iterable uint32_t
 * represented sample sequences and the Calculation is sequentially updated with
 * these sequences in order. After the last update, the Calculation
 * returns the calculation result on request. The calculated Checksums
 * are represented as an iterable aggregate of
 * @link arcstk::v_1_0_0::ChecksumSet ChecksumSets @endlink.
 *
 * CalcContext represents the per-file metadata information that is used
 * during calculation. Calculation computes checksums from a sequence of
 * sample blocks according to its current CalcContext.
 *
 * Checksums represent a calculation result for all requested checksum
 * types and all tracks of the audio input.
 *
 * ChecksumSet is a set of @link arcstk::v_1_0_0::Checksum Checksums @endlink
 * of different
 * @link arcstk::v_1_0_0::checksum::type checksum::types @endlink. It represents
 * the calculation result for a particular track.
 *
 * AudioSize represents the size of the audio data in a file in frames,
 * samples and bytes.
 *
 * PCMForwardIterator wraps the concrete iterator of the sample sequence so any
 * class with a compatible iterator can be used.
 *
 * InvalidAudioException is thrown if the audio input is insufficient or
 * invalid.
 */

#include <cstdint>
#include <cstddef>     // for size_t
#include <iterator>    // for forward_iterator_tag
#include <memory>
#include <stdexcept>   // for logic_error
#include <string>
#include <type_traits> // for is_same, decay_t
#include <utility>     // for declval

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif

/**
 * \brief libarcstk main namespace
 */
namespace arcstk
{

/**
 * \brief API version 1.0.0
 */
inline namespace v_1_0_0
{

/**
 * \defgroup calc Checksum Calculation
 * @{
 */


/**
 * \brief Uniform access to the size of the input audio information.
 *
 * Some decoders provide the number of frames, other the number of samples and
 * maybe in some situations just the number of bytes of the sample stream is
 * known. To avoid implementing the appropriate conversion for each decoder,
 * AudioSize provides an interface for uniform representation to this
 * information. Any of the informations provided will determine all of the
 * others.
 */
class AudioSize final
{
public: /* types */

	/**
	 * \brief Distinguish units for size declaration
	 */
	enum class UNIT
	{
		SAMPLES, FRAMES, BYTES
	};

public: /* functions */

	/**
	 * \brief Constructor.
	 */
	AudioSize() noexcept;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] unit  Unit for \c value
	 * \param[in] value Size value
	 */
	AudioSize(const UNIT unit, const uint32_t value) noexcept;

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs The AudioSize to copy
	 */
	AudioSize(const AudioSize &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs The AudioSize to move
	 */
	AudioSize(AudioSize &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~AudioSize() noexcept;

	/**
	 * \brief Set the 1-based index of the LBA leadout frame.
	 *
	 * This also determines the number of PCM samples and the number of
	 * PCM bytes.
	 *
	 * \param[in] leadout LBA leadout frame
	 */
	void set_leadout_frame(const uint32_t leadout) noexcept;

	/**
	 * \brief Return the LBA leadout frame.
	 *
	 * \return LBA leadout frame
	 */
	uint32_t leadout_frame() const noexcept;

	/**
	 * \brief Set the total number of 32 bit PCM samples.
	 *
	 * This also determines the leadout frame and the number of PCM bytes.
	 *
	 * \param[in] smpl_count Total number of 32 bit PCM samples
	 */
	void set_sample_count(const uint32_t smpl_count) noexcept;

	/**
	 * \brief Return the total number of 32 bit PCM samples.
	 *
	 * \return The total number of 32 bit PCM samples
	 */
	uint32_t sample_count() const noexcept;

	/**
	 * \brief Set the total number of bytes holding decoded 32 bit PCM samples
	 *
	 * This also determines the leadout frame and the number of 32 bit PCM
	 * samples.
	 *
	 * \param[in] byte_count Total number of bytes holding 32 bit PCM samples
	 */
	void set_pcm_byte_count(const uint64_t byte_count) noexcept;

	/**
	 * \brief Return the number of bytes holding 32 bit PCM samples.
	 *
	 * \return The total number of bytes holding 32 bit PCM samples
	 */
	uint64_t pcm_byte_count() const noexcept;

	/**
	 * \brief Return TRUE if the AudioSize is 0.
	 *
	 * \return TRUE if the AudioSize is 0
	 */
	bool null() const noexcept;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	AudioSize& operator = (AudioSize rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	AudioSize& operator = (AudioSize &&rhs) noexcept;

	/**
	 * \brief Equality.
	 *
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE iff \c this equals \c rhs, otherwise FALSE
	 */
	bool operator == (const AudioSize &rhs) const noexcept;

	/**
	 * \brief Inequality.
	 *
	 * \param[in] rhs Right hand side of the comparison
	 *
	 * \return TRUE iff not \c this == \c rhs, otherwise FALSE
	 */
	bool operator != (const AudioSize &rhs) const noexcept;


private:

	// forward declaration for AudioSize::Impl
	class Impl;

	/**
	 * \brief Private implementation of AudioSize.
	 */
	std::unique_ptr<AudioSize::Impl> impl_;
};


/**
 * \internal
 * \brief Implementation details of API version 1.0.0
 */
namespace details
{

/**
 * \internal
 *
 * \brief Get value_type of Iterator.
 *
 * \tparam Iterator Iterator type to test
 */
template<typename Iterator>
using it_value_type = std::decay_t<decltype(*std::declval<Iterator>())>;
// This is SFINAE compatible and respects bare pointers, which would not
// have been respected when using std::iterator_traits<Iterator>::value_type.
// Nonetheless I am not quite sure whether bare pointers indeed should be used
// in this context.


/**
 * \internal
 *
 * \brief Check a given Iterator whether it iterates over type T.
 *
 * \tparam Iterator Iterator type to test
 * \tparam T        Type to test for
 */
template<typename Iterator, typename T>
using is_iterator_over = std::is_same< it_value_type<Iterator>, T >;

} // namespace details


/**
 * \brief Type erasing interface for iterators over PCM 32 Bit samples.
 *
 * Type erasure class for iterators with the value_type uint32_t. It wraps the
 * concrete iterator to be passed for updating a \ref Calculation.
 */
class PCMForwardIterator final
{

public:

	/**
	 * \brief Iterator category is ForwardIterator.
	 */
	using iterator_category = std::forward_iterator_tag;

	/**
	 * \brief The type this iterator enumerates.
	 */
	using value_type        = uint32_t;

	/**
	 * \brief Same as value_type, *not* a reference type.
	 */
	using reference         = uint32_t;

	/**
	 * \brief Const pointer to the value_type.
	 */
	using pointer           = const uint32_t*;

	/**
	 * \brief Pointer difference type.
	 */
	using difference_type   = std::ptrdiff_t;


private:

	/// \cond UNDOC_FUNCTION_BODIES

	/**
	 * \brief Internal interface to the type-erased object.
	 */
	struct Concept
	{
		/**
		 * \brief Virtual default destructor.
		 */
		virtual ~Concept() noexcept
		= default;

		/**
		 * \brief Preincrements the iterator.
		 */
		virtual void preincrement()
		= 0;

		/**
		 * \brief Advances iterator by \c n positions
		 *
		 * \param[in] n Number of positions to advance
		 */
		virtual void advance(const uint32_t n)
		= 0;

		/**
		 * \brief Reference to the actual value under the iterator.
		 *
		 * \return Reference to actual value.
		 */
		virtual reference dereference() const
		= 0;

		/**
		 * \brief Returns TRUE if \c rhs is equal to the instance.
		 *
		 * Required by the equality operator.
		 *
		 * \param[in] rhs The instance to test for equality
		 *
		 * \return TRUE if \c rhs is equal to the instance, otherwise FALSE
		 */
		virtual bool equals(const void* rhs) const
		= 0;

		/**
		 * \brief Returns RTTI.
		 *
		 * \return Runtime type information of this instance
		 */
		virtual const std::type_info& type() const
		= 0;

		/**
		 * \brief Returns the address of the instance
		 *
		 * Required by the equality operator.
		 *
		 * \return Address of the instance
		 */
		virtual const void* pointer() const
		= 0;

		/**
		 * \brief Returns a deep copy of the instance
		 *
		 * \return A deep copy of the instance
		 */
		virtual std::unique_ptr<Concept> clone() const // for copy constructor
		= 0;
	};


	/**
	 * \brief Internal object representation
	 *
	 * \tparam Iterator The iterator type to wrap
	 */
	template<class Iterator>
	struct Model : Concept
	{
		explicit Model(Iterator iterator)
			: iterator_(iterator)
		{
			// empty
		}

		void preincrement() final
		{
			++iterator_;
		}

		void advance(const uint32_t n) final
		{
			std::advance(iterator_, n);
		}

		reference dereference() const final
		{
			return *iterator_;
		}

		bool equals(const void* rhs) const final
		{
			return iterator_ == static_cast<const Model*>(rhs)->iterator_;
		}

		const std::type_info& type() const final
		{
			return typeid(iterator_);
		}

		const void* pointer() const final
		{
			return this;
		}

		std::unique_ptr<Concept> clone() const final
		{
			return std::make_unique<Model>(*this);
		}

		private:

			/**
			 * \brief The type-erased iterator instance.
			 */
			Iterator iterator_;
	};

	/// \endcond


public:

	/**
	 * \brief Converting constructor.
	 *
	 * \tparam Iterator The iterator type to wrap
	 *
	 * \param[in] i Instance of an iterator over sample_type
	 */
	template <class Iterator,
			typename = std::enable_if_t<
				details::is_iterator_over<Iterator, sample_type>::value
			>
	>
	PCMForwardIterator(const Iterator &i)
		: object_(std::make_unique<Model<Iterator>>(std::move(i)))
	{
		// empty
	}

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	PCMForwardIterator(const PCMForwardIterator& rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	PCMForwardIterator(PCMForwardIterator&& rhs) noexcept;

	/**
	 * \brief Dereferences the iterator.
	 *
	 * \return A uint32_t sample, returned by value
	 */
	reference operator * () const; // required by ForwardIterator

	/**
	 * \brief Pre-increment iterator.
	 *
	 * \return Incremented iterator
	 */
	PCMForwardIterator& operator ++ (); // required by ForwardIterator

	/**
	 * \brief Post-increment iterator.
	 *
	 * \return Iterator representing the state befor the increment
	 */
	PCMForwardIterator operator ++ (int); // required by ForwardIterator

	/**
	 * \brief Returns TRUE if \c rhs is equal to the instance.
	 *
	 * \param[in] rhs The instance to test for equality
	 *
	 * \return TRUE if \c rhs is equal to the instance, otherwise FALSE
	 */
	bool operator == (const PCMForwardIterator& rhs) const;
	// required by ForwardIterator

	/**
	 * \brief Returns TRUE if \c rhs is not equal to the instance.
	 *
	 * \param[in] rhs The instance to test for inequality
	 *
	 * \return TRUE if \c rhs is not equal to the instance, otherwise FALSE
	 */
	bool operator != (const PCMForwardIterator& rhs) const;
	// required by ForwardIterator

	/**
	 * \brief Advance the iterator by a non-negative amount.
	 *
	 * \param[in] amount A non-negative amount to advance the iterator
	 *
	 * \return Iterator pointing to the position advanced by \c amount
	 */
	PCMForwardIterator operator + (const uint32_t amount) const;


private:

	/**
	 * \brief Internal representation of wrapped object
	 */
	std::unique_ptr<Concept> object_;
};


/**
 * \brief Interface for information about the current audio input.
 *
 * Computationally relevant data are TOC information as there is
 * track count, offsets, and leadout. Additionally, the filename is provided.
 *
 * Determines whether the calculation is single- or multitrack and whether it
 * has to skip samples on the beginning of the first or the end of the last
 * track.
 *
 * CalcContext also provides service methods for identifying the first and
 * last sample of a track relevant for computation or to get the track for a
 * given sample.
 */
class CalcContext
{

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~CalcContext() noexcept
	= 0;

	/**
	 * \brief Inform about the AudioSize of the current file.
	 *
	 * This contains the information about the leadout frame. This information
	 * must be known before Calculation::update is called on the last
	 * block.
	 *
	 * \param[in] audio_size AudioSize
	 */
	void set_audio_size(const AudioSize &audio_size);

	/**
	 * \brief Return the number of bytes of the PCM samples.
	 *
	 * \return The total number of bytes of the PCM samples
	 */
	const AudioSize& audio_size() const;

	/**
	 * \brief Set the name of the current audio file.
	 *
	 * \param[in] filename Name of the audio file that is to be processed
	 */
	void set_filename(const std::string &filename);

	/**
	 * \brief Name of current audio file.
	 *
	 * \return Name of the audio file that is currently processed
	 */
	std::string filename() const;

	/**
	 * \brief Convenience method: Total number of tracks.
	 *
	 * This number will aways be a non-negative integer up to \c 99 .
	 *
	 * Since the track count should be accessed in a uniform way, regardless
	 * whether we actually have multiple tracks or only one.
	 *
	 * Note that this is independent from <tt>is_multi_track()</tt>. A
	 * TOC containing only one track would have a CalcContext in
	 * which <tt>track_count()</tt> is \c 1 but <tt>is_multi_track()</tt> is
	 * TRUE. Method <tt>is_multi_track()</tt> specifies the processing mode
	 * while <tt>track_count()</tt> just provides information about the TOC.
	 *
	 * \return The number of tracks represented in this file
	 */
	uint8_t track_count() const;

	/**
	 * \brief Returns TRUE if this instances indicates a processing for multiple
	 * tracks on a single input file.
	 *
	 * If this is FALSE, no chunks will be available. Multitrack mode is
	 * activated by setting a TOC.
	 *
	 * \return TRUE if this context specifies multitrack mode, otherwise FALSE
	 */
	bool is_multi_track() const;

	/**
	 * \brief Service method: Get 0-based index of the first relevant sample of
	 * the specified 1-based track.
	 *
	 * Note that parameter \c track is 1-based, which means that
	 * \c first_relevant_sample(2) returns the last 0-based sample of track 2
	 * (and not track 3).
	 *
	 * \param[in] track The 1-based track number
	 *
	 * \return Index of the first sample contributing to the track's ARCS
	 */
	uint32_t first_relevant_sample(const TrackNo track) const;

	/**
	 * \brief Get 0-based index of the first sample to be counted in
	 * computation.
	 *
	 * Which sample is actually the first relevant one depends on the offset
	 * of the first track and whether samples in the beginning of the first
	 * track are to be skipped.
	 *
	 * Always equivalent with
	 * @link CalcContext::first_relevant_sample() first_relevant_sample(1) @endlink.
	 *
	 * \return Index of the first sample contributing to the first track's ARCS
	 */
	uint32_t first_relevant_sample() const;

	/**
	 * \brief Service method: Get 0-based index of the last relevant sample of
	 * the specified 1-based track.
	 *
	 * Note that parameter \c track is 1-based, which means that
	 * last_relevant_sample(2) returns the last 0-based sample of track 2
	 * (and not track 3).
	 *
	 * If no offsets are set, the output will always be identical to
	 * @link CalcContext::last_relevant_sample() last_relevant_sample() @endlink.
	 *
	 * For <tt>track == 0</tt>, the last sample of the offset before track 1 is
	 * returned. This may of course be 0 iff the offset of track 1 is 0.
	 *
	 * \param[in] track 1-based track number, accepts 0 as offset of track 1
	 *
	 * \return Index of last sample contributing to the specified track's ARCS
	 */
	uint32_t last_relevant_sample(const TrackNo track) const;

	/**
	 * \brief Get 0-based index of the last sample to be counted in computation.
	 *
	 * Which sample is actualley the last relevant one depends on whether
	 * samples in the end of the last track are to be skipped.
	 *
	 * Always equivalent with
	 * @link CalcContext::last_relevant_sample(const TrackNo track) const last_relevant_sample(this->track_count()) @endlink.
	 *
	 * \return Index of the last sample contributing to the last track's ARCS
	 */
	uint32_t last_relevant_sample() const;

	/**
	 * \brief Returns 1-based track number of the track containing the specified
	 * 0-based sample.
	 *
	 * If sample_count() is 0, the method will return 0 regardless of
	 * the actual value of \c smpl.
	 *
	 * If \c smpl is bigger than <tt>sample_count() - 1</tt>, the method will
	 * return an invalid high track number ("infinite").
	 *
	 * The user has therefore to check for the validity of the result by
	 * checking whether <tt>0 < result <= CDDA.MAX_TRACKCOUNT</tt>.
	 *
	 * \param[in] smpl The sample to get the track for
	 *
	 * \return Track number of the track containing sample \c smpl
	 */
	TrackNo track(const uint32_t smpl) const;

	/**
	 * \brief Return the offset of the specified 0-based track from the TOC.
	 *
	 * Note that <tt>offset(i) == toc().offset(i+1)</tt> for all
	 * <tt>i: 0 <= i < toc().track_count()</tt>.
	 *
	 * \param[in] track The 0-based track to get the offset for
	 *
	 * \return The offset for the specified 0-based track
	 */
	uint32_t offset(const uint8_t track) const;

	/**
	 * \brief Return the normalized length of the specified 0-based track.
	 *
	 * Note that <tt>length(i) == offset(i+2) - offset(i+1)</tt> for all
	 * <tt>i: 0 <= i < toc().track_count() - 1</tt>.
	 *
	 * For the last track <tt>t = toc().track_count() - 1</tt>, the value of
	 * <tt>length(t)</tt> may be \c 0 if the TOC is incomplete and the instance
	 * is not yet updated with the concrete AudioSize, otherwise it is
	 * <tt>length(t) == leadout_frame() - offset(t+1)</tt>.
	 *
	 * \param[in] track The 0-based track to get the length for
	 *
	 * \return The length for the specified 0-based track
	 */
	uint32_t length(const uint8_t track) const;

	/**
	 * \brief Return the ARId of the current medium, if known.
	 *
	 * The value returned will only be significant iff non-empty offsets and
	 * non-zero total PCM byte count are available. Otherwise the ARId
	 * returned will be \c empty() .
	 *
	 * \return The ARId of the current medium
	 */
	ARId id() const;

	/**
	 * \brief Returns TRUE iff this context will skip the first 2939 samples of
	 * the first track.
	 *
	 * \return TRUE iff context will signal to skip the first samples.
	 */
	bool skips_front() const;

	/**
	 * \brief Returns TRUE iff this context will skip the last 2940 samples
	 * (5 LBA frames) of the last track.
	 *
	 * \return TRUE iff context will signal to skip the last samples.
	 */
	bool skips_back() const;

	/**
	 * \brief Returns the amount of samples to skip at the beginning of the
	 * first track - or, in a single track scenario, once at the beginning.
	 *
	 * The skipping is already active if this instance skips_front().
	 *
	 * \return The number of samples to skip at the beginning of the first track
	 */
	uint32_t num_skip_front() const;

	/**
	 * \brief Returns the amount of samples to skip at the end of the last track
	 * - or, in a single track scenario, once at the end.
	 *
	 * The skipping is already active if this instance skips_back()
	 *
	 * \return The number of samples to skip at the end of the last track
	 */
	uint32_t num_skip_back() const;

	/**
	 * \brief Notifies the instance about configured skipping amounts at the
	 * beginning of the first track and the end of the last track.
	 *
	 * Whether actual skipping takes place can be determined by
	 * skips_front() and skips_back().
	 *
	 * \param[in] num_skip_front Actual amount of skipped samples at the beginning
	 * \param[in] num_skip_back  Actual amount of skipped samples at the end
	 */
	void notify_skips(const uint32_t num_skip_front,
			const uint32_t num_skip_back);

	/**
	 * \brief Clone this CalcContext object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return A clone of this instance
	 */
	std::unique_ptr<CalcContext> clone() const;


private:

	/**
	 * \brief Implements set_audio_size(const AudioSize &audio_size).
	 *
	 * \param[in] audio_size AudioSize
	 */
	virtual void do_set_audio_size(const AudioSize &audio_size)
	= 0;

	/**
	 * \brief Implements audio_size() const.
	 *
	 * \return The total number of bytes of the PCM samples
	 */
	virtual const AudioSize& do_audio_size() const
	= 0;

	/**
	 * \brief Implements set_filename(const std::string &filename).
	 *
	 * \param[in] filename Name of the audio file that is to be processed
	 */
	virtual void do_set_filename(const std::string &filename)
	= 0;

	/**
	 * \brief Implements filename() const.
	 *
	 * \return Name of the audio file that is currently processed
	 */
	virtual std::string do_filename() const
	= 0;

	/**
	 * \brief Implements track_count() const.
	 *
	 * \return The number of tracks represented in this file
	 */
	virtual uint8_t do_track_count() const
	= 0;

	/**
	 * \brief Implements is_multi_track() const.
	 *
	 * \return TRUE if this context specifies multitrack mode, otherwise FALSE
	 */
	virtual bool do_is_multi_track() const
	= 0;

	/**
	 * \brief Implements first_relevant_sample(const TrackNo track) const.
	 *
	 * \param[in] track The 1-based track number
	 *
	 * \return Index of the first sample contributing to the track's ARCS
	 */
	virtual uint32_t do_first_relevant_sample(const TrackNo track) const
	= 0;

	/**
	 * \brief Implements first_relevant_sample() const.
	 *
	 * \return Index of the first sample contributing to the first track's ARCS
	 */
	virtual uint32_t do_first_relevant_sample_0() const
	= 0;

	/**
	 * \brief Implements last_relevant_sample(const TrackNo track) const.
	 *
	 * \param[in] track 1-based track number, accepts 0 as offset of track 1
	 *
	 * \return Index of last sample contributing to the specified track's ARCS
	 */
	virtual uint32_t do_last_relevant_sample(const TrackNo track) const
	= 0;

	/**
	 * \brief Implements last_relevant_sample() const.
	 *
	 * \return Index of the last sample contributing to the last track's ARCS
	 */
	virtual uint32_t do_last_relevant_sample_0() const
	= 0;

	/**
	 * \brief Implements track(const uint32_t smpl) const.
	 *
	 * \param[in] smpl The sample to get the track for
	 *
	 * \return Track number of the track containing sample \c smpl
	 */
	virtual TrackNo do_track(const uint32_t smpl) const
	= 0;

	/**
	 * \brief Implements offset(const uint8_t track) const.
	 *
	 * \param[in] track The 0-based track to get the offset for
	 *
	 * \return The offset for the specified 0-based track
	 */
	virtual uint32_t do_offset(const uint8_t track) const
	= 0;

	/**
	 * \brief Implements length(const uint8_t track) const.
	 *
	 * \param[in] track The 0-based track to get the length for
	 *
	 * \return The length for the specified 0-based track
	 */
	virtual uint32_t do_length(const uint8_t track) const
	= 0;

	/**
	 * \brief Implements id() const.
	 *
	 * \return The ARId of the current medium
	 */
	virtual ARId do_id() const
	= 0;

	/**
	 * \brief Implements skips_front() const.
	 *
	 * \return TRUE iff context will signal to skip the first samples.
	 */
	virtual bool do_skips_front() const
	= 0;

	/**
	 * \brief Implements skips_back() const.
	 *
	 * \return TRUE iff context will signal to skip the last samples.
	 */
	virtual bool do_skips_back() const
	= 0;

	/**
	 * \brief Implements num_skip_front() const.
	 *
	 * \return The number of samples to skip at the beginning of the first track
	 */
	virtual uint32_t do_num_skip_front() const
	= 0;

	/**
	 * \brief Implements num_skip_back() const.
	 *
	 * \return The number of samples to skip at the end of the last track
	 */
	virtual uint32_t do_num_skip_back() const
	= 0;

	/**
	 * \brief Implements notify_skips(const uint32_t num_skip_front, const uint32_t num_skip_back).
	 *
	 * \param[in] num_skip_front Actual amount of skipped samples at the beginning
	 * \param[in] num_skip_back  Actual amount of skipped samples at the end
	 */
	virtual void do_notify_skips(const uint32_t num_skip_front, const uint32_t
		num_skip_back) = 0;

	/**
	 * \brief Implements clone() const.
	 *
	 * \return A clone of this instance
	 */
	virtual std::unique_ptr<CalcContext> do_clone() const
	= 0;
};


/**
 * \brief Create a CalcContext from an audio filename and two skip flags.
 *
 * The file will not be read, the filename is just declared. It is allowed to be
 * omitted and defaults to an empty std::string in this case.
 *
 * \param[in] skip_front    Tell wether to skip the front samples
 * \param[in] skip_back     Tell wether to skip the back samples
 * \param[in] audiofilename The name of the audiofile
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const bool &skip_front,
		const bool &skip_back,
		const std::string &audiofilename = std::string());


/**
 * \brief Create a CalcContext from an audio filename and a TOC.
 *
 * The file will not be read, the filename is just declared. It is allowed to be
 * omitted and defaults to an empty std::string in this case.
 *
 * \param[in] toc           The TOC to use
 * \param[in] audiofilename The name of the audiofile, empty be default
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const TOC &toc,
		const std::string &audiofilename = std::string());


/**
 * \copydoc make_context(const TOC&, const std::string &)
 */
std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC> &toc,
		const std::string &audiofilename = std::string());


/**
 * \brief Result of a Calculation
 */
class Checksums final
{

public: /* types */

	/**
	 * \brief Iterator for Checksums.
	 */
	using iterator = ChecksumSet*;

	/**
	 * \brief Const_iterator for Checksums.
	 */
	using const_iterator = const ChecksumSet*;

	/**
	 * \brief Size type for Checksums.
	 */
	using size_type = std::size_t;


public: /* methods */

	/**
	 * \brief Constructor.
	 *
	 * \param[in] size Number of elements
	 */
	explicit Checksums(size_type size);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs The Checksums to copy
	 */
	Checksums(const Checksums &rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs The Checksums to move
	 */
	Checksums(Checksums &&rhs) noexcept;

	/**
	 * \brief Returns a pointer to the first element.
	 *
	 * \return Pointer to the first element
	 */
	iterator begin();

	/**
	 * \brief Returns a pointer after the last element.
	 *
	 * \return Pointer after the last element
	 */
	iterator end();

	/**
	 * \brief Returns a pointer to the first element.
	 *
	 * \return Pointer to the first element
	 */
	const_iterator begin() const;

	/**
	 * \brief Returns a pointer after the last element.
	 *
	 * \return Pointer after the last element
	 */
	const_iterator end() const;

	/**
	 * \brief Returns a pointer to the first element.
	 *
	 * \return Pointer to the first element
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Returns a pointer after the last element.
	 *
	 * \return Pointer after the last element
	 */
	const_iterator cend() const;

	/**
	 * \brief Access element by its 0-based \c index .
	 *
	 * Legal range is from <tt>0</tt> to <tt>size - 1</tt>.
	 *
	 * \param[in] index The 0-based index of the element
	 *
	 * \return The element with index \c index
	 */
	ChecksumSet& operator [] (const size_type index);

	/**
	 * \brief Access element by its 0-based \c index .
	 *
	 * Legal range is from <tt>0</tt> to <tt>size - 1</tt>.
	 *
	 * \param[in] index The 0-based index of the element
	 *
	 * \return The element with index \c index
	 */
	const ChecksumSet& operator [] (const size_type index) const;

	/**
	 * \brief Return the number of elements.
	 *
	 * \return Number of elements
	 */
	size_type size() const;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Checksums& operator = (const Checksums &rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Checksums& operator = (Checksums &&rhs) noexcept;


private:

	/**
	 * \brief Implementation of the set.
	 */
	std::unique_ptr<ChecksumSet[]> sets_;

	/**
	 * \brief Number of elements.
	 */
	size_type size_;
};


/**
 * \brief Checksum calculation for a requested checksum::type.
 *
 * To calculate the checksum for a given entirety of samples, an appropriate
 * CalcContext must be set and, optionally, a request for the checksum
 * types to calculate.
 *
 * The CalcContext is optionally configured with a TOC (for multitrack
 * calculation) and configures the track bounds, if any. If the input is
 * processed as a single track, the CalcContext configures whether it is
 * the first or last track or an intermediate track.
 *
 * Requesting the checksum types determines whether ARCSs v1 only or ARCSs v1
 * and ARCSs v2 are calculated.
 *
 * After context and requested types are set, Calculation::update() can be
 * called for a each subsequent sample block until all samples declared in the
 * CalcContext have been processed. After the last call of
 * Calculation::update(), a call of Calculation::result() will return
 * the result of the calculation on the entire sample stream.
 *
 * Note that at least before processing the last block, the total number of
 * samples must be known to the Calculation instance. The instance will be
 * informed about this value either early in the process by the TOC, or,
 * if the TOC is incomplete, by a callback call of
 * Calculation::update_audiosize().
 */
class Calculation final
{

public:

	/**
	 * \brief Construct calculation for specified checksum type and context.
	 *
	 * \param[in] type The checksum type to calculate
	 * \param[in] ctx The context for this calculation
	 */
	Calculation(const checksum::type type, std::unique_ptr<CalcContext> ctx);

	/**
	 * \brief Construct calculation for specified context with
	 * checksum::type::ARCS2.
	 *
	 * \param[in] ctx The context for this calculation
	 */
	explicit Calculation(std::unique_ptr<CalcContext> ctx);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs The Calculation to copy
	 */
	Calculation(const Calculation& rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs The Calculation to move
	 */
	Calculation(Calculation &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~Calculation() noexcept;

	/**
	 * \brief Set the stream context for the current stream of samples.
	 *
	 * \param[in] context The CalcContext for this instance
	 */
	void set_context(std::unique_ptr<CalcContext> context);

	/**
	 * \brief Read the CalcContext of this instance.
	 *
	 * \return The CalcContext of this instance
	 */
	const CalcContext& context() const;

	/**
	 * \brief Returns current type.
	 *
	 * \return A disjunction of all requested types.
	 */
	checksum::type type() const;

	/**
	 * \brief Update the calculation with a sequence of samples.
	 *
	 * \param[in] begin Iterator pointing to the beginning of the sequence
	 * \param[in] end   Iterator pointing to the end of the sequence
	 */
	void update(PCMForwardIterator begin, PCMForwardIterator end);

	/**
	 * \brief Updates the instance with a new AudioSize.
	 *
	 * This must be done before the last call of Calculation::update().
	 *
	 * \param[in] audiosize The updated AudioSize
	 */
	void update_audiosize(const AudioSize &audiosize);

	/**
	 * \brief Returns TRUE iff this Calculation is completed, otherwise FALSE.
	 *
	 * FALSE indicates that the instance expects more updates. If the instance
	 * returns TRUE it is safe to call result().
	 *
	 * \return TRUE if the Calculation is completed, otherwise FALSE
	 */
	bool complete() const;

	/**
	 * \brief Returns the total number of expected PCM 32 bit samples.
	 *
	 * This is just for debugging.
	 *
	 * \return Total number of PCM 32 bit samples expected.
	 */
	int64_t samples_expected() const;

	/**
	 * \brief Returns the counter for PCM 32 bit samples.
	 *
	 * This is just for debugging.
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	int64_t samples_processed() const;

	/**
	 * \brief Returns the number of PCM 32 bit samples that is yet to process.
	 *
	 * This value is equivalent to samples_expected() - samples_processed().
	 *
	 * This is just for debugging.
	 *
	 * \return Total number of PCM 32 bit samples yet to process.
	 */
	int64_t samples_todo() const;

	/**
	 * \brief Acquire the resulting Checksums.
	 *
	 * \return The computed Checksums
	 */
	Checksums result() const;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Calculation& operator = (Calculation rhs);

	/**
	 * \brief Move assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Calculation& operator = (Calculation &&rhs) noexcept;


private:

	/**
	 * \brief Request a checksum type or set of checksum types.
	 *
	 * \param[in] type Type to request
	 */
	void set_type(const checksum::type type);


	// forward declaration for Calculation::Impl
	class Impl;

	/**
	 * \brief Private implementation of Calculation.
	 */
	std::unique_ptr<Calculation::Impl> impl_;
};


/**
 * \brief Reports invalid audio data.
 */
class InvalidAudioException final : public std::logic_error
{

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidAudioException(const std::string &what_arg);

	/**
	 * \brief Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidAudioException(const char *what_arg);
};

/** @} */


inline PCMForwardIterator::PCMForwardIterator(const PCMForwardIterator& rhs)
	: object_(rhs.object_->clone())
{
	// empty
}


inline PCMForwardIterator::PCMForwardIterator(PCMForwardIterator&& rhs) noexcept
	: object_(std::move(rhs.object_))
{
	// empty
}


inline PCMForwardIterator::reference PCMForwardIterator::operator * () const
{
	return object_->dereference();
}


inline PCMForwardIterator& PCMForwardIterator::operator ++ ()
{
	object_->preincrement();
	return *this;
}


inline PCMForwardIterator PCMForwardIterator::operator ++ (int)
{
	PCMForwardIterator prev_val(*this);
	object_->preincrement();
	return prev_val;
}


inline bool PCMForwardIterator::operator == (const PCMForwardIterator& rhs)
	const
{
	return object_->equals(rhs.object_->pointer());
}


inline bool PCMForwardIterator::operator != (const PCMForwardIterator& rhs)
	const
{
	return not (*this == rhs);
}


inline PCMForwardIterator PCMForwardIterator::operator + (const uint32_t amount)
	const
{
	PCMForwardIterator it(*this);
	it.object_->advance(amount);
	return it;
}

} // namespace v_1_0_0

} // namespace arcstk

#endif

