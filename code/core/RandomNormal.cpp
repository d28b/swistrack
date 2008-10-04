// Copyright (c) 2006, Thomas Lochmatter, thl@lothosoft.ch

#include "RandomNormal.h"
#define THISCLASS RandomNormal

THISCLASS::RandomNormal(RandomMersenneTwister *rmt): mRandomMersenneTwister(rmt) {
	const double m1 = 2147483648.0, m2 = 4294967296.;
	double dn = 3.442619855899, tn = dn, vn = 9.91256303526217e-3, q;
	double de = 7.697117470131487, te = de, ve = 3.949659822581572e-3;
	int i, jsr = 0;

	while (jsr == 0) {
		jsr = mRandomMersenneTwister->randInt();
	}

	/* Tables for RNOR: */
	q = vn / exp(-.5 * dn * dn);
	kn[0] = (unsigned long)((dn / q) * m1);
	kn[1] = 0;
	wn[0] = q / m1;
	wn[127] = dn / m1;
	fn[0] = 1.;
	fn[127] = exp(-.5 * dn * dn);
	for (i = 126; i >= 1; i--) {
		dn = sqrt(-2.*log(vn / dn + exp(-.5 * dn * dn)));
		kn[i+1] = (unsigned long)((dn / tn) * m1);
		tn = dn;
		fn[i] = exp(-.5 * dn * dn);
		wn[i] = dn / m1;
	}
}

double THISCLASS::nfix(long hz, unsigned long iz) {
	const double r = 3.442620f;
	double x, y;

	for (;;) {
		x = hz * wn[iz];
		if (iz == 0) {
			do {
				x = -log(mRandomMersenneTwister->randExc()) * 0.2904764;
				y = -log(mRandomMersenneTwister->randExc());
			} while (y + y < x*x);
			return (hz > 0) ? r + x : -r - x;
		}

		if (fn[iz] + mRandomMersenneTwister->randExc()*(fn[iz-1] - fn[iz]) < exp(-.5*x*x)) {
			return x;
		}

		hz = mRandomMersenneTwister->randInt();
		iz = hz & 127;
		if ((unsigned long)abs(hz) < kn[iz]) {
			return (hz*wn[iz]);
		}
	}
}
