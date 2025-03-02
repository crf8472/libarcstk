#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#define __LIBARCSTK_IDENTIFIER_HPP__

/**
 * \file
 *
 * \brief Public API for \link id calculating AccurateRip ids\endlink
 */

#include <cstdint>               // for uint32_t, int32_t, uint64_t
#include <memory>                // for unique_ptr
#include <string>                // for string
#include <vector>                // for vector

#ifndef __LIBARCSTK_POLICIES_HPP__
#include "policies.hpp"          // for Comparable
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

// avoid includes
class AudioSize;
class ToC;


/** \defgroup id AccurateRip IDs
 *
 * \brief Calculate and manage \link ARId AccurateRip identifier\endlink
 *
 * ARId is an AccurateRip identifier. It determines the request URL for an
 * album and as well its canonical savefile name. \link ARId ARIds\endlink are
 * constructed by other IDs and metadata like offsets and track count. As a
 * convenience, functions make_arid() construct the ARId of an album by its
 * ToC.
 *
 * @{
 */

/**
 * \brief AccurateRip-Identifier of a compact disc.
 *
 * The AccurateRip identifier determines the URL of the compact disc dataset as
 * well as the standard filename of the AccurateRip response.
 *
 * \link ARId ARIds\endlink can be constructed either from three
 * precomputed ids or from a ToC using function make_arid().
 *
 * In some cases, an ARId is syntactically required, but semantically
 * superflous. An ARId can be empty() to indicate that it carries no identifier.
 * An ARId that qualifies as empty() can be constructed by make_empty_arid().
 */
class ARId final : public Comparable<ARId>
{
public:

	/**
	 * \brief Construct ARId.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] id_1        Id 1 of this medium
	 * \param[in] id_2        Id 2 of this medium
	 * \param[in] cddb_id     CDDB id of this medium
	 */
	ARId(const int track_count,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id);

	/**
	 * \brief Copy constructor.
	 *
	 * \param[in] rhs The ARId to copy
	 */
	ARId(const ARId& rhs);

	ARId& operator = (const ARId& rhs);

	/**
	 * \brief Default move constructor.
	 *
	 * \param[in] rhs The ARId to move
	 */
	ARId(ARId&& rhs) noexcept;

	ARId& operator = (ARId&& rhs) noexcept;

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
	 * \brief Swap with another instance.
	 */
	void swap(ARId& rhs) const noexcept;

	/**
	 * \brief TRUE iff this instance is equal to another instance.
	 *
	 * \param[in] rhs Instance to check for equality
	 *
	 * \return TRUE iff \c rhs == \c this
	 */
	bool equals(const ARId& rhs) const noexcept;

	/**
	 * \brief Create a string representation of this instance.
	 *
	 * \return String representation
	 */
	std::string to_string() const noexcept;

private:

	class Impl;
	std::unique_ptr<Impl> impl_;

public:

	friend void swap(ARId& lhs, ARId& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	friend bool operator == (const ARId& lhs, const ARId& rhs) noexcept
	{
		return lhs.equals(rhs);
	}

	/**
	 * \brief Create a string representation of the ARId.
	 *
	 * \param[in] arid The ARId to represent
	 *
	 * \return The actual AccurateRip ID as a string
	 */
	friend std::string to_string(const ARId& arid)
	{
		return arid.to_string();
	}
};


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
 * \brief Create an ARId from a ToC and a specified leadout.
 *
 * The input is validated.
 *
 * Parameter \c toc is allowed to be non-\link arcstk::v_1_0_0::ToC::complete()
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
 * \param[in] toc     ToC to use
 * \param[in] leadout Leadout LBA frame
 *
 * \return ARId
 *
 * \throw InvalidMetadataException If \c toc and \c leadout are inconsistent
 */
std::unique_ptr<ARId> make_arid(const ToC& toc, const AudioSize& leadout);

/**
 * \brief Create an ARId from a
 * \link arcstk::v_1_0_0::ToC::complete() complete()\endlink ToC.
 *
 * \details
 *
 * The \c toc is validated.
 *
 * \param[in] toc ToC to use
 *
 * \return ARId corresponding to the input ToC
 *
 * \throw InvalidMetadataException
 * If \c toc is not \link arcstk::v_1_0_0::ToC::complete() complete()\endlink.
 */
std::unique_ptr<ARId> make_arid(const ToC& toc);

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
 * \brief Safely create an
 * \link arcstk::v_1_0_0::ARId::empty() empty()\endlink ARId.
 *
 * The implementation of make_empty_arid() defines emptiness for ARIds.
 *
 * An empty ARId has the invalid value 0 for the track count and also 0
 * for disc id 1, disc id 2 and cddb id. An empty ARId is not a valid
 * description of a CDDA medium.
 *
 * Building an empty ARId also provides the possibility to just provide an
 * ARId on sites where an ARId is required without having to test for null.
 *
 * It may help provide an uniforming implementation of cases where
 * an ARId in fact is expected but cannot be provided due to missing
 * data, e.g. when processing single tracks without knowing the offset.
 *
 * \return An empty ARId
 */
std::unique_ptr<ARId> make_empty_arid() noexcept;

/** @} */

} //namespace v_1_0_0
} // namespace arcstk

#endif

