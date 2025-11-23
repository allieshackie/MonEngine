#pragma once
#include <chrono>

using Clock = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<double>;
using TimePoint = std::chrono::time_point<Clock>;

struct Timer
{
	const float mDT = 0.01f;
	const double mAccumulatorMax = 0.1;

	TimePoint mCurrentTime;
	double mAccumulator = 0.0;
};
