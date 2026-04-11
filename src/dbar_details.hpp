#ifndef LIBARCSTK_DBAR_HPP_
#error "Do not include dbar_details.hpp, include dbar.hpp instead"
#endif

#ifndef LIBARCSTK_DBAR_DETAILS_HPP_
#define LIBARCSTK_DBAR_DETAILS_HPP_

/**
 * \internal
 *
 * \file
 *
 * \brief Implementation details for dbar.hpp.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"            // for DBAR::size_type + ...
#endif

#include <cstring>   // for memmove, memcpy, memset
#include <cstddef>   // for byte
#include <cstdint>   // for uint32_t, uint8_t
#include <iosfwd>    // for char_traits, streampos
#include <istream>   // for istream
#include <optional>  // for optional
#include <string>    // for string
#include <vector>    // for vector

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */
// avoid includes
class ARId;

namespace details
{

/**
 * \brief Maximal file size to be accepted as input.
 *
 * Currently this is the equivalent of 8 MiB.
 */
static constexpr auto MAX_DBAR_BYTES_ACCEPTABLE = std::size_t {
		static_cast<std::size_t>(8)/* MiB*/ * 1024 * 1024 };

/**
 * \brief Size of bytes of a dBAR block header
 */
static constexpr int BLOCK_HEADER_BYTES { 13 };

/**
 * \brief Size in bytes of a dBAR triplet
 */
static constexpr int TRIPLET_BYTES { 9 };

/**
 * \brief Size in bytes of \c n occurrences of \c CharT.
 *
 * Provides the size as a signed value.
 *
 * \tparam n     Total occurrences
 * \tparam CharT Char type
 *
 * \return Size in bytes
 */
template <int n, typename CharT>
struct size_of_bytes
{
	constexpr static int value = n * static_cast<int>(sizeof(CharT));
};

/**
 * \brief Alias for size_of_bytes<n, CharT>::value.
 *
 * \tparam n     Total occurrences
 * \tparam CharT Char type
 *
 * \return Size in bytes
 */
template <int n, typename CharT>
constexpr int size_of_bytes_v = size_of_bytes<n, CharT>::value;

/**
 * \brief Indicates an invalid discId1, discId2 or cddbId value.
 */
static constexpr uint32_t UNPARSED_DISC_ID = 0;

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
 * If \c e is not nullptr, e->on_error() is called. Otherwise, the default
 * behaviour defined by on_parse_error_default() is executed.
 *
 * \param[in] byte_pos       Last 1-based global byte pos before exception
 * \param[in] block          Current block index (1-based)
 * \param[in] block_byte_pos Last 1-based block byte pos before exception
 * \param[in] e              Error handler
 *
 * \throws SteamParseException On behalf of default behaviour if \c e is nullptr
 */
void on_parse_error(const byte_position_t byte_pos, const unsigned block,
			const byte_position_t block_byte_pos, ParseErrorHandler* e);

/**
 * \brief Worker: default behaviour on parse errors.
 *
 * A StreamParseException with position data is thrown.
 *
 * \param[in] byte_pos       Last 1-based global byte pos before exception
 * \param[in] block          Current block index (1-based)
 * \param[in] block_byte_pos Last 1-based block byte pos before exception
 * \param[in] e              Error handler
 *
 * \throws SteamParseException On every call, thereby providing positional data
 */
void on_parse_error_default(const byte_position_t byte_pos,
		const unsigned block, const byte_position_t block_byte_pos);

/**
 * \brief Generate a default message on parse error.
 *
 * \param[in] byte_pos       Last 1-based global byte pos before exception
 * \param[in] block          Current block index (1-based)
 * \param[in] block_byte_pos Last 1-based block byte pos before exception
 *
 * \return Default message containing the exact position of the parse error
 */
std::string default_positional_message(const byte_position_t byte_pos,
		const unsigned block, const byte_position_t block_byte_pos);

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
 */
template <typename CharT, typename TraitsT = std::char_traits<CharT>>
std::size_t parse_dbar_stream(std::basic_istream<CharT, TraitsT>& stream,
		ParseHandler* p, ParseErrorHandler* e);

// two explicit specializations for uint8_t and char
extern template
std::size_t parse_dbar_stream<uint8_t>(std::basic_istream<uint8_t>&,
		ParseHandler*, ParseErrorHandler*);

extern template
std::size_t parse_dbar_stream<char>(std::basic_istream<char>&, ParseHandler*,
		ParseErrorHandler*);

/**
 * \brief Worker method for parsing a dBAR file.
 *
 * This implementation reads the entire file by a single I/O read operation to a
 * vector and then parses the vector as a stream. The expected advantage is to
 * lower the cost of I/O by performing a single read operation. However, for
 * small files - like dBAR files - this advantage is neglectable. The
 * disadvantage is that memory consumption is doubled up by holding the file
 * content as well as the parsed DBAR object in memory. Of course, this doubling
 * is also neglectable. Implemented by using file_content().
 *
 * \param[in] filename The file to be parsed
 * \param[in] p        Parse handler
 * \param[in] e        Error handler
 *
 * \throw StreamReadException If reading of the stream fails
 *
 * \return Number of parsed bytes
 */
std::size_t parse_dbar_file(const std::string& filename, ParseHandler* p,
		ParseErrorHandler* e);

/**
 * \deprecated
 *
 * \brief Worker method for parsing a dBAR file.
 *
 * A former implementation of parse_dbar_file(): just opens the file as a stream
 * and parses the stream. This is considerably slower than the default
 * implementation.
 *
 * \param[in] filename The file to be parsed
 * \param[in] p        Parse handler
 * \param[in] e        Error handler
 *
 * \throw StreamReadException If reading of the file fails
 *
 * \return Number of parsed bytes
 */
std::size_t parse_dbar_file0(const std::string& filename, ParseHandler* p,
		ParseErrorHandler* e);

/**
 * \brief Determine file size of a filepath and throw on errors.
 *
 * \param[in] filepath The file to get the file size of
 *
 * \return File size of \c filepath
 *
 * \throw std::runtime_error If file does not exist or acquiring size fails
 */
std::uintmax_t file_size_or_throw(const std::string &filepath);

/**
 * \deprecated
 *
 * \brief Load the content of a file into a vector.
 *
 * This is a partial implementation of parse_dbar_file2().
 *
 * \param[in] filename Name of the file to load
 * \param[in] max_size Do not load file if size in bytes exceeds this value
 *
 * \throws runtime_error If file size is bigger than \c max_size
 *
 * \return File content as a sequence of bytes
 */
std::optional<std::vector<uint8_t>> file_content(const std::string &filepath,
		const std::uintmax_t max_size);

/**
 * \brief Convert a dBARHeader to an ARId;
 *
 * \param[in] header The dBARHeader to convert
 *
 * \return The ARId represented by the header
 */
ARId get_arid(const DBARBlockHeader& header);

/**
 * \brief Wrap a vector of some char type in an istream.
 */
template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class istream_wrapper : public std::basic_streambuf<CharT, TraitsT>
{
public:

	/**
	 * \brief Constructor
	 *
	 * \param[in] v The vector to wrap
	 */
	explicit istream_wrapper(std::vector<CharT>& v)
	{
		this->setg(v.data(), v.data(), v.data() + v.size());
	}

	// https://stackoverflow.com/a/8815308
};

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

	/**
	 * \brief Number of index positions of a header.
	 */
	static constexpr unsigned header_size = 3;

	/**
	 * \brief Number of index positions of a track.
	 */
	static constexpr unsigned track_size  = 2;

public:

	using size_type = DBAR::size_type;

	Impl();

	size_type size() const noexcept;

	size_type size(const size_type block_idx) const;

	const uint32_t& arcs_value(const size_type block_idx, const size_type track)
		const;

	const unsigned& confidence_value(const size_type block_idx,
			const size_type track) const;

	const uint32_t& frame450_arcs_value(const size_type block_idx,
			const size_type track) const;

	unsigned total_tracks(const size_type block_idx) const;

	DBARBlockHeader header(const size_type block_idx) const;

	DBARTriplet triplet(const size_type block_idx,
		const size_type track_idx) const;

	void add_header(const uint8_t total_tracks, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id);

	void add_triplet(const uint32_t arcs, const uint8_t confidence,
			const uint32_t frame450_arcs);


	void swap(Impl& rhs) noexcept;

	bool equals(const Impl& rhs) const noexcept;

private: /* use size_type */

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

	/**
	 * \brief Total number of tracks before specified block.
	 *
	 * \param[in] block Block index
	 *
	 * \return Total number of tracks of the first <tt>block - 1</tt> blocks.
	 */
	unsigned total_tracks_accumulated(const size_type block) const;
};


/**
 * \brief State of a CheckingDBARBuilder.
 */
class CheckingDBARBuilderState final
{
	/**
	 * \brief Type of the not yet parsed ARId.
	 */
	using id_type = std::tuple<uint8_t, uint32_t, uint32_t, uint32_t>;

	/**
	 * \brief Initial value for the internal ARId.
	 *
	 * If \c current_id_ has this value, it means that \c current_id_ has not
	 * yet been initialized.
	 */
	static constexpr id_type UNINITIALIZED_ID = { 0, 0, 0, 0 };

	/**
	 * \brief Cached previous ARId constants.
	 */
	id_type current_id_;

	/**
	 * \brief Count triplets in current block.
	 */
	std::size_t triplet_counter_;

	/**
	 * \brief Current validity value.
	 */
	bool is_valid_;

	/**
	 * \brief Current uniformity value.
	 */
	bool is_uniform_;

	/**
	 * \brief Update uniformity state.
	 *
	 * \param[in] id Update uniformity by \c id
	 */
	void update_uniformity(const id_type& id);

	/**
	 * \brief Update validity state.
	 *
	 * \param[in] total_triplets Update validity by \c total_triplets
	 */
	void update_validity(const std::size_t& total_triplets);

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	CheckingDBARBuilderState();

	/**
	 * \brief Inform about a header.
	 */
	void header(const uint8_t total_tracks, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id);

	/**
	 * \brief Inform about a triplet.
	 */
	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs);

	/**
	 * \brief Inform about end of current block.
	 */
	void end_block();

	/**
	 * \brief Return validity state.
	 *
	 * \return Current validity state.
	 */
	bool is_valid() const;

	/**
	 * \brief Return uniformity state.
	 *
	 * \return Current uniformity state.
	 */
	bool is_uniform() const;

	/**
	 * \brief Resets the instance to its initial state.
	 */
	void reset();
};


/**
 * \brief ParseHandler to build a DBAR object and check for regularity.
 */
class CheckingDBARBuilder::Impl final
{
	/**
	 * \brief Internal DBARBuilder.
	 */
	DBARBuilder builder_;

	/**
	 * \brief Internal state.
	 */
	CheckingDBARBuilderState state_;

public:

	Impl();

	// ParseHandler API mimicked

	void start_input();

	void start_block();

	void header(const uint8_t total_tracks, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id);

	void start_triplets();

	void triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs);

	void end_triplets();

	void end_block();

	void end_input();

	// Impl-specific members

	bool result_is_valid() const;

	bool result_is_uniform() const;

	DBAR result();

	void reset();
};

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk


namespace std
{
	// Define char_traits for uint8_t to use use uint8_t as a CharT.
	// Motivation: put vector<uint8_t> into an istream_wrapper

    template<>
    struct char_traits<uint8_t>
	{
        using char_type  = uint8_t;
        using int_type   = int;
        using off_type   = std::streamoff;
        using pos_type   = std::streampos;
        using state_type = std::mbstate_t;

        static void assign (char_type& r, const char_type& a) { r = a; }
        static bool eq (char_type a, char_type b)             { return a == b; }
        static bool lt (char_type a, char_type b)             { return a  < b; }

        static int compare (const char_type* s1, const char_type* s2, size_t n)
		{
            return std::memcmp(s1, s2, n);
        }

        static size_t length (const char_type* s)
		{
            size_t len = 0;
            while (s[len] != char_type(0)) { ++len; }
            return len;
        }

        static const char_type* find (const char_type* s, size_t n,
				const char_type& a)
		{
            for (size_t i = 0; i < n; ++i)
			{
                if (s[i] == a) return s + i;
            }
            return nullptr;
        }

        static char_type* copy (char_type* s1, const char_type* s2, size_t n)
		{
            return static_cast<char_type*>(std::memcpy(s1, s2, n));
        }

        static char_type* move (char_type* s1, const char_type* s2, size_t n)
		{
            return static_cast<char_type*>(std::memmove(s1, s2, n));
        }

        static char_type* assign (char_type* s, size_t n, char_type a)
		{
            return static_cast<char_type*>(
					std::memset(s, static_cast<int>(a), n));
        }

        static constexpr int_type not_eof (int_type c)
		{
            return c == eof() ? 0 : c;
        }

        static constexpr char_type to_char_type (int_type c)
		{
            return static_cast<char_type>(c);
        }

        static constexpr int_type to_int_type (char_type c)
		{
            return static_cast<int_type>(c);
        }

        static constexpr bool eq_int_type (int_type c1, int_type c2)
		{
            return c1 == c2;
        }

        static constexpr int_type eof()
		{
            return -1;
        }
    };
} // namespace std

#endif

