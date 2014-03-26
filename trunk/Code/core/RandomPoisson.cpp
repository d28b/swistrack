// Copyright (c) 2006, Thomas Lochmatter, thl@lothosoft.ch

#include "RandomPoisson.h"
#define THISCLASS RandomPoisson

/***********************************************************************
* 2002 Agner Fog. GNU General Public License www.gnu.org/copyleft/gpl.html
* with some restructuration by Thomas Lochmatter
************************************************************************/

const double SHAT1 = 2.943035529371538573;    // 8/e
const double SHAT2 = 0.8989161620588987408;   // 3-sqrt(12/e)
const int FAK_LEN = 1024;              // length of factorial table used by LnFac

long int THISCLASS::Poisson(double L) {
	/*
	   This function generates a random variate with the poisson distribution.

	   Uses down/up search from the mode by chop-down technique for L < 20,
	   and patchwork rejection method for L >= 20.

	   For L < 1.E-6 numerical inaccuracy is avoided by direct calculation.
	*/
	//------------------------------------------------------------------
	//                 choose method
	//------------------------------------------------------------------
	if (L < 20) {
		if (L < 1.E-6) {
			if (L == 0) return 0;
			if (L < 0) return -1; //wxASSERT_MSG(0, "Lambda must be >=0.");

			//--------------------------------------------------------------
			// calculate probabilities
			//--------------------------------------------------------------
			// For extremely small L we calculate the probabilities of x = 1
			// and x = 2 (ignoring higher x). The reason for using this
			// method is to prevent numerical inaccuracies in other methods.
			//--------------------------------------------------------------
			return PoissonLow(L);
		} else {

			//--------------------------------------------------------------
			// down/up search from mode
			//--------------------------------------------------------------
			// The computation time for this method grows with sqrt(L).
			// Gives overflow for L > 60
			//--------------------------------------------------------------
			return PoissonModeSearch(L);
		}
	} else {
		if (L > 2.E9) return -1; //wxASSERT_MSG(0, wxT("Parameter too big in Poisson function"));

		//----------------------------------------------------------------
		// patchword rejection method
		//----------------------------------------------------------------
		// The computation time for this method does not depend on L.
		// Use where other methods would be slower.
		//----------------------------------------------------------------
		return PoissonPatchwork(L);
	}
}

long int THISCLASS::PoissonModeSearch(double L) {
	/*
	   This subfunction generates a random variate with the poisson
	   distribution by down/up search from the mode, using the chop-down
	   technique (PMDU).

	   Execution time grows with sqrt(L). Gives overflow for L > 60.
	*/
	static double P_L_last = -1;
	static double P_f0;
	static long int P_bound;
	double r, c, d;
	long int x, i, mode;

	mode = (long int)L;

	if (L != P_L_last) {  // set up
		P_L_last = L;
		P_bound = (long int)floor(L + 0.5 + 7.0 * (sqrt(L + L + 1.) + 1.5));// safety-bound
		P_f0 = exp(mode * log(L) - L - LnFac(mode));
	}        // probability of x=mode

	while (1) {
		r = mRandomMersenneTwister->randExc();
		if ((r -= P_f0) <= 0) return mode;
		c = d = P_f0;

		// alternating down/up search from the mode
		for (i = 1; i <= mode; i++) {
			// down
			x = mode - i;
			c *= x + 1;
			r *= L;
			d *= L;
			if ((r -= c) <= 0) return x;
			// up
			x = mode + i;
			d *= L;
			r *= x;
			c *= x;
			if ((r -= d) <= 0) return x;
		}

		// continue upward search from 2*mode+1 to bound
		for (x = mode + mode + 1; x <= P_bound; x++) {
			d *= L;
			r *= x;
			if ((r -= d) <= 0) return x;
		}
	}
}


long int THISCLASS::PoissonPatchwork(double L) {
	/*
	   This subfunction generates a random variate with the poisson
	   distribution using the Patchwork Rejection method (PPRS):
	   The area below the histogram function f(x) is rearranged in
	   its body by two point reflections. Within a large center
	   interval variates are sampled efficiently by rejection from
	   uniform hats. Rectangular immediate acceptance regions speed
	   up the generation. The remaining tails are covered by
	   exponential functions.

	   For detailed explanation, see:
	   Stadlober, E & Zechner, H: "The Patchwork Rejection Technique for
	   Sampling from Unimodal Distributions". ACM Transactions on Modeling
	   and Computer Simulation, vol. 9, no. 1, 1999, p. 59-83.

	   This method is valid for L >= 10.

	   The computation time hardly depends on L, except that it matters
	   a lot whether L is within the range where the LnFac function is
	   tabulated.
	*/

	static double        P_L_last = -1.;
	static long int      P_k1, P_k2, P_k4, P_k5;
	static double        P_dl, P_dr, P_r1, P_r2, P_r4, P_r5,
	P_ll, P_lr, P_l_my, P_c_pm,
	P_f1, P_f2, P_f4, P_f5,
	P_p1, P_p2, P_p3, P_p4, P_p5, P_p6;
	long int             mode, Dk, X, Y;
	double               Ds, U, V, W;

	if (L != P_L_last) {            // set-up
		P_L_last = L;

		// approximate deviation of reflection points k2, k4 from L - 1/2
		Ds = sqrt(L + 0.25);

		// mode, reflection points k2 and k4, and points k1 and k5, which
		// delimit the centre region of h(x)
		mode = (long int) L;
		P_k2 = (long int) ceil(L - 0.5 - Ds);
		P_k4 = (long int)     (L - 0.5 + Ds);
		P_k1 = P_k2 + P_k2 - mode + 1;
		P_k5 = P_k4 + P_k4 - mode;

		// range width of the critical left and right centre region
		P_dl = (double) (P_k2 - P_k1);
		P_dr = (double) (P_k5 - P_k4);

		// recurrence constants r(k) = p(k)/p(k-1) at k = k1, k2, k4+1, k5+1
		P_r1 = L / (double) P_k1;
		P_r2 = L / (double) P_k2;
		P_r4 = L / (double)(P_k4 + 1);
		P_r5 = L / (double)(P_k5 + 1);

		// reciprocal values of the scale parameters of expon. tail envelopes
		P_ll =  log(P_r1);                                   // expon. tail left
		P_lr = -log(P_r5);                                   // expon. tail right

		// Poisson constants, necessary for computing function values f(k)
		P_l_my = log(L);
		P_c_pm = mode * P_l_my - LnFac(mode);

		// function values f(k) = p(k)/p(mode) at k = k2, k4, k1, k5
		P_f2 = PoissonF(P_k2, P_l_my, P_c_pm);
		P_f4 = PoissonF(P_k4, P_l_my, P_c_pm);
		P_f1 = PoissonF(P_k1, P_l_my, P_c_pm);
		P_f5 = PoissonF(P_k5, P_l_my, P_c_pm);

		// area of the two centre and the two exponential tail regions
		// area of the two immediate acceptance regions between k2, k4
		P_p1 = P_f2 * (P_dl + 1.);                            // immed. left
		P_p2 = P_f2 * P_dl         + P_p1;                       // centre left
		P_p3 = P_f4 * (P_dr + 1.) + P_p2;                       // immed. right
		P_p4 = P_f4 * P_dr         + P_p3;                       // centre right
		P_p5 = P_f1 / P_ll         + P_p4;                       // expon. tail left
		P_p6 = P_f5 / P_lr         + P_p5;
	}                      // expon. tail right

	for (;;) {
		// generate uniform number U -- U(0, p6)
		// case distinction corresponding to U
		if ((U = mRandomMersenneTwister->randExc() * P_p6) < P_p2) {     // centre left

			// immediate acceptance region R2 = [k2, mode) *[0, f2),  X = k2, ... mode -1
			if ((V = U - P_p1) < 0.0)  return(P_k2 + (long int)(U / P_f2));
			// immediate acceptance region R1 = [k1, k2)*[0, f1),  X = k1, ... k2-1
			if ((W = V / P_dl) < P_f1 )  return(P_k1 + (long int)(V / P_f1));

			// computation of candidate X < k2, and its counterpart Y > k2
			// either squeeze-acceptance of X or acceptance-rejection of Y
			Dk = (long int)(P_dl * mRandomMersenneTwister->randExc()) + 1;
			if (W <= P_f2 - Dk * (P_f2 - P_f2 / P_r2)) { // quick accept of
				return(P_k2 - Dk);
			}                  // X = k2 - Dk

			if ((V = P_f2 + P_f2 - W) < 1.0) {       // quick reject of Y
				Y = P_k2 + Dk;
				if (V <= P_f2 + Dk * (1.0 - P_f2) / (P_dl + 1.0)) {  // quick accept of
					return(Y);
				}                                 // Y = k2 + Dk
				if (V <= PoissonF(Y, P_l_my, P_c_pm)) return(Y);
			}       // final accept of Y
			X = P_k2 - Dk;
		}

		else if (U < P_p4) {                 // centre right
			//  immediate acceptance region R3 = [mode, k4+1)*[0, f4), X = mode, ... k4
			if ((V = U - P_p3) < 0.)  return(P_k4 - (long int)((U - P_p2) / P_f4));
			// immediate acceptance region R4 = [k4+1, k5+1)*[0, f5)
			if ((W = V / P_dr) < P_f5)  return(P_k5 - (long int)(V / P_f5));

			// computation of candidate X > k4, and its counterpart Y < k4
			// either squeeze-acceptance of X or acceptance-rejection of Y
			Dk = (long int)(P_dr * mRandomMersenneTwister->randExc()) + 1L;
			if (W <= P_f4 - Dk * (P_f4 - P_f4*P_r4)) {          // quick accept of
				return(P_k4 + Dk);
			}                         // X = k4 + Dk
			if ((V = P_f4 + P_f4 - W) < 1.0) {              // quick reject of Y
				Y = P_k4 - Dk;
				if (V <= P_f4 + Dk * (1.0 - P_f4) / P_dr) {        // quick accept of
					return(Y);
				}                                 // Y = k4 - Dk
				if (V <= PoissonF(Y, P_l_my, P_c_pm))  return(Y);
			}       // final accept of Y
			X = P_k4 + Dk;
		}
		else {
			W = mRandomMersenneTwister->randExc();
			if (U < P_p5) {                                // expon. tail left
				Dk = (long int)(1.0 - log(W) / P_ll);
				if ((X = P_k1 - Dk) < 0L)  continue;           // 0 <= X <= k1 - 1
				W *= (U - P_p4) * P_ll;                          // W -- U(0, h(x))
				if (W <= P_f1 - Dk * (P_f1 - P_f1 / P_r1)) return(X);
			} // quick accept of X
			else {                                       // expon. tail right
				Dk = (long int)(1.0 - log(W) / P_lr);
				X  = P_k5 + Dk;                                // X >= k5 + 1
				W *= (U - P_p5) * P_lr;                          // W -- U(0, h(x))
				if (W <= P_f5 - Dk * (P_f5 - P_f5*P_r5)) return(X);
			}
		} // quick accept of X

		// acceptance-rejection test of candidate X from the original area
		// test, whether  W <= f(k),    with  W = U*h(x)  and  U -- U(0, 1)
		// log f(X) = (X - mode)*log(L) - log X! + log mode!
		if (log(W) <= X * P_l_my - LnFac(X) - P_c_pm) return(X);
	}
}


double THISCLASS::PoissonF(long int k, double l_nu, double c_pm) {
	// used by PoissonPatchwork
	return  exp(k * l_nu - LnFac(k) - c_pm);
}


/***********************************************************************
                      Subfunctions used by poisson
***********************************************************************/
long int THISCLASS::PoissonLow(double L) {
	/*
	   This subfunction generates a random variate with the poisson
	   distribution for extremely low values of L.

	   The method is a simple calculation of the probabilities of x = 1
	   and x = 2. Higher values are ignored.

	   The reason for using this method is to avoid the numerical inaccuracies
	   in other methods.
	*/
	double d, r;
	d = sqrt(L);
	if (mRandomMersenneTwister->randExc() >= d) return 0;
	r = mRandomMersenneTwister->randExc() * d;
	if (r > L * (1. - L)) return 0;
	if (r > 0.5 * L*L * (1. - L)) return 1;
	return 2;
}


long int THISCLASS::PoissonInver(double L) {
	/*
	   This subfunction generates a random variate with the poisson
	   distribution using inversion by the chop down method (PIN).

	   Execution time grows with L. Gives overflow for L > 80.

	   The value of bound must be adjusted to the maximal value of L.
	*/
	const int bound = 130;             // safety bound. Must be > L + 8*sqrt(L).
	static double p_L_last = -1.;      // previous value of L
	static double p_f0;                // value at x=0
	double r;                          // uniform random number
	double f;                          // function value
	long int x;                        // return value

	if (L != p_L_last) {               // set up
		p_L_last = L;
		p_f0 = exp(-L);
	}                 // f(0) = probability of x=0

	while (1) {
		r = mRandomMersenneTwister->randExc();
		x = 0;
		f = p_f0;
		do {                        // recursive calculation: f(x) = f(x-1) * L / x
			r -= f;
			if (r <= 0) return x;
			x++;
			f *= L;
			r *= x;
		}                       // instead of f /= x
		while (x <= bound);
	}
}


long int THISCLASS::PoissonRatioUniforms(double L) {
	/*
	   This subfunction generates a random variate with the poisson
	   distribution using the ratio-of-uniforms rejection method (PRUAt).

	   Execution time does not depend on L, except that it matters whether L
	   is within the range where ln(n!) is tabulated.

	   Reference: E. Stadlober: "The ratio of uniforms approach for generating
	   discrete random variates". Journal of Computational and Applied Mathematics,
	   vol. 31, no. 1, 1990, pp. 181-189.
	*/
	static double p_L_last = -1.0;            // previous L
	static double p_a;                       // hat center
	static double p_h;                       // hat width
	static double p_g;                       // ln(L)
	static double p_q;                       // value at mode
	static long int p_bound;                 // upper bound
	long int mode;                           // mode
	double u;                                // uniform random
	double lf;                               // ln(f(x))
	double x;                                // real sample
	long int k;                              // integer sample

	if (p_L_last != L) {
		p_L_last = L;                           // Set-up
		p_a = L + 0.5;                          // hat center
		mode = (long int)L;                     // mode
		p_g  = log(L);
		p_q = mode * p_g - LnFac(mode);         // value at mode
		p_h = sqrt(SHAT1 * (L + 0.5)) + SHAT2;  // hat width
		p_bound = (long int)(p_a + 6.0 * p_h);
	} // safety-bound

	while (1) {
		u = mRandomMersenneTwister->randExc();
		if (u == 0) continue;                   // avoid division by 0
		x = p_a + p_h * (mRandomMersenneTwister->randExc() - 0.5) / u;
		if (x < 0 || x >= p_bound) continue;    // reject if outside valid range
		k = (long int)(x);
		lf = k * p_g - LnFac(k) - p_q;
		if (lf >= u * (4.0 - u) - 3.0) break;   // quick acceptance
		if (u * (u - lf) > 1.0) continue;       // quick rejection
		if (2.0 * log(u) <= lf) break;
	}         // final acceptance
	return(k);
}


/***********************************************************************
                     Log factorial function
***********************************************************************/
double THISCLASS::LnFac(long int n) {
	// log factorial function. gives natural logarithm of n!

	// define constants
	static const double        // coefficients in Stirling approximation
	C0 =  0.918938533204672722,   // ln(sqrt(2*pi))
	      C1 =  1. / 12.,
	            C3 = -1. / 360.;
	// C5 =  1./1260.,  // use r^5 term if FAK_LEN < 50
	// C7 = -1./1680.;  // use r^7 term if FAK_LEN < 20
	// static variables
	static double fac_table[FAK_LEN]; // table of ln(n!):
	static int initialized = 0;   // remember if fac_table has been initialized

	if (n < FAK_LEN) {
		if (n <= 1) {
			if (n < 0) return 0; //FatalError(wxT("Parameter negative in LnFac function"));
			return 0;
		}
		if (!initialized) { // first time. Must initialize table
			// make table of ln(n!)
			double sum = fac_table[0] = 0.;
			for (int i = 1; i < FAK_LEN; i++) {
				sum += log((double)i);
				fac_table[i] = sum;
			}
			initialized = 1;
		}
		return fac_table[n];
	}

	// not found in table. use Stirling approximation
	double  n1, r;
	n1 = n;
	r  = 1. / n1;
	return (n1 + 0.5)*log(n1) - n1 + C0 + r*(C1 + r*r*C3);
}
