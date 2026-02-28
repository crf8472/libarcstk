#ifndef __LIBARCSTK_VERIFY_HPP__
#define __LIBARCSTK_VERIFY_HPP__

/**
 * \file
 *
 * \brief Public API for \link verify AccurateRip checksum verification\endlink.
 *
 * \details
 *
 * Includes the module for \link verify verifiying AccurateRip checksums and
 * ids\endlink.
 *
 * Fine-granular comparison between reference checksums and locally computed
 * checksums.
 */

#include <cstddef>        // for size_t
#include <cstdint>        // for uint32_t
#include <memory>         // for unique_ptr
#include <ostream>        // for ostream
#include <tuple>          // for tuple
#include <vector>         // for vector


namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

class ARId;
class Checksum;
class ChecksumSet;
class DBAR;

using Checksums = std::vector<ChecksumSet>; // duplicate from calculate.hpp

/**
 * \defgroup verify AccurateRip Checksum Verification
 *
 * \brief Verify actual Checksums against a reference.
 *
 * \details
 *
 * A Verifier verifies actual Checksums against some reference Checksums
 * provided by a ChecksumSource. A ChecksumSource is an interface to different
 * kinds of input for Checksums. For convenience, a DBARSource is provided that
 * makes a DBAR object available as input for verification.
 *
 * A custom class T can be made available as input provider by subclassing
 * ChecksumSourceOf<T> and implementing the access to the reference values in
 * question.
 *
 * The result of a verification process is a VerificationResult. It holds every
 * result of every match operation performed during verification.
 *
 * AlbumVerifier is suitable for verifying input that contains a ToC. Situations
 * without a ToC (e.g. a set of input audio files) is supported by
 * TracksetVerifier.
 *
 * \see AlbumVerifier
 * \see TracksetVerifier
 *
 * @{
 */

/**
 * \brief Interface: unified access to checksum containers.
 *
 * \details
 *
 * A checksum container contains several blocks while each block consists of an
 * ARId and an ordered sequence of checksums. A single checksum is accessed by a
 * block index in combination with the index of the checksum within the block.
 *
 * A type \c T can be made available as a ChecksumSource via definig a subclass
 * of ChecksumSourceOf<T>.
 *
 * \see ChecksumSourceOf
 * \see DBARSource
 */
class ChecksumSource
{
public:

	using size_type = std::size_t;

private:

	virtual ARId do_id(const size_type block_idx) const
	= 0;

	virtual Checksum do_checksum(const size_type block_idx, const size_type idx)
		const
	= 0;

	virtual const uint32_t& do_arcs_value(const size_type block_idx,
		const size_type track_idx) const
	= 0;

	virtual const uint32_t& do_confidence(const size_type block_idx,
		const size_type idx) const
	= 0;

	virtual const uint32_t& do_frame450_arcs_value(const size_type block_idx,
			const size_type track_idx) const
	= 0;

	virtual std::size_t do_size(const size_type block_idx) const
	= 0;

	virtual std::size_t do_size() const
	= 0;

	virtual std::unique_ptr<ChecksumSource> do_clone() const
	= 0;

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ChecksumSource() noexcept = default;

	/**
	 * \brief Read id in section with the specified \c block_idx.
	 *
	 * \param[in] block_idx 0-based block index to access
	 *
	 * \return The id of the specified block
	 */
	ARId id(const size_type block_idx) const;

	/**
	 * \brief Read checksum on the specified position.
	 *
	 * Note that the wrapping of the ARCS value involves a copy.
	 *
	 * \param[in] block_idx 0-based block index to access
	 * \param[in] track_idx 0-based track index to access
	 *
	 * \return The checksum of the specified index position
	 */
	Checksum checksum(const size_type block_idx, const size_type track_idx)
		const;

	/**
	 * \brief Return the ARCS value on the specified position.
	 *
	 * \param[in] block_idx 0-based block index to access
	 * \param[in] track_idx 0-based track index to access
	 *
	 * \return The ARCS value of the specified index position
	 */
	const uint32_t& arcs_value(const size_type block_idx,
			const size_type track_idx) const;

	/**
	 * \brief Read confidence \c idx in section with the specified \c block_idx.
	 *
	 * \param[in] block_idx 0-based block index to access
	 * \param[in] track_idx 0-based track index to access
	 *
	 * \return The confidence of the specified index position
	 */
	const unsigned& confidence(const size_type block_idx,
			const size_type track_idx) const;

	/**
	 * \brief Return the ARCS value of frame 450 on the specified position.
	 *
	 * \param[in] block_idx 0-based block index to access
	 * \param[in] track_idx 0-based track index to access
	 *
	 * \return The ARCS value of frame 450 of the specified index position
	 */
	const uint32_t& frame450_arcs_value(const size_type block_idx,
			const size_type track_idx) const;

	/**
	 * \brief Size of the block specified by \c block_idx.
	 *
	 * The number of tracks in this block is its size.
	 *
	 * \param[in] block_idx 0-based block index to access
	 *
	 * \return The size of the specified block
	 */
	size_type size(const size_type block_idx) const;

	/**
	 * \brief Number of blocks.
	 *
	 * Greatest legal block is size() - 1.
	 *
	 * \return Number of blocks in this object
	 */
	size_type size() const;

	/**
	 * \brief Returns a deep copy of the instance
	 *
	 * \return A deep copy of the instance
	 */
	std::unique_ptr<ChecksumSource> clone() const;
};


/**
 * \brief Template/Base: wrap a checksum container type in a ChecksumSource.
 *
 * \details
 *
 * A type \c T can be made available as a ChecksumSource via definig a subclass
 * of ChecksumSourceOf<T>. The subclass has to implement the virtual functions
 * of ChecksumSource. It can reuse the constructor of ChecksumSourceOf<T>.
 *
 * \tparam T The type to wrap
 *
 * \see DBARSource
 */
template <typename T>
class ChecksumSourceOf : public ChecksumSource
{
	/**
	 * \brief Internal checksum source.
	 */
	const T* checksum_source_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] t The primary checksum source
	 */
	ChecksumSourceOf(const T* t)
		: checksum_source_ { t }
	{
		// empty
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs Instance to get a copy of
	 */
	ChecksumSourceOf(const ChecksumSourceOf& rhs)
		: checksum_source_ { rhs.checksum_source_ }
	{
		// empty
	}

	/**
	 * \brief Copy assignment operator
	 *
	 * \param[in] rhs Instance to get a copy of
	 *
	 * \return Copy of \c rhs
	 */
	ChecksumSourceOf& operator = (const ChecksumSourceOf& rhs)
	{
		checksum_source_ = rhs.checksum_source_;
		return *this;
	}

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~ChecksumSourceOf() noexcept = default;

	/**
	 * \brief The wrapped checksum source.
	 *
	 * \return The wrapped checksum source.
	 */
	const T* source() const
	{
		return checksum_source_;
	}
};


/**
 * \brief Access DBAR as a ChecksumSource.
 *
 * Make DBAR instances available for verification.
 */
class DBARSource final : public ChecksumSourceOf<DBAR>
{
	// ChecksumSource

	ARId do_id(const size_type block_idx) const final;

	Checksum do_checksum(const size_type block_idx,
			const size_type idx) const final;

	const uint32_t& do_arcs_value(const size_type block_idx,
			const size_type idx) const final;

	const unsigned& do_confidence(const size_type block_idx,
			const size_type idx) const final;

	const uint32_t& do_frame450_arcs_value(const size_type block_idx,
			const size_type idx) const final;

	std::size_t do_size(const size_type block_idx) const final;

	std::size_t do_size() const final;

	std::unique_ptr<ChecksumSource> do_clone() const final;

public:

	using ChecksumSourceOf::ChecksumSourceOf;

	using ChecksumSourceOf::operator=;
};


// TODO API for working with best block
// Say a typedef like: using block_result = std::tuple<int, bool, int>
// And accessors for that like get_index(), get_algo(), get_total_fails().
// Maybe enum constants for the tuple indices: INDEX, ALGO, TOTAL_FAILS

/**
 * \brief Interface: Result of a Verifier performing a verification process.
 *
 * \details
 *
 * A VerificationResult is the result of a complete matching of actual Checksums
 * against a ChecksumSource of reference checksums.
 *
 * It holds the result of any verification task of any reference track value.
 * Access to each of these results is provided in terms of
 * <tt>block:track:%version</tt>. The \c block and \c track address components
 * are integers, that refer to the respective 0-based block and 0-based track in
 * the ChecksumSource. The \c %version is a boolean that indicates whether the
 * match is for ARCSv2 (\c TRUE) or for ARCSv1 (\c FALSE).
 *
 * The result contains also the track-based interpretation of the flags, i.e.
 * whether a given track is considered to be verified or not.
 */
class VerificationResult
{
	virtual int do_total_unverified_tracks() const
	= 0;

	virtual bool do_is_verified(const int track) const
	= 0;

	virtual int do_verify_track(const int b, const int t, const bool v2)
	= 0;

	virtual bool do_track(const int b, const int t, const bool v2) const
	= 0;

	virtual int do_verify_id(const int b)
	= 0;

	virtual bool do_id(const int b) const
	= 0;

	virtual int do_difference(const int b, const bool v2) const
	= 0;

	virtual int do_total_blocks() const
	= 0;

	virtual int do_tracks_per_block() const
	= 0;

	virtual size_t do_size() const
	= 0;

	virtual std::tuple<int, bool, int> do_best_block() const
	= 0;

	virtual int do_best_block_difference() const
	= 0;

	virtual bool do_strict() const
	= 0;

	virtual std::unique_ptr<VerificationResult> do_clone() const
	= 0;

	friend std::ostream& operator << (std::ostream&,
			const VerificationResult& match);

public:

	/**
	 * \brief Virtual default destructor
	 */
	virtual ~VerificationResult() noexcept;

	/**
	 * \brief TRUE iff each track is verified.
	 *
	 * This is shorthand for checking whether total_unverified_tracks() is 0.
	 * The verification should be considered as successful iff this function
	 * returns TRUE.
	 *
	 * A rip for which all tracks are verified can be considered accurate
	 * relative to the applied verification method. Iff the method is strict,
	 * then a total match requires at least one block in the ChecksumSource that
	 * is identical to the list of actual Checksums. Iff the method is
	 * non-strict, every actual Checksum occurred at least in one block of the
	 * ChecksumSource.
	 *
	 * \return TRUE iff each track is verified otherwise FALSE
	 */
	bool all_tracks_verified() const;

	/**
	 * \brief Total number of unverified tracks.
	 *
	 * \return Total number of unverified tracks.
	 */
	int total_unverified_tracks() const;

	/**
	 * \brief TRUE iff specified 0-based track is verified, otherwise FALSE.
	 *
	 * \param[in] track 0-based track
	 *
	 * \return TRUE iff specified track is verified, otherwise FALSE
	 */
	bool is_verified(const int track) const;

	/**
	 * \brief Mark the checksum of a specified track in a specified block as
	 * 'matched'.
	 *
	 * \param[in] b  0-based index of the block to verify in the ChecksumSource
	 * \param[in] t  0-based index of the track to verify in the ChecksumSource
	 * \param[in] v2 Verifies the ARCSv2 iff \c TRUE, otherwise ARCSv1
	 *
	 * \throws std::runtime_error Iff \c b or \c t are out of range
	 *
	 * \return Index position to store the verification flag
	 */
	int verify_track(int b, int t, bool v2);

	/**
	 * \brief Return the verification status of an ARCS of the specified track
	 * in the specified block.
	 *
	 * \note
	 * The call <tt>myVerificationResult.value(0,17,true)</tt> refers to the
	 * ARCSv2 verification status of track 18 in the first block. If this call
	 * returns \c TRUE, track 18 in the first block in the \c ref_sums was
	 * matched (and thereby turned out to be ARCSv2). Whether this indicates
	 * that track 18 of the actual Checksums caused the match is implementation
	 * defined and depends on whether the result is strict(). Otherwise, it just
	 * indicates that \em one of the actual Checksums matched track 18.
	 *
	 * \param[in] b  0-based index of the block to verify in the ChecksumSource
	 * \param[in] t  0-based index of the track to verify in the ChecksumSource
	 * \param[in] v2 Returns the ARCSv2 flag iff \c TRUE, otherwise ARCSv1
	 *
	 * \throws std::runtime_error Iff \c b or \c t are out of range
	 *
	 * \return Flag for ARCS of track \c t in block \c b
	 */
	bool track(int b, int t, bool v2) const;

	/**
	 * \brief Mark the ARId of the specified block as 'matched'.
	 *
	 * \param[in] block 0-based index of the block to verify
	 *
	 * \throws std::runtime_error Iff \c b is out of range
	 *
	 * \return Index position to store the verification flag
	 */
	int verify_id(int block);

	/**
	 * \brief \c TRUE iff the ARId of the specified block matches the ARId of
	 * the original request, otherwise \c FALSE.
	 *
	 * \param[in] b 0-based index of the block to verify in \c response
	 *
	 * \throws std::runtime_error Iff \c b is out of range
	 *
	 * \return \c TRUE iff the ARId of block \c b matches the ARId of the
	 *         request
	 */
	bool id(int b) const;

	/**
	 * \brief Returns the difference for block \c b .
	 *
	 * The difference is the number of ARCSs in \c b that do not match their
	 * corresponsing positions in the request. The difference is only of
	 * relevance iff the ARId of \c b matchs the ARId of the request.
	 *
	 * \note
	 * The concrete difference value may depend on the implementation. For
	 * example, consider an album with 15 tracks. A block of reference checksums
	 * with each track not matching and a non-matching ARId will be a assigned
	 * a difference of 16 by a strict result interpretation when respecting the
	 * ARId. When ignoring the ARId (a non-strict interpretation) it does not
	 * contribute to the difference. In this case, the exact same block will
	 * have a difference of only 15.
	 *
	 * \param[in] b  0-based index of the block to verify in the ChecksumSource
	 * \param[in] v2 Returns the ARCSv2 iff \c TRUE, otherwise ARCSv1
	 *
	 * \throws std::runtime_error Iff \c b is out of range
	 *
	 * \return Difference of block \c b
	 */
	int difference(int b, bool v2) const;

	/**
	 * \brief Returns the number of analyzed reference blocks.
	 *
	 * This is identical to the total number of blocks in the ChecksumSource.
	 *
	 * \return Total number of analyzed blocks.
	 */
	int total_blocks() const;

	/**
	 * \brief Returns the number of compared tracks per reference block.
	 *
	 * This is identical to the total number of tracks in each block in the
	 * ChecksumSource.
	 *
	 * \return Total number of tracks per block.
	 */
	int tracks_per_block() const;

	/**
	 * \brief Returns the number of comparison flags stored.
	 *
	 * \note
	 * The size of a VerificationResult with a number \f$b\f$ of total_blocks()
	 * and \f$t\f$ of tracks_per_block() is \f$b * (2 * t + 1)\f$. The
	 * coefficient \f$2\f$ is required since each block is matched against
	 * ARCSv1 and ARCSv2 sums. The \f$+1\f$ is required since the ARId of each
	 * block contributes an additional verification flag to the
	 * VerificationResult.
	 *
	 * \return Number of flags stored
	 */
	size_t size() const;

	/**
	 * \brief Identify best matching block (the one with smallest difference).
	 *
	 * If there is more than one block with the smallest difference, return the
	 * one with the lowest index position.
	 *
	 * \todo API must support to access single parts the resulting tuple.
	 *
	 * \return 0-based index, ARCS version, and difference of the best block
	 */
	std::tuple<int, bool, int> best_block() const;

	/**
	 * \brief Difference of the best block in this result.
	 *
	 * This is shorthand for getting element 2 of best_block().
	 *
	 * \return Smallest difference of any block
	 */
	int best_block_difference() const;

	/**
	 * \brief TRUE iff verification obeys a strict policy.
	 *
	 * \return TRUE iff verification obeys a strict policy, otherwise FALSE.
	 */
	bool strict() const;

	/**
	 * \brief Returns a deep copy of the instance
	 *
	 * \return A deep copy of the instance
	 */
	std::unique_ptr<VerificationResult> clone() const;
};


/**
 * \brief Interface: perform a verification.
 *
 * A Verifier declares the interface for matching actual checksums against some
 * reference checksums. The actual checksums are passed as constructor arguments
 * in the concrete subclasses. Some Verifier types may return \c nullptr for
 * their actual_id().
 *
 * Function perform() does the match against the reference checksums passed as
 * an argument.
 *
 * Any Verifier can be either strict or non-strict. A strict Verifier will
 * produce a verified result only iff there is at least one block in the
 * ChecksumSource that is identical to the list of actual Checksums. Iff the
 * Verifier is non-strict, its result will qualify as verified already iff every
 * actual Checksum occurred at least in one block of the ChecksumSource.
 *
 * Any Verifier is strict() by default.
 */
class Verifier { virtual const ARId* do_actual_id() const noexcept = 0;

	virtual const Checksums* do_actual_checksums() const noexcept
	= 0;

	virtual bool do_strict() const noexcept
	= 0;

	virtual void do_set_strict(const bool strict) noexcept
	= 0;

	virtual std::unique_ptr<VerificationResult> do_perform(
			const ChecksumSource& ref_sums) const
	= 0;

	virtual std::unique_ptr<Verifier> do_clone() const
	= 0;

public:

	/**
	 * \brief Virtual default destructor.
	 */
	virtual ~Verifier() noexcept = default;

	/**
	 * \brief Actual ARId.
	 *
	 * \return Actual ARId.
	 */
	const ARId* actual_id() const noexcept;

	/**
	 * \brief Actual Checksums.
	 *
	 * \return Actual Checksums.
	 */
	const Checksums* actual_checksums() const noexcept;

	/**
	 * \brief TRUE iff verification is peformed by a strict policy.
	 *
	 * \return TRUE iff verification is peformed by a strict policy.
	 */
	bool strict() const noexcept;

	/**
	 * \brief Activate or deactivate strict verification.
	 *
	 * \param[in] strict Activate strict verification by \c TRUE.
	 */
	void set_strict(const bool strict) noexcept;

	/**
	 * \brief Perform a verification.
	 *
	 * \param[in] ref_sums Reference checksums to match against
	 *
	 * \return The verification result
	 */
	std::unique_ptr<VerificationResult> perform(const ChecksumSource& ref_sums)
		const;

	/**
	 * \brief Perform a verification.
	 *
	 * \param[in] ref_sums Reference checksums to match against
	 *
	 * \return The verification result
	 */
	std::unique_ptr<VerificationResult> perform(const DBAR& ref_sums) const;

	/**
	 * \brief Returns a deep copy of the instance
	 *
	 * \return A deep copy of the instance
	 */
	std::unique_ptr<Verifier> clone() const;
};


/**
 * \brief Verifier for a list of checksums accompanied by a ToC.
 *
 * \details
 *
 * Matches each position \c i in each set of actual checksums exclusively with
 * position \c i in the current block of reference tracks of the ChecksumSource.
 * Intuitively this means that each track in the input is matched against the
 * exact single reference track with the respective track number.
 *
 * If available each input block is checked whether its ARId is identical to
 * the on resulting from the ToC. Reference blocks whose ARId is not identical
 * get a difference of at least \c 1.
 *
 * \note AlbumVerifier is suitable for matching of entire album rips.
 *
 * \see TracksetVerifier
 */
class AlbumVerifier final : public Verifier { class Impl; std::unique_ptr<Impl>
	impl_;

	// Verifier

	virtual const ARId* do_actual_id() const noexcept final;

	virtual const Checksums* do_actual_checksums() const noexcept final;

	virtual bool do_strict() const noexcept final;

	virtual void do_set_strict(const bool strict) noexcept final;

	virtual std::unique_ptr<VerificationResult> do_perform(
			const ChecksumSource& ref_sums) const final;

	virtual std::unique_ptr<Verifier> do_clone() const final;

public:

	/**
	 * \brief Constructor for actual input sums and their ARId.
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 * \param[in] actual_id   Actual ARId to check for
	 */
	AlbumVerifier(const Checksums& actual_sums, const ARId& actual_id);

	AlbumVerifier(const AlbumVerifier& verifier);
	AlbumVerifier& operator=(const AlbumVerifier& verifier);

	AlbumVerifier(AlbumVerifier&& verifier) noexcept;
	AlbumVerifier& operator=(AlbumVerifier&& verifier) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~AlbumVerifier() noexcept;
};


/**
 * \brief Verifier for a set of checksums without a ToC.
 *
 * \details
 *
 * Matches each position \c i in each set of actual checksums with every
 * position in the current block of reference tracks of the ChecksumSource.
 * Intuitively this means that each track in the input is matched against each
 * track of the current block of reference tracks.
 *
 * This targets the situation where a set of tracks is to be matched that
 * actually forms an album but there is no ToC present. This means that there is
 * also no ARId known and maybe not even the actual order of tracks.
 *
 * TracksetVerifier requires that the set of actual checksums and the blocks
 * of the ChecksumSource have the same cardinality. This means intuitively that
 * the number of tracks in local input as well as in the reference must be the
 * same. The verification of only some tracks against a superset of references
 * is not supported.
 *
 * \note
 * TracksetVerifier is a generalization of AlbumVerifier. AlbumVerifier adds the
 * restriction that the order of tracks in the reference must be matched too.
 *
 * \note
 * AlbumVerifier is slightly less ressource-consuming than TracksetVerifier
 * since AlbumVerifier has to perform only a single match for every actual
 * input value. It is therefore recommended to prefer AlbumVerifier whenever a
 * ToC is available.
 *
 * \see AlbumVerifier
 */
class TracksetVerifier final : public Verifier
{
	class Impl;
	std::unique_ptr<Impl> impl_;

	// Verifier

	virtual const ARId* do_actual_id() const noexcept final;

	virtual const Checksums* do_actual_checksums() const noexcept final;

	virtual bool do_strict() const noexcept final;

	virtual void do_set_strict(const bool strict) noexcept final;

	virtual std::unique_ptr<VerificationResult> do_perform(
			const ChecksumSource& ref_sums) const final;

	virtual std::unique_ptr<Verifier> do_clone() const final;

public:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] actual_sums Actual checksums to check for
	 */
	explicit TracksetVerifier(const Checksums& actual_sums);

	TracksetVerifier(const TracksetVerifier& verifier);
	TracksetVerifier& operator=(const TracksetVerifier& verifier);

	TracksetVerifier(TracksetVerifier&& verifier) noexcept;
	TracksetVerifier& operator=(TracksetVerifier&& verifier) noexcept;

	/**
	 * \brief Default destructor.
	 */
	~TracksetVerifier() noexcept;
};

/** @} */
                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

