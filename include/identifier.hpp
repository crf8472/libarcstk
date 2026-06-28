#ifndef LIBARCSTK_IDENTIFIER_HPP_
#define LIBARCSTK_IDENTIFIER_HPP_

/**
 * \file
 *
 * \brief Public API for \link id calculating AccurateRip ids\endlink
 *
 * \details
 *
 * Calculate and represent AccurateRip IDs.
 */

#include <cstddef>               // for size_t
#include <cstdint>               // for uint32_t, int32_t, uint64_t
#include <memory>                // for unique_ptr
#include <string>                // for string
#include <vector>                // for vector

#ifndef LIBARCSTK_MIXINS_HPP_
#include "mixins.hpp"            // for Comparable
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


/**
 * \defgroup id AccurateRip IDs
 *
 * \brief Calculate and manage \link ARId AccurateRip identifier\endlink
 *
 * ARId is an AccurateRip identifier. It determines the request URL for an album
 * and as well the canonical savefile name for its AccurateRip data. \link ARId
 * ARIds\endlink are calculated by track offsets, leadout and track count.
 *
 * As a convenience, functions make_arid() construct the ARId of an album by its
 * metadata. These functions will not validate their input.
 *
 * Functions validated_arid() are wrappers around make_arid() that add
 * input validation by function toc::validate().
 *
 * Every ARId can be turned into a dBAR filename. Those filenames are calculated
 * anyway as part of the AccurateRip request URL but also denote the canonical
 * name of the savefile to store the binary data from the AccurateRip service.
 *
 * Every ARId can be turned into a URL that can be used to request
 * the AccurateRip service. Those URLs are constructed by using a global URL
 * prefix that is modifiable via static class ACCURATERIP.
 *
 * An ARId may be \link ARId::empty() empty()\endlink. This indicates that the
 * ARId does for any reason not contain actual information (while it may or may
 * not carry actual values which are meaningless though). It is not possible to
 * turn an empty ARId into a functional request URL.
 *
 * Every ARId constructed by the parameterless constructor is empty(). Empty
 * ARIds will turn into FALSE when checked via operator bool() while every
 * non-empty ARId will turn into TRUE.
 *
 * @{
 */

class ARId; // forward declaration

// ensure to put declaration in this namespace
std::ostream& operator << (std::ostream& out, const ARId& i);

/**
 * \brief AccurateRip identifier of a compact disc.
 *
 * The AccurateRip identifier is a virtually unique identifier for an album. It
 * determines the request URL for an album and as well the canonical savefile
 * name for its AccurateRip data.
 *
 * It is only virtually unique since it depends on the three input values track
 * count, offsets and leadout which are not guaranteed to differ about any two
 * distinct albums. While in fact there are different albums sharing the same
 * id, the probability for such a collision is very small. A collision will not
 * have any influence on whether the album can be verified correctly - it will
 * only increase the set of ARCSs associated with the particular ARId. Therefore
 * it will likely require more comparisons to verify such albums but it is not
 * an impediment for correctness.
 *
 * \link ARId ARIds\endlink can be constructed either directly from three
 * precomputed ids or from album metadata using functions make_arid(). To
 * construct the ARId of a compact disc, two values are required: the complete
 * set of track offsets and the leadout. (Track count as a third required value
 * can usually be determined by the total number of input offsets.)
 *
 * An ARId can be empty() to indicate that it carries no actual identifier. An
 * ARId that qualifies as empty() can be constructed by the parameterless
 * default constructor: <tt>ARId{}</tt>.
 */
class ARId final : Equality<ARId>, Comparable<ARId>, Swap<ARId>, ToString<ARId>
{
	class Impl;

	// intentionally undocumented
	std::unique_ptr<Impl> impl_ {};

public:

	/**
	 * \brief Construct an empty ARId.
	 */
	ARId()
		: ARId { 0, 0, 0, 0 }
	{
		// empty
	}

	/**
	 * \brief Construct ARId.
	 *
	 * \param[in] total_tracks Number of tracks in this medium
	 * \param[in] id_1         Id 1 of this medium
	 * \param[in] id_2         Id 2 of this medium
	 * \param[in] cddb_id      CDDB id of this medium
	 */
	ARId(const std::size_t total_tracks,
			const uint32_t id_1,
			const uint32_t id_2,
			const uint32_t cddb_id);

	/**
	 * \copydoc SNPT_sm_copy_ctor
	 */
	ARId(const ARId& rhs);

	/**
	 * \copydoc SNPT_sm_copy_op
	 */
	ARId& operator = (const ARId& rhs);

	/**
	 * \copydoc SNPT_sm_move_ctor
	 */
	ARId(ARId&& rhs) noexcept;

	/**
	 * \copydoc SNPT_sm_move_op
	 */
	ARId& operator = (ARId&& rhs) noexcept;

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~ARId() noexcept;

	/**
	 * \brief Return the AccurateRip request URL.
	 *
	 * \return The AccurateRip request URL
	 */
	std::string url() const;

	/**
	 * \brief Return the AccurateRip filename of the response file.
	 *
	 * \return AccurateRip filename of the response file
	 */
	std::string filename() const;

	/**
	 * \brief Return the track count.
	 *
	 * \return Track count of this medium
	 */
	unsigned total_tracks() const noexcept;

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
	 * \brief Return the current URL prefix for AccurateRip request urls.
	 *
	 * \return URL prefix for AccurateRip request URLs.
	 */
	std::string prefix() const noexcept;

	/**
	 * \copydoc SNPT_mf_empty
	 */
	bool empty() const noexcept;

	/**
	 * \copydoc SNPT_mf_op_bool_if_empty
	 */
	explicit operator bool() const noexcept;

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(ARId& rhs) noexcept;

	/**
	 * \copydoc SNPT_mf_equals
	 */
	bool equals(const ARId& rhs) const noexcept;

	/**
	 * \copydoc SNPT_mf_to_string
	 */
	std::string to_string() const;

	/**
	 * \copydoc SNPT_nf_stream_in
	 */
	friend std::ostream& operator << (std::ostream& out, const ARId& i);

};

/**
 * \brief Create an ARId from offsets and leadout.
 *
 * The size of the container \c offsets is interpreted as track count.
 *
 * \param[in] offsets Offsets
 * \param[in] leadout Leadout
 *
 * \return ARId
 */
[[nodiscard]] ARId make_arid(const std::vector<AudioSize>& offsets,
					const AudioSize& leadout);

/**
 * \brief Create an ARId from a ToC and a leadout.
 *
 * \param[in] toc     ToC to use
 * \param[in] leadout Leadout LBA frame
 *
 * \return ARId
 */
[[nodiscard]] ARId make_arid(const ToC& toc, const AudioSize& leadout);

/**
 * \brief Create an ARId from a ToC.
 *
 * \param[in] toc ToC to use
 *
 * \return ARId
 */
[[nodiscard]] ARId make_arid(const ToC& toc);

/**
 * \brief Create an ARId from validated offsets and validated leadout.
 *
 * The size of the container \c offsets is interpreted as track count. The
 * validation is performed by function toc::validate().
 *
 * \param[in] offsets Offsets
 * \param[in] leadout Leadout
 *
 * \return ARId
 *
 * \throw invalid_argument If \c offsets or \c leadout could not be validated.
 */
[[nodiscard]] ARId validated_arid(const std::vector<AudioSize>& offsets,
		const AudioSize& leadout);

/**
 * \brief Create an ARId from a validated pair of ToC and leadout.
 *
 * The value of \c toc.leadout() is ignored and the value of \c leadout is used
 * instead. The validation is performed by function toc::validate().
 *
 * \param[in] toc     ToC to use
 * \param[in] leadout Leadout
 *
 * \return ARId
 *
 * \throw invalid_argument If \c toc or \c leadout could not be validated.
 */
[[nodiscard]] ARId validated_arid(const ToC& toc, const AudioSize& leadout);

/**
 * \brief Create an ARId from a
 * \link arcstk::ToC::complete() complete()\endlink ToC.
 *
 * The validation is performed by function toc::validate().
 *
 * \param[in] toc ToC to use
 *
 * \return ARId
 *
 * \throw invalid_argument If \c toc is not complete or could not be validated.
 */
[[nodiscard]] ARId validated_arid(const ToC& toc);

/** @} */

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

