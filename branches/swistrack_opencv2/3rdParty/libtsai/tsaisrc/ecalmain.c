/*******************************************************************************\
*                                                                               *
* This file contains routines for calibrating the extrinsic parameters of       *
* Tsai's 11 parameter camera model.  The inputs to the routines are a set of    *
* precalibrated intrinsic camera parameters:                                    *
*                                                                               *
*       f       - effective focal length of the pin hole camera                 *
*       kappa1  - 1st order radial lens distortion coefficient                  *
*       Cx, Cy  - coordinates of center of radial lens distortion               *
*                 (also used as the piercing point of the camera coordinate     *
*                  frame's Z axis with the camera's sensor plane)               *
*       sx      - uncertainty factor for scale of horizontal scanline           *
*                                                                               *
* and a set of calibration data consisting of the (x,y,z) world coordinates of  *
* a feature point (in mm) and the corresponding coordinates (Xf,Yf) (in pixels) *
* of the feature point in the image.  The outputs of the routines are the 6     *
* external (also called extrinsic or exterior) camera parameters:               *
*                                                                               *
*       Rx, Ry, Rz, Tx, Ty, Tz  - rotational and translational components of    *
*                                 the transform between the world's coordinate  *
*                                 frame and the camera's coordinate frame.      *
*                                                                               *
* describing the camera's pose.                                                 *
*                                                                               *
* This file provides two routines:                                              *
*                                                                               *
*       coplanar_extrinsic_parameter_estimation ()                              *
* and                                                                           *
*       noncoplanar_extrinsic_parameter_estimation ()                           *
*                                                                               *
* which are used respectively for coplanar and non-coplanar calibration data.   *
*                                                                               *
* Initial estimates for the extrinsic camera parameters are determined using a  *
* modification of the first stage of Tsai's algorithm.  These estimates are     *
* then refined using iterative non-linear optimization.                         *
*                                                                               *
*                                                                               *
* History                                                                       *
* -------                                                                       *
* April 1997 Thierry Zimmerman zimmeman@imt.dmt.epfl.ch				*
*	Replaced all references to global external variables cd,cp,cc by 	*
*	GlobCDPtr, GlobCCPtr and GlobCPPtr.					*
*	The link with the variables in cal_main is done in the argument list	*
*	of the functions. These local references are used within the module	*
*	to acces the data.							*
*	This way was the easiest to get rid of the external reference without   *
*	having to rewrite all the function calls.				*
*                                                                               *
* 15-Oct-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Added routines to coplanar_extrinsic_parameter_estimation to pick       *
*       the correct rotation matrix solution from the two possible solutions.   *
*       Bug tracked down by Pete Rander <Peter.Rander@IUS4.IUS.CS.CMU.EDU>.     *
*                                                                               *
* 20-May-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Return the error to lmdif rather than the squared error.                *
*         lmdif calculates the squared error internally during optimization.    *
*         Before this change calibration was essentially optimizing error^4.    *
*                                                                               *
* 02-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Rewrite memory allocation to avoid memory alignment problems            *
*       on some machines.                                                       *
*       Strip out IMSL code.  MINPACK seems to work fine.                       *
*       Filename changes for DOS port.                                          *
*                                                                               *
* 04-Jun-94  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Added alternate macro definitions for less common math functions.       *
*                                                                               *
* 25-Mar-94  Torfi Thorhallsson (torfit@verk.hi.is) at the University of Iceland* 
*       Added a new version of the routine epe_optimize() which uses the        *
*       *public domain* MINPACK optimization library instead of IMSL.           *
*       To select the new routine, compile this file with the flag -DMINPACK    *
*                                                                               *
* 11-Nov-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Original implementation.                                                *
*                                                                               *
\*******************************************************************************/
#include "libtsai.h"
#include "lmdif.h"

#ifdef __GNUC__
#define _hypot hypot
#endif

calibration_data	*GlobCDPtr=NULL;
calibration_constants	*GlobCCPtr=NULL;
camera_parameters	*GlobCPPtr=NULL;

/***********************************************************************\
* Routines for coplanar extrinsic parameter estimation         *
\***********************************************************************/
void      cepe_compute_U (U)
    double    U[];
{
    dmat      M,
              a,
              b;

    double    Xd,
              Yd,
              Xu,
              Yu,
              distortion_factor;

    int       i;

    M = newdmat (0, (GlobCDPtr->point_count - 1), 0, 4, &errno);
    if (errno) {
   fprintf (stderr, "cepe compute U: unable to allocate matrix M\n");
   exit (-1);
    }

    a = newdmat (0, 4, 0, 0, &errno);
    if (errno) {
   fprintf (stderr, "cepe compute U: unable to allocate vector a\n");
   exit (-1);
    }

    b = newdmat (0, (GlobCDPtr->point_count - 1), 0, 0, &errno);
    if (errno) {
   fprintf (stderr, "cepe compute U: unable to allocate vector b\n");
   exit (-1);
    }

    for (i = 0; i < GlobCDPtr->point_count; i++) {
   /* convert from image coordinates to distorted sensor coordinates */
   Xd = GlobCPPtr->dpx * (GlobCDPtr->Xf[i] - GlobCPPtr->Cx) / GlobCPPtr->sx;
   Yd = GlobCPPtr->dpy * (GlobCDPtr->Yf[i] - GlobCPPtr->Cy);

   /* convert from distorted sensor coordinates to undistorted sensor coordinates */
   distortion_factor = 1 + GlobCCPtr->kappa1 * (SQR (Xd) + SQR (Yd));
   Xu = Xd * distortion_factor;
   Yu = Yd * distortion_factor;

   M.el[i][0] = Yu * GlobCDPtr->xw[i];
   M.el[i][1] = Yu * GlobCDPtr->yw[i];
   M.el[i][2] = Yu;
   M.el[i][3] = -Xu * GlobCDPtr->xw[i];
   M.el[i][4] = -Xu * GlobCDPtr->yw[i];
   b.el[i][0] = Xu;
    }

    if (solve_system (M, a, b)) {
   fprintf (stderr, "cepe compute U: unable to solve system  Ma=b\n");
   exit (-1);
    }

    U[0] = a.el[0][0];
    U[1] = a.el[1][0];
    U[2] = a.el[2][0];
    U[3] = a.el[3][0];
    U[4] = a.el[4][0];

    freemat (M);
    freemat (a);
    freemat (b);
}


void      cepe_compute_Tx_and_Ty (U)
    double    U[];
{
    double    Tx,
              Ty,
              Ty_squared,
              x,
              y,
              Sr,
              r1p,
              r2p,
              r4p,
              r5p,
              r1,
              r2,
              r4,
              r5,
              distance,
              far_distance;

    int       i,
              far_point;

    r1p = U[0];
    r2p = U[1];
    r4p = U[3];
    r5p = U[4];

    /* first find the square of the magnitude of Ty */
    if ((fabs (r1p) < EPSILON) && (fabs (r2p) < EPSILON))
   Ty_squared = 1 / (SQR (r4p) + SQR (r5p));
    else if ((fabs (r4p) < EPSILON) && (fabs (r5p) < EPSILON))
   Ty_squared = 1 / (SQR (r1p) + SQR (r2p));
    else if ((fabs (r1p) < EPSILON) && (fabs (r4p) < EPSILON))
   Ty_squared = 1 / (SQR (r2p) + SQR (r5p));
    else if ((fabs (r2p) < EPSILON) && (fabs (r5p) < EPSILON))
   Ty_squared = 1 / (SQR (r1p) + SQR (r4p));
    else {
   Sr = SQR (r1p) + SQR (r2p) + SQR (r4p) + SQR (r5p);
   Ty_squared = (Sr - sqrt (SQR (Sr) - 4 * SQR (r1p * r5p - r4p * r2p))) /
    (2 * SQR (r1p * r5p - r4p * r2p));
    }

    /* find a point that is far from the image center */
    far_distance = 0;
    far_point = 0;
    for (i = 0; i < GlobCDPtr->point_count; i++)
   if ((distance = SQR (GlobCDPtr->Xf[i] - GlobCPPtr->Cx) + SQR (GlobCDPtr->Yf[i] - GlobCPPtr->Cy)) > far_distance) {
       far_point = i;
       far_distance = distance;
   }

    /* now find the sign for Ty */
    /* start by assuming Ty > 0 */
    Ty = sqrt (Ty_squared);
    r1 = U[0] * Ty;
    r2 = U[1] * Ty;
    Tx = U[2] * Ty;
    r4 = U[3] * Ty;
    r5 = U[4] * Ty;
    x = r1 * GlobCDPtr->xw[far_point] + r2 * GlobCDPtr->yw[far_point] + Tx;
    y = r4 * GlobCDPtr->xw[far_point] + r5 * GlobCDPtr->yw[far_point] + Ty;

    /* flip Ty if we guessed wrong */
    if ((SIGNBIT (x) != SIGNBIT (GlobCDPtr->Xf[far_point] - GlobCPPtr->Cx)) ||
   (SIGNBIT (y) != SIGNBIT (GlobCDPtr->Yf[far_point] - GlobCPPtr->Cy)))
   Ty = -Ty;

    /* update the calibration constants */
    GlobCCPtr->Tx = U[2] * Ty;
    GlobCCPtr->Ty = Ty;
}


void      cepe_compute_R (U)
    double    U[];
{
    double    r1,
              r2,
              r3,
              r4,
              r5,
              r6,
              r7,
              r8,
              r9;

    r1 = U[0] * GlobCCPtr->Ty;
    r2 = U[1] * GlobCCPtr->Ty;
    r3 = sqrt (1 - SQR (r1) - SQR (r2));

    r4 = U[3] * GlobCCPtr->Ty;
    r5 = U[4] * GlobCCPtr->Ty;
    r6 = sqrt (1 - SQR (r4) - SQR (r5));
    if (!SIGNBIT (r1 * r4 + r2 * r5))
   r6 = -r6;

    /* use the outer product of the first two rows to get the last row */
    r7 = r2 * r6 - r3 * r5;
    r8 = r3 * r4 - r1 * r6;
    r9 = r1 * r5 - r2 * r4;

    /* update the calibration constants */
    GlobCCPtr->r1 = r1;
    GlobCCPtr->r2 = r2;
    GlobCCPtr->r3 = r3;
    GlobCCPtr->r4 = r4;
    GlobCCPtr->r5 = r5;
    GlobCCPtr->r6 = r6;
    GlobCCPtr->r7 = r7;
    GlobCCPtr->r8 = r8;
    GlobCCPtr->r9 = r9;

    /* fill in GlobCCPtr->Rx, GlobCCPtr->Ry and GlobCCPtr->Rz */
    solve_RPY_transform (GlobCCPtr);
}


void      cepe_compute_approximate_f (f)
    double   *f;
{
    dmat      M,
              a,
              b;

    double    Yd;

    int       i;

    M = newdmat (0, (GlobCDPtr->point_count - 1), 0, 1, &errno);
    if (errno) {
   fprintf (stderr, "cepe compute apx: unable to allocate matrix M\n");
   exit (-1);
    }

    a = newdmat (0, 1, 0, 0, &errno);
    if (errno) {
   fprintf (stderr, "cepe compute apx: unable to allocate vector a\n");
   exit (-1);
    }

    b = newdmat (0, (GlobCDPtr->point_count - 1), 0, 0, &errno);
    if (errno) {
   fprintf (stderr, "cepe compute apx: unable to allocate vector b\n");
   exit (-1);
    }

    for (i = 0; i < GlobCDPtr->point_count; i++) {
   Yd = GlobCPPtr->dpy * (GlobCDPtr->Yf[i] - GlobCPPtr->Cy);

   M.el[i][0] = GlobCCPtr->r4 * GlobCDPtr->xw[i] + GlobCCPtr->r5 * GlobCDPtr->yw[i] + GlobCCPtr->Ty;
   M.el[i][1] = -Yd;
   b.el[i][0] = (GlobCCPtr->r7 * GlobCDPtr->xw[i] + GlobCCPtr->r8 * GlobCDPtr->yw[i]) * Yd;
    }

    if (solve_system (M, a, b)) {
   fprintf (stderr, "cepe compute apx: unable to solve system  Ma=b\n");
   exit (-1);
    }

    /* return the approximate effective focal length */
    *f = a.el[0][0];

    freemat (M);
    freemat (a);
    freemat (b);
}


/***********************************************************************\
* Routines for noncoplanar extrinsic parameter estimation      *
\***********************************************************************/
void      ncepe_compute_U (U)
    double    U[];
{
    dmat      M,
              a,
              b;

    double    Xu,
              Yu,
              Xd,
              Yd,
              distortion_factor;

    int       i;

    M = newdmat (0, (GlobCDPtr->point_count - 1), 0, 6, &errno);
    if (errno) {
   fprintf (stderr, "ncepe compute U: unable to allocate matrix M\n");
   exit (-1);
    }

    a = newdmat (0, 6, 0, 0, &errno);
    if (errno) {
   fprintf (stderr, "ncepe compute U: unable to allocate vector a\n");
   exit (-1);
    }

    b = newdmat (0, (GlobCDPtr->point_count - 1), 0, 0, &errno);
    if (errno) {
   fprintf (stderr, "ncepe compute U: unable to allocate vector b\n");
   exit (-1);
    }

    for (i = 0; i < GlobCDPtr->point_count; i++) {
   /* convert from image coordinates to distorted sensor coordinates */
   Xd = GlobCPPtr->dpx * (GlobCDPtr->Xf[i] - GlobCPPtr->Cx) / GlobCPPtr->sx;
   Yd = GlobCPPtr->dpy * (GlobCDPtr->Yf[i] - GlobCPPtr->Cy);

   /* convert from distorted sensor coordinates to undistorted sensor coordinates */
   distortion_factor = 1 + GlobCCPtr->kappa1 * (SQR (Xd) + SQR (Yd));
   Xu = Xd * distortion_factor;
   Yu = Yd * distortion_factor;

   M.el[i][0] = Yu * GlobCDPtr->xw[i];
   M.el[i][1] = Yu * GlobCDPtr->yw[i];
   M.el[i][2] = Yu * GlobCDPtr->zw[i];
   M.el[i][3] = Yu;
   M.el[i][4] = -Xu * GlobCDPtr->xw[i];
   M.el[i][5] = -Xu * GlobCDPtr->yw[i];
   M.el[i][6] = -Xu * GlobCDPtr->zw[i];
   b.el[i][0] = Xu;
    }

    if (solve_system (M, a, b)) {
   fprintf (stderr, "ncepe compute U: unable to solve system  Ma=b\n");
   exit (-1);
    }

    U[0] = a.el[0][0];
    U[1] = a.el[1][0];
    U[2] = a.el[2][0];
    U[3] = a.el[3][0];
    U[4] = a.el[4][0];
    U[5] = a.el[5][0];
    U[6] = a.el[6][0];

    freemat (M);
    freemat (a);
    freemat (b);
}


void      ncepe_compute_Tx_and_Ty (U)
    double    U[];
{
    double    Tx,
              Ty,
              Ty_squared,
              x,
              y,
              r1,
              r2,
              r3,
              r4,
              r5,
              r6,
              distance,
              far_distance;

    int       i,
              far_point;

    /* first find the square of the magnitude of Ty */
    Ty_squared = 1 / (SQR (U[4]) + SQR (U[5]) + SQR (U[6]));

    /* find a point that is far from the image center */
    far_distance = 0;
    far_point = 0;
    for (i = 0; i < GlobCDPtr->point_count; i++)
   if ((distance = SQR (GlobCDPtr->Xf[i] - GlobCPPtr->Cx) + SQR (GlobCDPtr->Yf[i] - GlobCPPtr->Cy)) > far_distance) {
       far_point = i;
       far_distance = distance;
   }

    /* now find the sign for Ty */
    /* start by assuming Ty > 0 */
    Ty = sqrt (Ty_squared);
    r1 = U[0] * Ty;
    r2 = U[1] * Ty;
    r3 = U[2] * Ty;
    Tx = U[3] * Ty;
    r4 = U[4] * Ty;
    r5 = U[5] * Ty;
    r6 = U[6] * Ty;
    x = r1 * GlobCDPtr->xw[far_point] + r2 * GlobCDPtr->yw[far_point] + r3 * GlobCDPtr->zw[far_point] + Tx;
    y = r4 * GlobCDPtr->xw[far_point] + r5 * GlobCDPtr->yw[far_point] + r6 * GlobCDPtr->zw[far_point] + Ty;

    /* flip Ty if we guessed wrong */
    if ((SIGNBIT (x) != SIGNBIT (GlobCDPtr->Xf[far_point] - GlobCPPtr->Cx)) ||
   (SIGNBIT (y) != SIGNBIT (GlobCDPtr->Yf[far_point] - GlobCPPtr->Cy)))
   Ty = -Ty;

    /* update the calibration constants */
    GlobCCPtr->Tx = U[3] * Ty;
    GlobCCPtr->Ty = Ty;
}


void      ncepe_compute_R (U)
    double    U[];
{
    double    r1,
              r2,
              r3,
              r4,
              r5,
              r6,
              r7,
              r8,
              r9;

    r1 = U[0] * GlobCCPtr->Ty;
    r2 = U[1] * GlobCCPtr->Ty;
    r3 = U[2] * GlobCCPtr->Ty;

    r4 = U[4] * GlobCCPtr->Ty;
    r5 = U[5] * GlobCCPtr->Ty;
    r6 = U[6] * GlobCCPtr->Ty;

    /* use the outer product of the first two rows to get the last row */
    r7 = r2 * r6 - r3 * r5;
    r8 = r3 * r4 - r1 * r6;
    r9 = r1 * r5 - r2 * r4;

    /* update the calibration constants */
    GlobCCPtr->r1 = r1;
    GlobCCPtr->r2 = r2;
    GlobCCPtr->r3 = r3;
    GlobCCPtr->r4 = r4;
    GlobCCPtr->r5 = r5;
    GlobCCPtr->r6 = r6;
    GlobCCPtr->r7 = r7;
    GlobCCPtr->r8 = r8;
    GlobCCPtr->r9 = r9;

    /* fill in GlobCCPtr->Rx, GlobCCPtr->Ry and GlobCCPtr->Rz */
    solve_RPY_transform (GlobCCPtr);
}


/************************************************************************/
void      epe_compute_Tx_Ty_Tz ()
{
    dmat      M,
              a,
              b;

    double    xk,
              yk,
              zk,
              Xu,
              Yu,
              Xd,
              Yd,
              distortion_factor;

    int       i,
              j;

    M = newdmat (0, (2 * GlobCDPtr->point_count - 1), 0, 2, &errno);
    if (errno) {
   fprintf (stderr, "epe compute Tx Ty Tz: unable to allocate matrix M\n");
   exit (-1);
    }

    a = newdmat (0, 2, 0, 0, &errno);
    if (errno) {
   fprintf (stderr, "epe compute Tx Ty Tz: unable to allocate vector a\n");
   exit (-1);
    }

    b = newdmat (0, (2 * GlobCDPtr->point_count - 1), 0, 0, &errno);
    if (errno) {
   fprintf (stderr, "epe compute Tx Ty Tz: unable to allocate vector b\n");
   exit (-1);
    }

    for (i = 0, j = GlobCDPtr->point_count; i < GlobCDPtr->point_count; i++, j++) {
   /* convert from world coordinates to untranslated camera coordinates */
   xk = GlobCCPtr->r1 * GlobCDPtr->xw[i] + GlobCCPtr->r2 * GlobCDPtr->yw[i] + GlobCCPtr->r3 * GlobCDPtr->zw[i];
   yk = GlobCCPtr->r4 * GlobCDPtr->xw[i] + GlobCCPtr->r5 * GlobCDPtr->yw[i] + GlobCCPtr->r6 * GlobCDPtr->zw[i];
   zk = GlobCCPtr->r7 * GlobCDPtr->xw[i] + GlobCCPtr->r8 * GlobCDPtr->yw[i] + GlobCCPtr->r9 * GlobCDPtr->zw[i];

   /* convert from image coordinates to distorted sensor coordinates */
   Xd = GlobCPPtr->dpx * (GlobCDPtr->Xf[i] - GlobCPPtr->Cx) / GlobCPPtr->sx;
   Yd = GlobCPPtr->dpy * (GlobCDPtr->Yf[i] - GlobCPPtr->Cy);

   /* convert from distorted sensor coordinates to undistorted sensor coordinates */
   distortion_factor = 1 + GlobCCPtr->kappa1 * (SQR (Xd) + SQR (Yd));
   Xu = Xd * distortion_factor;
   Yu = Yd * distortion_factor;

   M.el[i][0] = GlobCCPtr->f;
   M.el[i][1] = 0;
   M.el[i][2] = -Xu;
   b.el[i][0] = Xu * zk - GlobCCPtr->f * xk;

   M.el[j][0] = 0;
   M.el[j][1] = GlobCCPtr->f;
   M.el[j][2] = -Yu;
   b.el[j][0] = Yu * zk - GlobCCPtr->f * yk;
    }

    if (solve_system (M, a, b)) {
   fprintf (stderr, "epe compute Tx Ty Tz: unable to solve system  Ma=b\n");
   exit (-1);
    }

    GlobCCPtr->Tx = a.el[0][0];
    GlobCCPtr->Ty = a.el[1][0];
    GlobCCPtr->Tz = a.el[2][0];

    freemat (M);
    freemat (a);
    freemat (b);
}


/************************************************************************/
void      epe_optimize_error (m_ptr, n_ptr, params, err)
    integer  *m_ptr;    /* pointer to number of points to fit */
    integer  *n_ptr;    /* pointer to number of parameters */
    double *params;    /* vector of parameters */
    double *err;    /* vector of error from data */
{
    int       i;

    double    xc,
              yc,
              zc,
              Xd,
              Yd,
              Xu_1,
              Yu_1,
              Xu_2,
              Yu_2,
              distortion_factor,
              Rx,
              Ry,
              Rz,
              Tx,
              Ty,
              Tz,
              r1,
              r2,
              r3,
              r4,
              r5,
              r6,
              r7,
              r8,
              r9,
              sa,
              sb,
              sg,
              ca,
              cb,
              cg;

    Rx = params[0];
    Ry = params[1];
    Rz = params[2];
    Tx = params[3];
    Ty = params[4];
    Tz = params[5];

    SINCOS (Rx, sa, ca);
    SINCOS (Ry, sb, cb);
    SINCOS (Rz, sg, cg);

    r1 = cb * cg;
    r2 = cg * sa * sb - ca * sg;
    r3 = sa * sg + ca * cg * sb;
    r4 = cb * sg;
    r5 = sa * sb * sg + ca * cg;
    r6 = ca * sb * sg - cg * sa;
    r7 = -sb;
    r8 = cb * sa;
    r9 = ca * cb;

    for (i = 0; i < GlobCDPtr->point_count; i++) {
   /* convert from world coordinates to camera coordinates */
   xc = r1 * GlobCDPtr->xw[i] + r2 * GlobCDPtr->yw[i] + r3 * GlobCDPtr->zw[i] + Tx;
   yc = r4 * GlobCDPtr->xw[i] + r5 * GlobCDPtr->yw[i] + r6 * GlobCDPtr->zw[i] + Ty;
   zc = r7 * GlobCDPtr->xw[i] + r8 * GlobCDPtr->yw[i] + r9 * GlobCDPtr->zw[i] + Tz;

   /* convert from camera coordinates to undistorted sensor plane coordinates */
   Xu_1 = GlobCCPtr->f * xc / zc;
   Yu_1 = GlobCCPtr->f * yc / zc;

   /* convert from 2D image coordinates to distorted sensor coordinates */
   Xd = GlobCPPtr->dpx * (GlobCDPtr->Xf[i] - GlobCPPtr->Cx) / GlobCPPtr->sx;
   Yd = GlobCPPtr->dpy * (GlobCDPtr->Yf[i] - GlobCPPtr->Cy);

   /* convert from distorted sensor coordinates to undistorted sensor plane coordinates */
   distortion_factor = 1 + GlobCCPtr->kappa1 * (SQR (Xd) + SQR (Yd));
   Xu_2 = Xd * distortion_factor;
   Yu_2 = Yd * distortion_factor;

   /* record the error in the undistorted sensor coordinates */
   err[i] = _hypot (Xu_1 - Xu_2, Yu_1 - Yu_2);
    }
}


void      epe_optimize ()
{
#define NPARAMS 6

    int       i;

    /* Parameters needed by MINPACK's lmdif() */

    integer     m = GlobCDPtr->point_count;
    integer     n = NPARAMS;
    doublereal  x[NPARAMS];
    doublereal *fvec;
    doublereal  ftol = REL_SENSOR_TOLERANCE_ftol;
    doublereal  xtol = REL_PARAM_TOLERANCE_xtol;
    doublereal  gtol = ORTHO_TOLERANCE_gtol;
    integer     maxfev = MAXFEV;
    doublereal  epsfcn = EPSFCN;
    doublereal  diag[NPARAMS];
    integer     mode = MODE;
    doublereal  factor = FACTOR;
    integer     nprint = 0;
    integer     info;
    integer     nfev;
    doublereal *fjac;
    integer     ldfjac = m;
    integer     ipvt[NPARAMS];
    doublereal  qtf[NPARAMS];
    doublereal  wa1[NPARAMS];
    doublereal  wa2[NPARAMS];
    doublereal  wa3[NPARAMS];
    doublereal *wa4;

    /* allocate some workspace */
    if (( fvec = (doublereal *) calloc ((unsigned int) m, (unsigned int) sizeof(doublereal))) == NULL ) {
       fprintf(stderr,"calloc: Cannot allocate workspace fvec\n");
       exit(-1);
    }
 
    if (( fjac = (doublereal *) calloc ((unsigned int) m*n, (unsigned int) sizeof(doublereal))) == NULL ) {
       fprintf(stderr,"calloc: Cannot allocate workspace fjac\n");
       exit(-1);
    }
 
    if (( wa4 = (doublereal *) calloc ((unsigned int) m, (unsigned int) sizeof(doublereal))) == NULL ) {
       fprintf(stderr,"calloc: Cannot allocate workspace wa4\n");
       exit(-1);
    }

    /* use the current calibration and camera constants as a starting point */
    x[0] = GlobCCPtr->Rx;
    x[1] = GlobCCPtr->Ry;
    x[2] = GlobCCPtr->Rz;
    x[3] = GlobCCPtr->Tx;
    x[4] = GlobCCPtr->Ty;
    x[5] = GlobCCPtr->Tz;

    /* define optional scale factors for the parameters */
    if ( mode == 2 ) {
        for (i = 0; i < NPARAMS; i++)
            diag[i] = 1.0;             /* some user-defined values */
    }
       
    /* perform the optimization */
    lmdif_ (epe_optimize_error,
            &m, &n, x, fvec, &ftol, &xtol, &gtol, &maxfev, &epsfcn,
            diag, &mode, &factor, &nprint, &info, &nfev, fjac, &ldfjac,
            ipvt, qtf, wa1, wa2, wa3, wa4);

    /* update the calibration and camera constants */
    GlobCCPtr->Rx = x[0];
    GlobCCPtr->Ry = x[1];
    GlobCCPtr->Rz = x[2];
    apply_RPY_transform (GlobCCPtr);

    GlobCCPtr->Tx = x[3];
    GlobCCPtr->Ty = x[4];
    GlobCCPtr->Tz = x[5];

    /* release allocated workspace */
    free (fvec);
    free (fjac);
    free (wa4);

#ifdef DEBUG
    /* print the number of function calls during iteration */
    fprintf(stderr,"info: %d nfev: %d\n\n",info,nfev);
#endif

#undef NPARAMS
}


/************************************************************************/
void coplanar_extrinsic_parameter_estimation(calibration_data *pcd, calibration_constants *pcc,
		camera_parameters *pcp)
  {
  double    trial_f,U[5];

  GlobCDPtr=pcd; GlobCCPtr=pcc; GlobCPPtr=pcp;
  cepe_compute_U (U);
  cepe_compute_Tx_and_Ty (U);
  
  cepe_compute_R (U);
  cepe_compute_approximate_f (&trial_f);
  if (trial_f < 0)
    {
    /* try the other rotation matrix solution */
    GlobCCPtr->r3 = -GlobCCPtr->r3;
    GlobCCPtr->r6 = -GlobCCPtr->r6;
    GlobCCPtr->r7 = -GlobCCPtr->r7;
    GlobCCPtr->r8 = -GlobCCPtr->r8;
    solve_RPY_transform (GlobCCPtr);
    cepe_compute_approximate_f (&trial_f);
    if (trial_f < 0)
      { fprintf (stderr, "error - possible handedness problem with data\n"); exit (-1); }
    }
  GlobCCPtr->f=trial_f;
  epe_compute_Tx_Ty_Tz ();
  epe_optimize ();
  }


/************************************************************************/
void noncoplanar_extrinsic_parameter_estimation(calibration_data *pcd, calibration_constants *pcc,
		camera_parameters *pcp)
  {
  double    U[7];

  GlobCDPtr=pcd; GlobCCPtr=pcc; GlobCPPtr=pcp;
  ncepe_compute_U (U);
  ncepe_compute_Tx_and_Ty (U);
  ncepe_compute_R (U);
  epe_compute_Tx_Ty_Tz ();
  epe_optimize ();
  }
