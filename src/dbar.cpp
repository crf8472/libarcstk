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
#include <cstdint>          // for uint32_t
#include <cstdio>           // for EOF
#include <filesystem>       // for file_size
#include <fstream>          // for basic_ifstream
#include <initializer_list> // for initializer_list
#include <memory>           // for unique_ptr, make_unique
#include <numeric>			// for accumulate
#include <set>				// for set
#include <sstream>			// for ostringstream
#include <stdexcept>		// for runtime_error
#include <string>			// for string
#include <tuple>			// for get, tuple
#include <utility>			// for pair, move
#include <vector>			// for vector

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"
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

uint32_t le_bytes_to_uint32(const char b1, const char b2, const char b3,
		const char b4)
{
	return  static_cast<uint32_t>(b4 & 0xFF) << 24 |
			static_cast<uint32_t>(b3 & 0xFF) << 16 |
			static_cast<uint32_t>(b2 & 0xFF) <<  8 |
			static_cast<uint32_t>(b1 & 0xFF);
}


void on_parse_error(const unsigned byte_pos, const unsigned block,
		const unsigned block_byte_pos, ParseErrorHandler* e)
{
	if (e)
	{
		e->on_error(byte_pos, block, block_byte_pos);
	} else
	{
		on_parse_error_default(byte_pos, block, block_byte_pos);
	}
}


void on_parse_error_default(const unsigned byte_pos, const unsigned block,
		const unsigned block_byte_pos)
{
	throw StreamParseException { byte_pos, block, block_byte_pos };
}


std::string default_positional_message(const unsigned byte_pos,
		const unsigned block, const unsigned block_byte_pos)
{
	auto ss = std::ostringstream {};
	ss << "Read error on input byte " << byte_pos << " (block " << block
			<< ", byte " << block_byte_pos << ")";
	return ss.str();
}


uint32_t parse_dbar_stream(std::istream& in, ParseHandler* p,
		ParseErrorHandler* e)
{
	if (!p)
	{
		ARCS_LOG_WARNING
			<< "Parser has no content handler attached, skip parsing";
		return 0;
	}

	using details::BLOCK_HEADER_BYTES;
	using details::TRIPLET_BYTES;

	std::vector<char> id(BLOCK_HEADER_BYTES * sizeof(char));
	std::vector<char> triplet(TRIPLET_BYTES * sizeof(char));

	auto track_count   = unsigned { 0 };
	auto discId1       = uint32_t { 0 };
	auto discId2       = uint32_t { 0 };
	auto cddbId        = uint32_t { 0 };
	auto confidence    = unsigned { 0 };
	auto trk_arcs      = uint32_t { 0 };
	auto frame450_arcs = uint32_t { 0 };

	auto bytes_read         = unsigned { 0 };
	auto byte_counter       = unsigned { 0 };
	auto block_counter      = unsigned { 0 };
	auto block_byte_counter = unsigned { 0 };

	p->start_input();

	while (in.good() and in.peek() != EOF)
	{
		++block_counter;

		block_byte_counter = 0;
		bytes_read = 0;

		p->start_block();

		// Read header of current block

		// track_count = 0;
		// discId1 = 0;
		// discId2 = 0;
		// cddbId  = 0;

		try
		{
			in.read(&id[0], BLOCK_HEADER_BYTES * sizeof(id[0]));
			bytes_read = in.gcount();

		} catch (const std::istream::failure& flr)
		{
			bytes_read = in.gcount();
		}

		ARCS_LOG(DEBUG2) << "Read " << bytes_read << " header bytes";

		byte_counter += bytes_read;
		block_byte_counter += bytes_read;

		ARCS_LOG(DEBUG2) << "Read " << byte_counter << " bytes total";

		if (bytes_read == 0)
		{
			on_parse_error(byte_counter, block_counter, block_byte_counter, e);
			break;
		} else
		{
			// At least 1 byte has been read. We want to pass the bytes parsed
			// so far to the content handler

			track_count = id[0] & 0xFF;

			if (bytes_read <= 4 * sizeof(id[0]))
			{
				p->header(track_count, 0, 0, 0);

				on_parse_error(byte_counter, block_counter, block_byte_counter,
						e);
				break;
			}

			discId1 = le_bytes_to_uint32(id[1], id[ 2], id[ 3], id[ 4]);

			if (bytes_read <= 8 * sizeof(id[0]))
			{
				p->header(track_count, discId1, 0, 0);

				on_parse_error(byte_counter, block_counter, block_byte_counter,
						e);
				break;
			}

			discId2 = le_bytes_to_uint32(id[5], id[ 6], id[ 7], id[ 8]);

			if (bytes_read <= 12 * sizeof(id[0]))
			{
				p->header(track_count, discId1, discId2, 0);

				on_parse_error(byte_counter, block_counter, block_byte_counter,
						e);
				break;
			}

			cddbId  = le_bytes_to_uint32(id[9], id[10], id[11], id[12]);

			ARCS_LOG(DEBUG1) << "New block (" << track_count
				<< " tracks) starts. ID: "
				<< ARId(track_count, discId1, discId2, cddbId).filename();

			p->header(track_count, discId1, discId2, cddbId);
		}

		if (not in.good())
		{
			on_parse_error(byte_counter, block_counter, block_byte_counter, e);
			break;
		}

		p->start_triplets();

		bytes_read = 0;

		// Read triplets of current block

		for (auto trk = uint8_t { 0 }; trk < track_count; ++trk)
		{
			try
			{
				in.read(&triplet[0], TRIPLET_BYTES * sizeof(triplet[0]));
				bytes_read = in.gcount();

			} catch (std::istream::failure& flr)
			{
				bytes_read = in.gcount();
			}

			ARCS_LOG(DEBUG2) << "Read " << bytes_read << " triplet bytes";

			byte_counter += bytes_read;
			block_byte_counter += bytes_read;

			ARCS_LOG(DEBUG2) << "Read " << byte_counter << " bytes total";

			if (bytes_read == 0)
			{
				on_parse_error(byte_counter, block_counter, block_byte_counter,
						e);
				break;
			} else
			{
				// At least 1 byte has been read. We want to pass the bytes
				// parsed so far to the content handler

				confidence = triplet[0] & 0xFF;

				if (bytes_read <= 4 * sizeof(id[0]))
				{
					// => We have read the confidence value, but reading
					// failed on the actual ARCS.

					p->triplet(UNPARSED_ARCS, confidence, UNPARSED_ARCS);
					// ARCS + frame450 ARCS are not valid

					on_parse_error(byte_counter, block_counter,
							block_byte_counter, e);
					break;
				}

				trk_arcs = le_bytes_to_uint32(
								triplet[1], triplet[2],
								triplet[3], triplet[4]);

				if (bytes_read <= 8 * sizeof(id[0]))
				{
					p->triplet(trk_arcs, confidence, UNPARSED_ARCS);
					// frame450 ARCS is not valid

					on_parse_error(byte_counter, block_counter,
							block_byte_counter, e);
					break;
				}

				frame450_arcs = le_bytes_to_uint32(
								triplet[5], triplet[6],
								triplet[7], triplet[8]);

				p->triplet(trk_arcs, confidence, frame450_arcs);
				// everything is valid
			}

			if (not in.good())
			{
				on_parse_error(byte_counter, block_counter, block_byte_counter,
						e);
			}
		} // for

		p->end_triplets();

		p->end_block();
	}

	p->end_input();

	ARCS_LOG(DEBUG1)  << "Parsed " << byte_counter << " bytes";

	return byte_counter;
}


uint32_t parse_dbar_file(const std::string& filename, ParseHandler* p,
		ParseErrorHandler* e)
{
	std::ifstream input_stream;
	input_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		input_stream.open(filename, std::ifstream::in | std::ifstream::binary);
	}
	catch (const std::ifstream::failure& f)
	{
		throw std::runtime_error(std::string{
			"Failed to open file '" + filename + "'. Message: " + f.what()
		});
	}

	const auto byte_counter { parse_stream(input_stream, p, e) };

	ARCS_LOG_DEBUG << "Successfully finished to parse file '"
		<< filename << "'.";

	return byte_counter;
}


std::size_t parse_dbar_file2(const std::string& filename, ParseHandler* p,
		ParseErrorHandler* e)
{
	static constexpr auto MAX_BYTES = std::size_t { 8/* MiB*/ * 1024 * 1024 };

	auto bytes        = file_content(filename, MAX_BYTES);
	auto char_stream  = istream_wrapper(bytes);
	auto input_stream = std::istream(&char_stream);

	const auto byte_counter { parse_stream(input_stream, p, e) };

	ARCS_LOG_DEBUG << "Successfully finished to parse file '"
		<< filename << "'.";

	return byte_counter;
}


std::vector<char> file_content(const std::string &filepath, const
		std::size_t max_size)
{
	// Check file size

    const auto length { std::filesystem::file_size(filepath) };

	if (length > max_size)
	{
		using std::to_string;

		throw std::runtime_error("File too large, more than maximum of " +
				to_string(max_size) + " bytes");
	}

	// Load file content into vector

    auto input = std::ifstream { filepath, std::ios::in | std::ios::binary };

    if (!input.good())
	{
		using std::to_string;

		throw std::runtime_error("Unable to correctly open file '" + filepath
				+ "'");
    }

    auto bytes = std::vector<char>(length);

    input.read(bytes.data(), static_cast<long>(length));
	// https://www.reddit.com/r/cpp_questions/comments/zl9p9p/is_there_a_better_way_to_read_a_file_into_a/
	// https://stackoverflow.com/a/77038066

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
	using std::cbegin;
	using std::cend;

	/* empty DBAR is valid */
	return std::accumulate(cbegin(dbar), cend(dbar), true,
			[](const bool result, const DBARBlock& block)
			{
				return result && is_valid(block);
			});
}


bool is_regular(const DBAR& dbar)
{
	auto same_id = false;

	/* requirement 1: same ARId in all blocks */
	{
		using std::cbegin;
		using std::cend;

		auto ids = std::set<std::string> {};

		// TODO only last block can be irregular, start with crbegin
		std::for_each(cbegin(dbar), cend(dbar),
				[&ids](const DBARBlock& block)
				{
					ids.insert(details::get_arid(block.header()).to_string());
				});

		same_id = ids.size() <= 1; /* 0 or 1 is regular */
	}

	return same_id && /* requirement 2: dbar is valid */is_valid(dbar);
}


// specialization for DBAR
template<>
auto get_element(const DBAR& object, const typename DBAR::size_type i)
	-> DBAR::value_type
{
	return object.block(i);
};


// specialization for DBARBlock
template<>
auto get_element(const DBARBlock& object, const typename DBARBlock::size_type i)
	-> DBARBlock::value_type
{
	return object.triplet(i);
};


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
	using std::to_string;

	// Order in which the values occurr in the byte stream
	return to_string(total_tracks()) + ", "
		+  ACCURATERIP::default_id_format(id1()) + ", "
		+  ACCURATERIP::default_id_format(id2()) + ", "
		+  ACCURATERIP::default_id_format(cddb_id());
}


// DBARTriplet


DBARTriplet::DBARTriplet()
	: arcs_          { 0 }
	, confidence_    { 0 }
	, frame450_arcs_ { 0 }
{
	// empty
}


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

	// Order in which the values occurr in the byte stream
	return to_string(confidence()) + ", "
		+ ACCURATERIP::default_arcs_format(arcs()) + ", "
		+ ACCURATERIP::default_arcs_format(frame450_arcs());
}


// DBARBlock


DBARBlock::DBARBlock()
	: dBAR_ { nullptr }
	, idx_  { 0 }
{
	// empty
}


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


DBARBlock::size_type DBARBlock::index() const noexcept
{
	return idx_;
}


DBARBlock::size_type DBARBlock::size() const
{
	return dBAR_->size(idx_);
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


DBAR::Impl::Impl()
	: total_tracks_ { /* default */ }
	, confidence_   { /* default */ }
	, sums_         { /* default */ }
{
	// empty
}


DBAR::Impl::size_type DBAR::Impl::size() const
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


void DBAR::Impl::add_header(const uint8_t track_count, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id)
{
	total_tracks_.push_back(track_count);

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
		+ total_tracks_accumulated(block_idx) * track_size;
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


DBAR::DBAR(DBAR&& rhs) noexcept
	: impl_ { std::move(rhs.impl_) }
{
	// empty
}


DBAR& DBAR::operator= (DBAR&& rhs) noexcept
{
	impl_ = std::move(rhs.impl_);
	return *this;
}


DBAR::~DBAR() noexcept = default;


DBAR::size_type DBAR::size() const noexcept
{
	return impl_->size();
}


DBAR::size_type DBAR::size(const DBAR::size_type block_idx) const
{
	return impl_->size(block_idx);
}


bool DBAR::empty() const noexcept
{
	return impl_->size() == 0;
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


void ParseHandler::header(const uint8_t track_count, const uint32_t id1,
			const uint32_t id2, const uint32_t cddb_id)
{
	do_header(track_count, id1, id2, cddb_id);
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


DBARBuilder::DBARBuilder()
	: result_ { nullptr }
{
	// empty
}


DBARBuilder::~DBARBuilder() noexcept = default;


DBAR DBARBuilder::result()
{
	if (result_)
	{
		//return DBAR(std::make_unique<DBAR::Impl>(*result_));
		return DBAR(std::move(result_));
	}

	throw std::runtime_error("Cannot obtain parsing result before parsing");
}


void DBARBuilder::do_start_input()
{
	if (result_)
	{
		result_.reset();
	}

	result_ = std::make_unique<DBAR::Impl>();
	// Initializing with nullptr is okay as long as DBARBuilder does not
	// try to get an iterator of the object.
}


void DBARBuilder::do_start_block()
{
	// TODO
}


void DBARBuilder::do_header(const uint8_t track_count, const uint32_t id1,
	const uint32_t id2, const uint32_t cddb_id)
{
	result_->add_header(track_count, id1, id2, cddb_id);
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
	// TODO
}


void DBARBuilder::do_end_input()
{
	// empty
}


// ParseErrorHandler


void ParseErrorHandler::on_error(const unsigned bytes, const unsigned blocks,
		const unsigned block_bytes)
{
	do_on_error(bytes, blocks, block_bytes);
}


// StreamParseException


StreamParseException::StreamParseException(const unsigned byte_pos,
		const unsigned block, const unsigned block_byte_pos,
		const std::string& what_arg)
	: std::runtime_error { what_arg       }
	, byte_pos_          { byte_pos       }
	, block_             { block          }
	, block_byte_pos_    { block_byte_pos }
{
	// empty
}


StreamParseException::StreamParseException(const unsigned byte_pos,
		const unsigned block, const unsigned block_byte_pos)
	: std::runtime_error {
		details::default_positional_message(byte_pos, block, block_byte_pos) }
	, byte_pos_       { byte_pos       }
	, block_          { block          }
	, block_byte_pos_ { block_byte_pos }
{
	// empty
}


unsigned StreamParseException::byte_position() const noexcept
{
	return byte_pos_;
}


unsigned StreamParseException::block() const noexcept
{
	return block_;
}


unsigned StreamParseException::block_byte_position() const noexcept
{
	return block_byte_pos_;
}


// is_valid_arcs()


bool is_valid_arcs(const uint32_t value)
{
	return value > details::UNPARSED_ARCS;
}


// is_valid_confidence()


bool is_valid_confidence(const unsigned value)
{
	return value > details::UNPARSED_CONFIDENCE;
}


// parse_stream()


uint32_t parse_stream(std::istream& in, ParseHandler* p,
		ParseErrorHandler* e)
{
	return details::parse_dbar_stream(in, p, e);
}


uint32_t parse_file(const std::string& filename, ParseHandler* p,
		ParseErrorHandler* e)
{
	return details::parse_dbar_file(filename, p, e);
}


// load_file()


DBAR load_file(const std::string& filename)
{
	DBARBuilder builder;
	details::parse_dbar_file(filename, &builder, nullptr);
	return builder.result();
}

} // namespace v_1_0_0
} // namespace arcstk

