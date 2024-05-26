#ifndef SAFE_MULTIPLY_INT64_H
#define SAFE_MULTIPLY_INT64_H

#include <cstdint>
#include <cstring>
#include <cmath>

// Performs 64-bit multiplication with overflow checking, without any division.
// Sets result to value congruent to x*y modulo 2^64.
// Returns false if no overflow ocurred, i.e. z is x*y evaluated in infinite precision.
// Returns true if overflow occurred.
// Assumes float is IEEE 32-bit floating-point.
// Works for any IEEE rounding mode.
inline bool safeMultiplyInt64(int64_t x, int64_t y, int64_t* result)
{
	// Compute the result using unsigned arithmetic and bit-casting to int64_t.
	uint64_t u = static_cast<uint64_t>(x) * static_cast<uint64_t>(y);
	std::memcpy(result, &u, sizeof(result));

	// Compute result using floating-point.
	// In the worst case, when rounding is not to-nearest, the conversions to
	// float introduce a worst-case relative error of about pow(0.5,23).
	// The exact multiplication doubles those errors, and the final round-off can
	// introduce more error. Thus worst-case relative error for v is about 3*pow(0.5,23). 
	float p = static_cast<float>(x) * static_cast<float>(y);

	// Compare p with *result.
	// If overflow happens, *result differs from infinite precision x*y by at least pow(2.0,63).
	// The value p approximates the infinite precision x*y with absolute error of about 3*pow(2.0,63-23).  
	// So a threshold somewhere between can distinguish the overflow case.
	constexpr float threshold = static_cast<int64_t>(1) << 48;
	return std::fabs(*result - p) > threshold;
}

#endif /* SAFE_MULTIPLY_INT64_H */