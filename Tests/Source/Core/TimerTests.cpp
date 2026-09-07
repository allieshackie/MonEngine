#include <catch2/catch_test_macros.hpp>

#include "Core/Timer.h"

TEST_CASE("Timer has sensible fixed-step defaults", "[core][timer]")
{
    const Timer timer;

    CHECK(timer.mDT == 1.0f / 60.0f);
    CHECK(timer.mAccumulator == 0.0);
    CHECK(timer.mAccumulatorMax == 0.1);
}
