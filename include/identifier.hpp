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

#ifndef LIBARCSTK_POLICIES_HPP_
#include "policies.hpp"          // for Comparable
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
 * prefix.
 *
 * This prefix can be read by function ACCURATERIP::request_url_prefix() and
 * modified by function ACCURATERIP::set_request_url_prefix(). After setting the
 * global URL prefix to a new value, every call of ARId::url() and
 * ARId::prefix() in client code will reflect this updated value. The global
 * value can be reset to its default by function
 * ACCURATERIP::reset_request_url_prefix().
 *
 * An ARId may be \link ARId::empty() empty()\endlink. This indicates that the
 * ARId does for any reason not contain actual information (while it may or may
 * not carry actual values which are meaningless though). It is not possible to
 * turn an empty ARId into a functional request URL.
 *
 * Empty ARIds will turn into FALSE when checked via operator bool() while every
 * non-empty ARId will turn into TRUE. A call of <tt>myARId.empty()</tt> will
 * yield TRUE iff <tt>myARId == arcstk::EmptyARId</tt> yields TRUE.
 *
 * An empty ARId can be used to indicate that no valid ARId could be provided
 * when nonetheless some ARId-typed value is required by the situation. This
 * use-case motivates function make_empty_arid() which constructs empty ARIds.
 * It is usually not required to explicitly construct a new empty ARId instance
 * since a reference or pointer to arcstk::EmptyARId maybe returned instead
 * without requiring additional memory.
 *
 * @{
 */

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
 * ARId that qualifies as empty() can be constructed by make_empty_arid(). An
 * alternative to constructing an empty ARId is to return a reference or pointer
 * to arcstk::EmptyARId.
 */
class ARId final : public Comparable<ARId>
{
public:

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
	~ARId() noexcept final;

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
	 * \brief Return \c TRUE iff this instance is empty.
	 *
	 * \return \c TRUE iff this instance is empty
	 */
	bool empty() const noexcept;

	/**
	 * \brief Return \c TRUE iff instance is not empty(), otherwise \c FALSE.
	 *
	 * \return Return \c TRUE iff instance is not empty(), otherwise \c FALSE.
	 */
	explicit operator bool() const noexcept;

	/**
	 * \brief Swap with another instance.
	 *
	 * \param[in] rhs Instance to swap
	 */
	void swap(ARId& rhs) noexcept;

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
	std::string to_string() const;

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

	friend std::ostream& operator << (std::ostream& out, const ARId& arid)
	{
		out << arid.to_string();
		return out;
	}

	friend std::string to_string(const ARId& arid)
	{
		return arid.to_string();
	}
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
ARId make_arid(const std::vector<AudioSize>& offsets, const AudioSize& leadout);

/**
 * \brief Create an ARId from a ToC and a leadout.
 *
 * \param[in] toc     ToC to use
 * \param[in] leadout Leadout LBA frame
 *
 * \return ARId
 */
ARId make_arid(const ToC& toc, const AudioSize& leadout);

/**
 * \brief Create an ARId from a ToC.
 *
 * \param[in] toc ToC to use
 *
 * \return ARId
 */
ARId make_arid(const ToC& toc);

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
ARId validated_arid(const std::vector<AudioSize>& offsets,
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
ARId validated_arid(const ToC& toc, const AudioSize& leadout);

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
ARId validated_arid(const ToC& toc);

/**
 * \brief Global instance of an empty ARId.
 *
 * This is for convenience since in most cases, the creation of an empty
 * ARId can be avoided when a reference instance is at hand.
 *
 * The definition of EmptyARId defines emptiness for ARIds.
 */
extern const ARId EmptyARId;

/**
 * \brief Create an \link arcstk::ARId::empty() empty()\endlink ARId.
 *
 * An empty ARId is not a valid description of a CDDA conforming medium.
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
ARId make_empty_arid();


/**
 * \brief Constants for the AccurateRip service.
 */
class ACCURATERIP
{
	/**
	 * \brief Current request URL prefix.
	 */
	static std::string request_url_prefix_;

	// ... may contain more constants

public:

	/**
	 * \brief The current URL prefix to construct request URLs.
	 *
	 * \return Current prefix to construct request URLs.
	 */
	static std::string request_url_prefix() noexcept;

	/**
	 * \brief The default URL prefix to construct request URLs.
	 *
	 * \return Default prefix to construct request URLs.
	 */
	static std::string default_request_url_prefix() noexcept;

	/**
	 * \brief Set the global URL prefix for AccurateRip request URLs.
	 *
	 * \param[in] prefix URL prefix to use for constructing ARId URLs
	 */
	static void set_request_url_prefix(const std::string& prefix) noexcept;

	/**
	 * \brief Set the global URL prefix for AccurateRip request URLs to its
	 * default value.
	 *
	 * The default value is defined by
	 * ACCURATERIP::default_request_url_prefix().
	 */
	static void reset_request_url_prefix() noexcept;

	/**
	 * \brief Format an unsigned 32bit integer as an ARCS in the default format.
	 *
	 * The default format is the format in which ARCSs are printed in most
	 * client applications.
	 *
	 * The ARCS default format entails:
	 * - hexadecimal representation
	 * - base (like "0x") is not represented
	 * - always 8 digits wide, possibly with leading zeros
	 * - digits A-F are always uppercase
	 *
	 * \param[in] number The number to format
	 *
	 * \return Default-ARCS-formatted representation of the input number
	 */
	static std::string default_arcs_format(const uint32_t number);
};

/** @} */

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

