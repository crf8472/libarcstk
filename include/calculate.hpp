#ifndef __LIBARCSTK_CALCULATE_HPP__
#define __LIBARCSTK_CALCULATE_HPP__

/**
 * \file
 *
 * \brief Calculation interface.
 */

#include <cstdint>          // for int32_t
#include <memory>           // for unique_ptr
#include <unordered_set>    // for unordered_set

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"     // for ChecksumSet, Checksums
#endif
#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"     // for AudioSize, CDDA, ToC
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

// avoid includes
class ToC;


/** \defgroup calc Checksum calculation
 *
 * \brief Calculate checksums for audio files.
 *
 * \details
 *
 * SampleInputIterator is a wrapper iterator for any iterator with a
 * <tt>value_type</tt> of <tt>uint32_t</tt>.
 *
 * An Algorithm is a procedure to calculate Checksums over an input of audio
 * samples.
 *
 * A Calculation represents the process of calculating Checksums by an
 * Algorithm. It can be parametized with an Algorithm, initialized with the
 * offsets and the leadout of the audio image and then be  updated with portions
 * of samples. A Calculation can be also finetuned with Settings.
 *
 * The Context in which an Algorithm or a Calculation work. The Context
 * indicates if either the FIRST_TRACK, the LAST_TRACK or an entire ALBUM has to
 * be respected.
 *
 * @{
 */

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

SampleInputIterator operator + (SampleInputIterator lhs, const int32_t amount)
	noexcept;

SampleInputIterator operator + (const int32_t amount, SampleInputIterator rhs)
	noexcept;

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
 * \brief Calculation mode.
 */
enum class Context : unsigned char
{
	NONE        = 0, // shorthand for "!(FIRST_TRACK | LAST_TRACK)"
	FIRST_TRACK = 1,
	LAST_TRACK  = 2,
	ALBUM       = 3  // shorthand for "FIRST_TRACK | LAST_TRACK"
};

constexpr Context operator | (const Context lhs, const Context rhs);

constexpr Context operator | (const Context lhs, const Context rhs)
{
	return static_cast<Context>(
			static_cast<unsigned char>(lhs) | static_cast<unsigned char>(rhs));
}

// TODO Other bitwise operators for Context

constexpr bool operator == (const Context lhs, const Context rhs);

constexpr bool operator == (const Context lhs, const Context rhs)
{
	return static_cast<unsigned char>(lhs) == static_cast<unsigned char>(rhs);
}

/**
 * \brief Returns TRUE iff rhs != Context::NONE
 */
bool any(const Context& rhs) noexcept;


/**
 * \brief Settings for a Calculation.
 */
class Settings final
{
public:

	/**
	 * \brief Converting constructor.
	 *
	 * \param[in] c Context for a calculation
	 */
	Settings(const Context& c);

	/**
	 * \brief Set context for this algorithm.
	 *
	 * \param[in] c Context to set on this instance
	 */
	void set_context(const Context c);

	/**
	 * \brief Current context of this algorithm.
	 *
	 * \return Context of this instance
	 */
	Context context() const;

private:

	/**
	 * \brief Internal context, default is ALBUM.
	 */
	Context context_ { Context::ALBUM };
};


/**
 * \brief Set of checksum types.
 *
 * The set is iterable and duplicate-free.
 */
using ChecksumtypeSet = std::unordered_set<checksum::type>;


/**
 * \brief Type of a list of split points within a range of samples.
 */
using Points = std::vector<AudioSize>;


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

/**
 * \brief Checksum calculation algorithm interface.
 */
class Algorithm
{
public:

	/**
	 * \brief Default constructor.
	 */
	Algorithm();

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~Algorithm() noexcept;

	/**
	 * \brief Configure the algorithm with settings.
	 *
	 * \param[in] s Settings to use on this instance
	 */
	void set_settings(const Settings* s) noexcept;

	/**
	 * \brief Return the settings of this instance.
	 *
	 * \return Settings of this instance
	 */
	const Settings* settings() const noexcept;

	/**
	 * \brief Determine the legal range of samples for the calculation performed
	 * on the input amount.
	 *
	 * The algorithm may request to process only a part of the input - e.g. it
	 * may skip an amount of samples at the beginning and at the end.
	 *
	 * \param[in] size   The input size of samples to process
	 * \param[in] points The offset points in number of PCM samples
	 *
	 * \return Input range of 1-based sample indices to use for calculation.
	 */
	std::pair<int32_t,int32_t> range(const AudioSize& size,
			const Points& points) const;

	/**
	 * \brief Update with a sequence of samples.
	 *
	 * \param[in] start Iterator pointing to the first sample of the sequence
	 * \param[in] stop  Iterator pointing behind the last sample of the sequence
	 */
	void update(SampleInputIterator start, SampleInputIterator stop);

	/**
	 * \brief Mark current track as finished.
	 *
	 * What the instance has to do whenever a track is finished can be
	 * implemented in this hook.
	 *
	 * \param[in] t      Track number
	 * \param[in] length Track length as calculated
	 */
	void track_finished(const int t, const AudioSize& length);

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
	ChecksumtypeSet types() const;

	/**
	 * \brief Clone this instance..
	 *
	 * \return Deep copy of the instance
	 */
	std::unique_ptr<Algorithm> clone() const;

private:

	virtual void do_setup(const Settings* s)
	= 0;

	virtual std::pair<int32_t,int32_t> do_range(const AudioSize& size,
			const Points& points) const
	= 0;

	virtual void do_update(SampleInputIterator begin, SampleInputIterator end)
	= 0;

	virtual void do_track_finished(const int t, const AudioSize& length)
	= 0;

	virtual ChecksumSet do_result() const
	= 0;

	virtual ChecksumtypeSet do_types() const
	= 0;

	virtual std::unique_ptr<Algorithm> do_clone() const
	= 0;

	/**
	 * \brief Internal settings of the algorithm.
	 */
	const Settings* settings_;
};

#pragma GCC diagnostic pop


/**
 * \brief Calculates checksums.
 */
class Calculation final
{
public:

	/**
	 * \brief Create a calculation instance.
	 *
	 * If <tt>size.zero()</tt>, then first <tt>update()</tt> will throw.
	 *
	 * \param[in] settings  The settings for the calculation
	 * \param[in] algorithm The algorithm to use for calculating
	 * \param[in] size      Size of the expected input
	 * \param[in] points    Track offsets (as samples)
	 */
	Calculation(const Settings& settings, std::unique_ptr<Algorithm> algorithm,
			const AudioSize& size, const Points& points);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs Instance to be copied
	 */
	explicit Calculation(const Calculation& rhs);

	Calculation& operator=(const Calculation& rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs Instance to be moved
	 */
	explicit Calculation(Calculation&& rhs) noexcept;

	Calculation& operator=(Calculation&& rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~Calculation() noexcept;

	/**
	 * \brief Configure the algorithm with settings.
	 *
	 * \param[in] s Settings to use on this instance
	 */
	void set_settings(const Settings& s) noexcept;

	/**
	 * \brief Return the settings of this instance.
	 *
	 * \return Settings of this instance
	 */
	Settings settings() const noexcept;

	/**
	 * \brief Set the algorithm instance to use.
	 *
	 * Note that the algorithm is stateful and may therefore not be shared
	 * between calculations.
	 *
	 * \param[in] algorithm Algorithm to use on update
	 */
	void set_algorithm(std::unique_ptr<Algorithm> algorithm) noexcept;

	/**
	 * \brief Returns the algorithm instance used by this Calculation.
	 *
	 * \return Algorithm used by this Calculation.
	 */
	const Algorithm* algorithm() const noexcept;

	/**
	 * \brief Returns the types requested to this Calculation.
	 *
	 * Convenience function for <tt>mycalculation.algorithm().types()</tt>.
	 *
	 * \return All requested Checksum types.
	 */
	ChecksumtypeSet types() const noexcept;

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
	 * \param[in] start Iterator pointing to the first sample of the sequence
	 * \param[in] stop  Iterator pointing behind the last sample of the sequence
	 */
	void update(SampleInputIterator start, SampleInputIterator stop);

	/**
	 * \brief Updates the instance with a new AudioSize.
	 *
	 * This can be done safely at any time before the last call of update().
	 *
	 * \param[in] audiosize The updated AudioSize
	 */
	void update(const AudioSize &audiosize);

	/**
	 * \brief Acquire the resulting Checksums.
	 *
	 * \return The computed Checksums
	 */
	Checksums result() const noexcept;

private:

	// Private implementation
	class Impl;
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Create a calculation from a ToC.
 *
 * If the ToC is not complete, the Calculation must be updated with the correct
 * total number of input samples.
 *
 * \param[in] algorithm    The algorithm to use for calculating
 * \param[in] toc          Complete ToC to perform calculation for
 */
std::unique_ptr<Calculation> make_calculation(
		std::unique_ptr<Algorithm> algorithm, const ToC& toc);

/** @} */

} // namespace v_1_0_0
} // namespace arcstk

#endif

