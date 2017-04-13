#pragma once
#include <climits>

// USAGE
// rng.GetNumber() / (ULONG_MAX + 1.0f) = 0.0...1.0

class RNGesus
{
public:
	RNGesus(const unsigned long& xseed, const unsigned long& yseed, const unsigned long& zseed);

	unsigned long GetNumber();
	float getZeroToOne();

private:
	unsigned long x, y, z;
	unsigned long t = 0;
};