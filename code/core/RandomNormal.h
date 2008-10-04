// Copyright (c) 2006, Thomas Lochmatter, thl@lothosoft.ch

#ifndef classRandomNormal
#define classRandomNormal

class RandomNormal;

#include <cmath>
#include "RandomMersenneTwister.h"

//! Normal random number generator based on the Ziggurat Method.
class RandomNormal {

protected:
	RandomMersenneTwister *mRandomMersenneTwister;
	unsigned long kn[128];
	double wn[128], fn[128];

	double nfix(long hz, unsigned long iz);

public:
	//! Constructor.
	RandomNormal(RandomMersenneTwister *rmt);

	//! Returns a double with a gaussian distribution.
	double Normal() {
		long hz = mRandomMersenneTwister->randInt();
		unsigned long iz = hz & 127;
		return ((unsigned long)abs(hz) < kn[iz]) ? hz*wn[iz] : nfix(hz, iz);
	}
};

#endif
