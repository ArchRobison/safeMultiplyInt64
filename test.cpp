#include "safeMultiplyInt64.h"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <iostream>
#include <type_traits>

// Convert x from uint64_t to T in standard-conforming way,
// where T is int64_t or uint64_t.
// Assumes target machine uses two's complement.
template <typename T>
inline T toInt64(uint64_t x)
{
	T y;
	std::memcpy(&y, &x, sizeof(y));
	return y;
}

// Return x+y with wrap-around
template <typename T>
inline T wrappingAdd(T x, int64_t y)
{
	return toInt64<T>(static_cast<uint64_t>(x) + static_cast<uint64_t>(y));
}

// Reference implementation of safeMultiplyInt64.
template<typename T>
bool refSafeMultiplyInt64(T x, T y, T* result)
{
	*result = toInt64<T>(static_cast<uint64_t>(x) * static_cast<uint64_t>(y));
	if (x == 0)
		return false;
	if (std::is_signed<T>::value && x == -1)
		return y == std::numeric_limits<T>::lowest();
	return *result / x != y;
}

int errorCount = 0;

//! Check that safeMultiplyInt64 and refSafeMultiplyInt64 agree for x*y.
//! Check that safeMultiplyUInt64 and refSafeMultiplyUInt64 agree for x*y.
template <typename T>
void check(T x, T y)
{
	T actualResult, refResult;
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
template <typename T>
void checkNear(T x, T y)
{
	for (int64_t i = -8; i <= 8; ++i)
		for (int64_t j = -8; j <= 8; ++j)
			check(wrappingAdd(x, i), wrappingAdd(y, j));
}

template <typename T>
void test(char const* type)
{
	std::cout << "Testing " << type;
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<T> dist(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max());

	// Test values at extremes and near 0.
	for (int64_t x : {static_cast<T>(0), std::numeric_limits<T>::max()})
		for (int64_t y : {static_cast<T>(0), std::numeric_limits<T>::max()})
			checkNear(x, y);

	// Test products that are near the representable extremes.
	for (int32_t trial = 0; trial < 100000; ++trial)
	{
		// Choose first factor. The loop covers a wide dynamic range.
		for (T x = dist(gen); x != 0; x /= 2)
		{
			// Choose second factor so that product is close to upper bound of int64_t.
			checkNear(x, std::numeric_limits<T>::max() / x);
			if (std::numeric_limits<T>::min() != 0 && x != -1)
				// Choose second factor so that product is close to lower bound of int64_t.
				checkNear(x, std::numeric_limits<T>::min() / x);
		}
		if (trial % 1000 == 999)
			std::cout << "." << std::flush;
	}
	std::cout << std::endl;
}

int main()
{
	test<uint64_t>("uint64_t");
	test<int64_t>("int64_t");
	if (errorCount == 0)
		std::cout << "No errors found" << std::endl;
	return errorCount != 0;
}
