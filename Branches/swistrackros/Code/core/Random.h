// Copyright (c) 2006, Thomas Lochmatter, thl@lothosoft.ch

#ifndef classRandom
#define classRandom

class Random;

//#include <stdlib.h>
#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include "RandomMersenneTwister.h"
#include "RandomNormal.h"
#include "RandomExponential.h"
#include "RandomPoisson.h"

//! A collection of random number functions.
class Random {

protected:
	//! The RandomMersenneTwister object.
	static RandomMersenneTwister* smRandomMersenneTwister;
	//! The RandomNormal object.
	static RandomNormal* smRandomNormal;
	//! The RandomExponential object.
	static RandomExponential* smRandomExponential;
	//! The RandomPoisson object.
	static RandomPoisson* smRandomPoisson;

	//! Reinitializes the distribution objects.
	static void ReinitializeDistributions();

public:
	//! Constructor.
	Random() {
		assert(smRandomMersenneTwister);
	}
	//! Destructor.
	~Random() {}

	//! Initializes the random generator.
	static void Initialize();
	//! Uninitializes the random generator and frees all static objects.
	static void Uninitialize();

	//! Saves the current state of the RNG.
	void SaveState(std::ofstream &in);
	//! Loads a previously saved RNG state.
	void LoadState(std::ifstream &in);

	//! Returns 0 or 1 with equal probability.
	int Binary() {
		return smRandomMersenneTwister->randInt() & 1;
		//return random() & 1;
	}

	//! Returns 1 with probability p and 0 with probability 1-p.
	int Binary(double p) {
		return (Uniform() < p) ? 1 : 0;
	}

	//! Returns an integer in the range [from, to) with uniform distribution.
	int Uniform(int from, int to) {
		if (from >= to) {
			return from;
		}
		return smRandomMersenneTwister->randInt(to - from - 1) + from;
		//return (int)((double)random() / ((double)RAND_MAX+1) * (double)(to-from)) + from;
	}

	//! Returns a double in the range [0, 1) with uniform distribution.
	double Uniform() {
		return smRandomMersenneTwister->randExc();
		//return (double)random() / ((double)RAND_MAX+1);
	}

	//! Returns a double in the range [from, to) with uniform distribution.
	double Uniform(double from, double to) {
		return smRandomMersenneTwister->randExc()*(to - from) + from;
		//return ((double)random() / ((double)RAND_MAX+1) * (double)(to-from)) + from;
	}

	//! Returns a float with a gaussian distribution.
	double Normal() {
		return smRandomNormal->Normal();
	}

	//! Returns a float with a gaussian distribution with a given standard deviation and mean.
	double Normal(float mean, float stddev) {
		return smRandomNormal->Normal()*stddev + mean;
	}

	//! Returns a float with an exponential distribution.
	double Exponential() {
		return smRandomExponential->Exponential();
	}

	//! Returns an integer with a Poisson distribution.
	long int Poisson(double mean) {
		return smRandomPoisson->Poisson(mean);
	}
};

#endif
