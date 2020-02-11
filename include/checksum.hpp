#ifndef __LIBARCSTK_CHECKSUM_HPP__
#define __LIBARCSTK_CHECKSUM_HPP__

/**
 * \file
 *
 * \brief Public API for \link calc checksum management\endlink.
 *
 * Represent metadata of a given compact disc image, the computed checksums of
 * an audio file and the Checksums calculating class along with its
 * configuration and state.
 *
 * Basic data representation classes include TOC, Checksum and
 * ChecksumSet.
 *
 * TOC represents the toc information of a compact disc along with a
 * function make_toc() that guarantees to provide only valid
 * @link arcstk::v_1_0_0::TOC TOCs @endlink or to throw an exception.
 *
 * Checksum represents a single checksum and a ChecksumSet the
 * @link arcstk::v_1_0_0::Checksum Checksums @endlink for a single track.
 *
 * Checksums is an aggregation of the
 * @link arcstk::v_1_0_0::Checksum Checksums @endlink of an audio input.
 */

#include <array>
#include <cstdint>
#include <cstddef>     // for size_t
#include <memory>
#include <set>
#include <string>

namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \addtogroup calc
 *
 * \brief Public API for management of checksums.
 *
 * @{
 */


class Checksum; // forward declaration for operator == and to_hex_str()


/**
 * \brief Everything regarding operation with Checksums.
 */
namespace checksum
{
	// Note: 'names' array must have same size as 'type' class and the name
	// strings must occurr in exact the same order as in 'type'. Otherwise,
	// function 'type_name()' will fail.

	/**
	 * \brief Type IDs of the pre-defined checksum types.
	 */
	enum class type : uint32_t
	{
		ARCS1   = 1,
		ARCS2   = 2
		//THIRD_TYPE  = 4,
		//FOURTH_TYPE = 8 ...
	};

	/**
	 * \brief Checksum type names.
	 */
	static const std::array<std::string,4> names {
		"ARCSv1",
		"ARCSv2",
		// "THIRD_TYPE" ,
		// "FOURTH_TYPE" ...
	};

	/**
	 * \brief Iterable sequence of all predefined checksum types.
	 */
	static const type types[] = {
		type::ARCS1,
		type::ARCS2
		// type::THIRD_TYPE,
		// type::FOURTH_TYPE ...
	};

	/**
	 * \brief Return the name of a checksum::type.
	 *
	 * \param[in] t Type to get name of
	 *
	 * \return Name of type
	 */
	std::string type_name(type t);

	/**
	 * \internal
	 *
	 * \brief Creates a hexadecimal string representation of a 32bit checksum.
	 *
	 * \param[in] checksum The Checksum to represent
	 * \param[in] upper    TRUE indicates to print digits A-F in uppercase
	 * \param[in] base     TRUE indicates to print base '0x'
	 *
	 * \return A hexadecimal representation of the \c checksum as a string
	 */
	std::string to_hex_str(const Checksum &checksum, const bool upper,
			const bool base);

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
 * \brief A 32-bit wide checksum for a single file or track.
 */
class Checksum final
{

public:

	friend bool operator == (const Checksum &lhs, const Checksum &rhs) noexcept;

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

	//Checksum(const Checksum &rhs);

	//Checksum(Checksum &&rhs) noexcept;

	//virtual ~Checksum() noexcept = default;

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

	//Checksum& operator = (const Checksum &rhs);

	//Checksum& operator = (Checksum &&rhs) noexcept;

	Checksum& operator = (const uint32_t rhs);


private:

	/**
	 * \brief Actual checksum value;
	 */
	uint32_t value_;
};


/**
 * \brief Inequality.
 *
 * \param[in] lhs The left hand side instance to check for inequality
 * \param[in] rhs The right hand side instance to check for inequality
 *
 * \return TRUE if \c lhs is not equal to \c rhs, otherwise FALSE
 */
bool operator != (const Checksum &lhs, const Checksum &rhs) noexcept;

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
 * \brief A set of Checksum instances of different types for a single track.
 */
class ChecksumSet final : public ChecksumSetBase
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
	explicit ChecksumSet(const int64_t length);

	/**
	 * \brief Default destructor
	 */
	~ChecksumSet() noexcept;

	/**
	 * \brief Length (in LBA frames) of this track.
	 *
	 * \return Length of this track in LBA frames
	 */
	int64_t length() const noexcept;

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

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

