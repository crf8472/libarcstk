/**
 * \file
 *
 * \brief Implementation of the class Algorithm and its helpers.
 */

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"
#endif

#include <string>        // for string

namespace arcstk
{
inline namespace v_1_0_0
{

// Context


void swap(Context& lhs, Context& rhs) noexcept
{
	Context tmp { lhs };
	lhs = rhs;
	rhs = tmp;
}


std::string name(const Context& c) noexcept
{
	switch (c)
	{
		case Context::ALBUM:       return "ALBUM";
		case Context::LAST_TRACK:  return "LAST_TRACK";
		case Context::FIRST_TRACK: return "FIRST_TRACK";
		case Context::TRACK:       return "TRACK";
		default: ;
	}

	return {};
}


std::string to_string(const Context& c) noexcept
{
	return name(c);
}


bool any(const Context& rhs) noexcept
{
	return static_cast<unsigned>(rhs) != 0;
}


// Algorithm


Algorithm::Algorithm(const Context c)
	: context_ { c }
{
	// empty
}


void Algorithm::set_context(const Context c) noexcept
{
	context_ = c;

	do_setup(c);
}


Context Algorithm::context() const noexcept
{
	return context_;
}


std::pair<int32_t, int32_t> Algorithm::range(const AudioSize& size,
		const Points& points) const
{
	return this->do_range(size, points);
}


void Algorithm::track_finished(const int t, const AudioSize& length)
{
	this->do_track_finished(t, length);
}


ChecksumSet Algorithm::result() const
{
	return this->do_result();
}


ChecksumtypeSet Algorithm::types() const
{
	return this->do_types();
}


std::unique_ptr<Algorithm> Algorithm::clone() const
{
	return this->do_clone();
}


void Algorithm::swap_base(Algorithm& rhs)
{
	using std::swap;
	swap(context_, rhs.context_);
}

} // namespace v_1_0_0
} // namespace arcstk

