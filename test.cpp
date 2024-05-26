#include "safeMultiplyInt64.h"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <iostream>


// Convert x from uint64_t to int64_t in standard-conforming way.
// Assumes target machine uses two's complement.
inline int64_t toInt64(uint64_t x)
{
	int64_t y;
	std::memcpy(&y, &x, sizeof(y));
	return y;
}

// Return x+y with wrap-around.
inline int64_t wrappingAdd(int64_t x, int64_t y)
{
	return toInt64(static_cast<uint64_t>(x) + static_cast<uint64_t>(y));
}

// Reference implementation of safeMultiplyInt64
bool refSafeMultiplyInt64(int64_t x, int64_t y, int64_t* result)
{
	*result = toInt64(static_cast<uint64_t>(x) * static_cast<uint64_t>(y));
	if (x == 0)
		return false;
	if (x == -1)
		return y == std::numeric_limits<int64_t>::min();
	return *result / x != y;
}

int errorCount = 0;

//! Check that safeMultiplyInt64 and refSafeMultiply64 agree for x*y.
void check(int64_t x, int64_t y)
{
	int64_t actualResult, refResult;
	bool actualOverflow = safeMultiplyInt64(x, y, &actualResult);
	bool refOverflow = refSafeMultiplyInt64(x, y, &refResult);
	if (actualOverflow != refOverflow || actualResult != refResult)
	{
		std::cout << "product:" << x << "*" << y << " actualOverflow:" << actualOverflow << " refOverflow:" << refOverflow << std::endl;
		if (++errorCount >= 10)
		{
			std::cout << "too many errors to continue" << std::endl;
			std::exit(1);
		}
	}
}

// Check 
void checkNear(int64_t x, int64_t y)
{
	for (int64_t i = -8; i <= 8; ++i)
		for (int64_t j = -8; j <= 8; ++j)
			check(wrappingAdd(x, i), wrappingAdd(y, j));
}

void test()
{
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<int64_t> dist(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());

	// Test values at extremes and near 0.
	for (int64_t x : {static_cast<int64_t>(0), std::numeric_limits<int64_t>::max()})
		for (int64_t y : {static_cast<int64_t>(0), std::numeric_limits<int64_t>::max()})
			checkNear(x, y);

	// Test products that are near the representable extremes.
	for (int32_t trial = 0; trial < 100000; ++trial)
	{
		// Choose first factor. The loop covers a wide dynamic range.
		for (int64_t x = dist(gen); x != 0; x /= 2)
		{
			// Choose second factor so that product is close to upper bound of int64_t.
			checkNear(x, std::numeric_limits<int64_t>::max() / x);
			if (x != -1)
				// Choose second factor so that product is close to lower bound of int64_t.
				checkNear(x, std::numeric_limits<int64_t>::min() / x);
		}
		if (trial % 1000 == 999)
			std::cout << "." << std::flush;
	}
}

int main()
{
	test();
	if (errorCount == 0)
		std::cout << "No errors found" << std::endl;
	return errorCount != 0;
}
