#ifndef LIBARCSTK_CHECKSUM_HPP_
#define LIBARCSTK_CHECKSUM_HPP_

/**
 * \file
 *
 * \brief Representation for \link calc checksums\endlink, their aggregates and
 * their types.
 *
 * \details
 *
 * Part of the API for \link calc calculating AccurateRip checksums\endlink.
 */

#include <array>            // for array
#include <climits>          // for CHAR_BIT
#include <cstdint>          // for int32_t, uint32_t
#include <initializer_list> // for initializer_list
#include <ios>              // for fmtflags
#include <ostream>          // for ostream
#include <set>              // for set
#include <unordered_map>    // for unordered_map
#include <utility>          // for pair
#include <string>           // for string
#include <vector>           // for vector

#ifndef LIBARCSTK_MIXINS_HPP_
#include "mixins.hpp"       // for Comparable
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"     // for AudioSize
#endif

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
class Checksum; // forward declaration

/** \addtogroup calc */
/** @{ */

/**
 * \brief Operations on checksum types and their names.
 */
namespace checksum
{

/**
 * \brief Pre-defined checksum types.
 *
 * ARCS1 is AccurateRip v1, ARCS2 is AccurateRip v2, and
 * F4501 is AccurateRip v1 of frame 450.
 */
enum class type : uint8_t
{
	ARCS1   = 1,
	ARCS2   = 2,
	F4501   = 4,
	//FOURTH_TYPE = 8,
	//FIFTH_TYPE = 16 ...
};


/**
 * \brief Iterable sequence of all predefined checksum types.
 *
 * The order of the types is identical to the total order of numeric values the
 * types have in enum class checksum::type.
 */
inline constexpr std::array<type, 3> types = {
	type::ARCS1,
	type::ARCS2,
	type::F4501
};


/**
 * \brief Obtain the name of a checksum::type.
 *
 * \param[in] t Type to get name of
 *
 * \return Name of type \c t
 */
std::string name(const type t);


/**
 * \copydoc SNPT_nf_stream_in
 */
inline std::ostream& operator << (std::ostream& out, const type i)
{
	if (!out.good())
	{
		return out;
	}

	out << name(i);
	return out;
}


/**
 * \brief Print a checksum value to a stream.
 *
 * This defines how to print a Checksum by default.
 *
 * It provides the default implementation of Checksum::operator << and
 * Checksum::to_string().
 *
 * \param[in] stream   The stream to print to
 * \param[in] checksum Checksum to print
 */
void print(std::ostream& stream, const Checksum& checksum);

} // namespace checksum

/**
 * \brief A checksum for a single file or track.
 *
 * \details
 *
 * A Checksum has a value_type of an unsigned integer of 32 bit length.
 *
 * A Checksum can be represented by its numeric value(). A Checksum can be
 * compared for equality with instances of its value_type using operator ==.
 * As a technical convenience, a Checksum is zero() if its value() is 0. In this
 * case it is converted by operator bool() to FALSE. Any Checksum that is not
 * zero() converts to TRUE.
 *
 * Operator << is overloaded for printing Checksums to streams. The Checksum
 * will then occur in its standard layout: as a hexadecimal number without the
 * base '0x', all digits in upper case, and filled with leading zeros up to a
 * width of 8 digits.
 */
class Checksum final : Equality<Checksum>, Comparable<Checksum>, Swap<Checksum>,
						ToString<Checksum>
{
public:

	/**
	 * \copydoc SNPT_tp_value
	 *
	 * \details Numerical type of checksums: an unsigned 32-bit integer.
	 */
	using value_type = uint32_t;

	/**
	 * \brief Total number of printed hexadecimal digits of an ARCS.
	 */
	constexpr static std::size_t TOTAL_HEX_DIGITS =
		(sizeof(Checksum::value_type) * CHAR_BIT) / 4;
	// 4 bits are required to represent a single hexadecimal digit
	// (since 2^4 == 16). We could express 4 as log_2(16).

	/**
	 * \copydoc SNPT_sm_default_ctor
	 *
	 * \details Creates an empty Checksum.
	 */
	Checksum()
		: Checksum { 0 }
	{
		// empty
	}

	/**
	 * \brief Converting constructor for value_type instances.
	 *
	 * \param[in] value Actual checksum value
	 */
	explicit Checksum(const value_type value);

	/**
	 * \brief Assignment operator to assign value_type values
	 *
	 * \param[in] rhs Actual checksum value
	 *
	 * \return Reference to instance
	 */
	Checksum& operator = (const value_type rhs);

	/**
	 * \brief Numeric value of the checksum.
	 *
	 * \return Numeric value of the checksum
	 */
	value_type value() const noexcept;

	/**
	 * \copydoc SNPT_mf_zero
	 */
	bool zero() const noexcept;

	/**
	 * \copydoc SNPT_mf_op_bool_if_zero
	 */
	explicit operator bool() const noexcept;

	/**
	 * \brief Convert this instance to an equivalent raw \c value_type.
	 *
	 * \return Raw value
	 */
	explicit operator value_type() const noexcept;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(Checksum& rhs) noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const Checksum& rhs) const noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals_value(const value_type rhs) const noexcept;

	/**
	 * \copydoc SNPT_mf_to_string
	 */
	std::string to_string() const;

	/**
	 * \copydoc SNPT_nf_stream_in
	 */
	friend std::ostream& operator << (std::ostream& out, const Checksum& i)
	{
		checksum::print(out, i);
		return out;
	}

private:

	/**
	 * \brief Actual checksum value;
	 */
	value_type value_ {};
};


class ChecksumSet; // forward declaration

// ensure to put declaration in this namespace
std::ostream& operator << (std::ostream& out, const ChecksumSet& i);

/**
 * \brief A set of Checksum instances of different types for a single track.
 *
 * The ChecksumSet represents the calculation result for a single track. It also
 * holds optionally the track length as number of LBA frames for convenience.
 * The length may be zero which counts as "unknown".
 */
class ChecksumSet final : Equality<ChecksumSet>, Comparable<ChecksumSet>,
						  Swap<ChecksumSet>, ToString<ChecksumSet>
{
public:

	/**
	 * \brief Value type of the ChecksumSet.
	 */
	using value_type = Checksum;

private:

	/**
	 * \brief Track length as number of LBA frames.
	 */
	AudioSize length_ {};

	/**
	 * \brief Key type of the internal type map.
	 */
	using key_type = checksum::type;

	/**
	 * \brief Type of the internal storage of the ChecksumSet.
	 */
	using storage_type = std::unordered_map<key_type, value_type>;

	/**
	 * \brief Internal storage of the ChecksumSet.
	 */
	storage_type set_ {};

	// intentionally undocumented
	using unspecified_forward_iterator_type = storage_type::iterator;

	// intentionally undocumented
	using unspecified_constant_forward_iterator_type =
		storage_type::const_iterator;

	// intentionally undocumented
	using unspecified_unsigned_size_type = storage_type::size_type;

public:

	/**
	 * \brief Unspecified forward iterator type.
	 */
	using iterator = unspecified_forward_iterator_type;

	/**
	 * \brief Unspecified forward iterator type.
	 */
	using const_iterator = unspecified_constant_forward_iterator_type;

	/**
	 * \brief Size type (unsigned integral type)
	 */
	using size_type = unspecified_unsigned_size_type;

	/**
	 *
	 * \copydoc SNPT_sm_default_ctor
	 *
	 * \details Constructs a ChecksumSet/track with unknown/zero length.
	 */
	ChecksumSet();

	/**
	 * \brief Constructor
	 *
	 * \param[in] length Length in LBA frames of the track
	 */
	explicit ChecksumSet(const AudioSize& length);

	/**
	 * \brief Constructor for a known set of typed Checksums.
	 *
	 * \param[in] length Track length
	 * \param[in] sums   Sequence of checksums represented as type-value pairs
	 */
	ChecksumSet(const AudioSize& length,
			std::initializer_list<
				std::pair<const checksum::type, value_type>> sums);
	//NOTE We do not expose key_type. If key is not of key_type, it just breaks.

	/**
	 * \brief Length (in LBA frames) of this track.
	 *
	 * For ChecksumSets constructed by an instance of Algorithm, this will be
	 * the length actually used for computing the Checksum. It may or may not be
	 * identical to the parsed length hold in the ToC. The parsed length might
	 * be smaller since it is possible that it does not contain the silence
	 * adjacent to the respective track. Hence, a mismatch between the length()
	 * of a ChecksumSet and the length() of the ToC used in the Calculation that
	 * created the ChecksumSet is not an error.
	 *
	 * \return Length of this track in LBA frames
	 */
	AudioSize length() const noexcept;

	/**
	 * \brief Set the length (in LBA frames) of this track.
	 *
	 * \param[in] length New length for this instance
	 */
	void set_length(const AudioSize& length) noexcept;

	/**
	 * \brief Returns \c TRUE iff the instance contains a Checksum of the type
	 * \c type.
	 *
	 * \param[in] type The type to lookup the Checksum for
	 *
	 * \return \c TRUE iff \c type is present in the instance, otherwise \c FALSE
	 */
	bool contains(const checksum::type& type) const;

	/**
	 * \brief Return the \ref Checksum for the specified \c type and a boolean.
	 *
	 * The boolean is TRUE iff the search was successful and the Checksum is an
	 * actual search result. If there is no Checksum represented for the
	 * \c type, the Checksum returned will be
	 * \link Checksum::zero() zero()\endlink and the boolean value will be
	 * FALSE.
	 *
	 * \param[in] type The checksum::type to return the value
	 *
	 * \return The checksum for the specified type
	 */
	std::pair<Checksum, bool> get(const checksum::type type) const;

	/**
	 * \brief Return the \ref Checksum for the specified \c type or throw.
	 *
	 * If there is no Checksum represented for the \c type, a
	 * std::invalid_argument will be thrown.
	 *
	 * \param[in] type The checksum::type to return the value
	 *
	 * \return The checksum for the specified type
	 *
	 * \throw std::invalid_argument If the ChecksumSet does not contain a
	 * Checksum of the specified \c type
	 */
	Checksum checksum(const checksum::type type) const;

	/**
	 * \brief Return the checksum types present in this ChecksumSet
	 *
	 * \return The checksum types in this ChecksumSet
	 */
	std::set<checksum::type> types() const;

	/**
	 * \brief Inserts a new <type, Checksum> pair to the instance.
	 *
	 * If the key is already present in the instance, the existing checksum will
	 * be left unmodified.
	 *
	 * The pair returned contains an iterator to the inserted Checksum and a
	 * bool that is \c TRUE iff the insertion was successful. If the insertion
	 * was not successful, the value \c FALSE is returned for the bool and the
	 * iterator will point to the element that prevented the insertion.
	 *
	 * \param[in] type  The key to use
	 * \param[in] value The checksum for the given key
	 *
	 * \return Pair with an iterator to the inserted value and a status flag
	 */
	std::pair<iterator, bool> insert(const checksum::type type,
			const Checksum& value);

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
	 * After a successful merge, \c rhs will contain only duplicates. The length
	 * of \c rhs is never modified.
	 *
	 * \param[in] rhs The list to be merged into the instance
	 *
	 * \throw std::domain_error If \c rhs.length() != \c length() and both
	 *							 are != 0
	 */
	void merge(ChecksumSet& rhs);

	/**
	 * \brief Erases the Checksum with the given type.
	 *
	 * Iff the given type is not contained in the instance, the call has
	 * no effect.
	 *
	 * \param[in] type The type to erase
	 */
	void erase(const checksum::type& type);

	/**
	 * \brief Erases all \link Checksum Checksums\endlink contained in the
	 * instance.
	 *
	 * After clear() has been called the size of the container will be \c 0 .
	 */
	void clear();

	/**
	 * \copydoc SNPT_mf_begin
	 */
	iterator begin();

	/**
	 * \copydoc SNPT_mf_end
	 */
	iterator end();

	/**
	 * \copydoc SNPT_mf_cbegin
	 */
	const_iterator cbegin() const;

	/**
	 * \copydoc SNPT_mf_cend
	 */
	const_iterator cend() const;

	/**
	 * \copydoc SNPT_mf_cbegin
	 */
	const_iterator begin() const;

	/**
	 * \copydoc SNPT_mf_cend
	 */
	const_iterator end() const;

	/**
	 * \copydoc SNPT_mf_size
	 */
	size_type size() const noexcept;

	/**
	 * \copydoc SNPT_mf_empty
	 */
	bool empty() const noexcept;

	/**
	 * \copydoc SNPT_mf_op_bool_if_empty
	 *
	 * A ChecksumSet with a length but without elements is FALSE.
	 */
	explicit operator bool() const noexcept;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(ChecksumSet& rhs) noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const ChecksumSet& rhs) const noexcept;

	/**
	 * \copydoc SNPT_nf_stream_in
	 */
	friend std::ostream& operator << (std::ostream& out, const ChecksumSet& i);

	/**
	 * \copydoc SNPT_mf_to_string
	 */
	std::string to_string() const;
};

/**
 * \brief List of \link arcstk::ChecksumSet ChecksumSets \endlink.
 *
 * Each ChecksumSet represents a track. The order of the ChecksumSets follows
 * the order of tracks on the original compact disc.
 */
using Checksums = std::vector<ChecksumSet>; // also defined in verify.hpp

/**
 * \brief List of Checksum values.
 *
 * Each Checksum represents a track. The order of the Checksum values follows
 * the order of tracks on the original compact disc.
 */
using ChecksumList = std::vector<Checksum>;

/** @} */ // group calc


namespace details
{

/**
 * \brief RAII guard that restores the formatting state of a stream.
 *
 * Saves and restores flags, fill character, width and precision of the original
 * stream. Does not touch the stream's iostate or locale.
 */
template <typename CharT, typename Traits = std::char_traits<CharT>>
class BasicStreamFlagsGuard final
{
	/**
	 * \brief Internal reference to the stream to be guarded.
	 */
	std::basic_ios<CharT, Traits>& stream_;

	/**
	 * \brief Previous flags.
	 */
	typename std::basic_ios<CharT, Traits>::fmtflags prev_flags_;

	/**
	 * \brief Previous fill character.
	 */
	CharT prev_fill_;

	/**
	 * \brief Previous width.
	 */
	std::streamsize  prev_width_;

	/**
	 * \brief Previous precision.
	 */
	std::streamsize  prev_precision_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] s Stream to save + restore
	 */
	explicit BasicStreamFlagsGuard(std::basic_ios<CharT, Traits>& s) noexcept
		: stream_         { s             }
		, prev_flags_     { s.flags()     } // iostate is not touched, no throw
		, prev_fill_      { s.fill()      }
		, prev_width_     { s.width()     }
		, prev_precision_ { s.precision() }
	{
		// empty
	}

	/**
	 * \copydoc SNPT_sm_default_dtor.
	 */
	~BasicStreamFlagsGuard() noexcept
	{
		stream_.flags(prev_flags_);
		stream_.fill (prev_fill_);
		stream_.width(prev_width_);
		stream_.precision(prev_precision_);
		// getloc() + imbue() must be restored once the locale is saved
	}

	BasicStreamFlagsGuard(const BasicStreamFlagsGuard&)
		= delete;

	BasicStreamFlagsGuard& operator = (const BasicStreamFlagsGuard&)
		= delete;

	BasicStreamFlagsGuard(BasicStreamFlagsGuard&&) noexcept
		= delete;

	BasicStreamFlagsGuard& operator =(BasicStreamFlagsGuard&&) noexcept
		= delete;
};

/**
 * \brief Properties guard for character streams.
 */
using StreamFlagsGuard = BasicStreamFlagsGuard<char>;

} // namespace details
                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk


// provided to use arcstk::Checksum in std::set and std::unordered_set
namespace std
{
	template <>// NOLINTNEXTLINE(bugprone-std-namespace-modification)
	struct hash<arcstk::Checksum>
	{
		std::size_t operator()(const arcstk::Checksum& c) const noexcept
		{
			return std::hash<uint32_t>{}(c.value());
		}
	};
} // namespace std

#endif

