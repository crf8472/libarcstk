#ifndef __LIBARCSTK_CHECKSUM_HPP___
#define __LIBARCSTK_CHECKSUM_HPP___

/**
 * \file
 *
 * \brief
 */

#include <array>    // for array
#include <cstdint>  // for uint32_t, int32_t
#include <map>      // for map
#include <memory>   // for unique_ptr
#include <set>      // for set
#include <vector>   // for vector

#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"
#endif

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"  // for lba_count_t
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

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
 * The order of the types is identical to the total order of numeric values the
 * types have in enum class checksum::type.
 */
static const std::array<type, 2> types = {
	type::ARCS1,
	type::ARCS2
	// type::THIRD_TYPE,
	// type::FOURTH_TYPE ...
};


/**
 * \brief Obtain the name of a checksum::type.
 *
 * \param[in] t Type to get name of
 *
 * \return Name of type \c t
 */
std::string type_name(const type t);

} // namespace checksum


// forward declaration for operator ==
class Checksum; // IWYU pragma keep

/**
 * \internal
 * \brief Overload operator == for comparing Checksums.
 *
 * \param[in] lhs Left hand side of the comparison
 * \param[in] rhs Right hand side of the comparison
 *
 * \return \c TRUE iff lhs.value() == rhs.value() or lhs and rhs are empty()
 */
bool operator == (const Checksum &lhs, const Checksum &rhs) noexcept;

/**
 * \internal
 * \brief Overload operator << for outputting Checksums.
 */
std::ostream& operator << (std::ostream& out, const Checksum &c);

/**
 * \brief An AccurateRip checksum for a single file or track.
 *
 * A Checksum has a value_type. This is its numeric representation. It is an
 * unsigned integer of 32 bit length.
 *
 * A Checksum can be represented by its numeric value() which is of type
 * value_type. A Checksum can be compared for equality with instances of its
 * value_type using operator ==.
 *
 * A Checksum has a converting constructor for its value_type, thus every
 * parameter that expects a checksum can be assigned a value of type value_type
 * instead of a Checksum. Some compilers will do the conversion with other
 * numerical types but issue a warning if a conversion from signed to unsigned
 * types is required (e.g. -Wsign-conversion).
 *
 * Operator << is overloaded for printing Checksums to streams. The Checksum
 * will then occurr in its standard layout: as a hexadecimal number without the
 * base '0x', all digits in upper case, and filled with leading zeros up to a
 * width of 8 digits.
 *
 * As a technical convenience, a Checksum may be empty() which means: it carries
 * no value. Calling value() on an empty() Checksum may lead any result. Two
 * empty Checksum instances qualify as equal when compared using operator ==.
 */
class Checksum final : public Comparable<Checksum>
{
public:

	/**
	 * \brief Numerical base type of checksums: a 32-bit wide unsigned integer.
	 */
	using value_type = uint32_t;

	/**
	 * \brief Constructor.
	 *
	 * Creates an empty Checksum.
	 */
	Checksum();

	/**
	 * \brief Converting constructor.
	 *
	 * \param[in] value Actual checksum value
	 */
	Checksum(const value_type value);

	/**
	 * \brief Numeric value of the checksum.
	 *
	 * \return Numeric value of the checksum
	 */
	value_type value() const noexcept;

	/**
	 * \brief Return \c TRUE iff this Checksum is empty, otherwise \c FALSE.
	 *
	 * A Checksum is empty if it contains no valid value. Note that this
	 * does not guarantee <tt>value() == 0</tt>.
	 *
	 * \return Return \c TRUE iff this Checksum is empty, otherwise \c FALSE.
	 */
	bool empty() const noexcept;


	Checksum& operator = (const value_type rhs);

private:

	/**
	 * \brief Actual checksum value;
	 */
	value_type value_;
};

namespace details
{

/**
 * \internal Get the iterator or const_iterator type of a container type.
 *
 * \tparam C        Container type to get iterator types from
 * \tparam is_const If TRUE, get C::const_iterator, otherwise C::iterator
 */
template <typename C, bool is_const>
using IteratorType = typename std::conditional<is_const,
			typename C::const_iterator,
			typename C::iterator
		>::type;


/**
 * \brief Functor to wrap an existing iterator.
 *
 * \tparam I Type of the iterator instance to wrap
 * \tparam T Value type whose iterator should be used for wrapping
 * \tparam is_const If TRUE, construct a const_iterator, otherwise not
 */
template<typename I, typename T, bool is_const>
class MakeIterator
{
public:

	using type = IteratorType<T, is_const>;

	auto operator()(I&& iterator_instance) const -> type
	{
		return type(std::forward<I>(iterator_instance));
	}
};


/**
 * \brief Abstract base class for a wrapping iterator.
 *
 * \tparam I        Iterator type to wrap.
 * \tparam is_const If TRUE create a const_iterator
 */
template<typename I, bool is_const>
class IteratorWrapper
{
public:

	using value_type = typename I::value_type;

	using difference_type   = std::ptrdiff_t;

	using pointer   = typename std::conditional<is_const,
		const value_type*, value_type*>::type;

	using reference = typename std::conditional<is_const,
		const value_type&, value_type&>::type;

	virtual ~IteratorWrapper() noexcept = default;

	// Assign a non-const iterator to a const_interator.
	IteratorWrapper& operator = (const IteratorWrapper<I, false> &rhs)
	{
		it_ = rhs.it_;
		return *this;
	}

	IteratorWrapper& operator ++ () // prefix increment
	{
		++it_;
		return *this;
	}

	IteratorWrapper operator ++ (int) // postfix increment
	{
		IteratorWrapper tmp { *this };
		++(*this);
		return tmp;
	}

	IteratorWrapper& operator -- () // prefix decrement
	{
		--it_;
		return *this;
	}

	IteratorWrapper operator -- (int) // postfix decrement
	{
		IteratorWrapper tmp { *this };
		--(*this);
		return tmp;
	}

	friend bool operator == (const IteratorWrapper& lhs,
			const IteratorWrapper& rhs)
	{
		return lhs.it_ == rhs.it_;
	}

	friend void swap(const IteratorWrapper& lhs,
			const IteratorWrapper& rhs)
	{
		using std::swap;
		swap(lhs.it_, rhs.it_);
	}

protected:

	// Called by subclass
	explicit IteratorWrapper(const I& it)
		: it_ { it }
	{
		// empty
	}

	auto wrapped_iterator() -> I&
	{
		return it_;
	}

private:

	I it_;
};


/**
 * \internal
 *
 * \brief Input iterator for \link ChecksumMap ChecksumMaps \endlink.
 *
 * ChecksumMapIterator is not default constructible since it wraps another
 * iterator instance. Therefore it does not satisfy all requirements for an
 * input iterator.
 *
 * ChecksumMapIterator can be preincremented and pre-decremented.
 *
 * Equality between const_iterator and iterator variants works like expected.
 *
 * \tparam K        The key type of the iterated ChecksumMap
 * \tparam is_const \c TRUE indicates a const_iterator
 */
template <typename K, bool is_const>
class ChecksumMapIterator : public Comparable<ChecksumMapIterator<K, is_const>>
{
	// Befriend the converse version of the type: const_iterator can access
	// private members of iterator (and vice versa)
	friend ChecksumMapIterator<K, not is_const>;

	// Exclusively construct iterators by their private constructor
	template<typename, typename, bool> friend class MakeIterator;

public:

	using value_type = Checksum;

	using difference_type   = std::ptrdiff_t;

	using pointer           = typename std::conditional<is_const,
			const Checksum*, Checksum*>::type;

	using reference         = typename std::conditional<is_const,
			const Checksum&, Checksum&>::type;

	/**
	 * \brief Iterator category
	 *
	 * A ChecksumMapIterator has only std::input_iterator_tag since to any
	 * higher-level iterator tag it would have to be default constructible.
	 */
	using iterator_category = std::input_iterator_tag;

	/**
	 * \brief Construct const_iterator from iterator
	 *
	 * \param[in] rhs The iterator to construct a const_iterator
	 */
	ChecksumMapIterator(const ChecksumMapIterator<K, false> &rhs)
		: it_ { rhs.it_ }
	{
		// empty
	}

	/**
	 * \brief Dereference operator
	 *
	 * \return A Checksum
	 */
	reference operator * ()
	{
		return (*it_).second;
	}

	/**
	 * \brief Dereference operator
	 *
	 * \return A Checksum
	 */
	pointer operator -> ()
	{
		return &it_->second;
	}


	ChecksumMapIterator& operator ++ ()
	{
		++it_;
		return *this;
	}


	ChecksumMapIterator& operator -- ()
	{
		--it_;
		return *this;
	}


	ChecksumMapIterator& operator = (const ChecksumMapIterator<K, false> &rhs)
	{
		it_ = rhs.it_;
		return *this;
	}


	friend bool operator == (const ChecksumMapIterator &lhs,
			const ChecksumMapIterator &rhs)
	{
		return lhs.it_ == rhs.it_;
	}


	friend void swap(const ChecksumMapIterator &lhs,
			const ChecksumMapIterator &rhs)
	{
		using std::swap;

		swap(lhs.it_, rhs.it_);
	}

private:

	/**
	 * \brief Type of the container to iterate.
	 */
	using IteratedContainerType = typename std::map<K, value_type>;

	/**
	 * \brief Type of the container's iterator to wrap.
	 */
	using WrappedIteratorType = typename std::conditional<is_const,
			typename IteratedContainerType::const_iterator,
			typename IteratedContainerType::iterator
		>::type;

	/**
	 * \brief Private Constructor.
	 *
	 * Constructs a ChecksumMapIterator from the iterator of the
	 * wrapped type.
	 *
	 * This constructor is private since ChecksumMap<> instantiates
	 * its iterators exclusively.
	 *
	 * \param[in] it iterator of the wrapped type
	 */
	explicit ChecksumMapIterator(const WrappedIteratorType& it)
		: it_ { it }
	{
		// empty
	}

	/**
	 * \brief Wrapped iterator of the class implementing ChecksumMap
	 */
	WrappedIteratorType it_;
};

} // namespace details


class ChecksumSet; // IWYU pragma keep

//forward declaration
bool operator == (const ChecksumSet &lhs, const ChecksumSet &rhs);

/**
 * \brief A set of Checksum instances of different types for a single track.
 *
 * The ChecksumSet represents the calculation result for a single track.
 */
class ChecksumSet final : public Comparable<ChecksumSet>
{
public:

	using value_type = Checksum;

	using const_iterator = details::ChecksumMapIterator<checksum::type, true>;

	using iterator = details::ChecksumMapIterator<checksum::type, false>;

	using size_type = std::size_t;

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
	explicit ChecksumSet(const lba_count_t length);

	/**
	 * \brief Default destructor
	 */
	~ChecksumSet() noexcept;

	/**
	 * \brief Returns the number of elements contained in the instance.
	 *
	 * \return Number of elements contained in the instance.
	 */
	size_type size() const;

	/**
	 * \brief Returns \c TRUE iff the instance contains no elements, otherwise
	 * \c FALSE.
	 *
	 * \return \c TRUE iff instance contains no elements, otherwise \c FALSE
	 */
	bool empty() const;

	/**
	 * \brief Obtain a pointer to the first Checksum.
	 *
	 * \return Pointer to the first Checksum
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Obtain a pointer pointing behind the last Checksum.
	 *
	 * \return Pointer pointing behind the last Checksum
	 */
	const_iterator cend() const;

	/**
	 * \copydoc cbegin()
	 */
	const_iterator begin() const;

	/**
	 * \copydoc cend()
	 */
	const_iterator end() const;

	/**
	 * \brief Obtain a pointer to the first Checksum.
	 *
	 * \return Pointer to the first Checksum
	 */
	iterator begin();

	/**
	 * \brief Obtain a pointer pointing behind the last Checksum.
	 *
	 * \return Pointer pointing behind the last Checksum
	 */
	iterator end();

	/**
	 * \brief Length (in LBA frames) of this track.
	 *
	 * \return Length of this track in LBA frames
	 */
	lba_count_t length() const noexcept;

	/**
	 * \brief Returns \c TRUE iff the instance contains a Checksum of the type
	 * \c type.
	 *
	 * \param[in] type The type to lookup the Checksum for
	 *
	 * \return \c TRUE iff \c type is present in the instance, otherwise \c FALSE
	 */
	bool contains(const checksum::type &type) const;

	/**
	 * \brief Return the \ref Checksum for the specified \c type
	 *
	 * If there is no Checksum represented for the \c type, the Checksum
	 * returned will be \link Checksum::empty() empty()\endlink.
	 *
	 * \param[in] type The checksum::type to return the value
	 *
	 * \return The checksum for the specified type
	 */
	Checksum get(const checksum::type type) const;

	/**
	 * \brief Return the checksum types present in this ChecksumSet
	 *
	 * \return The checksum types in this ChecksumSet
	 */
	std::set<checksum::type> types() const;

	/**
	 * \brief Inserts a new <type,Checksum> pair to the instance.
	 *
	 * If the key is already present in the instance, the existing checksum will
	 * be left unmodified.
	 *
	 * The pair returned contains an iterator to the inserted Checksum and a
	 * bool that is \c TRUE iff the insertion was successful. If the insertion
	 * was not successful, the value \c FALSE is returned for the bool and the
	 * iterator will point to the element that prevented the insertion.
	 *
	 * \param[in] type     The key to use
	 * \param[in] checksum The checksum for the given key
	 *
	 * \return Pair with an iterator to the inserted value and a status flag
	 */
	std::pair<iterator, bool> insert(const checksum::type type,
			const Checksum &checksum);

	/**
	 * \brief Merge the elements of another instance into this instance.
	 *
	 * If a key in the other instance is already present in this instance, the
	 * corresponding element will be left unmodified.
	 *
	 * Trying to merge a set with non-zero length into an instance with non-zero
	 * length of different value will cause a std::domain_error. If either
	 * \c rhs or \c this has zero length, the merge will succeed.
	 *
	 * \param[in] rhs The list to be merged into the instance
	 *
	 * \throws domain_error If \c rhs.length() != \c length() and both are != 0
	 */
	void merge(const ChecksumSet &rhs);

	/**
	 * \brief Erases the Checksum with the given type.
	 *
	 * Iff the given type is not contained in the instance, the call has
	 * no effect.
	 *
	 * \param[in] type The type to erase
	 */
	void erase(const checksum::type &type);

	/**
	 * \brief Erases all \link Checksum Checksums\endlink contained in the
	 * instance.
	 *
	 * After clear() has been called the size of the container will be \c 0 .
	 */
	void clear();


	ChecksumSet& operator = (const ChecksumSet &rhs);

	friend bool operator == (const ChecksumSet &lhs, const ChecksumSet &rhs);


private:

	// forward declaration for ChecksumSet::Impl
	class Impl;

	/**
	 * \brief Private implementation of ChecksumSet
	 */
	std::unique_ptr<Impl> impl_;
};


// forward declaration for operator == and swap()
class Checksums; // IWYU pragma keep


namespace details
{

/**
 * \internal Iterator type for non-associative object containers.
 *
 * \tparam C         Non-associative container type to iterate over
 * \tparam is_const  If TRUE, make instance a const_iterator, otherwise not
 */
template <typename C, bool is_const, typename I = IteratorType<C, is_const>>
class ObjectIterator :	public IteratorWrapper<I, is_const>,
						public Comparable<ObjectIterator<C, is_const>>
{
	// Befriend the converse version of the type: const_iterator can access
	// private members of iterator (and vice versa)
	friend ObjectIterator<C, not is_const>;

	// Exclusively construct iterators by their private constructor
	template<typename, typename, bool> friend class MakeIterator;

	// Declaration required
	using IteratorWrapper<I, is_const>::wrapped_iterator;

public:

	/**
	 * \brief Value type of this iterator.
	 */
	using value_type = typename C::value_type;

	using reference = typename IteratorWrapper<I, is_const>::reference;
	using pointer   = typename IteratorWrapper<I, is_const>::pointer;

	/**
	 * \brief Iterator category
	 *
	 * A ObjectIterator has only std::input_iterator_tag since to any
	 * higher-level iterator tag it would have to be default constructible.
	 */
	using iterator_category = std::input_iterator_tag;

	/**
	 * \brief Dereference operator
	 *
	 * \return A ChecksumSet representing a track
	 */
	reference operator * ()
	{
		return *wrapped_iterator();
	}

	/**
	 * \brief Dereference operator
	 *
	 * \return A ChecksumSet representing a track
	 */
	pointer operator -> ()
	{
		return &wrapped_iterator();
	}

private:

	explicit ObjectIterator(const I& it)
		: IteratorWrapper<I, is_const>{ it }
	{
		// empty
	}
};

} // namespace details


void swap(Checksums &lhs, Checksums &rhs) noexcept;

bool operator == (const Checksums &lhs, const Checksums &rhs) noexcept;

/**
 * \brief The result of a Calculation, an iterable list of
 * \link ChecksumSet ChecksumSets \endlink.
 *
 * A Checksums instance represents all calculated checksums of an input, i.e. an
 * album or a track list.
 */
class Checksums final
{
public:

	using value_type = ChecksumSet;

private:

	using storage_type = std::vector<value_type>;

public:

	using iterator = details::ObjectIterator<storage_type, false>;

	using const_iterator = details::ObjectIterator<storage_type, true>;

	using size_type = std::size_t;

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

	Checksums& operator = (Checksums rhs);

	/**
	 * \brief Move constructor.
	 *
	 * \param[in] rhs The Checksums to move
	 */
	Checksums(Checksums &&rhs) noexcept;

	Checksums& operator = (Checksums &&rhs) noexcept;

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
	 * \brief Obtain a pointer to the first ChecksumSet.
	 *
	 * \return Pointer to the first ChecksumSet
	 */
	iterator begin();

	/**
	 * \brief Obtain a pointer pointing behind the last ChecksumSet.
	 *
	 * \return Pointer pointing behind the last ChecksumSet
	 */
	iterator end();

	/**
	 * \copydoc cbegin()
	 */
	const_iterator begin() const;

	/**
	 * \copydoc cend()
	 */
	const_iterator end() const;

	/**
	 * \brief Obtain a const_iterator pointing to first ChecksumSet.
	 *
	 * \return const_iterator pointing to first ChecksumSet
	 */
	const_iterator cbegin() const;

	/**
	 * \brief Obtain a const_iterator pointing behind last ChecksumSet.
	 *
	 * \return const_iterator pointing behind last ChecksumSet
	 */
	const_iterator cend() const;

	/**
	 * \brief The ChecksumSet with the specified 0-based index \c index.
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
	 * \throws std::out_of_range Iff \c index >= Checksums::size()
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
	 * \brief Return the total number of elements.
	 *
	 * \return Total number of elements
	 */
	size_type size() const noexcept;


	friend bool operator == (const Checksums &lhs, const Checksums &rhs)
		noexcept;

	friend void swap(Checksums &lhs, Checksums &rhs) noexcept;

private:

	/**
	 * \brief Private implementation of Checksums.
	 */
	std::unique_ptr<Checksums::Impl> impl_;
};


/**
 * \brief Global instance of an empty Checksum.
 *
 * This is for convenience since in most cases, the creation of an empty
 * Checksum can be avoided when a reference instance is at hand.
 *
 * This instance defines emptyness for checksums since Checksum::empty()
 * just compares the instance with this instance.
 */
extern const Checksum EmptyChecksum;


} // namespace v_1_0_0
} // namespace arcstk

#endif

