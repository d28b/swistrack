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
	float wn[128], fn[128];

	float nfix(long hz, unsigned long iz);

public:
	//! Constructor.
	RandomNormal(RandomMersenneTwister *rmt);

	//! Returns a double with a gaussian distribution.
	float Normal() {
		long hz=mRandomMersenneTwister->randInt();
		unsigned long iz=hz & 127;
		return (abs(hz)<kn[iz]) ? hz*wn[iz] : nfix(hz, iz);
	}
};

#endif
