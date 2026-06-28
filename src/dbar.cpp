/**
 * \internal
 *
 * \file
 *
 * \brief Implementing the API for AccurateRip response parsing and syntactic
 * entities.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"
#endif
#ifndef LIBARCSTK_DBAR_DETAILS_HPP_
#include "dbar_details.hpp"
#endif

#include <algorithm>        // for for_each
#include <cstddef>          // for byte
#include <cstdint>          // for uint32_t
#include <cstdio>           // for EOF
#include <filesystem>       // for file_size
#include <fstream>          // for basic_ifstream
#include <initializer_list> // for initializer_list
#include <ios>              // for streamsize
#include <iosfwd>           // for char_traits, streampos
#include <limits>           // for numeric_limits
#include <memory>           // for unique_ptr, make_unique
#include <numeric>          // for accumulate
#include <optional>         // for optional, nullopt
#include <ostream>          // for ostream
#include <sstream>          // for ostringstream
#include <stdexcept>        // for runtime_error
#include <string>           // for string
#include <system_error>     // for error_code
#include <tuple>            // for get, tuple
#include <unordered_set>    // for unordered_set
#include <utility>          // for pair, move
#include <vector>           // for vector

#ifndef LIBARCSTK_BYTES_HPP_
#include "bytes.hpp"               // for combine
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"            // for checksum::print()
#endif
#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"          // for ARId
#endif
#ifndef LIBARCSTK_ACCURATERIP_FUNC_HPP_
#include "accuraterip_func.hpp"    // for accuraterip::id::print_impl()
#endif
#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{


namespace details
{

void on_parse_error(const byte_position_t byte_pos, const unsigned block,
		const byte_position_t block_byte_pos, ParseErrorHandler* e)
{
	if (e)
	{
		e->on_error(byte_pos, block, block_byte_pos);
	} else
	{
		on_parse_error_default(byte_pos, block, block_byte_pos);
	}
}


void on_parse_error_default(const byte_position_t byte_pos,
		const unsigned block, const byte_position_t block_byte_pos)
{
	throw StreamParseException { byte_pos, block, block_byte_pos };
}


std::string default_positional_message(const byte_position_t byte_pos,
		const unsigned block, const byte_position_t block_byte_pos)
{
	auto ss = std::ostringstream {};
	ss << "Read error after input byte " << byte_pos << " (block " << block
			<< ", byte " << block_byte_pos << ")";
	return ss.str();
}


template<typename CharT, typename TraitsT>
std::size_t parse_dbar_stream(std::basic_istream<CharT, TraitsT>& in,
		ParseHandler* p, ParseErrorHandler* e)
{
	if (!p)
	{
		ARCS_LOG_WARNING
			<< "Parser has no content handler attached, skip parsing";
		return 0;
	}

	using details::BLOCK_HEADER_BYTES;
	using details::TRIPLET_BYTES;

	// fixed sizes of the DBAR format
	constexpr auto TOTAL_HEADER_BYTES  { BLOCK_HEADER_BYTES * sizeof(CharT) };
	constexpr auto TOTAL_TRIPLET_BYTES { TRIPLET_BYTES      * sizeof(CharT) };

	constexpr auto SizeOf_4 = size_of_bytes_v<4, CharT>;
	constexpr auto SizeOf_8 = size_of_bytes_v<8, CharT>;

	// input buffers
	std::vector<CharT> id    (TOTAL_HEADER_BYTES);
	std::vector<CharT> t_buf (TOTAL_TRIPLET_BYTES);

	// parsed entities
	auto total_tracks  = unsigned { 0 };
	auto disc_id_1     = uint32_t { 0 };
	auto disc_id_2     = uint32_t { 0 };
	auto cddb_id       = uint32_t { 0 };
	auto confidence    = unsigned { 0 };
	auto trk_arcs      = uint32_t { 0 };
	auto frame450_arcs = uint32_t { 0 };

	// counters
	auto bytes_read        = std::streamsize { 0 };
	auto bytes_total       = byte_position_t { 0 };
	auto block_bytes_total = byte_position_t { 0 };
	auto bytes_expected    = unsigned        { 0 };
	auto byte_pos          = unsigned        { 0 };
	auto block_counter     = unsigned        { 0 };
	auto actual_tracks     = int             { 0 };
	auto bytes_left        = int             { 0 };

	p->start_input();

	while (true)
	{
		++block_counter;

		block_bytes_total = 0;
		bytes_read = 0;

		p->start_block();

		// Read header of current block

		if (!in.read(&id[0], TOTAL_HEADER_BYTES))
		{
			bytes_read = in.gcount();

			if (bytes_read == 0)
			{
				if (in.eof())
				{
					// This is OK if total_bytes == expected file size
					// Check result value!
					break;
				}

				if (in.fail())
				{
					on_parse_error(bytes_total, block_counter,
							block_bytes_total, e);
					break;
				}

			} // else
			// on_parse_error() is called below after processing
			// the bytes that were actually read
		} else
		{
			bytes_read = in.gcount();
		}

		ARCS_LOG(DEBUG2) << "Read " << bytes_read << " header bytes";

		bytes_total       += bytes_read;
		block_bytes_total += bytes_read;

		ARCS_LOG(DEBUG2) << "Read " << bytes_total << " bytes total";

		if (bytes_read > 0)
		{
			// At least 1 byte has been read. We want to pass the bytes parsed
			// so far to the content handler

			total_tracks = id[0] & 0xFF;

			if (bytes_read <= SizeOf_4) // disc_id_1 unfinished
			{
				p->header(total_tracks, 0, 0, 0);

				on_parse_error(bytes_total, block_counter, block_bytes_total,
						e);
				break;
			}

			disc_id_1 = combine(std::byte(id[ 1]), std::byte(id[ 2]),
								std::byte(id[ 3]), std::byte(id[ 4]));

			if (bytes_read <= SizeOf_8) // disc_id_2 unfinished
			{
				p->header(total_tracks, disc_id_1, 0, 0);

				on_parse_error(bytes_total, block_counter, block_bytes_total,
						e);
				break;
			}

			disc_id_2 = combine(std::byte(id[ 5]), std::byte(id[ 6]),
								std::byte(id[ 7]), std::byte(id[ 8]));

			if (bytes_read <= size_of_bytes_v<12, CharT>) // cddb_id unfinished
			{
				p->header(total_tracks, disc_id_1, disc_id_2, 0);

				on_parse_error(bytes_total, block_counter, block_bytes_total,
						e);
				break;
			}

			cddb_id = combine(std::byte(id[ 9]), std::byte(id[10]),
							  std::byte(id[11]), std::byte(id[12]));

			ARCS_LOG(DEBUG1) << "New block (" << total_tracks
				<< " tracks) starts. ID: "
				<< ARId { total_tracks, disc_id_1, disc_id_2, cddb_id };

			p->header(total_tracks, disc_id_1, disc_id_2, cddb_id);
		} else
		{
			on_parse_error(bytes_total, block_counter, block_bytes_total, e);
			break;
		}

		p->start_triplets();

		bytes_read = 0;

		// Read triplets of current block

		bytes_expected = total_tracks * TOTAL_TRIPLET_BYTES;

		if (t_buf.size() != bytes_expected)
		{
			t_buf.resize(bytes_expected);
		}

		if (!in.read(&t_buf[0], bytes_expected))
		{
			bytes_read = in.gcount();

			if (bytes_read == 0)
			{
				if (in.eof())
				{
					ARCS_LOG(WARNING) << "Unexpected EOF after DBARBlockHeader"
						<< ", read " << bytes_total << " bytes total";
				}

				if (in.fail())
				{
					on_parse_error(bytes_total, block_counter,
							block_bytes_total, e);
					break;
				}

			} // else
			// on_parse_error() is called below after processing
			// the bytes that were actually read
		} else
		{
			bytes_read = in.gcount();
		}

		ARCS_LOG(DEBUG2) << "Read " << bytes_read << " triplet bytes";

		bytes_total       += bytes_read;
		block_bytes_total += bytes_read;

		ARCS_LOG(DEBUG2) << "Read " << bytes_total << " bytes total";

		if (bytes_read > 0)
		{
			actual_tracks = bytes_read / static_cast<int>(TOTAL_TRIPLET_BYTES);

			byte_pos = 0;

			for (auto track = int { 0 }; track < actual_tracks; ++track)
			{
				// At least 1 byte has been read. We want to pass the bytes
				// parsed so far to the content handler

				confidence    = t_buf [byte_pos] & 0xFF;

				trk_arcs      = combine(std::byte(t_buf [byte_pos + 1]),
										std::byte(t_buf [byte_pos + 2]),
										std::byte(t_buf [byte_pos + 3]),
										std::byte(t_buf [byte_pos + 4]));

				frame450_arcs = combine(std::byte(t_buf [byte_pos + 5]),
										std::byte(t_buf [byte_pos + 6]),
										std::byte(t_buf [byte_pos + 7]),
										std::byte(t_buf [byte_pos + 8]));

				p->triplet(trk_arcs, confidence, frame450_arcs);

				byte_pos += TRIPLET_BYTES;
			} // for

			// handle bytes left, if any

			if (byte_pos != bytes_read) // 1-8 bytes left
			{
				bytes_left = static_cast<int>(bytes_read) -
					static_cast<int>(byte_pos);

				confidence = t_buf[byte_pos] & 0xFF;
				++byte_pos;

				if (bytes_left <= SizeOf_4)
				{
					// => We have read the confidence value, but reading
					// failed on the actual ARCS.

					p->triplet(UNPARSED_ARCS, confidence, UNPARSED_ARCS);
					// ARCS + frame450-ARCS are not valid

					on_parse_error(bytes_total, block_counter,
							block_bytes_total, e);
					break;
				}

				trk_arcs = combine( std::byte(t_buf[byte_pos + 1]),
									std::byte(t_buf[byte_pos + 2]),
									std::byte(t_buf[byte_pos + 3]),
									std::byte(t_buf[byte_pos + 4]));
				byte_pos += SizeOf_4;

				if (bytes_left <= SizeOf_8)
				{
					p->triplet(trk_arcs, confidence, UNPARSED_ARCS);
					// frame450-ARCS is not valid

					on_parse_error(bytes_total, block_counter,
							block_bytes_total, e);
					break;
				}
			} else // 0 bytes left
			{
				if (bytes_read != bytes_expected)
				{
					// failed just on byte 0 of a triplet sequence
					on_parse_error(bytes_total, block_counter,
							block_bytes_total, e);
					break;
				}
			}
		} else
		{
			// No bytes read, hence no bytes to process, but bytes expected!
			on_parse_error(bytes_total, block_counter, block_bytes_total, e);
			break;
		}

		if (in.eof())
		{
			ARCS_LOG(WARNING) << "Unexpected EOF after DBARTriplet, read "
				<< bytes_total << " bytes total";
			break;
		}

		if (in.fail())
		{
			on_parse_error(bytes_total, block_counter, block_bytes_total,
					e);
			break;
		}

		p->end_triplets();

		p->end_block();
	}

	p->end_input();

	ARCS_LOG(DEBUG1)  << "Parsed " << bytes_total << " bytes";

	return bytes_total;
}

template std::size_t parse_dbar_stream<char>(std::basic_istream<char>&,
		ParseHandler*, ParseErrorHandler*);


std::size_t parse_bytes(ByteVector& bytes, ParseHandler* p,
		ParseErrorHandler* e)
{
	auto byte_stream  = istream_wrapper<byte_t>(bytes);
	auto input_stream = std::basic_istream<byte_t>(&byte_stream);

	return parse_stream(input_stream, p, e);
}


std::size_t parse_dbar_file(const std::string& filepath, ParseHandler* p,
		ParseErrorHandler* e)
{
	// single read implementation

	if (auto bytes = file_content(filepath, MAX_DBAR_BYTES_ACCEPTABLE))
	{
		const auto total_bytes { parse_bytes(bytes.value(), p, e) };

		ARCS_LOG_DEBUG << "Successfully finished to parse file '"
			<< filepath << "'.";

		return total_bytes;
	} else
	{
		ARCS_LOG_INFO << "File '" << filepath << "' has size of 0, no parsing.";
	}

	return 0;
}


std::size_t parse_dbar_file0(const std::string& filepath, ParseHandler* p,
		ParseErrorHandler* e)
{
	// Get file size

	auto file_size = file_size_or_throw(filepath);

	if (file_size == 0)
	{
		return 0;
	}

	if (file_size > MAX_DBAR_BYTES_ACCEPTABLE)
	{
		auto msg = std::ostringstream{};

		msg << "File too large, more than maximum of "
			<< MAX_DBAR_BYTES_ACCEPTABLE
			<< " bytes";

		throw std::runtime_error(msg.str());
	}

	// just opens the stream to parse it manually (slow)

    auto input = std::ifstream { filepath, std::ios::binary };

    if (!input)
	{
		auto msg = std::ostringstream{};

		msg << "Failed to open file '"
			<< filepath
			<< "'";

		throw std::runtime_error(msg.str());
    }

	const auto total_bytes { parse_stream(input, p, e) };

	ARCS_LOG_DEBUG << "Successfully finished to parse file '"
		<< filepath << "'.";

	return total_bytes;
}


std::uintmax_t file_size_or_throw(const std::string &filepath)
{
	namespace fs = std::filesystem;

	// Check existence

	if (!fs::exists(filepath))
	{
        throw std::runtime_error("File not found");
    }

	// Check file size

	std::error_code rc;
    const auto file_size { fs::file_size(filepath, rc) };

	if (rc)
	{
		auto msg = std::ostringstream{};

		msg << "Unable to determine file size for file '"
			<< filepath
			<< "'";/* + "', error was: " + rc */

		throw std::runtime_error(msg.str());
	}

	return file_size;
}


std::optional<ByteVector> file_content(const std::string &filepath,
		const std::uintmax_t max_size)
{
	// Get file size

	auto file_size = file_size_or_throw(filepath);

	if (file_size == 0)
	{
		return std::nullopt;
	}

	if (file_size > max_size)
	{
		auto msg = std::ostringstream{};

		msg << "File too large, more than maximum of "
			<< max_size
			<< " bytes";

		throw std::runtime_error(msg.str());
	}

	// Check before casting to signed type when passing it to ifstream::read()
	if (file_size > static_cast<std::uintmax_t>(
				std::numeric_limits<std::streamsize>::max()))
	{
		throw std::runtime_error(
				"File too large, is not readable in a single read operation");
	}

	// Open file

    auto input = std::ifstream { filepath, std::ios::binary };

    if (!input)
	{
		auto msg = std::ostringstream{};

		msg << "Unable to correctly open file '"
			<< filepath
			<< "'";

		throw std::runtime_error(msg.str());
    }

	input.exceptions(std::ios::failbit | std::ios::badbit);

	// Load file content into vector

	auto bytes = ByteVector(file_size);

	// Note that this is the right thing in any case: we read char* from fs
	// either way and if ByteVector is uint8_t typed on this platform, the
	// chars will just be converted. However, note that this may or may not
	// lead to bytes being of type char!
	input.read(bytes.data(), static_cast<std::streamsize>(file_size));

    return bytes;
}


ARId get_arid(const DBARBlockHeader& header)
{
	return ARId { header.total_tracks(), header.id1(), header.id2(),
			header.cddb_id() };
}

} // namespace details


bool is_valid(const DBARBlock& block)
{
	return block.header().total_tracks() == block.size();
}


bool is_valid(const DBAR& dbar)
{
	using std::crbegin;
	using std::crend;

	/* empty DBAR is valid */
	return std::accumulate(crbegin(dbar), crend(dbar), true,
			[](const bool result, const DBARBlock& block)
			{
				return result && is_valid(block);
			});
}


bool is_uniform(const DBAR& dbar)
{
	using std::cbegin;
	using std::cend;

	auto ids = std::unordered_set<std::string> {};

	/* empty DBAR is uniform */
	std::for_each(cbegin(dbar), cend(dbar),
			[&ids](const DBARBlock& block)
			{
				using std::to_string;

				ids.insert(to_string(details::get_arid(block.header())));
			});

	return ids.size() <= 1; /* 0 (empty) or 1 is uniform */
}


bool is_regular(const DBAR& dbar)
{
	return is_uniform(dbar) && is_valid(dbar);
}


namespace details
{

// specialization of get_element() for DBAR
template<>
auto get_element(const DBAR& object, const typename DBAR::size_type i)
	-> DBAR::value_type
{
	return object.block(i);
};


// specialization of get_element() for DBARBlock
template<>
auto get_element(const DBARBlock& object, const typename DBARBlock::size_type i)
	-> DBARBlock::value_type
{
	return object.triplet(i);
};

} // namespace details


// DBARBlockHeader


DBARBlockHeader::DBARBlockHeader(const unsigned total_tracks,
		const uint32_t id1, const uint32_t id2, const uint32_t cddb_id)
	: total_tracks_ { total_tracks }
	, id1_          { id1 }
	, id2_          { id2 }
	, cddb_id_      { cddb_id }
{
	// empty
}


unsigned DBARBlockHeader::total_tracks() const noexcept
{
	return total_tracks_;
}


uint32_t DBARBlockHeader::id1() const noexcept
{
	return id1_;
}


uint32_t DBARBlockHeader::id2() const noexcept
{
	return id2_;
}


uint32_t DBARBlockHeader::cddb_id() const noexcept
{
	return cddb_id_;
}


void DBARBlockHeader::swap(DBARBlockHeader& rhs) noexcept
{
	using std::swap;

	swap(this->total_tracks_, rhs.total_tracks_);
	swap(this->id1_,          rhs.id1_);
	swap(this->id2_,          rhs.id2_);
	swap(this->cddb_id_,      rhs.cddb_id_);
}


bool DBARBlockHeader::equals(const DBARBlockHeader& rhs) const noexcept
{
	return this->total_tracks_  == rhs.total_tracks_
		&& this->id1_           == rhs.id1_
		&& this->id2_           == rhs.id2_
		&& this->cddb_id_       == rhs.cddb_id_;
}


std::string DBARBlockHeader::to_string() const
{
	auto out = std::ostringstream {};
	accuraterip::id::print_impl(out, total_tracks(), id1(), id2(), cddb_id(),
			", ");
	return out.str();
}


// DBARTriplet


DBARTriplet::DBARTriplet(const uint32_t arcs, const unsigned confidence,
		const uint32_t frame450_arcs)
	: arcs_          { arcs }
	, confidence_    { confidence }
	, frame450_arcs_ { frame450_arcs }
{
	// empty
}


uint32_t DBARTriplet::arcs() const noexcept
{
	return arcs_;
}


uint32_t DBARTriplet::frame450_arcs() const noexcept
{
	return frame450_arcs_;
}


unsigned DBARTriplet::confidence() const noexcept
{
	return confidence_;
}


void DBARTriplet::swap(DBARTriplet& rhs) noexcept
{
	using std::swap;

	swap(this->arcs_,          rhs.arcs_);
	swap(this->confidence_,    rhs.confidence_);
	swap(this->frame450_arcs_, rhs.frame450_arcs_);
}


bool DBARTriplet::equals(const DBARTriplet& rhs) const noexcept
{
	return this->arcs_          == rhs.arcs_
		&& this->confidence_    == rhs.confidence_
		&& this->frame450_arcs_ == rhs.frame450_arcs_;
}


std::string DBARTriplet::to_string() const
{
	using std::to_string;

	auto stream = std::ostringstream {};

	// Order in which the values occurr in the byte stream
	stream << to_string(confidence()) << ", ";
	checksum::print(stream, Checksum { arcs() });
	stream << ", ";
	checksum::print(stream, Checksum { frame450_arcs() });

	return stream.str();
}


// DBARBlock


DBARBlock::DBARBlock(const DBAR& dBAR, const DBAR::size_type block_idx)
	: dBAR_ { &dBAR }
	, idx_  { block_idx }
{
	// empty
}


void DBARBlock::set(const DBAR& dBAR, const DBAR::size_type block_idx)
{
	dBAR_ = &dBAR;
	idx_  = block_idx;
}


DBARBlock::iterator DBARBlock::begin()
{
	return DBARBlock::iterator(*this, 0);
}


DBARBlock::iterator DBARBlock::end()
{
	return DBARBlock::iterator(*this, dBAR_->size(idx_));
}


DBARBlock::const_iterator DBARBlock::cbegin() const
{
	return DBARBlock::const_iterator(*this, 0);
}


DBARBlock::const_iterator DBARBlock::cend() const
{
	return DBARBlock::const_iterator(*this, dBAR_->size(idx_));
}


DBARBlock::const_iterator DBARBlock::begin() const
{
	return this->cbegin();
}


DBARBlock::const_iterator DBARBlock::end() const
{
	return this->cend();
}


DBARBlock::reverse_iterator DBARBlock::rbegin()
{
	return DBARBlock::reverse_iterator { *this, dBAR_->size(idx_) - 1 };
}


DBARBlock::reverse_iterator DBARBlock::rend()
{
	return DBARBlock::reverse_iterator { *this,
		std::numeric_limits<DBAR::size_type>::max() };
}


DBARBlock::const_reverse_iterator DBARBlock::crbegin() const
{
	return DBARBlock::const_reverse_iterator { *this, dBAR_->size(idx_) - 1 };
}


DBARBlock::const_reverse_iterator DBARBlock::crend() const
{
	return DBARBlock::const_reverse_iterator { *this,
		std::numeric_limits<DBAR::size_type>::max() };
}


DBARBlock::const_reverse_iterator DBARBlock::rbegin() const
{
	return this->crbegin();
}


DBARBlock::const_reverse_iterator DBARBlock::rend() const
{

	return this->crend();
}


DBARBlock::size_type DBARBlock::index() const noexcept
{
	return idx_;
}


DBARBlock::size_type DBARBlock::size() const
{
	return dBAR_->size(idx_);
}


bool DBARBlock::empty() const
{
	return size() == 0;
}


DBARBlockHeader DBARBlock::header() const
{
	return dBAR_->header(idx_);
}


DBARTriplet DBARBlock::triplet(const size_type track_idx) const
{
	return dBAR_->triplet(idx_, track_idx);
}


ARId DBARBlock::id() const
{
	return details::get_arid(header());
}


void DBARBlock::swap(DBARBlock& rhs) noexcept
{
	using std::swap;

	swap(this->dBAR_, rhs.dBAR_);
	swap(this->idx_,  rhs.idx_);
}


bool DBARBlock::equals(const DBARBlock& rhs) const noexcept
{
	return dBAR_ == rhs.dBAR_ || dBAR_->equals(*rhs.dBAR_)
		|| idx_ == rhs.idx_;
}


// DBAR::Impl


DBAR::Impl::Impl() // TODO redundant? could just be defaulted
	: total_tracks_ { /* default */ }
	, confidence_   { /* default */ }
	, sums_         { /* default */ }
{
	// empty
}


DBAR::Impl::size_type DBAR::Impl::size() const noexcept
{
	return total_tracks_.size();
}


DBAR::Impl::size_type DBAR::Impl::size(const size_type block_idx) const
{
	auto end = size_type { 0 };

	if (block_idx + 1 < size())
	{
		end = start_idx(block_idx + 1);
	} else
	{
		end = sums_.size();
	}

	return (end - start_idx(block_idx) - header_size) / track_size;
}


const uint32_t& DBAR::Impl::arcs_value(const size_type block_idx,
		const size_type track_idx) const
{
	return sums_[arcs_idx(block_idx, track_idx)];
}


const unsigned& DBAR::Impl::confidence_value(
		const size_type block_idx,
		const size_type track) const
{
	return confidence_[total_tracks_accumulated(block_idx) + track];
}


const uint32_t& DBAR::Impl::frame450_arcs_value(const size_type block_idx,
		const size_type track_idx) const
{
	return sums_[frame450_arcs_idx(block_idx, track_idx)];
}


void DBAR::Impl::add_header(const uint8_t total_tracks, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id)
{
	total_tracks_.push_back(total_tracks);

	sums_.push_back(id1);
	sums_.push_back(id2);
	sums_.push_back(cddb_id);
}


void DBAR::Impl::add_triplet(const uint32_t arcs,
			const uint8_t confidence, const uint32_t frame450_arcs)
{
	confidence_.push_back(confidence);

	sums_.push_back(arcs);
	sums_.push_back(frame450_arcs);
}


unsigned DBAR::Impl::total_tracks(const DBAR::Impl::size_type block_idx) const
{
	return total_tracks_[block_idx];
}


DBARBlockHeader DBAR::Impl::header(const DBAR::Impl::size_type block_idx) const
{
	const auto i = start_idx(block_idx);
	return DBARBlockHeader {
		total_tracks(block_idx),
		sums_[i], sums_[i + 1], sums_[i + 2],
		};
}


DBARTriplet DBAR::Impl::triplet(const DBAR::Impl::size_type block_idx,
		const DBAR::Impl::size_type track_idx)
	const
{
	return DBARTriplet {
		sums_[arcs_idx(block_idx, track_idx)],
		confidence_[confidence_idx(block_idx, track_idx)],
		sums_[frame450_arcs_idx(block_idx, track_idx)] };
}


unsigned DBAR::Impl::total_tracks_accumulated(const size_type block_idx) const
{
	using diff_type = decltype( total_tracks_ )::difference_type;

	using std::cbegin;

	return std::accumulate(
		cbegin(total_tracks_),
		cbegin(total_tracks_) + static_cast<diff_type>(block_idx),
		0u);
}


DBAR::Impl::size_type DBAR::Impl::start_idx(
		const size_type block_idx) const
{
	return block_idx * header_size
		+ static_cast<size_type>(total_tracks_accumulated(block_idx))
		* track_size;
}


DBAR::Impl::size_type DBAR::Impl::arcs_idx(
		const size_type block_idx, const size_type track_idx) const
{
	return start_idx(block_idx) + header_size + track_idx * track_size;
}


DBAR::Impl::size_type DBAR::Impl::confidence_idx(
		const size_type block_idx, const size_type track_idx) const
{
	return total_tracks_accumulated(block_idx) + track_idx;
}


DBAR::Impl::size_type DBAR::Impl::frame450_arcs_idx(
		const size_type block_idx, const size_type track_idx) const
{
	return arcs_idx(block_idx, track_idx) + 1;
}


void DBAR::Impl::swap(Impl& rhs) noexcept
{
	using std::swap;

	swap(this->total_tracks_, rhs.total_tracks_);
	swap(this->confidence_,   rhs.confidence_);
	swap(this->sums_,         rhs.sums_);
}


bool DBAR::Impl::equals(const Impl& rhs) const noexcept
{
	return total_tracks_ == rhs.total_tracks_
		&& confidence_   == rhs.confidence_
		&& sums_         == rhs.sums_;
}


// DBAR


DBAR::DBAR()
	: impl_ { std::make_unique<DBAR::Impl>() }
{
	// empty
}


DBAR::DBAR(std::initializer_list<
			std::pair<
				std::tuple<unsigned, uint32_t, uint32_t, uint32_t>,
				std::initializer_list<std::tuple<uint32_t, int, uint32_t>>>>
			blocks)
	: impl_ { std::make_unique<DBAR::Impl>() }
{
	for (const auto& block : blocks)
	{
		impl_->add_header(
			std::get<0>(block.first),
			std::get<1>(block.first),
			std::get<2>(block.first),
			std::get<3>(block.first)
		);

		for (const auto& t : block.second)
		{
			impl_->add_triplet(
				std::get<0>(t),
				std::get<1>(t),
				std::get<2>(t)
			);
		}
	}
}


DBAR::DBAR(std::unique_ptr<DBAR::Impl> impl)
	: impl_ { std::move(impl) }
{
	//empty
}


DBAR::DBAR(const DBAR& rhs)
	: impl_ { std::make_unique<DBAR::Impl>(*rhs.impl_) }
{
	// empty
}


DBAR& DBAR::operator= (const DBAR& rhs)
{
	if (&rhs != this)
	{
		auto tmp { std::make_unique<DBAR::Impl>(*rhs.impl_) };
		impl_ = std::move(tmp);
	}
	return *this;
}


DBAR::DBAR(DBAR&& rhs) noexcept = default;


DBAR& DBAR::operator= (DBAR&& rhs) noexcept = default;


DBAR::~DBAR() noexcept = default; // Pimpl requirement


DBAR::size_type DBAR::size() const noexcept
{
	return impl_->size();
}


DBAR::size_type DBAR::size(const DBAR::size_type block_idx) const
{
	return impl_->size(block_idx);
}


const uint32_t& DBAR::arcs_value(const DBAR::size_type block,
		const DBAR::size_type track) const
{
	return impl_->arcs_value(block, track);
}


const unsigned& DBAR::confidence_value(const DBAR::size_type block,
		const DBAR::size_type track) const
{
	return impl_->confidence_value(block, track);
}


const uint32_t& DBAR::frame450_arcs_value(const DBAR::size_type block,
		const DBAR::size_type track) const
{
	return impl_->frame450_arcs_value(block, track);
}


unsigned DBAR::total_tracks(const DBAR::size_type block_idx) const
{
	return impl_->total_tracks(block_idx);
}


DBARBlockHeader DBAR::header(const DBAR::size_type block_idx) const
{
	return impl_->header(block_idx);
}


DBARTriplet DBAR::triplet(const DBAR::size_type block_idx,
		const DBAR::size_type track_idx) const
{
	return impl_->triplet(block_idx, track_idx);
}


DBARBlock DBAR::block(const DBAR::size_type block_idx) const
{
	return DBARBlock { *this, block_idx };
}


DBAR::iterator DBAR::begin()
{
	return DBAR::iterator { *this, 0 };
}


DBAR::iterator DBAR::end()
{
	return DBAR::iterator { *this, impl_->size() };
}


DBAR::const_iterator DBAR::cbegin() const
{
	return DBAR::const_iterator { *this, 0 };
}


DBAR::const_iterator DBAR::cend() const
{
	return DBAR::const_iterator { *this, impl_->size() };
}


DBAR::const_iterator DBAR::begin() const
{
	return this->cbegin();
}


DBAR::const_iterator DBAR::end() const
{
	return this->cend();
}


DBAR::reverse_iterator DBAR::rbegin()
{
	return DBAR::reverse_iterator { *this, impl_->size() - 1 };
}


DBAR::reverse_iterator DBAR::rend()
{
	return DBAR::reverse_iterator { *this,
		std::numeric_limits<DBAR::size_type>::max() };
}


DBAR::const_reverse_iterator DBAR::crbegin() const
{
	return DBAR::const_reverse_iterator { *this, impl_->size() - 1 };
}


DBAR::const_reverse_iterator DBAR::crend() const
{
	return DBAR::const_reverse_iterator { *this,
		std::numeric_limits<DBAR::size_type>::max() };
}


DBAR::const_reverse_iterator DBAR::rbegin() const
{
	return this->crbegin();
}


DBAR::const_reverse_iterator DBAR::rend() const
{
	return this->crend();
}


bool DBAR::empty() const noexcept
{
	return impl_->size() == 0;
}


DBAR::operator bool() const noexcept
{
	return !empty();
}


void DBAR::swap(DBAR& rhs) noexcept
{
	impl_->swap(*rhs.impl_);
}


bool DBAR::equals(const DBAR& rhs) const noexcept
{
	return impl_->equals(*rhs.impl_);
}


// ParseHandler


void ParseHandler::start_input()
{
	do_start_input();
}


void ParseHandler::start_block()
{
	do_start_block();
}


void ParseHandler::header(const uint8_t total_tracks, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id)
{
	do_header(total_tracks, id1, id2, cddb_id);
}


void ParseHandler::start_triplets()
{
	do_start_triplets();
}


void ParseHandler::triplet(const uint32_t arcs,
			const uint8_t confidence,
			const uint32_t frame450_arcs)
{
	do_triplet(arcs, confidence, frame450_arcs);
}


void ParseHandler::end_triplets()
{
	do_end_triplets();
}


void ParseHandler::end_block()
{
	do_end_block();
}


void ParseHandler::end_input()
{
	do_end_input();
}


// DBARBuilder


DBARBuilder::DBARBuilder() // TODO redundant? could just be defaulted
	: result_ { nullptr }
{
	// empty
}


DBARBuilder::~DBARBuilder() noexcept = default; // Pimpl requirement


void DBARBuilder::do_start_input()
{
	reset();
}


void DBARBuilder::do_start_block()
{
	// empty
}


void DBARBuilder::do_header(const uint8_t total_tracks, const uint32_t id1,
	const uint32_t id2, const uint32_t cddb_id)
{
	result_->add_header(total_tracks, id1, id2, cddb_id);
}


void DBARBuilder::do_start_triplets()
{
	// empty
}


void DBARBuilder::do_triplet(const uint32_t arcs,
	const uint8_t confidence, const uint32_t frame450_arcs)
{
	result_->add_triplet(arcs, confidence, frame450_arcs);
}


void DBARBuilder::do_end_triplets()
{
	// empty
}


void DBARBuilder::do_end_block()
{
	// empty
}


void DBARBuilder::do_end_input()
{
	// empty
}


DBAR DBARBuilder::result()
{
	if (result_)
	{
		return DBAR { std::move(result_) };
	}

	throw std::runtime_error("Cannot obtain parsing result before parsing");
}


void DBARBuilder::reset()
{
	result_ = std::make_unique<DBAR::Impl>();
}


// CheckingDBARBuilderState


CheckingDBARBuilderState::CheckingDBARBuilderState()
	: current_id_      { UNINITIALIZED_ID }
	, is_valid_        { true }
	, is_uniform_      { true }
{
	// empty
}


void CheckingDBARBuilderState::update_uniformity(const id_type& id)
{
	// previous and current id are identical?
	is_uniform_ = (current_id_ == id);
}


void CheckingDBARBuilderState::update_validity(const std::size_t& total_tracks)
{
	// all expected tracks counted?
	is_valid_ = std::get<0>(current_id_) == total_tracks;
}


void CheckingDBARBuilderState::header(const uint8_t total_tracks,
		const uint32_t id1, const uint32_t id2, const uint32_t cddb_id)
{
	if (is_uniform_ && current_id_ != UNINITIALIZED_ID)
	{
		update_uniformity({ total_tracks, id1, id2, cddb_id });
	}
	current_id_ = { total_tracks, id1, id2, cddb_id };
}


void CheckingDBARBuilderState::triplet(const uint32_t /*arcs*/,
		const uint8_t /*confidence*/, const uint32_t /*frame450_arcs*/)
{
	++triplet_counter_;
}


void CheckingDBARBuilderState::end_block()
{
	if (is_valid_)
	{
		update_validity(triplet_counter_);
	}
	triplet_counter_ = 0;
}


bool CheckingDBARBuilderState::is_uniform() const
{
	return is_uniform_;
}


bool CheckingDBARBuilderState::is_valid() const
{
	return is_valid_;
}


void CheckingDBARBuilderState::reset()
{
	current_id_      = UNINITIALIZED_ID;
	triplet_counter_ = 0;
	is_valid_        = true;
	is_uniform_      = true;
}


// CheckingDBARBuilder::Impl


CheckingDBARBuilder::Impl::Impl() // TODO redundant? could just be defaulted
	: builder_ { /* default */ }
	, state_   { /* default */ }
{
	// empty
}


void CheckingDBARBuilder::Impl::start_input()
{
	builder_.start_input();
}


void CheckingDBARBuilder::Impl::start_block()
{
	builder_.start_block();
}


void CheckingDBARBuilder::Impl::header(const uint8_t total_tracks,
		const uint32_t id1, const uint32_t id2, const uint32_t cddb_id)
{
	builder_.header(total_tracks, id1, id2, cddb_id);
	state_  .header(total_tracks, id1, id2, cddb_id); // for checks
}


void CheckingDBARBuilder::Impl::start_triplets()
{
	builder_.start_triplets();
}


void CheckingDBARBuilder::Impl::triplet(const uint32_t arcs,
	const uint8_t confidence, const uint32_t frame450_arcs)
{
	builder_.triplet(arcs, confidence, frame450_arcs);
	state_  .triplet(arcs, confidence, frame450_arcs); // for checks
}


void CheckingDBARBuilder::Impl::end_triplets()
{
	builder_.end_triplets();
}


void CheckingDBARBuilder::Impl::end_block()
{
	builder_.end_block();
	state_  .end_block(); // for checks
}


void CheckingDBARBuilder::Impl::end_input()
{
	builder_.end_input();
}


bool CheckingDBARBuilder::Impl::result_is_valid() const
{
	return state_.is_valid();
}


bool CheckingDBARBuilder::Impl::result_is_uniform() const
{
	return state_.is_uniform();
}


DBAR CheckingDBARBuilder::Impl::result()
{
	return builder_.result();
}


void CheckingDBARBuilder::Impl::reset()
{
	builder_.reset();
	state_.reset();
}


// CheckingDBARBuilder


CheckingDBARBuilder::CheckingDBARBuilder()
	: impl_ { std::make_unique<Impl>() }
{
	// empty
}


void CheckingDBARBuilder::do_start_input()
{
	impl_->start_input();
}


void CheckingDBARBuilder::do_start_block()
{
	impl_->start_block();
}


void CheckingDBARBuilder::do_header(const uint8_t total_tracks,
		const uint32_t id1, const uint32_t id2, const uint32_t cddb_id)
{
	impl_->header(total_tracks, id1, id2, cddb_id);
}


void CheckingDBARBuilder::do_start_triplets()
{
	impl_->start_triplets();
}


void CheckingDBARBuilder::do_triplet(const uint32_t arcs,
	const uint8_t confidence, const uint32_t frame450_arcs)
{
	impl_->triplet(arcs, confidence, frame450_arcs);
}


void CheckingDBARBuilder::do_end_triplets()
{
	impl_->end_triplets();
}


void CheckingDBARBuilder::do_end_block()
{
	impl_->end_block();
}


void CheckingDBARBuilder::do_end_input()
{
	impl_->end_input();
}


bool CheckingDBARBuilder::result_is_valid() const
{
	return impl_->result_is_valid();
}


bool CheckingDBARBuilder::result_is_uniform() const
{
	return impl_->result_is_uniform();
}


bool CheckingDBARBuilder::result_is_regular() const
{
	return result_is_valid() && result_is_uniform();
}


DBAR CheckingDBARBuilder::result()
{
	return impl_->result();
}


void CheckingDBARBuilder::reset()
{
	impl_->reset();
}


// ParseErrorHandler


void ParseErrorHandler::on_error(const byte_position_t bytes,
		const unsigned blocks, const byte_position_t block_bytes)
{
	do_on_error(bytes, blocks, block_bytes);
}


// StreamParseException


StreamParseException::StreamParseException(const byte_position_t byte_pos,
		const unsigned block, const byte_position_t block_byte_pos,
		const std::string& what_arg)
	: std::runtime_error { what_arg       }
	, byte_pos_          { byte_pos       }
	, block_             { block          }
	, block_byte_pos_    { block_byte_pos }
{
	// empty
}


StreamParseException::StreamParseException(const byte_position_t byte_pos,
		const unsigned block, const byte_position_t block_byte_pos)
	: std::runtime_error {
		details::default_positional_message(byte_pos, block, block_byte_pos) }
	, byte_pos_       { byte_pos       }
	, block_          { block          }
	, block_byte_pos_ { block_byte_pos }
{
	// empty
}


byte_position_t StreamParseException::byte_position() const noexcept
{
	return byte_pos_;
}


byte_position_t StreamParseException::block() const noexcept
{
	return block_;
}


byte_position_t StreamParseException::block_byte_position() const noexcept
{
	return block_byte_pos_;
}


// parse_stream()


template<typename CharT, typename TraitsT>
std::size_t parse_stream(std::basic_istream<CharT, TraitsT>& in,
		ParseHandler* p, ParseErrorHandler* e)
{
	return details::parse_dbar_stream<CharT, TraitsT>(in, p, e);
}

template std::size_t parse_stream<char>(std::basic_istream<char>&,
		ParseHandler*, ParseErrorHandler*);


// parse_file()


uint32_t parse_file(const std::string& filename, ParseHandler* p,
		ParseErrorHandler* e)
{
	return details::parse_dbar_file(filename, p, e);
}


// make_dbar()


DBAR make_dbar(const std::string& filename)
{
	DBARBuilder builder {};
	details::parse_dbar_file(filename, &builder, nullptr);
	return builder.result();
}


// validated_dbar()


std::tuple<DBAR,bool,bool> validated_dbar(const std::string& filename)
{
	CheckingDBARBuilder builder {};
	details::parse_dbar_file(filename, &builder, nullptr);
	return { builder.result(), builder.result_is_valid(),
		builder.result_is_uniform() };
}

} // namespace v_1_0_0
} // namespace arcstk

