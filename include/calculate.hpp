#ifndef __LIBARCSTK_CALCULATE_HPP__
#define __LIBARCSTK_CALCULATE_HPP__

/**
 * \file
 *
 * \brief Public API for checksum calculation.
 */

#include <array>
#include <cstdint>
#include <cstddef>     // for size_t
#include <iterator>    // for forward_iterator_tag
#include <memory>
#include <set>
#include <stdexcept>   // for logic_error
#include <string>
#include <type_traits> // for is_same, decay_t
#include <utility>     // for declval

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
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

/**
 * \defgroup calc Checksum Calculation
 *
 * \brief Public API for \link Calculation checksum calculation \endlink.
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
 * A Checksum refers to a particular track and a particular checksum::type.
 *
 * ChecksumSet is a set of @link arcstk::v_1_0_0::Checksum Checksums @endlink
 * of different @link arcstk::v_1_0_0::checksum::type checksum::types @endlink
 * of the same track.
 *
 * Checksums represent a calculation result for all requested checksum
 * types and all tracks of the audio input.
 * It is an aggregation of the
 * @link arcstk::v_1_0_0::Checksum Checksums @endlink of an audio input.
 * Depending on the input, it can represent either an entire album or a single
 * track.
 *
 * Considering the input of a Calculation, a CalcContext represents the per-file
 * metadata information that is used during calculation. Calculation computes
 * Checksums from a sequence of \link Calculation::update() updates \endlink
 * by sample sequences according to its current CalcContext.
 *
 * SampleInputIterator wraps the concrete iterator of the input sample sequence
 * so any class with a compatible iterator can be used.
 *
 * AudioSize represents the size of the audio data in a file in frames,
 * samples and bytes.
 *
 * InvalidAudioException is thrown during the
 * \link Calculation::update update() \endlink if the audio input is
 * insufficient or invalid.
 *
 * @{
 */


class Checksum; // forward declaration for operator == and to_hex_str()


/**
 * \brief Everything regarding operation with Checksums.
 */
namespace checksum
{

/**
 * \brief Pre-defined checksum types.
 */
enum class type : unsigned int
{
	ARCS1   = 1,
	ARCS2   = 2
	//THIRD_TYPE  = 4,
	//FOURTH_TYPE = 8 ...
};


/**
 * \brief Iterable sequence of all predefined checksum types.
 *
 * There is no guaranteed order of the types.
 */
static const type types[] = {
	type::ARCS1,
	type::ARCS2
	// type::THIRD_TYPE,
	// type::FOURTH_TYPE ...
};


/**
 * \brief Name of a checksum::type.
 *
 * \param[in] t Type to get name of
 *
 * \return Name of type
 */
std::string type_name(const type t);

} // namespace checksum


/**
 * \brief Equality.
 *
 * \param[in] lhs The left hand side instance to check for equality
 * \param[in] rhs The right hand side instance to check for equality
 *
 * \return TRUE if \c lhs is equal to \c rhs, otherwise FALSE
 */
bool operator == (const Checksum &lhs, const Checksum &rhs) noexcept;


/**
 * \brief A 32-bit wide unsigned checksum for a single file or track.
 */
class Checksum final : public Comparable<Checksum>
{

public:

	/**
	 * \brief Constructor.
	 *
	 * Creates an empty Checksum.
	 */
	Checksum();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] value Actual checksum value
	 */
	explicit Checksum(const uint32_t value);

	/**
	 * \brief Numeric value of the checksum.
	 *
	 * \return Numeric value of the checksum
	 */
	uint32_t value() const noexcept;

	/**
	 * \brief Return TRUE iff this Checksum is empty, otherwise FALSE.
	 *
	 * A Checksum is empty if it contains no valid value. Note that this
	 * does not entail <tt>value() == 0</tt> in all cases. Nonetheless, for most
	 * checksum types, the converse holds, they are <tt>empty()</tt> if they are
	 * \c 0 .
	 *
	 * \return Return TRUE iff this Checksum is empty, otherwise FALSE.
	 */
	bool empty() const noexcept;

	/**
	 * \brief Copy assignment
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return Result of the assignment
	 */
	Checksum& operator = (const uint32_t rhs);


private:

	/**
	 * \brief Actual checksum value;
	 */
	uint32_t value_;
};

/** @} */

} // namespace v_1_0_0
} // namespace arcstk


#ifndef __LIBARCSTK_CHECKSUM_TPP__
#include "details/checksum.tpp"
#endif


namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \addtogroup calc
 *
 * @{
 */


/**
 * \brief A set of Checksum instances of different types for a single track.
 */
class ChecksumSet final : public OpaqueChecksumSetBase
{

public:

	/**
	 * \brief Constructor for a track with unknown length (will be 0)
	 */
	ChecksumSet();

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs The instance to copy
	 */
	ChecksumSet(const ChecksumSet &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs The instance to move
	 */
	ChecksumSet(ChecksumSet &&rhs) noexcept;

	/**
	 * \brief Constructor
	 *
	 * \param[in] length Length in LBA frames of the track
	 */
	explicit ChecksumSet(const lba_count length);

	/**
	 * \brief Default destructor
	 */
	~ChecksumSet() noexcept;

	/**
	 * \brief Length (in LBA frames) of this track.
	 *
	 * \return Length of this track in LBA frames
	 */
	lba_count length() const noexcept;

	/**
	 * \brief Return the \ref Checksum for the specified \c type
	 *
	 * \param[in] type The checksum::type to return the value
	 *
	 * \return The checksum for the specified type
	 */
	Checksum get(const checksum::type type) const;

	/**
	 * \brief Return the checksum types present in this ChecksumSet
	 *
	 * Just an alias to ChecksumMap::keys().
	 *
	 * \return The checksum types in this ChecksumSet
	 */
	std::set<checksum::type> types() const;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	ChecksumSet& operator = (const ChecksumSet &rhs);


private:

	// forward declaration for ChecksumSet::Impl
	class Impl;

	/**
	 * \brief Private implementation of ChecksumSet
	 */
	std::unique_ptr<Impl> impl_;
};


class AudioSize; // forward declaration for operator ==


/**
 * \brief Equality.
 *
 * \param[in] lhs The left hand side of the comparison
 * \param[in] rhs The right hand side of the comparison
 *
 * \return TRUE if the content of \c lhs and \c rhs is equal, otherwise FALSE
 */
bool operator == (const AudioSize &lhs, const AudioSize &rhs) noexcept;


/**
 * \brief Less-than.
 *
 * \param[in] lhs The left hand side of the comparison
 * \param[in] rhs The right hand side of the comparison
 *
 * \return TRUE if \c lhs < \c rhs, otherwise FALSE
 */
bool operator < (const AudioSize &lhs, const AudioSize &rhs) noexcept;


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
	 * This also determines the number of PCM samples and the number of
	 * PCM bytes.
	 *
	 * \param[in] leadout LBA leadout frame
	 */
	void set_leadout_frame(const lba_count leadout) noexcept;

	/**
	 * \brief Return the LBA leadout frame.
	 *
	 * \return LBA leadout frame
	 */
	lba_count leadout_frame() const noexcept;

	/**
	 * \brief Set the total number of 32 bit PCM samples.
	 *
	 * This also determines the leadout frame and the number of PCM bytes.
	 *
	 * \param[in] smpl_count Total number of 32 bit PCM samples
	 */
	void set_total_samples(const sample_count smpl_count) noexcept;

	/**
	 * \brief Return the total number of 32 bit PCM samples.
	 *
	 * \return The total number of 32 bit PCM samples
	 */
	sample_count total_samples() const noexcept;

	/**
	 * \brief Set the total number of bytes holding decoded 32 bit PCM samples
	 *
	 * This also determines the leadout frame and the number of 32 bit PCM
	 * samples.
	 *
	 * \param[in] byte_count Total number of bytes holding 32 bit PCM samples
	 */
	void set_pcm_byte_count(const uint32_t byte_count) noexcept;

	/**
	 * \brief Return the number of bytes holding 32 bit PCM samples.
	 *
	 * \return The total number of bytes holding 32 bit PCM samples
	 */
	uint32_t pcm_byte_count() const noexcept;

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
 * \internal
 * \brief Defined iff \c Iterator is an iterator over \c sample_type.
 *
 * \tparam Iterator Iterator type to test
 */
template<typename Iterator>
using IsSampleIterator = std::enable_if_t<
		details::is_iterator_over<Iterator, sample_type>::value>;


class SampleInputIterator; // forward declaration


/**
 * \brief Equality
 *
 * \param[in] lhs The left hand side of the comparison
 * \param[in] rhs The right hand side of the comparison
 *
 * \return TRUE if the content of \c lhs and \c rhs is equal, otherwise FALSE
 */
bool operator == (const SampleInputIterator &lhs, const SampleInputIterator &rhs)
	noexcept;


/**
 * \brief Advance the iterator by a non-negative amount.
 *
 * \param[in] lhs    The iterator to add to
 * \param[in] amount A non-negative amount to advance the iterator
 *
 * \return Iterator pointing to the position advanced by \c amount
 */
SampleInputIterator operator + (SampleInputIterator lhs, const sample_count amount)
	noexcept;


/**
 * \brief Type erasing interface for iterators over PCM 32 bit samples.
 *
 * Wraps the concrete iterator to be passed to
 * \link Calculation::update() update() \endlink a Calculation.
 * This allows it to pass in fact any iterator type to a Calculation.
 *
 * SampleInputIterator can wrap any iterator with a value_type of uint32_t
 * except instances of itself, e.g. it can not be "nested".
 *
 * The type erasure interface only ensures that the requirements of a
 * <A HREF="https://en.cppreference.com/w/cpp/named_req/InputIterator">
 * LegacyInputIterator</A> are met. Those requirements are sufficient for
 * \link Calculation::update() updating \endlink a Calculation.
 *
 * Although SampleInputIterator is intended to provide the functionality of
 * an input iterator, it does not provide operator->() and does
 * therefore not completely fulfill the requirements for a LegacyInputIterator.
 *
 * SampleInputIterator provides iteration over values of type sample_type,
 * which is a primitve type. Samples have no members, therefore operator ->
 * would not provide any reasonable function.
 */
class SampleInputIterator final : public Comparable<SampleInputIterator>
{

public:

	friend bool operator == (const SampleInputIterator &lhs,
			const SampleInputIterator &rhs) noexcept;

	friend SampleInputIterator operator + (SampleInputIterator lhs,
			const sample_count amount) noexcept;

	/**
	 * \brief Iterator category is std::input_iterator_tag.
	 */
	using iterator_category = std::input_iterator_tag;

	/**
	 * \brief The type this iterator enumerates.
	 */
	using value_type = sample_type;

	/**
	 * \brief Same as value_type, *not* a reference type.
	 */
	using reference = sample_type;

	/**
	 * \brief Const pointer to an instance of value_type.
	 */
	//using pointer = const sample_type*;
	using pointer = void;

	/**
	 * \brief Pointer difference type.
	 */
	using difference_type = std::ptrdiff_t;


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
		virtual void preincrement() noexcept
		= 0;

		/**
		 * \brief Advances iterator by \c n positions
		 *
		 * \param[in] n Number of positions to advance
		 */
		virtual void advance(const sample_count n) noexcept
		= 0;

		/**
		 * \brief Reference to the actual value under the iterator.
		 *
		 * \return Reference to actual value.
		 */
		virtual reference dereference() noexcept
		= 0;

		/**
		 * \brief Returns TRUE if \c rhs is equal to the instance.
		 *
		 * \param[in] rhs The instance to test for equality
		 *
		 * \return TRUE if \c rhs is equal to the instance, otherwise FALSE
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

		void advance(const sample_count n) noexcept final
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

		/**
		 * \brief Swap for Models
		 *
		 * \param[in] lhs Left hand side to swap
		 * \param[in] rhs Right hand side to swap
		 */
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

	// LegacyIterator:
	// ok: value_type, difference_type, pointer, reference, iterator_category
	// ok: dereference operator *it mus have specified effect
	// ok: behaviour of ++it is defined
	// ok: CopyConstructible
	// ok: CopyAssignable
	// ok: Destructible
	// ok: lvalues must be swappable

	// EqualityComparable:
	// ok: it1 == it2 is possible (+ reflexivity, symmetry, transitivity)

	// LegacyInputIterator:
	// ok: it1 != it2 is possible (true iff !(it1 == it2))
	// ok?: if it1 == it2, then *it1 == *it2
	// ok?: it->m  (means (*it).m)
	// ok: ++it returns type T&
	// ok? (void)it++ is equivalent to (void)++i
	// ok? *it++ means { value_type x = *it; ++it; return x; }

	/**
	 * \brief Converting constructor.
	 *
	 * \tparam Iterator The iterator type to wrap
	 *
	 * \param[in] i Instance of an iterator over \c sample_type
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
	 * \return A sample_type sample, returned by value
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

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return Instance with the assigned value
	 */
	SampleInputIterator& operator = (SampleInputIterator rhs) noexcept;
	// required by LegacyIterator

	/**
	 * \brief Swap for SampleInputIterators.
	 *
	 * \param[in] lhs Left hand side to swap
	 * \param[in] rhs Right hand side to swap
	 */
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


/**
 * \brief Advance the iterator by a non-negative amount.
 *
 * \param[in] amount A non-negative amount to advance the iterator
 * \param[in] rhs    The iterator to add to
 *
 * \return Iterator pointing to the position advanced by \c amount
 */
SampleInputIterator operator + (const sample_count amount, SampleInputIterator rhs)
	noexcept;


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
	void set_audio_size(const AudioSize &audio_size) noexcept;

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
	 * Note that this is independent from <tt>is_multi_track()</tt>. A
	 * TOC containing only one track would have a CalcContext in
	 * which <tt>track_count()</tt> is \c 1 but <tt>is_multi_track()</tt> is
	 * TRUE. Method <tt>is_multi_track()</tt> specifies the processing mode
	 * while <tt>track_count()</tt> just provides information about the TOC.
	 *
	 * \return The number of tracks represented in this file
	 */
	uint8_t track_count() const noexcept;

	/**
	 * \brief Returns TRUE if this instances indicates a processing for multiple
	 * tracks on a single input file.
	 *
	 * If this is FALSE, no chunks will be available. Multitrack mode is
	 * activated by setting a TOC.
	 *
	 * \return TRUE if this context specifies multitrack mode, otherwise FALSE
	 */
	bool is_multi_track() const noexcept;

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
	sample_count first_relevant_sample(const TrackNo track) const noexcept;

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
	sample_count first_relevant_sample() const noexcept;

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
	sample_count last_relevant_sample(const TrackNo track) const noexcept;

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
	sample_count last_relevant_sample() const noexcept;

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
	TrackNo track(const sample_count smpl) const noexcept;

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
	lba_count offset(const uint8_t track) const noexcept;

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
	lba_count length(const uint8_t track) const noexcept;

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
	 * \brief Returns TRUE iff this context will skip the first 2939 samples of
	 * the first track.
	 *
	 * \return TRUE iff context will signal to skip the first samples.
	 */
	bool skips_front() const noexcept;

	/**
	 * \brief Returns TRUE iff this context will skip the last 2940 samples
	 * (5 LBA frames) of the last track.
	 *
	 * \return TRUE iff context will signal to skip the last samples.
	 */
	bool skips_back() const noexcept;

	/**
	 * \brief Returns the amount of samples to skip at the beginning of the
	 * first track - or, in a single track scenario, once at the beginning.
	 *
	 * The skipping is already active if this instance skips_front().
	 *
	 * \return The number of samples to skip at the beginning of the first track
	 */
	sample_count num_skip_front() const noexcept;

	/**
	 * \brief Returns the amount of samples to skip at the end of the last track
	 * - or, in a single track scenario, once at the end.
	 *
	 * The skipping is already active if this instance skips_back()
	 *
	 * \return The number of samples to skip at the end of the last track
	 */
	sample_count num_skip_back() const noexcept;

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
	void notify_skips(const sample_count num_skip_front,
			const sample_count num_skip_back) noexcept;

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
	virtual void do_set_audio_size(const AudioSize &audio_size) noexcept
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
	 * \brief Implements track_count() const.
	 *
	 * \return The number of tracks represented in this file
	 */
	virtual uint8_t do_track_count() const noexcept
	= 0;

	/**
	 * \brief Implements is_multi_track() const.
	 *
	 * \return TRUE if this context specifies multitrack mode, otherwise FALSE
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
	virtual sample_count do_first_relevant_sample(const TrackNo track) const
	noexcept
	= 0;

	/**
	 * \brief Implements first_relevant_sample() const.
	 *
	 * \return Index of the first sample contributing to the first track's ARCS
	 */
	virtual sample_count do_first_relevant_sample_0() const noexcept
	= 0;

	/**
	 * \brief Implements last_relevant_sample(const TrackNo track) const.
	 *
	 * \param[in] track 1-based track number, accepts 0 as offset of track 1
	 *
	 * \return Index of last sample contributing to the specified track's ARCS
	 */
	virtual sample_count do_last_relevant_sample(const TrackNo track) const
	noexcept
	= 0;

	/**
	 * \brief Implements last_relevant_sample() const.
	 *
	 * \return Index of the last sample contributing to the last track's ARCS
	 */
	virtual sample_count do_last_relevant_sample_0() const noexcept
	= 0;

	/**
	 * \brief Implements track(const sample_count smpl) const.
	 *
	 * \param[in] smpl The sample to get the track for
	 *
	 * \return Track number of the track containing sample \c smpl
	 */
	virtual TrackNo do_track(const sample_count smpl) const noexcept
	= 0;

	/**
	 * \brief Implements offset(const uint8_t track) const.
	 *
	 * \param[in] track The 0-based track to get the offset for
	 *
	 * \return The offset for the specified 0-based track
	 */
	virtual lba_count do_offset(const uint8_t track) const noexcept
	= 0;

	/**
	 * \brief Implements length(const uint8_t track) const.
	 *
	 * \param[in] track The 0-based track to get the length for
	 *
	 * \return The length for the specified 0-based track
	 */
	virtual lba_count do_length(const uint8_t track) const noexcept
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
	 * \return TRUE iff context will signal to skip the first samples.
	 */
	virtual bool do_skips_front() const noexcept
	= 0;

	/**
	 * \brief Implements skips_back() const.
	 *
	 * \return TRUE iff context will signal to skip the last samples.
	 */
	virtual bool do_skips_back() const noexcept
	= 0;

	/**
	 * \brief Implements num_skip_front() const.
	 *
	 * \return The number of samples to skip at the beginning of the first track
	 */
	virtual sample_count do_num_skip_front() const noexcept
	= 0;

	/**
	 * \brief Implements num_skip_back() const.
	 *
	 * \return The number of samples to skip at the end of the last track
	 */
	virtual sample_count do_num_skip_back() const noexcept
	= 0;

	/**
	 * \brief Implements notify_skips(const sample_count num_skip_front, const sample_count num_skip_back).
	 *
	 * \param[in] num_skip_front Actual amount of skipped samples at the beginning
	 * \param[in] num_skip_back  Actual amount of skipped samples at the end
	 */
	virtual void do_notify_skips(const sample_count num_skip_front,
			const sample_count num_skip_back) noexcept
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
 * The file will not be opened by the object, it is just declared as part of the
 * metadata.
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
 * The file will not be opened by the object, it is just declared as part of the
 * metadata.
 *
 * \param[in] toc           The TOC to use
 * \param[in] audiofilename The name of the audiofile, empty be default
 *
 * \return CalcContext for the input
 */
std::unique_ptr<CalcContext> make_context(const TOC &toc,
		const std::string &audiofilename);

/**
 * \copydoc make_context(const TOC&)
 */
std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC> &toc);

/**
 * \copydoc make_context(const TOC&, const std::string&)
 */
std::unique_ptr<CalcContext> make_context(const std::unique_ptr<TOC> &toc,
		const std::string &audiofilename);


class Checksums;

/**
 * \brief Swap for Checksums
 *
 * \param[in] lhs Left hand side to swap
 * \param[in] rhs Right hand side to swap
 */
void swap(Checksums &lhs, Checksums &rhs) noexcept;

/**
 * \brief Equality for Checksums
 *
 * \param[in] lhs Left hand side to compare
 * \param[in] rhs Right hand side to compare
 *
 * \return TRUE if \c lhs equals \c rhs
 */
bool operator == (const Checksums &lhs, const Checksums &rhs) noexcept;

/**
 * \brief The result of a Calculation, a list of
 * \link ChecksumSet ChecksumSets \endlink.
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


	friend void swap(Checksums &lhs, Checksums &rhs) noexcept;

	friend bool operator == (const Checksums &lhs, const Checksums &rhs)
		noexcept;


public: /* member functions */

	/**
	 * \brief Constructor.
	 *
	 * \param[in] size Number of elements
	 */
	explicit Checksums(size_type size);

	// forward declaration for Checksums::Impl
	class Impl;

	/**
	 * \internal
	 * \brief Construct with custom implementation
	 *
	 * \param[in] impl The implementation of this instance
	 */
	Checksums(std::unique_ptr<Impl> impl);

	/**
	 * \brief Constructor
	 *
	 * This constructor is intended for testing purposes only.
	 *
	 * \param[in] tracks Sequence of track checksums
	 */
	Checksums(std::initializer_list<ChecksumSet> tracks);

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
	 * \brief Default destructor
	 */
	~Checksums() noexcept;

	/**
	 * \brief Append a track's checksums
	 *
	 * \param[in] checksums The checksums of a track
	 */
	void append(const ChecksumSet &checksums);

	/**
	 * \brief Returns a pointer to the first element.
	 *
	 * \return Pointer to the first element
	 */
	const_iterator begin() const noexcept;

	/**
	 * \brief Returns a pointer after the last element.
	 *
	 * \return Pointer after the last element
	 */
	const_iterator end() const noexcept;

	/**
	 * \brief Returns a pointer to the first element.
	 *
	 * \return Pointer to the first element
	 */
	const_iterator cbegin() const noexcept;

	/**
	 * \brief Returns a pointer after the last element.
	 *
	 * \return Pointer after the last element
	 */
	const_iterator cend() const noexcept;

	/**
	 * \brief The ARTriplet with the specified 0-based index \c index.
	 *
	 * \details
	 *
	 * Bounds checking is performed. If \c index is illegal, an exception is
	 * thrown.
	 *
	 * \see \link Checksums::operator [](const size_type index) const
	 * operator[]\endlink
	 *
	 * \param[in] index Index of the ChecksumSet to read
	 *
	 * \return ChecksumSet at index \c index.
	 *
	 * \throws std::out_of_range Iff \c index > Checksums::size() - 1.
	 */
	const ChecksumSet& at(const size_type index) const;

	/**
	 * \brief The ChecksumSet with the specified \c index.
	 *
	 * No bounds checking is performed. For index based access with bounds
	 * checking, see
	 * \link Checksums::at(const size_type index) const at()\endlink.
	 *
	 * \param[in] index The 0-based index of the ChecksumSet to return
	 *
	 * \return ChecksumSet at the specified index
	 */
	const ChecksumSet& operator [] (const size_type index) const;

	/**
	 * \brief Return the number of elements.
	 *
	 * \return Number of elements
	 */
	size_type size() const noexcept;

	/**
	 * \brief Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Checksums& operator = (Checksums rhs);

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
	 * \brief Private implementation of Checksums.
	 */
	std::unique_ptr<Checksums::Impl> impl_;
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
	void set_context(std::unique_ptr<CalcContext> context) noexcept;

	/**
	 * \brief Read the CalcContext of this instance.
	 *
	 * \return The CalcContext of this instance
	 */
	const CalcContext& context() const noexcept;

	/**
	 * \brief Returns current type.
	 *
	 * \return A disjunction of all requested types.
	 */
	checksum::type type() const noexcept;

	/**
	 * \brief Update the calculation with a sequence of samples.
	 *
	 * \param[in] begin Iterator pointing to the beginning of the sequence
	 * \param[in] end   Iterator pointing to the end of the sequence
	 */
	void update(SampleInputIterator begin, SampleInputIterator end);

	/**
	 * \brief Updates the instance with a new AudioSize.
	 *
	 * This must be done before the last call of Calculation::update().
	 *
	 * \param[in] audiosize The updated AudioSize
	 */
	void update_audiosize(const AudioSize &audiosize) noexcept;

	/**
	 * \brief Returns TRUE iff this Calculation is completed, otherwise FALSE.
	 *
	 * FALSE indicates that the instance expects more updates. If the instance
	 * returns TRUE it is safe to call result().
	 *
	 * \return TRUE if the Calculation is completed, otherwise FALSE
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
	 * \brief Returns the counter for PCM 32 bit samples.
	 *
	 * This is just for debugging.
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	int64_t samples_processed() const noexcept;

	/**
	 * \brief Returns the number of PCM 32 bit samples that is yet to process.
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
		const sample_count amount) noexcept
{
	lhs.object_->advance(amount);
	return lhs;
}


inline SampleInputIterator operator + (const sample_count amount,
		SampleInputIterator rhs) noexcept
{
	return rhs + amount;
}

} // namespace v_1_0_0

} // namespace arcstk

#endif

