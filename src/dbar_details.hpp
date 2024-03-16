#ifndef __LIBARCSTK_DBAR_HPP__
#error "Do not include dbar_details.hpp, include dbar.hpp instead"
#endif

/**
 * \internal
 *
 * \file
 *
 * \brief Internal API for AccurateRip response parsing and syntactic entities.
 */

#ifndef __LIBARCSTK_DBAR_DETAILS_HPP__
#define __LIBARCSTK_DBAR_DETAILS_HPP__

#ifndef __LIBARCSTK_DBAR_HPP__
#include "dbar.hpp"            // for DBAR::size_type + ...
#endif

#include <cstdint>   // for uint32_t
#include <istream>   // for istream
#include <string>    // for string
#include <vector>    // for vector

namespace arcstk
{
inline namespace v_1_0_0
{

// avoid includes
class ARId;

namespace details
{

/**
 * \brief Size of bytes of a dBAR block header
 */
static constexpr int BLOCK_HEADER_BYTES { 13 };

/**
 * \brief Size in bytes of a dBAR triplet
 */
static constexpr int TRIPLET_BYTES { 9 };

/**
 * \brief Service method: Interpret 4 bytes as a 32 bit unsigned integer
 * with little endian storage, which means that the bits of b4 become the
 * most significant bits of the result.
 *
 * \param[in] b1 First input byte, least significant bits of the result
 * \param[in] b2 Second input byte
 * \param[in] b3 Third input byte
 * \param[in] b4 Fourth input byte, most significant bits of the result
 *
 * \return The bytes as 32 bit unsigned integer
 */
uint32_t le_bytes_to_uint32(const char b1, const char b2, const char b3,
	const char b4);


/**
 * \brief Indicates an invalid ARCS value.
 */
static constexpr uint32_t UNPARSED_ARCS = 0;


/**
 * \brief Indicates an invalid confidence value.
 */
static constexpr unsigned UNPARSED_CONFIDENCE = 0;


/**
 * \brief Worker: called by parse_dbar_stream() when a parse error occurrs.
 *
 * If \c e is not nullptr, e->on_error() is called. Otherwise, a
 * StreamParseException with position data is thrown as default behaviour.
 *
 * \param[in] byte_pos       Last 1-based global byte pos read before exception
 * \param[in] block          1-based block number
 * \param[in] block_byte_pos Last 1-based block byte pos read before exception
 * \param[in] e              Error handler
 *
 * \throws SteamParseException If \c e is \c nullptr or if \c e throws it
 */
void on_parse_error(const unsigned byte_pos, const unsigned block,
			const unsigned block_byte_pos, ParseErrorHandler* e);


/**
 * \brief Worker method for parsing an input stream.
 *
 * \param[in] in The stream to be parsed
 * \param[in] p  Parse handler
 * \param[in] e  Error handler
 *
 * \throw StreamReadException If reading of the stream fails
 *
 * \return Number of parsed bytes
 *
 * \todo This implementation silently relies on a little endian plattform.
 */
uint32_t parse_dbar_stream(std::istream &in, ParseHandler* p,
		ParseErrorHandler* e);

/**
 * \brief Worker method for parsing a file.
 *
 * \param[in] filename The file to be parsed
 * \param[in] p        Parse handler
 * \param[in] e        Error handler
 *
 * \throw StreamReadException If reading of the stream fails
 *
 * \return Number of parsed bytes
 */
uint32_t parse_dbar_file(const std::string& filename, ParseHandler* p,
		ParseErrorHandler* e);

/**
 * \brief Convert a dBARHeader to an ARId;
 *
 * \param[in] header The dBARHeader to convert
 *
 * \return The ARId represented by the header
 */
ARId get_arid(const DBARBlockHeader& header);

} // namespace details


/**
 * \brief Implementation of a DBAR.
 */
class DBAR::Impl final
{
	/**
	 * \brief List of total tracks.
	 *
	 * Size equals the total number of blocks. Each index position contains the
	 * total number of tracks in the block with the respektive 0-based index
	 * position.
	 */
	std::vector<unsigned> total_tracks_;

	/**
	 * \brief List of confidence values.
	 *
	 * Size equals the total accumulated number of tracks in the DBAR object.
	 * Each index position contains the confidence value for a track. The order
	 * of values is identical to the respective order of tracks in the DBAR
	 * object.
	 */
	std::vector<unsigned> confidence_;

	/**
	 * \brief Header ids, track ARCS values and ARCS values of frame 450.
	 *
	 * Size equals the sum of the size of each block. The size of a block is
	 * the sum of the constant header size (== 3) plus the product of the total
	 * number of tracks in this block and the constant track size (== 2).
	 */
	std::vector<uint32_t> sums_;  // header ids + arcss + frame450s

public:

	using size_type      = DBAR::size_type;

	/**
	 * \brief Constructor.
	 */
	Impl();

	/**
	 * \brief Total number of declared blocks..
	 *
	 * \return Size of the DBAR object.
	 */
	size_type size() const;

	/**
	 * \brief Physical total number of tracks in the specified block.
	 *
	 * \return Size of the specified block
	 */
	size_type size(const size_type block_idx) const;

	/**
	 * \brief ARCS value of a track.
	 *
	 * \param[in] block_idx Specified block index
	 * \param[in] track     Specified track index
	 *
	 * \return ARCS value of the specified track
	 */
	const uint32_t& arcs_value(const size_type block_idx, const size_type track)
		const;

	/**
	 * \brief Confidence value of a track.
	 *
	 * \param[in] block_idx Specified block index
	 * \param[in] track     Specified track index
	 *
	 * \return Confidence value of the specified track
	 */
	const unsigned& confidence_value(const size_type block_idx,
			const size_type track) const;

	/**
	 * \brief ARCS value of frame 450 of a track.
	 *
	 * \param[in] block_idx Specified block index
	 * \param[in] track     Specified track index
	 *
	 * \return ARCS value frame 450 of the specified track
	 */
	const uint32_t& frame450_arcs_value(const size_type block_idx,
			const size_type track) const;

	/**
	 * \brief Total number of tracks declared.
	 *
	 * \param[in] block_idx Block to access
	 *
	 * \return Total number of tracks block \c block_idx declares
	 */
	unsigned total_tracks(const size_type block_idx) const;

	/**
	 * \brief Header of the specified block.
	 *
	 * \param[in] block_idx Block to return header of
	 *
	 * \return Header of block \c block_idx
	 */
	DBARBlockHeader header(const size_type block_idx) const;

	/**
	 * \brief Specified triplet.
	 *
	 * \param[in] block_idx Block to lookup track
	 * \param[in] track_idx Track to return
	 *
	 * \return Triplet \c track_idx in block \c block_idx
	 */
	DBARTriplet triplet(const size_type block_idx,
		const size_type track_idx) const;

	/**
	 * \brief Add a header to the object.
	 *
	 * \param[in] total_tracks Total number of tracks in this block
	 * \param[in] id1          Id1 of the ARId
	 * \param[in] id2          Id2 of the ARId
	 * \param[in] cddb_id      CDDB Id
	 */
	void add_header(const uint8_t track_count, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id);

	/**
	 * \brief Add a triplet to the object.
	 *
	 * \param[in] arcs          ARCS value
	 * \param[in] confidence    Confidence value
	 * \param[in] frame450_arcs ARCS value of frame 450 of this track
	 */
	void add_triplet(const uint32_t arcs, const uint8_t confidence,
			const uint32_t frame450_arcs);

	friend void swap(Impl& lhs, Impl& rhs) noexcept
	{
		using std::swap;
		swap(lhs.total_tracks_, rhs.total_tracks_);
		swap(lhs.confidence_,   rhs.confidence_);
		swap(lhs.sums_,         rhs.sums_);
	}

private:

	/**
	 * \brief Number of index positions of a header.
	 */
	static constexpr unsigned header_size = 3;

	/**
	 * \brief Number of index positions of a track.
	 */
	static constexpr unsigned track_size  = 2;

	/**
	 * \brief Total number of tracks before specified block.
	 *
	 * \param[in] block Block index
	 *
	 * \return Total number of tracks of the first <tt>block - 1</tt> blocks.
	 */
	unsigned total_tracks_accumulated(const size_type block) const;

	/**
	 * \brief Start index of block \c block_idx in sums_.
	 *
	 * \param[in] block Block index
	 *
	 * \return Start index in \c sums_ for the specified block.
	 */
	size_type start_idx(const size_type block_idx) const;

	/**
	 * \brief Index position in \c sums_ of the track specified by \c block
	 * and \c track.
	 *
	 * \param[in] block Block index
	 * \param[in] track Track index
	 *
	 * \return Index in \c sums_ for the specified track.
	 */
	size_type arcs_idx(const size_type block, const size_type track) const;

	/**
	 * \brief Index position in \c confidence_ of the track specified by
	 * \c block and \c track.
	 *
	 * \param[in] block Block index
	 * \param[in] track Track index
	 *
	 * \return Index in \c confidence_ for the specified track.
	 */
	size_type confidence_idx(const size_type block_idx,
		const size_type track_idx) const;

	/**
	 * \brief Index position in \c sums_ of the track specified by \c block
	 * and \c track.
	 *
	 * \param[in] block Block index
	 * \param[in] track Track index
	 *
	 * \return Index in \c sums_ for the specified track.
	 */
	size_type frame450_arcs_idx(const size_type block_idx,
		const size_type track_idx) const;
};

} // namespace v_1_0_0
} // namespace arcstk

#endif

