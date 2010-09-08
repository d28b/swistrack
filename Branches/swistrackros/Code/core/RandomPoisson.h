// Copyright (c) 2006, Thomas Lochmatter, thl@lothosoft.ch

#ifndef classRandomPoisson
#define classRandomPoisson

class RandomPoisson;

#include <cmath>
#include "RandomMersenneTwister.h"

//! Poisson random number generator. You should use this only through the Random class.
class RandomPoisson {

protected:
	RandomMersenneTwister *mRandomMersenneTwister;

public:
	//! Constructor.
	RandomPoisson(RandomMersenneTwister *rmt): mRandomMersenneTwister(rmt) {}

	//! Returns a integer with a poisson distribution of density L.
	long int Poisson(double L);
	long int PoissonInver(double L);
	long int PoissonRatioUniforms(double L);
	long int PoissonLow(double L);
	long int PoissonModeSearch(double L);
	long int PoissonPatchwork(double L);
	static double PoissonF(long int k, double l_nu, double c_pm);
	static double LnFac(long int n);
};

#endif
