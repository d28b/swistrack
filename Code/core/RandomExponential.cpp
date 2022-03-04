// Copyright (c) 2006, Thomas Lochmatter, thl@lothosoft.ch

#include "RandomExponential.h"
#define THISCLASS RandomExponential

THISCLASS::RandomExponential(RandomMersenneTwister *rmt): mRandomMersenneTwister(rmt) {
	const double m2 = 4294967296.;
	double de = 7.697117470131487, te = de, ve = 3.949659822581572e-3;
	int i, jsr = 0;

	while (jsr == 0) {
		jsr = mRandomMersenneTwister->randInt();
	}

	double q = ve / exp(-de);
	ke[0] = (unsigned long)((de / q) * m2);
	ke[1] = 0;
	we[0] = q / m2;
	we[255] = de / m2;
	fe[0] = 1.;
	fe[255] = exp(-de);
	for (i = 254; i >= 1; i--) {
		de = -log(ve / de + exp(-de));
		ke[i + 1] = (unsigned long)((de / te) * m2);
		te = de;
		fe[i] = exp(-de);
		we[i] = de / m2;
	}
}

#define UNI (.5 + (signed) SHR3 * .2328306e-9)

double THISCLASS::efix(unsigned long jz, unsigned long iz) { /*provides REXP if #define cannot */
	double x;

	for (;;) {
		if (iz == 0) {
			return (7.69711 - log(mRandomMersenneTwister->randExc()));
		}

		x = jz * we[iz];
		if (fe[iz] + mRandomMersenneTwister->randExc() * (fe[iz - 1] - fe[iz]) < exp(-x)) {
			return (x);
		}

		jz = mRandomMersenneTwister->randInt();
		iz = (jz & 255);
		if (jz < ke[iz]) {
			return (jz * we[iz]);
		}
	}
}
