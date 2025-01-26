#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#define __LIBARCSTK_IDENTIFIER_HPP__

/**
 * \file
 *
 * \brief Public API for \link id calculating AccurateRip ids\endlink
 */

#include <cstdint>               // for uint32_t, int32_t, uint64_t
#include <initializer_list>      // for initializer_list
#include <memory>                // for unique_ptr
#include <stdexcept>             // for runtime_error
#include <string>                // for string
#include <type_traits>           // for enable_if_t
#include <utility>               // for forward
#include <vector>                // for vector

#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

// avoid includes
class AudioSize;
class ToC;

/**
 * \brief Type to represent 1-based track numbers.
 *
 * A signed integer type.
 *
 * Valid track numbers are in the range of 1-99. Note that 0 is not a valid
 * TrackNo. Hence, a TrackNo is not suitable to represent a total number of
 * tracks or a counter for tracks.
 *
 * The intention of this typedef is to provide a marker for parameters that
 * expect 1-based track numbers instead of 0-based track indices. TrackNo will
 * not occurr as a return type in the API.
 *
 * A validation check is not provided, though. Every function that accepts a
 * TrackNo will in fact accept 0 but will then either throw or return a default
 * error value.
 *
 * It is not encouraged to use TrackNo in client code.
 */
using TrackNo = int;


/** \defgroup id AccurateRip IDs
 *
 * \brief Calculate and manage \link ARId AccurateRip identifier\endlink
 *
 * ARId is an AccurateRip identifier. It determines the request URL for an
 * album and as well its canonical savefile name. \link ARId ARIds\endlink are
 * constructed by other IDs and metadata like offsets and track count. As a
 * convenience, functions make_arid() construct the ARId of an album by its
 * TOC.
 *
 * A TOC is the validated table of content information from a compact disc.
 * \link TOC TOCs\endlink are exclusively constructed by functions make_toc()
 * that try to validate the information used to construct the TOC. The
 * validation recognizes inconsistent input data that cannot form
 * a valid TOC. If the validation fails, an InvalidMetadataException is
 * thrown.
 *
 * An InvalidMetadataException indicates that no valid TOC can be constructed
 * from the input provided.
 *
 * A NonstandardMetadataException indicates that the input is not conforming to
 * the redbook standard. This exception can occurr in the internal validation
 * mechanism but is currently not used in the public API.
 *
 * @{
 */


// forward declaration for operator==
class ARId; // IWYU pragma keep

bool operator == (const ARId& lhs, const ARId& rhs) noexcept;


/**
 * \brief AccurateRip-Identifier of a compact disc.
 *
 * The AccurateRip identifier determines the URL of the compact disc dataset as
 * well as the standard filename of the AccurateRip response.
 *
 * \link ARId ARIds\endlink can be constructed either from three
 * precomputed ids or from a TOC using function make_arid().
 *
 * In some cases, an ARId is syntactically required, but semantically
 * superflous. An ARId can be empty() to indicate that it carries no identifier.
 * An ARId that qualifies as empty() can be constructed by make_empty_arid().
 */
class ARId final : public Comparable<ARId>
{
public:

	friend bool operator == (const ARId& lhs, const ARId& rhs) noexcept;

	/**
	 * \brief Construct ARId.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] id_1        Id 1 of this medium
	 * \param[in] id_2        Id 2 of this medium
	 * \param[in] cddb_id     CDDB id of this medium
	 */
	ARId(const TrackNo track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs The ARId to copy
	 */
	ARId(const ARId &rhs);

	ARId& operator = (const ARId &rhs);

	/**
	 * \brief Default move constructor.
	 *
	 * \param[in] rhs The ARId to move
	 */
	ARId(ARId &&rhs) noexcept;

	ARId& operator = (ARId &&rhs) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~ARId() noexcept;

	/**
	 * \brief Return the AccurateRip request URL.
	 *
	 * \return The AccurateRip request URL
	 */
	std::string url() const noexcept;

	/**
	 * \brief Return the AccurateRip filename of the response file.
	 *
	 * \return AccurateRip filename of the response file
	 */
	std::string filename() const noexcept;

	/**
	 * \brief Return the track count.
	 *
	 * \return Track count of this medium
	 */
	int track_count() const noexcept;

	/**
	 * \brief Return the disc_id 1.
	 *
	 * \return Disc id 1 of this medium
	 */
	uint32_t disc_id_1() const noexcept;

	/**
	 * \brief Return the disc_id 2.
	 *
	 * \return Disc id 2 of this medium
	 */
	uint32_t disc_id_2() const noexcept;

	/**
	 * \brief Return the CDDB id.
	 *
	 * \return CDDB id of this medium
	 */
	uint32_t cddb_id() const noexcept;

	/**
	 * \brief Return the standard URL prefix for AccurateRip request urls.
	 *
	 * \return URL prefix for AccurateRip request URLs.
	 */
	std::string prefix() const noexcept;

	/**
	 * \brief Return \c TRUE iff this ARId is empty (holding no information).
	 *
	 * \return \c TRUE iff this ARId is empty
	 */
	bool empty() const noexcept;

	/**
	 * \brief Return a default string representation of this ARId.
	 *
	 * \return Default string representation of this ARId
	 */
	std::string to_string() const noexcept; // FIXME Write to_string()

private:

	class Impl;
	std::unique_ptr<Impl> impl_;
};


/**
 * \brief Global instance of an empty ARId.
 *
 * This is for convenience since in most cases, the creation of an empty
 * ARId can be avoided when a reference instance is at hand.
 *
 * The instance is created using make_empty_arid().
 */
extern const ARId EmptyARId;


/**
 * \brief Create an ARId from the toc data.
 *
 * \param[in] offsets Offsets (in LBA frames)
 * \param[in] leadout Leadout (in LBA frames)
 *
 * \return ARId
 */
std::unique_ptr<ARId> make_arid(const std::vector<int32_t>& offsets,
		const int32_t leadout);

/**
 * \brief Create an ARId from the toc data.
 *
 * \param[in] offsets Offsets
 * \param[in] leadout Leadout
 *
 * \return ARId
 */
std::unique_ptr<ARId> make_arid(const std::vector<AudioSize>& offsets,
		const AudioSize& leadout);

/**
 * \brief Create an ARId from a TOC and a specified leadout.
 *
 * The input is validated.
 *
 * Parameter \c toc is allowed to be non-\link arcstk::v_1_0_0::TOC::complete()
 * complete()\endlink. Parameter \c leadout is intended to provide the value
 * possibly missing in \c toc.
 *
 * If \c leadout is 0, \c toc.leadout() is used and \c leadout is ignored. If
 * \c leadout is not 0, \c toc.leadout() is ignored. If both values are 0
 * an InvalidMetadataException is thrown.
 *
 * If \c leadout is 0 and \c toc cannot be validated, an
 * InvalidMetadataException is thrown. If
 * \c leadout is not 0 and \c leadout and \c toc cannot be validated as
 * consistent with each other, an InvalidMetadataException is thrown.
 *
 * \param[in] toc     TOC to use
 * \param[in] leadout Leadout LBA frame
 *
 * \return ARId
 *
 * \throw InvalidMetadataException If \c toc and \c leadout are inconsistent
 */
std::unique_ptr<ARId> make_arid(const ToC& toc, const AudioSize& leadout);

/**
 * \brief Create an ARId from a
 * \link arcstk::v_1_0_0::TOC::complete() complete()\endlink TOC.
 *
 * \details
 *
 * The \c toc is validated.
 *
 * \param[in] toc TOC to use
 *
 * \return ARId corresponding to the input TOC
 *
 * \throw InvalidMetadataException
 * If \c toc is not \link arcstk::v_1_0_0::TOC::complete() complete()\endlink.
 */
std::unique_ptr<ARId> make_arid(const ToC& toc);

/**
 * \brief Create an \link arcstk::v_1_0_0::ARId::empty() empty()\endlink ARId.
 *
 * The implementation of make_empty_arid() defines emptiness for ARIds.
 *
 * \return An \link arcstk::v_1_0_0::ARId::empty() empty()\endlink ARId
 */
std::unique_ptr<ARId> make_empty_arid() noexcept;

/** @} */

} //namespace v_1_0_0
} // namespace arcstk

#endif

