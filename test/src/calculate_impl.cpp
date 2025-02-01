#include "catch2/catch_test_macros.hpp"

/**
 * \file Fixtures for implementation details in module calculate
 */

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"                // for Points
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_impl.hpp"
#endif


// CalculationStateImpl


TEST_CASE ( "CalculationStateImpl", "[calculate] [calculationstateimpl]" )
{
	using arcstk::Algorithm;
	using arcstk::AccurateRipV1V2;
	using arcstk::details::CalculationStateImpl;

	using std::begin;
	using std::end;

	auto algorithm { std::make_unique<AccurateRipV1V2>() };
	auto impl1 { CalculationStateImpl { algorithm.get() } };

	{
		auto dummy_data = std::vector<uint32_t>(1000000);
		std::iota(begin(dummy_data), end(dummy_data), 1);

		const auto start_time { std::chrono::steady_clock::now() };
		impl1.update(begin(dummy_data), end(dummy_data));
		impl1.track_finished();
		const auto time_elapsed {
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - start_time)
		};

		impl1.increment_proc_time_elapsed(time_elapsed);
	}


	SECTION ("Construction is correct")
	{
		auto impl { CalculationStateImpl { algorithm.get() } };

		CHECK ( impl.algorithm()         == algorithm.get() );
		CHECK ( impl.samples_processed() == 0 );
	}


	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<CalculationStateImpl>::value );

		CHECK ( not
			std::is_nothrow_copy_constructible<CalculationStateImpl>::value );
	}


	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<CalculationStateImpl>::value );

		CHECK ( std::is_nothrow_move_constructible<CalculationStateImpl>::value );
	}


	SECTION ("Copy construction is correct")
	{
		auto impl2 { impl1 };

		CHECK ( impl2.algorithm()         == algorithm.get() );
		CHECK ( impl2.samples_processed() == 1000000 );
		CHECK ( impl2.proc_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("Move construction is correct")
	{
		auto impl3 { std::move(impl1) };

		CHECK ( impl3.algorithm()         == algorithm.get() );
		CHECK ( impl3.samples_processed() == 1000000 );
		CHECK ( impl3.proc_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("update() counts the amount of samples processed")
	{
		CHECK ( impl1.samples_processed() == 1000000 );
	}


	SECTION ("increment_proc_time_elapsed() updates time counter")
	{
		CHECK ( impl1.proc_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("current_subtotal() returns the subtotals")
	{
		auto checksums { impl1.current_subtotal() };

		CHECK ( checksums.size() == 2 );
	}
}


TEST_CASE ( "perform_update", "[calculate]" )
{
	SECTION ()
	{

	}
}

