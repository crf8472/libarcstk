#ifndef __LIBARCS_CALCULATE_HPP__
#define __LIBARCS_CALCULATE_HPP__


/**
 * \file calculate.hpp Checksum calculation API
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
 * are represented as an iterable aggregate of <tt>ChecksumSet</tt>s.
 *
 * CalcContext represents the per-file metadata information that is used
 * during calculation. Calculation computes checksums from a sequence of
 * sample blocks according to its current CalcContext.
 *
 * Checksums represent a calculation result for all requested checksum
 * types and all tracks of the audio input.
 *
 * ChecksumSet is a set of <tt>Checksum</tt>s of different
 * <tt>checksum::type</tt>s. It represents the calculation result for a
 * particular track.
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


#ifndef __LIBARCS_CHECKSUM_HPP__
#include "checksum.hpp"
#endif
#ifndef __LIBARCS_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif


/**
 * libarcs main namespace
 */
namespace arcs
{
/// \defgroup calc Checksum Calculation
/// @{
/**
 * libarcs API version 1.0.0
 */
inline namespace v_1_0_0
{

/**
 * Type for internal representation of samples.
 */
using sample_type = uint32_t;


/**
 * Uniform access to the size of the input audio information.
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

public:

	/**
	 * Constructor
	 */
	AudioSize();

	/**
	 * Copy constructor
	 *
	 * \param[in] rhs The AudioSize to copy
	 */
	AudioSize(const AudioSize &rhs);

	/**
	 * Move constructor
	 *
	 * \param[in] rhs The AudioSize to move
	 */
	AudioSize(AudioSize &&rhs) noexcept;

	/**
	 * Default destructor
	 */
	~AudioSize() noexcept;

	/**
	 * Set the 1-based index of the LBA leadout frame.
	 *
	 * This also determines the number of PCM samples and the number of
	 * PCM bytes.
	 *
	 * \param[in] leadout LBA leadout frame
	 */
	void set_leadout_frame(const uint32_t leadout);

	/**
	 * Return the LBA leadout frame.
	 *
	 * \return LBA leadout frame
	 */
	uint32_t leadout_frame() const;

	/**
	 * Set the total number of 32 bit PCM samples.
	 *
	 * This also determines the leadout frame and the number of PCM bytes.
	 *
	 * \param[in] smpl_count Total number of 32 bit PCM samples
	 */
	void set_sample_count(const uint32_t smpl_count);

	/**
	 * Return the total number of 32 bit PCM samples.
	 *
	 * \return The total number of 32 bit PCM samples
	 */
	uint32_t sample_count() const;

	/**
	 * Set the total number of bytes holding 32 bit PCM samples (if decoded).
	 *
	 * This also determines the leadout frame and the number of 32 bit PCM
	 * samples.
	 *
	 * \param[in] byte_count Total number of bytes holding 32 bit PCM samples
	 */
	void set_pcm_byte_count(const uint64_t byte_count);

	/**
	 * Return the number of bytes holding 32 bit PCM samples.
	 *
	 * \return The total number of bytes holding 32 bit PCM samples
	 */
	uint64_t pcm_byte_count() const;

	/**
	 * Return TRUE if the AudioSize is 0.
	 *
	 * \return TRUE if the AudioSize is 0
	 */
	bool null() const;

	/**
	 * Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	AudioSize& operator = (AudioSize rhs);

	/**
	 * Move assignment.
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
	 * Private implementation of AudioSize
	 */
	std::unique_ptr<AudioSize::Impl> impl_;
};


namespace details
{

/* *
 * Get value_type of Iterator.
 */
template<typename Iterator>
using it_value_type = std::decay_t<decltype(*std::declval<Iterator>())>;
// This is SFINAE compatible and respects bare pointers, which would not
// have been respected when using std::iterator_traits<Iterator>::value_type

/* *
 * Check a given Iterator whether it iterates over type T
 */
template<typename Iterator, typename T>
using is_iterator_over = std::is_same< it_value_type<Iterator>, T >;

} // namespace details

/**
 * ForwardIterator over PCM 32 Bit samples.
 *
 * Type erasure class for iterators with the value_type uint32_t. It wraps the
 * concrete iterator to be passed for updating a \ref Calculation.
 */
class PCMForwardIterator
{

public:

	using iterator_category = std::forward_iterator_tag;

	using value_type        = uint32_t;

	using reference         = uint32_t;

	using pointer           = const uint32_t*;

	using difference_type   = std::ptrdiff_t;


private:

	/**
	 * Internal object interface
	 */
	struct Concept
	{
		virtual ~Concept() noexcept
		= default;


		/**
		 * Advances iterator by \c n positions
		 *
		 * \param[in] n Number of positions to advance
		 */
		virtual void advance(const int n)
		= 0;

		/**
		 * Reference to the actual value under the iterator.
		 *
		 * \return Reference to actual value
		 */
		virtual reference dereference() const
		= 0;


		virtual bool equals(const void* rhs) const // required by ==
		= 0;

		virtual const std::type_info& type() const // required by ==
		= 0;

		virtual const void* pointer() const // required by ==
		= 0;


		virtual std::unique_ptr<Concept> clone() const // for copy constructor
		= 0;
	};

	/**
	 * Internal object representation
	 */
	template<class Iter>
	struct Model : Concept
	{
		Model(Iter iter)
			: iterator_(iter)
		{
			// empty
		}


		void advance(const int n) override
		{
			std::advance(iterator_, n);
		}

		reference dereference() const override
		{
			return *iterator_;
		}


		bool equals(const void* rhs) const override // required by ==
		{
			return iterator_ == static_cast<const Model*>(rhs)->iterator_;
		}

		const std::type_info& type() const override // required by ==
		{
			return typeid(iterator_);
		}

		const void* pointer() const override // required by ==
		{
			return this;
		}


		std::unique_ptr<Concept> clone() const override // for copy constructor
		{
			return std::make_unique<Model>(*this);
		}


		private:

			Iter iterator_;
	};


public:

	/**
	 * Converting constructor
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
	 * Copy constructor
	 *
	 * \param[in] rhs Instance to copy
	 */
	PCMForwardIterator(const PCMForwardIterator& rhs);

	reference operator * () const; // required by ForwardIterator

	PCMForwardIterator& operator ++ (); // required by ForwardIterator

	PCMForwardIterator operator ++ (int); // required by ForwardIterator

	// required by ForwardIterator
	bool operator == (const PCMForwardIterator& rhs) const;

	// required by ForwardIterator
	bool operator != (const PCMForwardIterator& rhs) const;


	PCMForwardIterator operator + (const uint32_t amount) const;


private:

	/**
	 * Internal representation of wrapped object
	 */
	std::unique_ptr<Concept> object_;
};


/**
 * Information about the current audio input.
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
	 * Virtual default destructor
	 */
	virtual ~CalcContext() noexcept
	= 0;

	/**
	 * Inform about the AudioSize of the current file
	 *
	 * This contains the information about the leadout frame. This information
	 * must be known before Calculation::update is called on the last
	 * block.
	 *
	 * \param[in] audio_size AudioSize
	 */
	virtual void set_audio_size(const AudioSize &audio_size)
	= 0;

	/**
	 * Return the number of bytes of the PCM samples.
	 *
	 * \return The total number of bytes of the PCM samples
	 */
	virtual const AudioSize& audio_size() const
	= 0;

	/**
	 * Set the name of the current audio file.
	 *
	 * \param[in] filename Name of the audio file that is to be processed
	 */
	virtual void set_filename(const std::string &filename)
	= 0;

	/**
	 * Name of current audio file.
	 *
	 * \return Name of the audio file that is currently processed
	 */
	virtual std::string filename() const
	= 0;

	/**
	 * Convenience method: Total number of tracks.
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
	virtual uint8_t track_count() const
	= 0;

	/**
	 * Returns TRUE if this instances indicates a processing for multiple
	 * tracks on a single input file. If this is FALSE, no chunks will be
	 * available. Multitrack mode is activated by setting a TOC.
	 *
	 * \return TRUE if this context specifies multitrack mode, otherwise FALSE
	 */
	virtual bool is_multi_track() const
	= 0;

	/**
	 * Service method: Get 0-based index of the first relevant sample of the
	 * specified 1-based track.
	 *
	 * Note that parameter \c track is 1-based, which means that
	 * <tt>first_relevant_sample(2)</tt> returns the last 0-based sample of
	 * track 2 (and not track 3).
	 *
	 * \param[in] track The 1-based track number
	 *
	 * \return Index of the first sample contributing to the track's ARCS
	 */
	virtual uint32_t first_relevant_sample(const TrackNo track) const
	= 0;

	/**
	 * Get 0-based index of the first sample to be counted in computation.
	 *
	 * Which sample is actualley the first relevant one depends on the offset
	 * of the first track and whether samples in the beginning of the first
	 * track are to be skipped.
	 *
	 * Always equivalent with <tt>CalcContext::first_relevant_sample(1)</tt>.
	 *
	 * \return Index of the first sample contributing to the first track's ARCS
	 */
	virtual uint32_t first_relevant_sample() const
	= 0;

	/**
	 * Service method: Get 0-based index of the last relevant sample of the
	 * specified 1-based track.
	 *
	 * Note that parameter \c track is 1-based, which means that
	 * <tt>last_relevant_sample(2)</tt> returns the last 0-based sample of track
	 * 2 (and not track 3).
	 *
	 * If no offsets are set, the output will always be identical to
	 * CalcContext::last_relevant_sample().
	 *
	 * For <tt>track == 0</tt>, the last sample of the offset before track 1 is
	 * returned. This may of course be 0 iff the offset of track 1 is 0.
	 *
	 * \param[in] track 1-based track number, accepts 0 as offset of track 1
	 *
	 * \return Index of last sample contributing to the specified track's ARCS
	 */
	virtual uint32_t last_relevant_sample(const TrackNo track) const
	= 0;

	/**
	 * Get 0-based index of the last sample to be counted in computation.
	 *
	 * Which sample is actualley the last relevant one depends on whether
	 * samples in the end of the last track are to be skipped.
	 *
	 * Always equivalent with
	 * CalcContext::last_relevant_sample(this->track_count()).
	 *
	 * \return Index of the last sample contributing to the last track's ARCS
	 */
	virtual uint32_t last_relevant_sample() const
	= 0;

	/**
	 * Returns 1-based track number of the track containing the specified
	 * 0-based sample.
	 *
	 * If <tt>sample_count()</tt> is 0, the method will return 0 regardless of
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
	virtual TrackNo track(const uint32_t smpl) const
	= 0;

	/**
	 * Return the offset of the specified 0-based track from the TOC.
	 *
	 * Note that <tt>offset(i) == toc().offset(i+1)</tt> for all
	 * <tt>i: 0 <= i < toc().track_count()</tt>.
	 *
	 * \param[in] track The 0-based track to get the offset for
	 *
	 * \return The offset for the specified 0-based track
	 */
	virtual uint32_t offset(const uint8_t track) const
	= 0;

	/**
	 * Return the normalized length of the specified 0-based track.
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
	virtual uint32_t length(const uint8_t track) const
	= 0;

	/**
	 * Return the ARId of the current medium, if known.
	 *
	 * The value returned will only be significant iff non-empty offsets and
	 * non-zero total PCM byte count are available. Otherwise the ARId
	 * returned will be \c empty() .
	 *
	 * \return The ARId of the current medium
	 */
	virtual ARId id() const
	= 0;

	/**
	 * Returns TRUE iff this context will skip the first 2939 samples of the
	 * first track.
	 *
	 * \return TRUE iff context will signal to skip the first samples.
	 */
	virtual bool skips_front() const
	= 0;

	/**
	 * Returns TRUE iff this context will skip the last 2940 samples (5 frames)
	 * of the last track.
	 *
	 * \return TRUE iff context will signal to skip the last samples.
	 */
	virtual bool skips_back() const
	= 0;

	/**
	 * Returns the amount of samples to skip at the beginning of the first track
	 * - or, in a single track scenario, once at the beginning.
	 *
	 * The skipping is already active if this instance skips_front()
	 *
	 * \return The number of samples to skip at the beginning of the first track
	 */
	virtual uint32_t num_skip_front() const
	= 0;

	/**
	 * Returns the amount of samples to skip at the end of the last track
	 * - or, in a single track scenario, once at the end.
	 *
	 * The skipping is already active if this instance skips_back()
	 *
	 * \return The number of samples to skip at the end of the last track
	 */
	virtual uint32_t num_skip_back() const
	= 0;

	/**
	 * Notifies the instance about configured skipping amounts at the beginning
	 * of the first track and the end of the last track.
	 *
	 * Whether actual skipping takes place can be determined by
	 * skips_front() and skips_back().
	 *
	 * \param[in] num_skip_front Actual amount of skipped samples at the beginning
	 * \param[in] num_skip_back  Actual amount of skipped samples at the end
	 */
	virtual void notify_skips(const uint32_t num_skip_front, const uint32_t
		num_skip_back) = 0;

	/**
	 * Clone this CalcContext object.
	 *
	 * A clone is a deep copy, i.e. the result of the cloning will be a
	 * different object with the exact same state.
	 *
	 * \return A clone of this instance
	 */
	virtual std::unique_ptr<CalcContext> clone() const
	= 0;
};


/**
 * Create a CalcContext from an audio filename and two skip flags.
 *
 * The file will not be read, the filename is just declared. It is allowed to be
 * empty.
 *
 * \param audiofilename The name of the audiofile
 * \param skip_front    Tell wether to skip the front samples
 * \param skip_back     Tell wether to skip the back samples
 */
std::unique_ptr<CalcContext> make_context(const std::string &audiofilename,
		const bool &skip_front, const bool &skip_back);


/**
 * Create a CalcContext from an audio filename and a TOC.
 *
 * The file will not be read, the filename is just declared. It is allowed to be
 * empty.
 *
 * \param audiofilename The name of the audiofile
 * \param toc           The TOC to use
 */
std::unique_ptr<CalcContext> make_context(const std::string &audiofilename,
		const TOC &toc);


/**
 * Result of a Calculation
 */
class Checksums final
{

public: /* types */

	using iterator = ChecksumSet*;

	using const_iterator = const ChecksumSet*;


public: /* methods */

	/**
	 * Constructor
	 *
	 * \param[in] size Number of elements
	 */
	explicit Checksums(const std::size_t size);

	/**
	 * Copy constructor.
	 *
	 * \param[in] rhs The Checksums to copy
	 */
	Checksums(const Checksums &rhs);

	/**
	 * Move constructor.
	 *
	 * \param[in] rhs The Checksums to move
	 */
	Checksums(Checksums &&rhs) noexcept;

	/**
	 * Returns a pointer to the first element
	 *
	 * \return Pointer to the first element
	 */
	iterator begin();

	/**
	 * Returns a pointer after the last element
	 *
	 * \return Pointer after the last element
	 */
	iterator end();

	/**
	 * Returns a pointer to the first element
	 *
	 * \return Pointer to the first element
	 */
	const_iterator begin() const;

	/**
	 * Returns a pointer after the last element
	 *
	 * \return Pointer after the last element
	 */
	const_iterator end() const;

	/**
	 * Returns a pointer to the first element
	 *
	 * \return Pointer to the first element
	 */
	const_iterator cbegin() const;

	/**
	 * Returns a pointer after the last element
	 *
	 * \return Pointer after the last element
	 */
	const_iterator cend() const;

	/**
	 * Access element by its 0-based \c index .
	 *
	 * Legal range is from <tt>0</tt> to <tt>size - 1</tt>.
	 *
	 * \param[in] index The 0-based index of the element
	 *
	 * \return The element with index \c index
	 */
	ChecksumSet& operator [] (const uint32_t index);

	/**
	 * Access element by its 0-based \c index .
	 *
	 * Legal range is from <tt>0</tt> to <tt>size - 1</tt>.
	 *
	 * \param[in] index The 0-based index of the element
	 *
	 * \return The element with index \c index
	 */
	const ChecksumSet& operator [] (const uint32_t index) const;

	/**
	 * Return the number of elements
	 *
	 * \return Number of elements
	 */
	std::size_t size() const;

	/**
	 * Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Checksums& operator = (const Checksums &rhs);

	/**
	 * Move assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Checksums& operator = (Checksums &&rhs) noexcept;


private:

	/**
	 * Implementation of the set
	 */
	std::unique_ptr<ChecksumSet[]> sets_;

	/**
	 * Number of elements
	 */
	std::size_t size_;
};


/**
 * Checksum calculation for a requested checksum::type.
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
	 * Construct calculation for specified checksum type and context
	 *
	 * \param[in] type The checksum type to calculate
	 * \param[in] ctx The context for this calculation
	 */
	Calculation(const checksum::type type, std::unique_ptr<CalcContext> ctx);

	/**
	 * Construct calculation for specified context with checksum::type::ARCS2
	 *
	 * \param[in] ctx The context for this calculation
	 */
	explicit Calculation(std::unique_ptr<CalcContext> ctx);

	/**
	 * Copy constructor.
	 *
	 * \param[in] rhs The Calculation to copy
	 */
	Calculation(const Calculation& rhs);

	/**
	 * Move constructor.
	 *
	 * \param[in] rhs The Calculation to move
	 */
	Calculation(Calculation &&rhs) noexcept;

	/**
	 * Default destructor
	 */
	~Calculation() noexcept;

	/**
	 * Set the stream context for the current stream of samples.
	 *
	 * \param[in] context The CalcContext for this instance
	 */
	void set_context(std::unique_ptr<CalcContext> context);

	/**
	 * Read the CalcContext of this instance.
	 *
	 * \return The CalcContext of this instance
	 */
	const CalcContext& context() const;

	/**
	 * Returns current type.
	 *
	 * \return A disjunction of all requested types.
	 */
	checksum::type type() const;

	/**
	 * Process a block of 32 bit PCM samples.
	 *
	 * \param[in] samples Sample block to process
	 */
	void update(PCMForwardIterator begin, PCMForwardIterator end);

	/**
	 * Updates the instance with a new AudioSize.
	 *
	 * This must be done before the last call of Calculation::update().
	 *
	 * \param[in] audiosize The updated AudioSize
	 */
	void update_audiosize(const AudioSize &audiosize);

	/**
	 * Returns TRUE iff this Calculation is completed, otherwise FALSE.
	 *
	 * FALSE indicates that the instance expects more updates. If the instance
	 * returns TRUE it is safe to call result().
	 *
	 * \return TRUE if the Calculation is completed, otherwise FALSE
	 */
	bool complete() const;

	/**
	 * Returns the counter for PCM 32 bit samples.
	 *
	 * This is just for debugging.
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	uint32_t sample_counter() const;

	/**
	 * Acquire the resulting Checksums
	 *
	 * \return The computed Checksums
	 */
	Checksums result() const;

	/**
	 * Copy assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Calculation& operator = (Calculation rhs);

	/**
	 * Move assignment.
	 *
	 * \param[in] rhs Right hand side of the assignment
	 *
	 * \return The right hand side of the assignment
	 */
	Calculation& operator = (Calculation &&rhs) noexcept;


private:

	/**
	 * Request a checksum type or set of checksum types.
	 *
	 * \param[in] type Type to request
	 */
	void set_type(const checksum::type type);


	// forward declaration for Calculation::Impl
	class Impl;

	/**
	 * Private implementation of Calculation
	 */
	std::unique_ptr<Calculation::Impl> impl_;
};


/**
 * Reports insufficient or invalid audio data that prevents further processing.
 */
class InvalidAudioException final : public std::logic_error
{

public:

	/**
	 * Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidAudioException(const std::string &what_arg);

	/**
	 * Constructor.
	 *
	 * \param[in] what_arg What argument
	 */
	explicit InvalidAudioException(const char *what_arg);
};


} // namespace v_1_0_0

/// @}

} // namespace arcs


//#ifndef __LIBARCS_CALCULATE_TPP__
//#include "calculate.tpp"
//#endif

#endif

