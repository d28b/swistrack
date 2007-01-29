// Copyright (c) 2006, Thomas Lochmatter, thl@lothosoft.ch

#include "Random.h"
#define THISCLASS Random

RandomMersenneTwister *Random::smRandomMersenneTwister=0;
RandomNormal *Random::smRandomNormal=0;
RandomExponential *Random::smRandomExponential=0;
RandomPoisson *Random::smRandomPoisson=0;

THISCLASS::Random() {
	if (! smRandomMersenneTwister) {
		smRandomMersenneTwister=new RandomMersenneTwister();
		std::cout << "MersenneTwister Random Number Generator initialized." << std::endl; 
		Initialize();
	}
}

void THISCLASS::Initialize() {
	delete smRandomNormal;
	delete smRandomExponential;
	delete smRandomPoisson;
	smRandomNormal=new RandomNormal(smRandomMersenneTwister);
	smRandomExponential=new RandomExponential(smRandomMersenneTwister);
	smRandomPoisson=new RandomPoisson(smRandomMersenneTwister);
}

void THISCLASS::SaveState(std::ofstream &out) {
	out << *smRandomMersenneTwister << std::endl;
	Initialize(); // Note that we need to do this in order to be at the same state after LoadState. (Disadvantage: SaveState modifies the current state.)
}

void THISCLASS::LoadState(std::ifstream &in) {
	in >> *smRandomMersenneTwister;
	Initialize();
}
