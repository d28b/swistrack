// Copyright (c) 2006, Thomas Lochmatter, thl@lothosoft.ch

#include "Random.h"
#define THISCLASS Random

RandomMersenneTwister *Random::smRandomMersenneTwister = 0;
RandomNormal *Random::smRandomNormal = 0;
RandomExponential *Random::smRandomExponential = 0;
RandomPoisson *Random::smRandomPoisson = 0;

void THISCLASS::Initialize() {
	if (smRandomMersenneTwister) {
		return;
	}
	smRandomMersenneTwister = new RandomMersenneTwister();
	ReinitializeDistributions();
}

void THISCLASS::ReinitializeDistributions() {
	delete smRandomNormal;
	delete smRandomExponential;
	delete smRandomPoisson;
	smRandomNormal = new RandomNormal(smRandomMersenneTwister);
	smRandomExponential = new RandomExponential(smRandomMersenneTwister);
	smRandomPoisson = new RandomPoisson(smRandomMersenneTwister);
}

void THISCLASS::Uninitialize() {
	delete smRandomMersenneTwister;
	delete smRandomNormal;
	delete smRandomExponential;
	delete smRandomPoisson;
	smRandomMersenneTwister = 0;
	smRandomNormal = 0;
	smRandomExponential = 0;
	smRandomPoisson = 0;
}

void THISCLASS::SaveState(std::ofstream &out) {
	assert(smRandomMersenneTwister);
	out << *smRandomMersenneTwister << std::endl;
	ReinitializeDistributions(); // Note that we need to do this in order to be at the same state after LoadState. (Disadvantage: SaveState modifies the current state.)
}

void THISCLASS::LoadState(std::ifstream &in) {
	assert(smRandomMersenneTwister);
	in >> *smRandomMersenneTwister;
	ReinitializeDistributions();
}
