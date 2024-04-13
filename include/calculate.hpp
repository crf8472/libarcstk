#ifndef __LIBARCSTK_CALCULATE_HPP__
#define __LIBARCSTK_CALCULATE_HPP__

/**
 * \file
 *
 * \brief Public API for \link calc calculating AccurateRip checksums\endlink.
 */

#include <cstddef>               // for ptrdiff_t, size_t
#include <cstdint>               // for uint32_t, int64_t
#include <initializer_list>      // for initializer_list
#include <iterator>              // for input_iterator_tag
#include <memory>                // for unique_ptr, swap, make_unique
#include <ostream>               // for ostream
#include <string>                // for string
#include <type_traits>           // for declval, decay_t, enable_if_t, is_same
#include <typeinfo>              // for type_info
#include <utility>               // for move


#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"
#endif
#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"
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

class ARId; // IWYU pragma keep
class TOC;  // IWYU pragma keep

/**
 * \defgroup calc AccurateRip Checksum Calculation
 *
 * \brief Public API for \link Calculation checksum calculation \endlink.
 *
 * An API to calculate different types of checksums for CDDA conforming audio
 * tracks.
 *
 * A Calculation represents a (stateful) concrete checksum calculation that
 * must be configured with a CalcContext specific to the input audio file
 * and a checksum::type that specifies the calculation algorithm. The input
 * of the audio file must be represented as a succession of iterable
 * @link arcstk::v_1_0_0::SampleSequence SampleSequences @endlink
 * and the Calculation is sequentially updated with these sequences in
 * order. After the last update, the Calculation returns the calculation result
 * on request. The calculated Checksums are represented as an iterable aggregate
 * of @link arcstk::v_1_0_0::ChecksumSet ChecksumSets @endlink.
 *
 * A Checksum refers to a particular track and a particular checksum::type.
 *
 * ChecksumSet is a set of @link arcstk::v_1_0_0::Checksum Checksums @endlink
 * of different @link arcstk::v_1_0_0::checksum::type checksum::types @endlink
 * of the same track.
 *
 * Checksums represent a calculation result for all requested checksum
 * types and all tracks of the audio input.
 * It is an aggregation of the
 * @link arcstk::v_1_0_0::ChecksumSet ChecksumSets @endlink for each track of
 * an respective audio input. Depending on the input, it can represent either
 * an entire album or a single track.
 *
 * Considering the input of a Calculation, a CalcContext represents the per-file
 * metadata information that is used during calculation. The CalcContext informs
 * the Calculation about technical aspects like the encoding of the samples and
 * the channel order, and as well about semantic aspects like the precise track
 * bounds and the total number of samples.
 *
 * SampleInputIterator wraps the concrete iterator of the input sample sequence
 * so any class with a compatible iterator can be used for generating the audio
 * input.
 *
 * AudioSize represents the size of the audio input in frames, samples or bytes.
 *
 * The CDDA constants provide some CDDA-related constants that are frequently
 * reused throughout the code.
 *
 * @{
 */

/**
 * \brief Type to represent amounts of PCM 32 bit samples.
 *
 * A signed integer of at least 32 bit length.
 *
 * The type is required to express the maximum sample count in a medium, which
 * is MAX_BLOCK_ADDRESS * SAMPLES_PER_FRAME == 264.599.412 samples.
 *
 * The type is intended to perform arithmetic operations on it.
 */
using sample_count_t = int32_t;


// forward declaration for operator ==
class AudioSize; // IWYU pragma keep

bool operator == (const AudioSize &lhs, const AudioSize &rhs) noexcept;

bool operator < (const AudioSize &lhs, const AudioSize &rhs) noexcept;

/**
 * \brief Uniform access to the size of the input audio information.
 *
 * Some decoders provide the number of frames, others the number of samples and
 * maybe in some situations just the number of bytes of the sample stream is
 * known. To avoid implementing the appropriate conversion for each decoder,
 * AudioSize provides an interface for uniform representation to this
 * information. Any of the informations provided will determine all of the
 * others.
 */
class AudioSize final : public TotallyOrdered<AudioSize>
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

	friend bool operator == (const AudioSize &lhs, const AudioSize &rhs)
		noexcept;

	/**
	 * \brief Constructor.
	 */
	AudioSize() noexcept;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] value Size value
	 * \param[in] unit  Unit for \c value
	 */
	AudioSize(const long int value, const UNIT unit) noexcept;

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
	 * This also determines the total number of PCM samples and the number of
	 * PCM bytes.
	 *
	 * \param[in] leadout LBA leadout frame
	 */
	void set_leadout_frame(const lba_count_t leadout) noexcept;

	/**
	 * \brief Return the LBA leadout frame.
	 *
	 * \return LBA leadout frame
	 */
	lba_count_t leadout_frame() const noexcept;

	/**
	 * \brief Set the total number of 32 bit PCM samples.
	 *
	 * This also determines the leadout frame and the number of PCM bytes.
	 *
	 * \param[in] smpl_count Total number of 32 bit PCM samples
	 */
	void set_total_samples(const sample_count_t smpl_count) noexcept;

	/**
	 * \brief Return the total number of 32 bit PCM samples.
	 *
	 * \return The total number of 32 bit PCM samples
	 */
	sample_count_t total_samples() const noexcept;

	/**
	 * \brief Set the total number of bytes holding decoded 32 bit PCM samples
	 *
	 * This also determines the leadout frame and the total number of 32 bit PCM
	 * samples.
	 *
	 * \param[in] byte_count Total number of bytes holding 32 bit PCM samples
	 */
	void set_total_pcm_bytes(const uint32_t byte_count) noexcept;

	/**
	 * \brief Return the total number of bytes holding 32 bit PCM samples.
	 *
	 * \return The total number of bytes holding 32 bit PCM samples
	 */
	uint32_t total_pcm_bytes() const noexcept;

	/**
	 * \brief Return \c TRUE if the AudioSize is 0.
	 *
	 * \return \c TRUE if the AudioSize is 0
	 */
	bool null() const noexcept; // TODO Should be named 'zero'


	AudioSize& operator = (AudioSize rhs);

	AudioSize& operator = (AudioSize &&rhs) noexcept;


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
 * \brief Type to represent a 32 bit PCM stereo sample.
 *
 * An unsigned integer of 32 bit length.
 *
 * The type is not intended to do arithmetic operations on it.
 *
 * Bitwise operators are required to work as on unsigned types.
 */
using sample_t = uint32_t;


/**
 * \internal
 * \brief Defined iff \c Iterator is an iterator over \c sample_t.
 *
 * \tparam Iterator Iterator type to test
 */
template<typename Iterator>
using IsSampleIterator = std::enable_if_t<
		details::is_iterator_over<Iterator, sample_t>::value>;


// forward declaration for operator == and binary ops
class SampleInputIterator; // IWYU pragma keep

bool operator == (const SampleInputIterator &lhs,
		const SampleInputIterator &rhs) noexcept;

SampleInputIterator operator + (SampleInputIterator lhs,
		const sample_count_t amount) noexcept;

/**
 * \brief Type erasing interface for iterators over PCM 32 bit samples.
 *
 * Wraps the concrete iterator to be passed to
 * \link Calculation::update() update \endlink a Calculation.
 * This allows to pass in fact iterators of any type to a Calculation.
 *
 * SampleInputIterator can wrap any iterator with a value_type of uint32_t
 * except instances of itself, e.g. it can not be "nested".
 *
 * The type erasure interface only ensures that (most of) the requirements of a
 * <A HREF="https://en.cppreference.com/w/cpp/named_req/InputIterator">
 * LegacyInputIterator</A> are met. Those requirements are sufficient for
 * \link Calculation::update() updating \endlink a Calculation.
 *
 * Although SampleInputIterator is intended to provide the functionality of
 * an input iterator, it does not provide operator->() and does
 * therefore not completely fulfill the requirements for a LegacyInputIterator.
 *
 * SampleInputIterator provides iteration over values of type
 * \link arcstk::v_1_0_0::sample_t sample_t\endlink which is defined as a
 * primitve type. Since samples therefore do not have members, operator -> would
 * not provide any reasonable function.
 *
 * \see Calculation::update()
 */
class SampleInputIterator final : public Comparable<SampleInputIterator>
{
public:

	friend bool operator == (const SampleInputIterator &lhs,
			const SampleInputIterator &rhs) noexcept;

	friend SampleInputIterator operator + (SampleInputIterator lhs,
			const sample_count_t amount) noexcept;

	/**
	 * \brief Iterator category is std::input_iterator_tag.
	 */
	using iterator_category = std::input_iterator_tag;

	/**
	 * \brief The type this iterator enumerates.
	 */
	using value_type = sample_t;

	/**
	 * \brief Same as value_type, *not* a reference type.
	 */
	using reference = sample_t;

	/**
	 * \brief Defined as void due to absence of operator ->.
	 */
	using pointer = void;
	// Note: Should be const value_type* when operator-> is provided

	/**
	 * \brief Pointer difference type.
	 */
	using difference_type = std::ptrdiff_t;


private:

	/// \cond IGNORE_DOCUMENTATION_FOR_THE_FOLLOWING

	/**
	 * \internal
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
		virtual void preincrement() noexcept
		= 0;

		/**
		 * \brief Advances iterator by \c n positions
		 *
		 * \param[in] n Number of positions to advance
		 */
		virtual void advance(const sample_count_t n) noexcept
		= 0;

		/**
		 * \brief Reference to the actual value under the iterator.
		 *
		 * \return Reference to actual value.
		 */
		virtual reference dereference() noexcept
		= 0;

		/**
		 * \brief Returns \c TRUE if \c rhs is equal to the instance.
		 *
		 * \param[in] rhs The instance to test for equality
		 *
		 * \return \c TRUE if \c rhs is equal to the instance, otherwise \c FALSE
		 */
		virtual bool equals(const Concept &rhs) const noexcept
		= 0;

		/**
		 * \brief Returns RTTI.
		 *
		 * \return Runtime type information of this instance
		 */
		virtual const std::type_info& type() const noexcept
		= 0;

		/**
		 * \brief Returns a deep copy of the instance
		 *
		 * \return A deep copy of the instance
		 */
		virtual std::unique_ptr<Concept> clone() const noexcept
		= 0;
	};


	/**
	 * \internal
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

		void preincrement() noexcept final
		{
			++iterator_;
		}

		void advance(const sample_count_t n) noexcept final
		{
			std::advance(iterator_, n);
		}

		reference dereference() noexcept final
		{
			return *iterator_;
		}

		bool equals(const Concept &rhs) const noexcept final
		{
			return iterator_ == static_cast<const Model&>(rhs).iterator_;
		}

		const std::type_info& type() const noexcept final
		{
			return typeid(iterator_);
		}

		std::unique_ptr<Concept> clone() const noexcept final
		{
			return std::make_unique<Model>(*this);
		}

		friend void swap(Model &lhs, Model &rhs) noexcept
		{
			using std::swap;

			swap(lhs.iterator_, rhs.iterator_);
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
	 * \param[in] i Instance of an iterator over \c sample_t
	 */
	template <class Iterator, typename = IsSampleIterator<Iterator> >
	SampleInputIterator(const Iterator &i)
		: object_ { std::make_unique<Model<Iterator>>(std::move(i)) }
	{
		// empty
	}

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	SampleInputIterator(const SampleInputIterator& rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	SampleInputIterator(SampleInputIterator&& rhs) noexcept;

	/**
	 * \brief Destructor
	 */
	~SampleInputIterator() noexcept;

	/**
	 * \brief Dereferences the iterator to the sample pointed to.
	 *
	 * \return A sample_t sample, returned by value
	 */
	reference operator * () const noexcept; // required by LegacyIterator

	/* *
	 * \brief Access members of the underlying referee
	 *
	 * \return A pointer to the underlying referee
	 */
	pointer operator -> () const noexcept; // required by LegacyInpuIterator

	/**
	 * \brief Pre-increment iterator.
	 *
	 * \return Incremented iterator
	 */
	SampleInputIterator& operator ++ () noexcept; // required by LegacyIterator

	/**
	 * \brief Post-increment iterator.
	 *
	 * \return Iterator representing the state befor the increment
	 */
	SampleInputIterator operator ++ (int) noexcept;
	// required by LegacyInputIterator


	SampleInputIterator& operator = (SampleInputIterator rhs) noexcept;
	// required by LegacyIterator

	friend void swap(SampleInputIterator &lhs, SampleInputIterator &rhs)
		noexcept
	{
		using std::swap;

		swap(lhs.object_, rhs.object_);
	} // required by LegacyIterator


private:

	/**
	 * \brief Internal representation of wrapped object
	 */
	std::unique_ptr<Concept> object_;
};


SampleInputIterator operator + (const sample_count_t amount,
		SampleInputIterator rhs) noexcept;


/**
 * \brief Interface for information about the current audio input.
 *
 * CalcContext provides the metadata that is required to perform the Calculation
 * correctly.
 *
 * The CalcContext determines whether the input is single- or multitrack and
 * whether it has to skip samples on the beginning of the first or the end of
 * the last track.
 *
 * CalcContext also provides service methods for identifying the first and
 * last sample of a track relevant for Calculation or to get the track for a
 * given sample.
 *
 * CalcContext offers also access to its underlying TOC data.
 *
 * CalcContext instances are created exclusively via the variants of
 * make_context().
 *
 * A CalcContext is intended as a "read-only" object, it should never be
 * required to modify the CalcContext of an audio input. The only exception
 * is notify_skips() which depends on the concrete algorithm that is only
 * known within Calculation. Thus, Calculation must inform its CalcContext about
 * it.
 *
 * \note
 * Currently it should not be required for the user to subclass CalcContext. All
 * subclasses required for calculating ARCSv1 and ARCSv2 in single- and
 * multitrack scenarios are already provided by libarcstk. Note that subclassing
 * CalcContext would require to also add a new variant of make_context() that
 * can yield an instance of this subclass.
 */
class CalcContext
{
public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~CalcContext() noexcept
	= default;

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
	 * \brief Return the size of the referenced audio file
	 *
	 * \return The size of the referenced audio file
	 */
	const AudioSize& audio_size() const noexcept;

	/**
	 * \brief Set the name of the current audio file.
	 *
	 * \param[in] filename Name of the audio file that is to be processed
	 */
	void set_filename(const std::string &filename) noexcept;

	/**
	 * \brief Name of current audio file.
	 *
	 * \return Name of the audio file that is currently processed
	 */
	std::string filename() const noexcept;

	/**
	 * \brief Convenience method: Total number of tracks.
	 *
	 * This number will aways be a non-negative integer up to \c 99 .
	 *
	 * Since the track count should be accessed in a uniform way, regardless
	 * whether we actually have multiple tracks or only one.
	 *
	 * \attention
	 * Note that this is independent from <tt>is_multi_track()</tt>. A
	 * TOC containing only one track would have a CalcContext in
	 * which <tt>total_tracks()</tt> is \c 1 but <tt>is_multi_track()</tt> is
	 * \c TRUE. Method <tt>is_multi_track()</tt> specifies the processing mode
	 * while <tt>total_tracks()</tt> just provides information about the TOC.
	 *
	 * \return The number of tracks represented in this file
	 */
	int total_tracks() const noexcept;

	/**
	 * \brief Returns \c TRUE if this instances indicates a processing for multiple
	 * tracks on a single input file.
	 *
	 * If this is \c FALSE, no chunks will be available. Multitrack mode is
	 * activated by setting a TOC.
	 *
	 * \attention
	 * Note that this is independent from <tt>total_tracks()</tt>. A
	 * TOC containing only one track would have a CalcContext in
	 * which <tt>total_tracks()</tt> is \c 1 but <tt>is_multi_track()</tt> is
	 * \c TRUE. Method <tt>is_multi_track()</tt> specifies the processing mode
	 * while <tt>total_tracks()</tt> just provides information about the TOC.
	 *
	 * \return \c TRUE if this context specifies multitrack mode, otherwise \c FALSE
	 */
	bool is_multi_track() const noexcept;

	/**
	 * \brief Get 0-based index of the first relevant sample of the specified
	 * 1-based track.
	 *
	 * Note that parameter \c track is 1-based, which means that
	 * \c first_relevant_sample(2) returns the first 0-based sample of the
	 * actual track 2 (and not track 3).
	 *
	 * \param[in] track The 1-based track number
	 *
	 * \return Index of the first sample contributing to the track's ARCS
	 */
	sample_count_t first_relevant_sample(const TrackNo track) const noexcept;

	/**
	 * \brief Get 0-based index of the first relevant sample of the current
	 * audio input.
	 *
	 * Which sample is actually the first relevant one depends on
	 * <ul>
	 *   <li>the offset of the first track and</li>
	 *   <li>whether samples in the beginning of the first track are to be
	 *       skipped.</li>
	 * </ul>
	 *
	 * Always equivalent with
	 * @link CalcContext::first_relevant_sample() first_relevant_sample(1) @endlink.
	 *
	 * \return Index of the first sample contributing to the first track's ARCS
	 */
	sample_count_t first_relevant_sample() const noexcept;

	/**
	 * \brief Get 0-based index of the last relevant sample of the specified
	 * 1-based track.
	 *
	 * Note that parameter \c track is 1-based, which means that
	 * last_relevant_sample(2) returns the last 0-based sample of the actual
	 * track 2 (and not track 3).
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
	sample_count_t last_relevant_sample(const TrackNo track) const noexcept;

	/**
	 * \brief Get 0-based index of the last relevant sample of the current
	 * audio input.
	 *
	 * Which sample is actually the last relevant one depends on whether
	 * samples in the end of the last track are to be skipped.
	 *
	 * Always equivalent with
	 * @link CalcContext::last_relevant_sample(const TrackNo track) const last_relevant_sample(this->total_tracks()) @endlink.
	 *
	 * \return Index of the last sample contributing to the last track's ARCS
	 */
	sample_count_t last_relevant_sample() const noexcept;

	/**
	 * \brief Returns 1-based track number of the track containing the specified
	 * 0-based sample.
	 *
	 * If total_samples() is 0, the method will return 0 regardless of
	 * the actual value of \c smpl.
	 *
	 * If \c smpl is bigger than <tt>total_samples() - 1</tt>, the method will
	 * return an invalid high track number ("infinite").
	 *
	 * The user has therefore to check for the validity of the result by
	 * checking whether <tt>0 < result <= CDDA::MAX_TRACKCOUNT</tt>.
	 *
	 * \param[in] smpl The sample to get the track for
	 *
	 * \return Track number of the track containing sample \c smpl
	 */
	int track(const sample_count_t smpl) const noexcept;

	/**
	 * \brief Return the offset of the specified 0-based track from the TOC.
	 *
	 * Note that <tt>offset(i) == toc().offset(i+1)</tt> for all
	 * <tt>i: 0 <= i < toc().total_tracks()</tt>.
	 *
	 * \param[in] track The 0-based track to get the offset for
	 *
	 * \return The offset for the specified 0-based track
	 */
	lba_count_t offset(const int track) const noexcept;

	/**
	 * \brief Return the normalized length of the specified 0-based track.
	 *
	 * Note that <tt>length(i) == offset(i+2) - offset(i+1)</tt> for all
	 * <tt>i: 0 <= i < toc().total_tracks() - 1</tt>.
	 *
	 * For the last track <tt>t = toc().total_tracks() - 1</tt>, the value of
	 * <tt>length(t)</tt> may be \c 0 if the TOC is incomplete and the instance
	 * is not yet updated with the concrete AudioSize, otherwise it is
	 * <tt>length(t) == leadout_frame() - offset(t+1)</tt>.
	 *
	 * \param[in] track The 0-based track to get the length for
	 *
	 * \return The length for the specified 0-based track
	 */
	lba_count_t length(const int track) const noexcept;

	/**
	 * \brief Return the ARId of the current medium, if known.
	 *
	 * The value returned will only be significant iff non-empty offsets and
	 * non-zero total PCM byte count are available. Otherwise the ARId
	 * returned will be \c empty() .
	 *
	 * \return The ARId of the current medium
	 */
	ARId id() const noexcept;

	/**
	 * \brief Returns \c TRUE iff this context will skip the first 2939 samples of
	 * the first track.
	 *
	 * \return \c TRUE iff context will signal to skip the first samples.
	 */
	bool skips_front() const noexcept;

	/**
	 * \brief Returns \c TRUE iff this context will skip the last 2940 samples
	 * (5 LBA frames) of the last track.
	 *
	 * \return \c TRUE iff context will signal to skip the last samples.
	 */
	bool skips_back() const noexcept;

	/**
	 * \brief Returns the amount of samples to skip at the beginning of the
	 * first track.
	 *
	 * If the audio input contains only one track, this track is the first track.
	 *
	 * The value is either 2939 or 0.
	 *
	 * The skipping is already active if this instance skips_front().
	 *
	 * \return The number of samples to skip at the beginning of the first track
	 */
	sample_count_t num_skip_front() const noexcept;

	/**
	 * \brief Returns the amount of samples to skip at the end of the last
	 * track.
	 *
	 * If the audio input contains only one track, this track is the last track.
	 *
	 * The value is either 2940 or 0.
	 *
	 * The skipping is already active if this instance skips_back().
	 *
	 * \return The number of samples to skip at the end of the last track
	 */
	sample_count_t num_skip_back() const noexcept;

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
	void notify_skips(const sample_count_t num_skip_front,
			const sample_count_t num_skip_back) noexcept;

	/**
	 * \brief Clone this CalcContext object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return A clone of this instance
	 */
	std::unique_ptr<CalcContext> clone() const noexcept;

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
	virtual const AudioSize& do_audio_size() const noexcept
	= 0;

	/**
	 * \brief Implements set_filename(const std::string &filename).
	 *
	 * \param[in] filename Name of the audio file that is to be processed
	 */
	virtual void do_set_filename(const std::string &filename) noexcept
	= 0;

	/**
	 * \brief Implements filename() const.
	 *
	 * \return Name of the audio file that is currently processed
	 */
	virtual std::string do_filename() const noexcept
	= 0;

	/**
	 * \brief Implements total_tracks() const.
	 *
	 * \return The number of tracks represented in this file
	 */
	virtual int do_total_tracks() const noexcept
	= 0;

	/**
	 * \brief Implements is_multi_track() const.
	 *
	 * \return \c TRUE if this context specifies multitrack mode, otherwise \c FALSE
	 */
	virtual bool do_is_multi_track() const noexcept
	= 0;

	/**
	 * \brief Implements first_relevant_sample(const TrackNo track) const.
	 *
	 * \param[in] track The 1-based track number
	 *
	 * \return Index of the first sample contributing to the track's ARCS
	 */
	virtual sample_count_t do_first_relevant_sample(const TrackNo track) const
	noexcept
	= 0;

	/**
	 * \brief Implements first_relevant_sample() const.
	 *
	 * \return Index of the first sample contributing to the first track's ARCS
	 */
	virtual sample_count_t do_first_relevant_sample_no_parms() const noexcept
	= 0;

	/**
	 * \brief Implements last_relevant_sample(const TrackNo track) const.
	 *
	 * \param[in] track 1-based track number, accepts 0 as offset of track 1
	 *
	 * \return Index of last sample contributing to the specified track's ARCS
	 */
	virtual sample_count_t do_last_relevant_sample(const TrackNo track) const
	noexcept
	= 0;

	/**
	 * \brief Implements last_relevant_sample() const.
	 *
	 * \return Index of the last sample contributing to the last track's ARCS
	 */
	virtual sample_count_t do_last_relevant_sample_no_parms() const noexcept
	= 0;

	/**
	 * \brief Implements track(const sample_count_t smpl) const.
	 *
	 * \param[in] smpl The sample to get the track for
	 *
	 * \return Track number of the track containing sample \c smpl
	 */
	virtual int do_track(const sample_count_t smpl) const noexcept
	= 0;

	/**
	 * \brief Implements offset(const int track) const.
	 *
	 * \param[in] track The 0-based track to get the offset for
	 *
	 * \return The offset for the specified 0-based track
	 */
	virtual lba_count_t do_offset(const int track) const noexcept
	= 0;

	/**
	 * \brief Implements length(const int track) const.
	 *
	 * \param[in] track The 0-based track to get the length for
	 *
	 * \return The length for the specified 0-based track
	 */
	virtual lba_count_t do_length(const int track) const noexcept
	= 0;

	/**
	 * \brief Implements id() const.
	 *
	 * \return The ARId of the current medium
	 */
	virtual ARId do_id() const noexcept
	= 0;

	/**
	 * \brief Implements skips_front() const.
	 *
	 * \return \c TRUE iff context will signal to skip the first samples.
	 */
	virtual bool do_skips_front() const noexcept
	= 0;

	/**
	 * \brief Implements skips_back() const.
	 *
	 * \return \c TRUE iff context will signal to skip the last samples.
	 */
	virtual bool do_skips_back() const noexcept
	= 0;

	/**
	 * \brief Implements num_skip_front() const.
	 *
	 * \return The number of samples to skip at the beginning of the first track
	 */
	virtual sample_count_t do_num_skip_front() const noexcept
	= 0;

	/**
	 * \brief Implements num_skip_back() const.
	 *
	 * \return The number of samples to skip at the end of the last track
	 */
	virtual sample_count_t do_num_skip_back() const noexcept
	= 0;

	/**
	 * \brief Implements notify_skips(const sample_count_t num_skip_front, const sample_count_t num_skip_back).
	 *
	 * \param[in] num_skip_front Actual amount of skipped samples at the beginning
	 * \param[in] num_skip_back  Actual amount of skipped samples at the end
	 */
	virtual void do_notify_skips(const sample_count_t num_skip_front,
			const sample_count_t num_skip_back) noexcept
	= 0;

	/**
	 * \brief Implements clone() const.
	 *
	 * \return A clone of this instance
	 */
	virtual std::unique_ptr<CalcContext> do_clone() const noexcept
	= 0;
};

/**
 * \brief Create a CalcContext from two skip flags.
 *
 * This addresses a situation where a single track has to be checksummed.
 *
 * \param[in] skip_front    Tell wether to skip the front samples
 * \param[in] skip_back     Tell wether to skip the back samples
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const bool &skip_front,
		const bool &skip_back);

/**
 * \brief Create a CalcContext from an audio filename and two skip flags.
 *
 * This addresses a situation where a single track has to be checksummed.
 *
 * The file will not be opened, it is just declared as part of the metadata.
 *
 * \param[in] skip_front    Tell wether to skip the front samples
 * \param[in] skip_back     Tell wether to skip the back samples
 * \param[in] audiofilename The name of the audiofile
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const bool &skip_front,
		const bool &skip_back,
		const std::string &audiofilename);

/**
 * \brief Create a CalcContext from a TOC.
 *
 * \param[in] toc The TOC to use
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const TOC &toc);

/**
 * \brief Create a CalcContext from an audio filename and a TOC.
 *
 * The file will not be opened, it is just declared as part of the metadata.
 *
 * \param[in] toc           The TOC to use
 * \param[in] audiofilename The name of the audiofile, empty be default
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const TOC &toc,
		const std::string &audiofilename);

/**
 * \copydoc arcstk::v_1_0_0::make_context(const TOC&)
 */
std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC> &toc);

/**
 * \copydoc arcstk::v_1_0_0::make_context(const TOC&, const std::string&)
 */
std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC> &toc,
		const std::string &audiofilename);



/**
 * \brief Checksum calculation for a requested checksum::type.
 *
 * \attention
 * Calculation is a stateful object. It is designed to be updated with a
 * sequence of chunks of the actual audio input. Thereafter, it will provide
 * the caller with a Checksums instance that represents the result.
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
 * \attention
 * After having called update() for the first time, it is not supported to alter
 * the CalcContext before Calculation is complete(). Otherwise, the result()
 * will be probably incorrect.
 *
 * Requesting the checksum types determines whether ARCSsv1 only or ARCSsv1
 * and ARCSsv2 are calculated. The checksum type can be skipped in which case
 * Calculation will use ARCSv2 as default.
 *
 * After context and requested types are set, update() must be called for each
 * subsequent sample block until all samples declared in the CalcContext have
 * been processed. After the last call of update(), a call of complete() can
 * do a sanity check of the result().
 *
 * Note that at least before processing the last block, the total number of
 * samples must be known to the Calculation instance. The instance must be
 * informed about this value either early in the process by the TOC, or,
 * in case the TOC is incomplete, by a callback call of
 * Calculation::update_audiosize() as soon as the total number of input samples
 * is known. The caller is responsible for informing Calculation about the total
 * number of input samples before the last call of update().
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
	void set_context(std::unique_ptr<CalcContext> context) noexcept;

	/**
	 * \brief Read the CalcContext of this instance.
	 *
	 * \return The CalcContext of this instance
	 */
	const CalcContext& context() const noexcept;

	/**
	 * \brief Returns current type requested.
	 *
	 * \return A disjunction of all requested types.
	 */
	checksum::type type() const noexcept;

	/**
	 * \brief Update the calculation with a sequence of samples.
	 *
	 * \param[in] begin Iterator pointing to the first sample of the sequence
	 * \param[in] end   Iterator pointing behind the last sample of the sequence
	 */
	void update(SampleInputIterator begin, SampleInputIterator end);

	/**
	 * \brief Updates the instance with a new AudioSize.
	 *
	 * This must be done before the last call of update().
	 *
	 * \param[in] audiosize The updated AudioSize
	 */
	void update_audiosize(const AudioSize &audiosize);

	/**
	 * \brief Returns \c TRUE iff this Calculation is completed, otherwise \c FALSE.
	 *
	 * \c FALSE indicates that the instance expects more updates. If the instance
	 * returns \c TRUE it is safe to call result().
	 *
	 * \return \c TRUE if the Calculation is completed, otherwise \c FALSE
	 */
	bool complete() const noexcept;

	/**
	 * \brief Returns the total number of expected PCM 32 bit samples.
	 *
	 * This is just for debugging.
	 *
	 * \return Total number of PCM 32 bit samples expected.
	 */
	int64_t samples_expected() const noexcept;

	/**
	 * \brief Returns the total number for PCM 32 bit samples yet processed.
	 *
	 * This is just for debugging.
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	int64_t samples_processed() const noexcept;

	/**
	 * \brief Returns the total number of PCM 32 bit samples that is yet to be
	 * processed.
	 *
	 * This value is equivalent to samples_expected() - samples_processed().
	 *
	 * This is just for debugging.
	 *
	 * \return Total number of PCM 32 bit samples yet to process.
	 */
	int64_t samples_todo() const noexcept;

	/**
	 * \brief Acquire the resulting Checksums.
	 *
	 * \return The computed Checksums
	 */
	Checksums result() const noexcept;


	Calculation& operator = (Calculation rhs);

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

/** @} */


inline SampleInputIterator::SampleInputIterator(const SampleInputIterator& rhs)
	: object_ { rhs.object_->clone() }
{
	// empty
}


inline SampleInputIterator::SampleInputIterator(SampleInputIterator&& rhs)
noexcept
	: object_ { std::move(rhs.object_) }
{
	// empty
}


inline SampleInputIterator::~SampleInputIterator() noexcept = default;


inline SampleInputIterator::reference SampleInputIterator::operator * () const
noexcept
{
	return object_->dereference();
}


inline SampleInputIterator& SampleInputIterator::operator ++ () noexcept
{
	object_->preincrement();
	return *this;
}


inline SampleInputIterator SampleInputIterator::operator ++ (int) noexcept
{
	SampleInputIterator prev_val(*this);
	object_->preincrement();
	return prev_val;
}


inline SampleInputIterator& SampleInputIterator::operator = (
		SampleInputIterator rhs) noexcept
{
	swap(*this, rhs); // finds SampleInputIterator's friend swap via ADL
	return *this;
}


// operators SampleInputIterator


inline bool operator == (const SampleInputIterator &lhs,
		const SampleInputIterator &rhs) noexcept
{
	return lhs.object_->equals(*rhs.object_);
}


inline SampleInputIterator operator + (SampleInputIterator lhs,
		const sample_count_t amount) noexcept
{
	lhs.object_->advance(amount);
	return lhs;
}


inline SampleInputIterator operator + (const sample_count_t amount,
		SampleInputIterator rhs) noexcept
{
	return rhs + amount;
}

} // namespace v_1_0_0

} // namespace arcstk

#endif

