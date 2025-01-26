/**
 * \file
 *
 * \brief Implementation of metadata.hpp.
 */

#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"
#endif
#ifndef __LIBARCSTK_METADATA_DETAILS_HPP__
#include "metadata_details.hpp"
#endif

#include <algorithm>     // for transform
#include <sstream>       // for stringstream
#include <stdexcept>     // for invalid_argument
#include <string>        // for vector
#include <vector>        // for string
#include <unordered_set> // for unordered_set

namespace arcstk
{
inline namespace v_1_0_0
{


// metadata_details.hpp


namespace details
{

int32_t frames2samples(const int32_t frames)
{
	return frames * CDDA::SAMPLES_PER_FRAME;
}


int32_t samples2frames(const int32_t samples)
{
	return samples / CDDA::SAMPLES_PER_FRAME;
}


int32_t frames2bytes(const int32_t frames)
{
	return frames * CDDA::BYTES_PER_FRAME;
}


int32_t bytes2frames(const int32_t bytes)
{
	return bytes / CDDA::BYTES_PER_FRAME;
}


int32_t samples2bytes(const int32_t samples)
{
	return samples * CDDA::BYTES_PER_SAMPLE;
}


int32_t bytes2samples(const int32_t bytes)
{
	return bytes / CDDA::BYTES_PER_SAMPLE;
}


// TODO Implement this generically like toc_get
int32_t to_bytes(const int32_t value, const AudioSize::UNIT unit) noexcept
{
	using UNIT = AudioSize::UNIT;

	switch (unit) // TODO template?
	{
		case UNIT::FRAMES:  return frames2bytes(value);
		case UNIT::SAMPLES: return samples2bytes(value);
		default:            return value;
	}

	return value;
}


// TODO Implement this generically like toc_get
int32_t from_bytes(const int32_t value, const AudioSize::UNIT unit) noexcept
{
	using UNIT = AudioSize::UNIT;

	switch (unit) // TODO template?
	{
		case UNIT::FRAMES:  return bytes2frames(value);
		case UNIT::SAMPLES: return bytes2samples(value);
		default:            return value;
	}

	return value;
}


namespace validate
{

/**
 * \brief Worker to throw when ToCData validation fails.
 *
 * \param[in] msg Error message
 *
 * \throws std::invalid_argument
 */
void throw_on_invalid_tocdata(const std::string& msg);


void throw_on_invalid_tocdata(const std::string& msg)
{
	throw std::invalid_argument(msg);
}


/**
 * \brief Worker to validate frame value for being in legal range.
 *
 * \param[in] frames LBA frame amount to validate
 *
 * \throws std::invalid_argument
 */
void in_legal_frame_range(const int32_t frames);

void in_legal_frame_range(const int32_t frames)
{
	using std::to_string;

	if (frames < CDDA::MIN_TRACK_OFFSET_DIST)
	{
		auto ss = std::stringstream {};
		ss << "Value " << frames << " is smaller than minimum track length";
		throw_on_invalid_tocdata(ss.str());
	}

	if (frames > CDDA::MAX_BLOCK_ADDRESS)
	{
		auto ss = std::stringstream {};
		ss << "Value " << frames << " exceeds physical maximum";
		throw_on_invalid_tocdata(ss.str());
	}

	if (frames > MAX_OFFSET_99)
	{
		auto ss = std::stringstream {};
		ss << "Value exceeds physical range of 99 min ("
				<< to_string(MAX_OFFSET_99) << " frames)";
		throw_on_invalid_tocdata(ss.str());
	}

	if (frames > MAX_OFFSET_90)
	{
		auto ss = std::stringstream {};
		ss << "Value exceeds "
			<< std::to_string(MAX_OFFSET_90) << " frames (90 min)";
		throw_on_invalid_tocdata(ss.str());
	}

	if (frames > CDDA::MAX_OFFSET)
	{
		auto ss = std::stringstream {};
		ss << "Value " << frames << " exceeds redbook maximum";
		throw_on_invalid_tocdata(ss.str());
	}
}


void legal_leadout_size(const ToCData& toc_data)
{
	const auto leadout { toc::leadout(toc_data).total_frames() };
	in_legal_frame_range(leadout);
}


void legal_offset_sizes(const ToCData& /*toc_data*/)
{
	// TODO Implement
}


void legal_total_tracks(const ToCData& /*toc_data*/)
{
	// TODO Implement
}


void legal_ordering(const ToCData& /*toc_data*/)
{
	// TODO Implement
}


void legal_minimum_distances(const ToCData& toc_data)
{
	// Has each offset legal minimal distance to its predecessor?

	auto curr_len = int32_t { 0 };
	auto prev_len = curr_len;

	auto track = ToCData::size_type { 1 }; // track number
	auto c     = ToCData::size_type { 0 }; // count comparisons
	while (c < toc_data.size())
	{
		prev_len = toc_data[track].total_frames();

		++track;
		track %= toc_data.size(); // after last track, flip back to 0

		curr_len = toc_data[track].total_frames();

		if (curr_len - prev_len < CDDA::MIN_TRACK_LEN_FRAMES)
		{
			using std::to_string;
			auto ss = std::stringstream {};
			ss << "Illegal length: Track "
				<< to_string(track > 0 ? track - 1 : toc_data.size() - 1)
				<< "is too short (" << (curr_len - prev_len) << " frames)";

			throw_on_invalid_tocdata(ss.str());
		}

		++c;
	}
}

} // namespace validate

} // namespace details


// metadata.hpp


// AudioSize


AudioSize::AudioSize() noexcept
	: AudioSize { 0, AudioSize::UNIT::BYTES }
{
	// empty
}


AudioSize::AudioSize(const int32_t value, const AudioSize::UNIT unit) noexcept
	: total_pcm_bytes_ { details::to_bytes(value, unit) }
{
	// empty
}


int32_t AudioSize::leadout_frame() const
{
	return total_frames();
}


int32_t AudioSize::total_frames() const
{
	return read_as(AudioSize::UNIT::FRAMES);
}


void AudioSize::set_total_frames(const int32_t frame_count)
{
	set_value(frame_count, AudioSize::UNIT::FRAMES);
}


int32_t AudioSize::total_samples() const
{
	return read_as(AudioSize::UNIT::SAMPLES);
}


void AudioSize::set_total_samples(const int32_t sample_count)
{
	set_value(sample_count, AudioSize::UNIT::SAMPLES);
}


int32_t AudioSize::total_pcm_bytes() const noexcept
{
	return read_as(AudioSize::UNIT::BYTES);
}


void AudioSize::set_total_pcm_bytes(const int32_t byte_count) noexcept
{
	set_value(byte_count, AudioSize::UNIT::BYTES);
}


bool AudioSize::zero() const noexcept
{
	return 0 == total_pcm_bytes();
}


int32_t AudioSize::max(const UNIT unit) noexcept
{
	static constexpr int32_t error_value { 0 };

	switch (unit)
	{
		case UNIT::FRAMES:
			return CDDA::MAX_BLOCK_ADDRESS;

		case UNIT::SAMPLES:
			return CDDA::MAX_BLOCK_ADDRESS * CDDA::SAMPLES_PER_FRAME;

		case UNIT::BYTES:
			return CDDA::MAX_BLOCK_ADDRESS * CDDA::BYTES_PER_FRAME;

		default:
			return error_value;
	}

	return error_value;
}


void AudioSize::set_value(const int32_t value, AudioSize::UNIT unit)
{
	using std::to_string;

	if (value > AudioSize::max(unit))
	{
		auto ss = std::stringstream {};
		ss << "Value too big for maximum: " << to_string(value);
		throw std::overflow_error(ss.str());
	}

	if (value < 0)
	{
		auto ss = std::stringstream {};
		ss << "Cannot set AudioSize to negative value: "
			<< to_string(value);
		throw std::underflow_error(ss.str());
	}

	total_pcm_bytes_ = details::to_bytes(value, unit);
}


int32_t AudioSize::read_as(const AudioSize::UNIT unit) const
{
	return details::from_bytes(total_pcm_bytes_, unit);
}


AudioSize::operator bool() const noexcept
{
	return !zero();
}


void swap(AudioSize& lhs, AudioSize& rhs) noexcept
{
	using std::swap;
	swap(lhs.total_pcm_bytes_, rhs.total_pcm_bytes_);
}


bool operator == (const AudioSize& lhs, const AudioSize& rhs) noexcept
{
	return lhs.total_pcm_bytes() == rhs.total_pcm_bytes();
}


bool operator < (const AudioSize& lhs, const AudioSize& rhs) noexcept
{
	return lhs.total_pcm_bytes() < rhs.total_pcm_bytes();
}


// ToCData


namespace toc
{

ToCData construct(const int32_t leadout, const std::vector<int32_t>& offsets)
{
	using std::cbegin;
	using std::cend;
	using std::begin;
	using std::end;

	ToCData toc ( 1 + offsets.size() );

	auto ptr { begin(toc) };

	*ptr = AudioSize { leadout, AudioSize::UNIT::FRAMES };
	++ptr;

	std::transform(cbegin(offsets), cend(offsets), ptr,
			[](const int32_t o) -> AudioSize
			{
				return AudioSize { o, AudioSize::UNIT::FRAMES };
			});

	toc.shrink_to_fit();

	return toc;
}


void set_leadout(const AudioSize& leadout, ToCData& data)
{
	if (data.size() < 1)
	{
		data.resize(1);
	}

	data[0] = leadout;
}


AudioSize leadout(const ToCData& data)
{
	return data.empty() ? AudioSize { 0, AudioSize::UNIT::FRAMES }
		: data.front();
}


std::vector<AudioSize> offsets(const ToCData& data)
{
	if (data.size() < 2)
	{
		return {};
	}

	using std::cbegin;
	using std::cend;

	return { cbegin(data) + 1, cend(data) };
}


int total_tracks(const ToCData& data)
{
	return data.size() - 1;
}


bool complete(const ToCData& data)
{
	return !data.empty()  &&  !data[0].zero()  &&  total_tracks(data) > 0;
}


void validate(const ToCData& toc_data)
{
	details::validate::legal_leadout_size(toc_data);
	details::validate::legal_offset_sizes(toc_data);
	details::validate::legal_total_tracks(toc_data);

	details::validate::legal_ordering(toc_data);
	details::validate::legal_minimum_distances(toc_data);
}

} // namespace toc


// ToC


class ToC::Impl final
{
public:

	Impl(const ToCData& toc_data, const std::vector<std::string>& filenames);

	Impl(const Impl& rhs);
	Impl& operator = (const Impl& rhs);

	Impl(Impl&& rhs) noexcept;
	Impl& operator = (Impl&& rhs) noexcept;

	~Impl() noexcept;

	int total_tracks() const noexcept;

	void set_leadout(const AudioSize leadout) noexcept;
	AudioSize leadout() const noexcept;

	std::vector<AudioSize>   offsets() const;
	std::vector<std::string> filenames() const;

	bool is_single_file() const noexcept;

	bool complete() const noexcept;

	friend bool operator == (const Impl& lhs, const Impl& rhs) noexcept
	{
		return lhs.toc_ == rhs.toc_ && lhs.filenames_ == rhs.filenames_;
	}

	friend void swap(Impl& lhs, Impl& rhs) noexcept
	{
		using std::swap;

		swap(lhs.toc_,       rhs.toc_);
		swap(lhs.filenames_, rhs.filenames_);
	}

private:

	ToCData toc_;
	std::vector<std::string> filenames_;
};


ToC::Impl::Impl(const ToCData& toc, const std::vector<std::string>& filenames)
	: toc_       { toc       }
	, filenames_ { filenames }
{
	// empty
}


ToC::Impl::Impl(const Impl& rhs)
	: toc_       { rhs.toc_       }
	, filenames_ { rhs.filenames_ }
{
	// empty
}


ToC::Impl& ToC::Impl::operator = (const Impl& rhs)
{
	using std::swap;

	Impl tmp { rhs };
	swap(*this, tmp);
	return *this;
}


ToC::Impl::Impl(Impl&& rhs) noexcept
	: toc_        { std::move(rhs.toc_)       }
	, filenames_  { std::move(rhs.filenames_) }
{
	// empty
}


ToC::Impl& ToC::Impl::operator = (Impl&& rhs) noexcept
{
	toc_       = std::move(rhs.toc_);
	filenames_ = std::move(rhs.filenames_);
	return *this;
}


ToC::Impl::~Impl() noexcept = default;


int ToC::Impl::total_tracks() const noexcept
{
	return toc::total_tracks(toc_);
}


void ToC::Impl::set_leadout(const AudioSize leadout) noexcept
{
	toc::set_leadout(leadout, toc_);
}


AudioSize ToC::Impl::leadout() const noexcept
{
	return toc::leadout(toc_);
}


std::vector<AudioSize> ToC::Impl::offsets() const
{
	return toc::offsets(toc_);
}


std::vector<std::string> ToC::Impl::filenames() const
{
	return filenames_;
}


bool ToC::Impl::is_single_file() const noexcept
{
	if (filenames_.size() == 1)
	{
		return true;
	}

	using std::cbegin;
	using std::cend;
	std::unordered_set<std::string> names(cbegin(filenames_), cend(filenames_));

	return names.size() == 1;
}


bool ToC::Impl::complete() const noexcept
{
	return toc::complete(toc_);
}


// ToC


ToC::ToC(const ToCData& toc_data, const std::vector<std::string>& filenames)
	: impl_ { std::make_unique<ToC::Impl>(toc_data, filenames) }
{
	// empty
}


ToC::ToC(const ToCData& toc_data)
	: impl_ { std::make_unique<ToC::Impl>(toc_data,
			std::vector<std::string>{/* empty */}) }
{
	// empty
}


ToC::ToC(const ToC& rhs)
	: impl_ { std::make_unique<ToC::Impl>(*rhs.impl_) }
{
	// empty
}


ToC& ToC::operator = (const ToC& rhs)
{
	impl_ = std::make_unique<ToC::Impl>(*rhs.impl_);
	return *this;
}


ToC::ToC(ToC&& rhs) noexcept
	: impl_ { std::move(rhs.impl_) }
{
	// empty
}


ToC& ToC::operator = (ToC&& rhs) noexcept
{
	impl_ = std::move(rhs.impl_);
	return *this;
}


ToC::~ToC() noexcept = default;


int ToC::total_tracks() const noexcept
{
	return impl_->total_tracks();
}


void ToC::set_leadout(const AudioSize leadout) noexcept
{
	impl_->set_leadout(leadout);
}


AudioSize ToC::leadout() const noexcept
{
	return impl_->leadout();
}


std::vector<AudioSize> ToC::offsets() const
{
	return impl_->offsets();
}


std::vector<std::string> ToC::filenames() const
{
	return impl_->filenames();
}


bool ToC::complete() const noexcept
{
	return impl_->complete();
}


bool operator == (const ToC& lhs, const ToC& rhs) noexcept
{
	return *lhs.impl_ == *rhs.impl_;
}


void swap(ToC& lhs, ToC& rhs) noexcept
{
	using std::swap;

	swap(lhs.impl_, rhs.impl_);
}


// make_toc


std::unique_ptr<ToC> make_toc(const int32_t leadout,
		const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	return std::make_unique<ToC>(toc::construct(leadout, offsets), filenames);
}


std::unique_ptr<ToC> make_toc(const int32_t leadout,
		const std::vector<int32_t>& offsets)
{
	return std::make_unique<ToC>(toc::construct(leadout, offsets));
}


std::unique_ptr<ToC> make_toc(const std::vector<int32_t>& offsets,
		const std::vector<std::string>& filenames)
{
	return make_toc(0, offsets, filenames);
}


std::unique_ptr<ToC> make_toc(const std::vector<int32_t>& offsets)
{
	return make_toc(0, offsets);
}


// InvalidMetadataException


InvalidMetadataException::InvalidMetadataException(const std::string &what_arg)
	: std::runtime_error { what_arg }
{
	// empty
}


InvalidMetadataException::InvalidMetadataException(const char *what_arg)
	: std::runtime_error { what_arg }
{
	// empty
}


// NonstandardMetadataException


NonstandardMetadataException::NonstandardMetadataException(
		const std::string &what_arg)
	: std::runtime_error { what_arg }
{
	// empty
}


NonstandardMetadataException::NonstandardMetadataException(const char *what_arg)
	: std::runtime_error { what_arg }
{
	// empty
}

} // namespace v_1_0_0
} // namespace arcstk

