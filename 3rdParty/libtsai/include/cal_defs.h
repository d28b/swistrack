#ifndef cal_defs_h
#define cal_defs_h
/*******************************************************************************\
* This file contains operating constants, data structure definitions and I/O  	*
* variable declarations for the calibration routines contained in the file	*
* cal_main.c.                                         				*
*                                                                               *
* An explanation of the basic algorithms and description of the variables  	*
* can be found in "An Efficient and Accurate Camera Calibration Technique for 	*
* 3D Machine Vision", Roger Y. Tsai, Proceedings of IEEE Conference on Computer *
* Vision and Pattern Recognition, 1986, pages 364-374.  This work also appears  *
* in several other publications under Roger Tsai's name.       			*
*                                                                               *
* Notation                                                                      *
* --------                                                                      *
*                                                                               *
* The camera's X axis runs along increasing column coordinates in the           *
* image/frame.  The Y axis runs along increasing row coordinates.               *
*                                                                               *
* pix == image/frame grabber picture element                                    *
* sel == camera sensor element                                                  *
*                                                                               *
*                                                                               *
* History                                                                       *
* -------                                                                       *
*                                                                               *
* April 1997 Thierry Zimmerman IMT/DMT/EPFL                                     *
*	Used to be in cal_main.h. Contains the macros and misc. defines
*	No more reference is done to any global varaiables			*
*                                                                               *
* 01-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Filename changes for DOS port.                                          *
*                                                                               *
* 04-Jun-94  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Added alternate macro definitions for less common math functions.       *
*                                                                               *
* 25-Mar-94  Torfi Thorhallsson (torfit@verk.hi.is) at the University of Iceland*
*       Added definitions of parameters controlling MINPACK's lmdif()           *
*       optimization routine.                                                   *
*                                                                               *
* 30-Nov-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Added declaration for camera type.                                      *
*                                                                               *
* 07-Feb-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Original implementation.                                                *
\*******************************************************************************/

/* An arbitrary tolerance factor */
#define EPSILON      1.0E-8

/* Commonly used macros */
#define ABS(a)          (((a) > 0) ? (a) : -(a))
#define SIGNBIT(a)      (((a) > 0) ? 0 : 1)
#define SQR(a)          ((a) * (a))
#define CUB(a)          ((a)*(a)*(a))

#ifndef MAX  // added by Etienne
	#define MAX(a,b)        (((a) > (b)) ? (a) : (b))  
	#define MIN(a,b)        (((a) < (b)) ? (a) : (b))
#endif

/* Some math libraries may not have the sincos() routine */
#ifdef _SINCOS_
  #define SINCOS(x,s,c)   sincos(x,&s,&c)
#else
  #define SINCOS(x,s,c)   s=sin(x);c=cos(x)
#endif


/* Parameters controlling MINPACK's lmdif() optimization routine. */
/* See the file lmdif.f for definitions of each parameter.        */
#define REL_SENSOR_TOLERANCE_ftol    1.0E-5      /* [pix] */
#define REL_PARAM_TOLERANCE_xtol     1.0E-7
#define ORTHO_TOLERANCE_gtol         0.0
#define MAXFEV                       (1000*n)
#define EPSFCN                       1.0E-16     /* Do not set to zero! */
#define MODE                         1           /* variables are scalled internally */
#define FACTOR                       100.0 

#endif
