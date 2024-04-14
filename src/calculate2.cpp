/**
 * \file
 *
 * \brief Implementation of the new checksum calculation API
 */

#ifndef __LIBARCSTK_CALCULATE2_HPP__
#include "calculate2.hpp"
#endif

#include <string>      // for vector
#include <vector>      // for string

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{


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

