#ifndef __LIBARCSTK_CALCULATE_HPP__
#define __LIBARCSTK_CALCULATE_HPP__

/**
 * \file
 *
 * \brief Calculation interface.
 */

#include <cstdint>          // for int32_t
#include <memory>           // for unique_ptr
#include <unordered_map>    // for unordered_map

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"             // for Checksum, Checksums
#endif
#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"             // for TotallyOrdered
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

// avoid includes
using TrackNo = int;         // TODO TrackNo also defined in identifier.hpp
using lba_count_t = int32_t; // TODO lba_count_t also defined in identifier.hpp
class ARId;
class TOC;


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
 * \brief Uniform access to the size of the input audio information.
 *
 * Some decoders provide the number of frames, others the number of samples and
 * maybe in some situations just the number of bytes of the sample stream is
 * known. To avoid implementing the appropriate conversion for each decoder,
 * AudioSize provides an interface for uniform representation to this
 * information. Any of the informations provided will determine all of the
 * others.
 *
 * An AudioSize converts to TRUE if it is greater than 0. An AudioSize of 0
 * converts to FALSE.
 */
class AudioSize final : TotallyOrdered<AudioSize>
{
	/**
	 * \brief Data: Total number of pcm sample bytes in the audio file.
	 */
	int32_t total_pcm_bytes_;

public:

	/**
	 * \brief Units for size declaration
	 */
	enum class UNIT
	{
		SAMPLES, FRAMES, BYTES
	};

	/**
	 * \brief Constructor.
	 *
	 * Constructs an AudioSize of zero().
	 */
	AudioSize();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] value Size value
	 * \param[in] unit  Unit for \c value
	 */
	AudioSize(const int32_t value, const UNIT unit);
	// FIXME This allows setting a value without a bounds check

	/**
	 * \brief Return the LBA leadout frame.
	 *
	 * Note that the number is 1-based.
	 *
	 * \return LBA leadout frame
	 */
	int32_t leadout_frame() const;

	/**
	 * \brief Return the total number of LBA frames.
	 *
	 * Maximum value is CDDA::MAX_BLOCK_ADDRESS which is a value of
	 * 449.999 LBA frames on a disc.
	 *
	 * \return Total number of LBA frames
	 */
	int32_t total_frames() const;

	/**
	 * \brief Set the total number of LBA frames.
	 *
	 * \param[in] frame_count Total number of LBA frames
	 *
	 * \throw std::underflow_error If value is negative
	 * \throw std::overflow_error  If value is greater than the unit maximum
	 */
	void set_total_frames(const int32_t frame_count);

	/**
	 * \brief Return the total number of 32 bit PCM samples.
	 *
	 * Maximum value is CDDA::MAX_BLOCK_ADDRESS * CDDA::SAMPLES_PER_FRAME
	 * which is a value of 264.599.412 stereo samples on a disc.
	 *
	 * \return The total number of 32 bit PCM samples
	 */
	int32_t total_samples() const;

	/**
	 * \brief Set the total number of 32 bit PCM samples.
	 *
	 * This also determines the leadout frame and the number of PCM bytes.
	 *
	 * \param[in] sample_count Total number of 32 bit PCM samples
	 *
	 * \throw std::underflow_error If value is negative
	 * \throw std::overflow_error  If value is greater than the unit maximum
	 */
	void set_total_samples(const int32_t sample_count);

	/**
	 * \brief Return the total number of bytes holding 32 bit PCM samples.
	 *
	 * Maximum value is CDDA::MAX_BLOCK_ADDRESS * CDDA::BYTES_PER_SAMPLE which
	 * is a value of 1.058.397.648 bytes on a disc.
	 *
	 * \return The total number of bytes holding 32 bit PCM samples
	 */
	int32_t total_pcm_bytes() const noexcept;

	/**
	 * \brief Set the total number of bytes holding decoded 32 bit PCM samples
	 *
	 * This also determines the leadout frame and the total number of 32 bit PCM
	 * samples.
	 *
	 * \param[in] byte_count Total number of bytes holding 32 bit PCM samples
	 *
	 * \throw std::underflow_error If value is negative
	 * \throw std::overflow_error  If value is greater than the unit maximum
	 */
	void set_total_pcm_bytes(const int32_t byte_count) noexcept;

	/**
	 * \brief Return \c TRUE if the AudioSize is 0.
	 *
	 * \return \c TRUE if the AudioSize is 0
	 */
	bool zero() const noexcept;

	/**
	 * \brief Return maximum size for the specified unit.
	 */
	static int32_t max(const UNIT unit) noexcept;

	/**
	 * \brief Return \c TRUE iff this AudioSize is zero(), otherwise \c FALSE.
	 *
	 * \return Return \c TRUE iff this AudioSize is zero(), otherwise \c FALSE.
	 */
	explicit operator bool() const noexcept;


	friend void swap(AudioSize& lhs, AudioSize& rhs) noexcept;

private:

	/**
	 * \brief Set the internal value, possibly converting from \c unit.
	 *
	 * \param[in] value Value to set
	 * \param[in] unit  Unit to convert to bytes from
	 */
	void set_value(const int32_t value, AudioSize::UNIT unit);

	/**
	 * \brief Get internal size in the specified unit.
	 *
	 * \param[in] unit  Unit to convert bytes amount to
	 */
	int32_t read_as(const AudioSize::UNIT unit) const;
};

bool operator == (const AudioSize& lhs, const AudioSize& rhs) noexcept;

bool operator  < (const AudioSize& lhs, const AudioSize& rhs) noexcept;


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
using IsSampleIterator =
	std::enable_if_t<is_iterator_over<Iterator, sample_t>::value>;


// forward declaration for operator == and binary ops
class SampleInputIterator; // IWYU pragma keep

bool operator == (const SampleInputIterator &lhs,
		const SampleInputIterator &rhs) noexcept;

SampleInputIterator operator + (SampleInputIterator lhs,
		const int32_t amount) noexcept;

SampleInputIterator operator + (const int32_t amount,
		SampleInputIterator rhs) noexcept;

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
			const SampleInputIterator &rhs) noexcept
	{
		return lhs.object_->equals(*rhs.object_);
	}

	friend SampleInputIterator operator + (SampleInputIterator lhs,
			const int32_t amount) noexcept
	{
		lhs.object_->advance(amount);
		return lhs;
	}

	// operator + (amount, rhs) is not a friend

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
		virtual void advance(const int32_t n) noexcept
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

		void advance(const int32_t n) noexcept final
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
	SampleInputIterator(const Iterator &i) // FIXME Do not move a const ref
		: object_ { std::make_unique<Model<Iterator>>(std::move(i)) }
	{
		// empty
	}

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to copy
	 */
	SampleInputIterator(const SampleInputIterator& rhs)
		: object_ { rhs.object_->clone() }
	{
		// empty
	}

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to move
	 */
	SampleInputIterator(SampleInputIterator&& rhs) noexcept
		: object_ { std::move(rhs.object_) }
	{
		// empty
	}

	/**
	 * \brief Destructor
	 */
	~SampleInputIterator() noexcept = default;

	/**
	 * \brief Dereferences the iterator to the sample pointed to.
	 *
	 * \return A sample_t sample, returned by value
	 */
	reference operator * () const noexcept // required by LegacyIterator
	{
		return object_->dereference();
	}

	/* *
	 * \brief Access members of the underlying referee
	 *
	 * \return A pointer to the underlying referee
	 */
	pointer operator -> () const noexcept; // required by LegacyInpuIterator
	// TODO implement

	/**
	 * \brief Pre-increment iterator.
	 *
	 * \return Incremented iterator
	 */
	SampleInputIterator& operator ++ () noexcept // required by LegacyIterator
	{
		object_->preincrement();
		return *this;
	}

	/**
	 * \brief Post-increment iterator.
	 *
	 * \return Iterator representing the state befor the increment
	 */
	SampleInputIterator operator ++ (int) noexcept
	{
		SampleInputIterator prev_val(*this);
		object_->preincrement();
		return prev_val;
	}
	// required by LegacyInputIterator

	/**
	 * \brief Copy assignment.
	 */
	SampleInputIterator& operator = (SampleInputIterator rhs) noexcept
	{
		swap(*this, rhs); // finds SampleInputIterator's friend swap via ADL
		return *this;
	}
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


/**
 * \brief Buffer for resulting checksums.
 */
using ChecksumBuffer = std::unordered_map<TrackNo, ChecksumSet>;


/**
 * \brief Checksum calculation algorithm.
 */
class Algorithm
{
public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~Algorithm() noexcept = default;

	/**
	 * \brief Update with a sequence of samples.
	 *
	 * \param[in] begin Iterator pointing to the first sample of the sequence
	 * \param[in] end   Iterator pointing behind the last sample of the sequence
	 */
	void update(SampleInputIterator begin, SampleInputIterator end);

	/**
	 * \brief Return the result of the algorithm.
	 *
	 * \return Calculation result.
	 */
	ChecksumSet result() const;

	/**
	 * \brief Types of checksums the algorithm calculates.
	 *
	 * \return Checksum types calculated by this algorithm
	 */
	std::vector<checksum::type> types() const;

private:

	/**
	 * \brief Update with a sequence of samples.
	 *
	 * \param[in] begin Iterator pointing to the first sample of the sequence
	 * \param[in] end   Iterator pointing behind the last sample of the sequence
	 */
	virtual void do_update(SampleInputIterator begin, SampleInputIterator end)
	= 0;

	/**
	 * \brief Return the result of the algorithm.
	 *
	 * \return Calculation result.
	 */
	virtual ChecksumSet do_result() const
	= 0;

	/**
	 * \brief Types of checksums the algorithm calculates.
	 *
	 * \return Checksum types calculated by this algorithm
	 */
	virtual std::vector<checksum::type> do_types() const
	= 0;
};


/**
 * \brief Current state of a Calculation.
 */
class CalculationState
{
	virtual ChecksumSet do_current_subtotal() const
	= 0;

	virtual void do_set_samples_expected(const int32_t total_expected)
	= 0;

	virtual int32_t do_samples_expected() const noexcept
	= 0;

	virtual int32_t do_samples_processed() const noexcept
	= 0;

	virtual std::chrono::milliseconds do_proc_time_elapsed() const noexcept
	= 0;

	virtual void do_update(SampleInputIterator start, SampleInputIterator stop)
	= 0;

	virtual void do_increment_proc_time_elapsed(
			const std::chrono::milliseconds amount)
	= 0;

	/**
	 * \brief Advance calculation state by the specified amount of samples.
	 *
	 * \param[in] amount Amount of samples to advance
	 */
	virtual void do_advance(const int32_t amount)
	= 0;

public:

	CalculationState() = default;

	explicit CalculationState(Algorithm* const algorithm);

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~CalculationState() noexcept = default;

	ChecksumSet current_subtotal() const;

	void set_samples_expected(const int32_t total_expected);

	/**
	 * \brief Returns the total number of initially expected PCM 32 bit samples.
	 *
	 * This value is equivalent to samples_processed() + samples_todo(). It will
	 * always remain constant for the given instance.
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples expected.
	 */
	int32_t samples_expected() const noexcept;

	/**
	 * \brief Returns the total number for PCM 32 bit samples yet processed.
	 *
	 * This value is equivalent to samples_expected() - samples_todo().
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	int32_t samples_processed() const noexcept;

	/**
	 * \brief Amount of milliseconds elapsed so far by processing.
	 *
	 * This includes the time of reading as well as of calculation.
	 *
	 * \return Amount of milliseconds elapsed so far by processing.
	 */
	std::chrono::milliseconds proc_time_elapsed() const noexcept;

	/**
	 * \brief Update the calculation state with an contigous amount of samples.
	 *
	 * \param[in] start First sample of update
	 * \param[in] stop  Sample behind last sample of update
	 */
	void update(SampleInputIterator start, SampleInputIterator stop);

	/**
	 * \brief Increment the amount of time elapsed.
	 *
	 * \param[in] amount Amount of milliseconds to advance
	 */
	void increment_proc_time_elapsed(const std::chrono::milliseconds amount);
};


/**
 * \brief Calculates checksums.
 */
class Calculation final
{
	class Impl;
	std::unique_ptr<Impl> impl_;

public:

	/**
	 * \brief Create a calculation instance.
	 *
	 * \param[in] algorithm The algorithm to use for calculating
	 * \param[in] toc       TOC to perform calculation for
	 * \param[in] size      Total size of the audio input
	 */
	Calculation(Algorithm& algorithm, const TOC& toc, const AudioSize& size);

	/**
	 * \brief Returns the algorithm used by this Calculation.
	 *
	 * \return Algorithm used by this Calculation.
	 */
	const Algorithm& algorithm() const noexcept;

	/**
	 * \brief Returns the types requested to this Calculation.
	 *
	 * Convenience function for <tt>mycalculation.algorithm().types()</tt>.
	 *
	 * \return All requested types.
	 */
	std::vector<checksum::type> types() const noexcept;

	/**
	 * \brief Returns the total number of initially expected PCM 32 bit samples.
	 *
	 * This value is equivalent to samples_processed() + samples_todo(). It will
	 * always remain constant for the given instance.
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples expected.
	 */
	int32_t samples_expected() const noexcept;

	/**
	 * \brief Returns the total number for PCM 32 bit samples yet processed.
	 *
	 * This value is equivalent to samples_expected() - samples_todo().
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples processed.
	 */
	int32_t samples_processed() const noexcept;

	/**
	 * \brief Returns the total number of PCM 32 bit samples that is yet to be
	 * processed.
	 *
	 * This value is equivalent to samples_expected() - samples_processed().
	 *
	 * Intended for debugging.
	 *
	 * \return Total number of PCM 32 bit samples yet to process.
	 */
	int32_t samples_todo() const noexcept;

	/**
	 * \brief Amount of milliseconds elapsed so far by processing.
	 *
	 * This includes the time of reading as well as of calculation.
	 *
	 * \return Amount of milliseconds elapsed so far by processing.
	 */
	std::chrono::milliseconds proc_time_elapsed() const noexcept;

	/**
	 * \brief Returns \c TRUE iff this Calculation is completed, otherwise
	 * \c FALSE.
	 *
	 * If the instance returns \c TRUE it is safe to call result(). Value
	 * \c FALSE indicates that the instance expects more updates.
	 *
	 * \return \c TRUE if the Calculation is completed, otherwise \c FALSE
	 */
	bool complete() const noexcept;

	/**
	 * \brief Update with a sequence of samples.
	 *
	 * \param[in] begin Iterator pointing to the first sample of the sequence
	 * \param[in] end   Iterator pointing behind the last sample of the sequence
	 */
	void update(SampleInputIterator begin, SampleInputIterator end);

	/**
	 * \brief Updates the instance with a new AudioSize.
	 *
	 * This can be done safely at any time before the last call of update().
	 *
	 * \param[in] audiosize The updated AudioSize
	 */
	void update_audiosize(const AudioSize &audiosize);

	/**
	 * \brief Acquire the resulting Checksums.
	 *
	 * \return The computed Checksums
	 */
	Checksums result() const noexcept;
};


/**
 * \brief Create a calculation.
 *
 * If the <tt>size</tt> is zero and <tt>toc</tt> is complete,
 * <tt>toc.leadout()</tt> is considered as the total audio size.
 *
 * \param[in] algorithm    The algorithm to use for calculating
 * \param[in] toc          TOC to perform calculation for
 * \param[in] size         Total size of the audio input
 */
std::unique_ptr<Calculation> make_calculation(Algorithm& algorithm,
		const TOC& toc, const AudioSize size);


/**
 * \brief Create a calculation.
 *
 * The TOC must be complete.
 *
 * \param[in] algorithm    The algorithm to use for calculating
 * \param[in] toc          Complete TOC to perform calculation for
 */
std::unique_ptr<Calculation> make_calculation(Algorithm& algorithm,
		const TOC& toc);

} // namespace v_1_0_0
} // namespace arcstk

#endif

