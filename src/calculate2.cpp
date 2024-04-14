/**
 * \file
 *
 * \brief Implementation of the new checksum calculation API
 */

#ifndef __LIBARCSTK_CALCULATE2_DETAILS_HPP__
#include "calculate2_details.hpp"
#endif

#include <string>      // for vector
#include <vector>      // for string

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{


void Subtotals::reset()
{
	Subtotals s;  // use default construction for reset to initial values
	swap(*this, s);
}


void swap(Subtotals& lhs, Subtotals& rhs) noexcept
{
	using std::swap;
	swap(lhs.multiplier,  rhs.multiplier);
	swap(lhs.update,      rhs.update);
	swap(lhs.subtotal_v1, rhs.subtotal_v1);
	swap(lhs.subtotal_v2, rhs.subtotal_v2);
}


// CalcCounters


sample_count_t CalcCounters::sample_offset() const
{
	return smpl_offset_.value();
}

void CalcCounters::increment_sample_offset(const sample_count_t amount)
{
	smpl_offset_.increment(amount);
}

std::chrono::milliseconds CalcCounters::proc_time_elapsed() const
{
	return proc_time_elapsed_.value();
}

void CalcCounters::increment_proc_time_elapsed(
		const std::chrono::milliseconds amount)
{
	proc_time_elapsed_.increment(amount);
}

} // namespace details
} // namespace v_1_0_0
} // namespace arcstk

