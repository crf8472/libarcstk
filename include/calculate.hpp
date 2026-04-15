#ifndef LIBARCSTK_CALCULATE_HPP_
#define LIBARCSTK_CALCULATE_HPP_

/**
 * \file
 *
 * \brief Public API for \link calc AccurateRip checksum calculation\endlink.
 *
 * \details
 *
 * Provides the API for \link calc calculating AccurateRip checksums\endlink.
 */

#include <chrono>           // for duration
#include <cstddef>          // for ptrdiff_t
#include <cstdint>          // for int32_t
#include <iterator>         // for advance, input_iterator_tag
#include <memory>           // for make_unique, unique_ptr
#include <string>           // for string
#include <type_traits>      // for decay_t, enable_if_t, is_same, decay
#include <unordered_set>    // for unordered_set
#include <utility>          // for declval, move, pair
#include <vector>           // for vector

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"     // for ChecksumSet, Checksums
#endif
#ifndef LIBARCSTK_MIXINS_HPP_
#include "mixins.hpp"       // for Comparable
#endif

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
// avoid includes
class AudioSize;
class ToC;

// redefined as in metadata.hpp, documented there
using ToCData = std::vector<AudioSize>;


/**
 * \defgroup calc AccurateRip Checksum Calculation
 *
 * \brief Calculate checksums of audio tracks.
 *
 * \details
 *
 * AccurateRip checksums are calculated from a sequence of input sample
 * sequences. The caller is responsible for determining an Algorithm and
 * providing the entire sequence of audio sample sequences to a Calculation
 * instance. The result is provided in a Checksums instance.
 *
 * An Algorithm specifies a method to calculate Checksums over a sequence of
 * audio samples. AccurateRip specifies two distinct algorithms for calculating
 * a checksum, v1 and v2. A v1 checksum can be materialized as a subtotal when
 * calculating a v2 checksum. Therefore a calculation of a v2 value can also
 * provide the v1 value for the input. Hence there are three variants of the
 * Algorithm available: \e V1, \e V2 and \e V1and2. The latter provides v1 as
 * well as v2.
 *
 * Class Settings provides an interface for configuring an Algorithm or the
 * calculation process.
 *
 * As part of the Settings there exists a Context in which the Calculation is
 * performed. The Algorithm is aware of the Context. The Context indicates if
 * either FIRST_TRACK, LAST_TRACK, or both have to be treated specially when
 * calculating.
 *
 * A Calculation represents the technical process of calculating Checksums by an
 * Algorithm. It has to be parametized with an Algorithm, initialized with the
 * offsets and the leadout of the audio image and then subsequently be updated
 * with portions of samples in their correct order. A Calculation works fine
 * with the default settings, but can be finetuned by providing custom Settings.
 *
 * Updating a Calculation with an actual sequence of samples is done by
 * providing a sequence of samples represented by two iterators. Those iterators
 * represent start and stop of the update. Any LegacyInputIterator with a
 * \c value_type of sample_t is allowed. Type sample_t is the declared type for
 * PCM 32 bit samples. Using a SampleSequence may be of convenience for
 * establishing compatibility of the sample input format.
 *
 * As soon as a Calculation is
 * \link arcstk::Calculation::complete() complete() \endlink its result can be
 * provided. The resulting Checksums represent the result for all requested
 * checksum types and all tracks of the audio input. It is an aggregation of the
 * ChecksumSet for each track of an respective audio input. Depending on the
 * input, it can represent either an entire album or a single track.
 *
 * ChecksumSet is a set of \link arcstk::Checksum Checksums \endlink of
 * different \link arcstk::checksum::type checksum::types \endlink of
 * the same track.
 *
 * A Checksum refers to a particular track and a particular checksum::type.
 * Checksums are calculated by updating a Calculation with a sequence of sample
 * sequences.
 *
 * @{
 */

/**
 * \brief Represent a 32 bit wide PCM stereo sample.
 *
 * An unsigned integer of exact 32 bit length.
 *
 * The type is not intended to do arithmetic operations on it.
 *
 * Bitwise operators are guaranteed to work as on unsigned types.
 */
using sample_t = uint32_t; // also defined in samples.hpp


namespace details
{

/**
 * \brief Get value_type of Iterator.
 *
 * \tparam Iterator Iterator type to test
 */
template<typename Iterator>
using it_value_type = std::decay_t<decltype( *std::declval<Iterator>() )>;
// This is SFINAE compatible and respects bare pointers, which would not
// have been respected when using std::iterator_traits<Iterator>::value_type.
// Nonetheless I am not quite sure whether bare pointers indeed should be used
// in this context at the first place.

/**
 * \brief Check a given Iterator whether it iterates over type T.
 *
 * \tparam Iterator Iterator type to test
 * \tparam T        Type to test for
 */
template<typename Iterator, typename T>
using is_iterator_over = std::is_same< it_value_type<Iterator>, T >;

/**
 * \brief Defined iff \c Iterator is an iterator over \c sample_t.
 *
 * \tparam Iterator Iterator type to test
 */
template<typename Iterator>
using IsSampleIterator =
	std::enable_if_t<is_iterator_over<Iterator, sample_t>::value>;

} // namespace details


/**
 * \internal
 *
 * \brief Type erasing interface for LegacyInputIterators over 32 bit samples.
 *
 * Wraps the concrete iterator to be passed to
 * \link arcstk::Calculation::update() update \endlink a Calculation.
 * This allows to pass in fact iterators of any type to a Calculation.
 *
 * SampleInputIterator can wrap any iterator with a value_type of uint32_t
 * except instances of itself, e.g. it can not be "nested".
 *
 * The type erasure interface only ensures that the requirements of a
 * <A HREF="https://en.cppreference.com/w/cpp/named_req/InputIterator">
 * LegacyInputIterator</A> are met. Those requirements are sufficient for
 * \link arcstk::Calculation::update() updating \endlink a Calculation.
 *
 * Although SampleInputIterator is intended to provide the functionality of
 * an input iterator, it does not provide operator->() and does
 * therefore not completely fulfill the requirements for a LegacyInputIterator.
 *
 * SampleInputIterator provides iteration over values of type
 * \link arcstk::sample_t sample_t\endlink which is defined as a
 * primitve type. Since samples therefore do not have members, operator -> would
 * not provide any reasonable function.
 *
 * \see Calculation::update()
 */
class SampleInputIterator final : public Comparable<SampleInputIterator>
{
public:

	/**
	 * \brief LegacyInputIterator
	 *
	 * See <A HREF="https://en.cppreference.com/w/cpp/named_req/InputIterator">
	 * LegacyInputIterator</A>
	 */
	using iterator_category = std::input_iterator_tag;

	/**
	 * \copydoc SNPT_tp_value
	 */
	using value_type = sample_t;

	/**
	 * \copydoc SNPT_tp_reference
	 *
	 * \details Not an actual reference type.
	 *
	 * Note that some iterator types like SampleIterator for instance do not
	 * yield lvalues, therefor no reference to the value under the iterator is
	 * available.
	 */
	using reference = value_type;

	/**
	 * \copydoc SNPT_tp_pointer
	 */
	using pointer = const value_type*;

	/**
	 * \copydoc SNPT_tp_difference
	 */
	using difference_type = std::ptrdiff_t;

private:

	/**
	 * \brief Internal interface to the type-erased object.
	 */
	struct Concept
	{
		/**
		 * \copydoc SNPT_sm_default_dtor
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
		 * \copydoc SNPT_mf_equals
		 */
		virtual bool equals(const Concept& rhs) const noexcept
		= 0;

		/**
		 * \copydoc SNPT_mf_clone
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
			: iterator_ { iterator }
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

		bool equals(const Concept& rhs) const noexcept final
		{
			return iterator_ == static_cast<const Model&>(rhs).iterator_;
		}

		std::unique_ptr<Concept> clone() const noexcept final
		{
			return std::make_unique<Model>(*this);
		}

		friend void swap(Model& lhs, Model& rhs) noexcept
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

public:

	/**
	 * \brief Converting constructor.
	 *
	 * \tparam Iterator The iterator type to wrap
	 *
	 * \param[in] i Instance of an iterator over \c sample_t
	 */
	template <class Iterator, typename = details::IsSampleIterator<Iterator> >
	SampleInputIterator(const Iterator& i)
		: object_ { std::make_unique<Model<Iterator>>(i) }
	{
		// empty
	}

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	SampleInputIterator(const SampleInputIterator& rhs)
		: object_ { rhs.object_->clone() }
	{
		// empty
	}                                            // required by LegacyIterator

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	SampleInputIterator& operator = (const SampleInputIterator& rhs) noexcept
	{
		if (&rhs != this)
		{
			using std::swap;

			auto tmp = SampleInputIterator { rhs };
			swap(*this, tmp);
		}
		return *this;
	}                                            // required by LegacyIterator

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	SampleInputIterator(SampleInputIterator&& rhs) noexcept = default;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	SampleInputIterator& operator = (SampleInputIterator&& rhs) noexcept
	= default;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~SampleInputIterator() noexcept = default;   // required by LegacyIterator

	/**
	 * \copydoc SNPT_mf_deref
	 */
	reference operator * () const noexcept       // required by LegacyIterator
	{
		return object_->dereference();
	}

	/**
	 * \copydoc SNPT_mf_inc_prefix
	 */
	SampleInputIterator& operator ++ () noexcept // required by LegacyIterator
	{
		object_->preincrement();
		return *this;
	}

	/**
	 * \copydoc SNPT_mf_inc_postfix
	 */
	SampleInputIterator operator ++ (int) noexcept
	{
		SampleInputIterator prev_val(*this);
		object_->preincrement();
		return prev_val;
	}                                       // required by LegacyInputIterator


	/**
	 * \copydoc SNPT_nf_inc_amount_lhs
	 */
	friend SampleInputIterator operator + (SampleInputIterator lhs,
			const int32_t amount) noexcept
	{
		lhs.object_->advance(amount);
		return lhs;
	}

	/**
	 * \copydoc SNPT_nf_inc_amount_rhs
	 */
	friend SampleInputIterator operator + (const int32_t amount,
			SampleInputIterator rhs) noexcept
	{
		return rhs + amount;
	}

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(SampleInputIterator& lhs, SampleInputIterator& rhs)
		noexcept
	{
		using std::swap;

		swap(lhs.object_, rhs.object_);
	}                                            // required by LegacyIterator

	/**
	 * \copydoc SNPT_nf_equality
	 */
	friend bool operator == (const SampleInputIterator& lhs,
			const SampleInputIterator& rhs) noexcept
	{
		return lhs.object_->equals(*rhs.object_);
	}                                        // required by LegacyInputIterator

private:

	/**
	 * \brief Internal representation of wrapped object
	 */
	std::unique_ptr<Concept> object_;
};


/**
 * \brief Indicate the track context.
 *
 * AccurateRip algorithms imply different restrictions for calculating the
 * checksums of the the first and last track of an album. Context represents
 * this information.
 */
enum class Context : uint8_t
{
	/**
	 * \brief Single track that is neither first or last track.
	 */
	TRACK       = 0,

	/**
	 * \brief First track is first track of an album.
	 */
	FIRST_TRACK = 1,

	/**
	 * \brief Last track is last track of an album.
	 */
	LAST_TRACK  = 2,

	/**
	 * \brief Entire album, hence first as well as last track.
	 */
	ALBUM       = 3
};

/**
 * \brief Logical OR for two contexts.
 *
 * \param[in] lhs Left hand side
 * \param[in] rhs Right hand side
 *
 * \return Context that respresents the result of lhs-OR-rhs
 */
inline constexpr Context operator | (const Context lhs, const Context rhs)
{
	return static_cast<Context>(
			static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

/**
 * \brief Logical AND for two contexts.
 *
 * \param[in] lhs Left hand side
 * \param[in] rhs Right hand side
 *
 * \return Context that respresents the result of lhs-AND-rhs
 */
inline constexpr Context operator & (const Context lhs, const Context rhs)
{
	return static_cast<Context>(
			static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
}

/**
 * \brief Equality for two contexts.
 *
 * \param[in] lhs Left hand side
 * \param[in] rhs Right hand side
 *
 * \return TRUE if \c lhs equals \c rhs, otherwise FALSE
 */
inline constexpr bool operator == (const Context lhs, const Context rhs)
{
	return static_cast<unsigned>(lhs) == static_cast<unsigned>(rhs);
}

/**
 * \brief Swap two Context instances.
 *
 * \param[in] lhs Left hand side to swap
 * \param[in] rhs Right hand side to swap
 */
void swap(Context& lhs, Context& rhs) noexcept;

/**
 * \brief Name of the specified Context.
 *
 * \param[in] c Context to provide name of
 *
 * \return Name of context \c
 */
std::string name(const Context& c) noexcept;

/**
 * \brief String representation of a Context.
 *
 * This will return the name of the context. It is equivalent to name().
 *
 * \param[in] c Context to transform to a string
 *
 * \return String representation of context \c
 */
std::string to_string(const Context& c) noexcept;

/**
 * \brief Returns TRUE iff \c c is not equivalent to Context::TRACK.
 *
 * Equivalent to <code>c != Context::TRACK</code>.
 *
 * \param[in] c Context to evaluate
 *
 * \return TRUE iff \c c is not equivalent to Context::TRACK
 */
bool any(const Context& c) noexcept;


/**
 * \brief Settings for a Calculation.
 */
class Settings final : Equality<Settings>, Comparable<Settings>, Swap<Settings>
{
	/**
	 * \brief Internal context.
	 */
	Context context_;

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 *
	 * \details Initializes the Context of the Settings instance as ALBUM.
	 */
	Settings();

	/**
	 * \brief Constructor.
	 *
	 * \param[in] c Context for a calculation
	 */
	explicit Settings(const Context& c);

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

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(Settings& rhs) noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const Settings& rhs) const noexcept;

	/**
	 * \copydoc SNPT_mf_to_string
	 */
	std::string to_string() const;
};


/**
 * \brief Set of \link arcstk::checksum::type Checksum types \endlink.
 *
 * Guaranteed to be iterable and duplicate-free.
 */
using ChecksumtypeSet = std::unordered_set<checksum::type>;


/**
 * \brief List of split points within a range of samples.
 *
 * Guaranteed to be forward iterable and have operator [].
 */
using Points = std::vector<AudioSize>;


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
// -Weffc++ is deactivated: warns about raw pointer member settings_
// The member is non-owning. Default copy + move is therefore ok. Rule of zero.

/**
 * \brief Interface: Checksum calculation algorithm.
 *
 * An Algorithm instance can be updated with new input by the caller and
 * provides the result after the last update. The calculation of tracks is to be
 * finished manually by calling track_finished(). Algorithm instances hold the
 * concrete subtotals.
 *
 * The caller is required to instantiate and setup an Algorithm. However, it
 * should usually not be required to update the Algorithm instance directly.
 * This is performed via a Calculation.
 */
class Algorithm
{
public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	Algorithm();

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	virtual ~Algorithm() noexcept = default;

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
	 * \param[in] trackno Track number
	 * \param[in] length  Track length as calculated
	 */
	void track_finished(const int trackno, const AudioSize& length);

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
	 * \copydoc SNPT_mf_clone
	 */
	std::unique_ptr<Algorithm> clone() const;

protected:

	/**
	 * \brief Implementation of swap for the base class.
	 *
	 * This is to be called by swap() implementations for subclasses.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap_base(Algorithm& rhs);

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
	const Settings* settings_; // non-owning
};

#pragma GCC diagnostic pop


/**
 * \brief Perform checksums calculation.
 *
 * A Calculation represents a concrete checksum calculation process. It is
 * manually performed by the caller by calling update().
 *
 * Calculation instances must be initialized with the specific size of the input
 * audio file and an Algorithm that defines the type of the checksums. If
 * multiple tracks e.g. an entire disc content is to be processed, the ToC
 * information of the disc is required. Additionally, a Settings instance can be
 * specified. Currently, the only supported Settings attribute is Context.
 *
 * The input of the audio file must be represented as a succession of iterable
 * \link arcstk::SampleSequence SampleSequences \endlink and the
 * Calculation is to be sequentially updated with these sequences in order.
 * After the last update, the Calculation returns the calculation result on
 * request. The calculated Checksums are represented as an iterable aggregate of
 * \link arcstk::ChecksumSet ChecksumSets \endlink.
 *
 * \see make_calculation
 */
class Calculation final : Swap<Calculation>
{
	class Impl;
	std::unique_ptr<Impl> impl_;

public:

	/**
	 * \brief Constructor.
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
	 * \brief Constructor.
	 *
	 * \param[in] settings  The settings for the calculation
	 * \param[in] algorithm The algorithm to use for calculating
	 * \param[in] toc       Track offsets and leadout
	 */
	Calculation(const Settings& settings, std::unique_ptr<Algorithm> algorithm,
			const ToCData& toc);

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	Calculation(const Calculation& rhs);

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	Calculation& operator = (const Calculation& rhs);

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	Calculation(Calculation&& rhs) noexcept;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	Calculation& operator = (Calculation&& rhs) noexcept;

	/**
	 * \copydoc SNPT_sm_default_dtor
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
	 * \brief Amount of time elapsed so far by update().
	 *
	 * \return Amount of time elapsed so far by update().
	 */
	std::chrono::duration<float> update_time_elapsed() const noexcept;

	/**
	 * \brief Amount of time elapsed so far by the algorithm instance.
	 *
	 * \return Amount of time elapsed so far by the algorithm instance.
	 */
	std::chrono::duration<float> algo_time_elapsed() const noexcept;

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
	 * \brief Update the instance with a new AudioSize.
	 *
	 * This can be done safely at any time before the last call of update().
	 *
	 * \param[in] audiosize The updated AudioSize
	 */
	void update(const AudioSize& audiosize);

	/**
	 * \brief Acquire the resulting Checksums.
	 *
	 * \return The computed Checksums
	 */
	Checksums result() const noexcept;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(Calculation& rhs) noexcept;
};


/**
 * \brief Create a Calculation from an Algorithm and a ToC.
 *
 * If the ToC is not \link arcstk::ToC::complete complete \endlink,
 * the Calculation must be updated with the correct
 * total number of input samples before calling Calculation::update().
 *
 * \param[in] algorithm The algorithm to use for calculating
 * \param[in] toc       Complete ToC to perform calculation for
 *
 * \return Calculation object using \c algorithm and \c toc.
 */
std::unique_ptr<Calculation> make_calculation(
		std::unique_ptr<Algorithm> algorithm, const ToC& toc);

/** @} */

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

