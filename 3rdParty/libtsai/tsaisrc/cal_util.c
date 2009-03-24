/*******************************************************************************\
*                                                                               *
* This file contains utility routines for dumping, loading and printing         *
* the data structures used by the routines contained in the file                *
* cal_main.c.                                                                   *
*                                                                               *
* History                                                                       *
* -------                                                                       *
*                                                                               *
* 01-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Filename changes for DOS port.                                          *
*                                                                               *
* 04-Jun-94  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN                 *
*       Added alternate macro definitions for less common math functions.       *
*                                                                               *
* 30-Nov-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*       Updated to use new calibration statistics routines.                     *
*                                                                               *
* 01-May-93  Reg Willson (rgw@cs.cmu.edu) at Carnegie-Mellon University         *
*            Original implementation.                                           *
*
* April 1997 Thierry Zimmerman IMT/DMT/EPFL
*		Aded the cal_util.h header file 
*
*                                                                               *
*                                                                               *
\*******************************************************************************/
#include "libtsai.h"
#include "string.h"

#define PI              3.14159265358979323846264338327950288419716939937511
/************************************************************************/
//added by Francis
void initialize_teli_matrox_parms (camera_parameters *pcp)      //model: CS5130P
  {
    /*strcpy (camera_type, "Teli CS5130P + Matrox");*/

    pcp->Ncx = 752;           // [sel]
    pcp->Nfx = 768;               // [pix]
    pcp->dx = 6.4/752;        // [mm/sel]
    pcp->dy = 4.8/582;        // [mm/sel]
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   // [mm/pix]
    pcp->dpy = pcp->dy;     // [mm/pix]
    pcp->Cx = (768-2)/ 2;     // [pix]      2 blank lines on the right
    pcp->Cy = (582+2)/ 2;     // [pix]      2 blank lines on the top
    pcp->sx = 1.0;            // []
  }
/************************************************************************/
//added by Francis
void      initialize_sony_xc999p_matrox_parms (camera_parameters *pcp)
{
    /*strcpy (camera_type, "Sony XC-999P + Matrox");*/

    pcp->Ncx = 752;           // [sel]
    pcp->Nfx = 768;               // [pix]
    pcp->dx = 6.4/752;        // [mm/sel]
    pcp->dy = 4.8/582;        // [mm/sel]
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   // [mm/pix]
    pcp->dpy = pcp->dy;     // [mm/pix]
    pcp->Cx = (768-4)/ 2;     // [pix]        4 blank lines on the right
    pcp->Cy = (582+2)/ 2;     // [pix]        2 blank lines on the top
    pcp->sx = 1.0;            // []
}

/************************************************************************/
//added by Francis
void      initialize_sony_dxc750p_matrox_parms (camera_parameters *pcp)
{
    /*strcpy (camera_type, "Sony DXC-750P + Matrox");*/

    pcp->Ncx = 786;           // [sel]
    pcp->Nfx = 768;               // [pix]
    pcp->dx = 8.8/786;        // [mm/sel]
    pcp->dy = 6.6/581;        // [mm/sel]
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   // [mm/pix]
    pcp->dpy = pcp->dy * 581 / 582;     // [mm/pix]
    pcp->Cx = (768)/ 2;       // [pix]        0 blank lines on the right
    pcp->Cy = (582)/ 2;       // [pix]        0 blank lines on the top
    pcp->sx = 1.0;            // []
}

/************************************************************************/
void      initialize_photometrics_parms (camera_parameters *pcp)
{
    /*strcpy (camera_type, "Photometrics Star I");*/

    pcp->Ncx = 576;       /* [sel]        */
    pcp->Nfx = 576;       /* [pix]        */
    pcp->dx = 0.023;      /* [mm/sel]     */
    pcp->dy = 0.023;      /* [mm/sel]     */
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   /* [mm/pix]     */
    pcp->dpy = pcp->dy;     /* [mm/pix]     */
    pcp->Cx = 576 / 2;        /* [pix]        */
    pcp->Cy = 384 / 2;        /* [pix]        */
    pcp->sx = 1.0;        /* []        */

    /* something a bit more realistic */
    pcp->Cx = 258.0;
    pcp->Cy = 204.0;
}


/************************************************************************/
void      initialize_general_imaging_mos5300_matrox_parms (camera_parameters *pcp)
{
    /*strcpy (camera_type, "General Imaging MOS5300 + Matrox");*/

    pcp->Ncx = 649;       /* [sel]        */
    pcp->Nfx = 512;       /* [pix]        */
    pcp->dx = 0.015;      /* [mm/sel]     */
    pcp->dy = 0.015;      /* [mm/sel]     */
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   /* [mm/pix]     */
    pcp->dpy = pcp->dy;     /* [mm/pix]     */
    pcp->Cx = 512 / 2;        /* [pix]        */
    pcp->Cy = 480 / 2;        /* [pix]        */
    pcp->sx = 1.0;        /* []        */
}


/************************************************************************/
void      initialize_panasonic_gp_mf702_matrox_parms (camera_parameters *pcp)
{
    /*strcpy (camera_type, "Panasonic GP-MF702 + Matrox");*/

    pcp->Ncx = 649;       /* [sel]        */
    pcp->Nfx = 512;       /* [pix]        */
    pcp->dx = 0.015;      /* [mm/sel]     */
    pcp->dy = 0.015;      /* [mm/sel]     */
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   /* [mm/pix]     */
    pcp->dpy = pcp->dy;     /* [mm/pix]     */

    pcp->Cx = 268;        /* [pix]        */
    pcp->Cy = 248;        /* [pix]        */

    pcp->sx = 1.078647;       /* []           */
}


/************************************************************************/
void      initialize_sony_xc75_matrox_parms (camera_parameters *pcp)
{
    /*strcpy (camera_type, "Sony XC75 + Matrox");*/

    pcp->Ncx = 768;       /* [sel]        */
    pcp->Nfx = 512;       /* [pix]        */
    pcp->dx = 0.0084;     /* [mm/sel]     */
    pcp->dy = 0.0098;     /* [mm/sel]     */
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   /* [mm/pix]     */
    pcp->dpy = pcp->dy;     /* [mm/pix]     */
    pcp->Cx = 512 / 2;        /* [pix]        */
    pcp->Cy = 480 / 2;        /* [pix]        */
    pcp->sx = 1.0;        /* []           */
}


/************************************************************************/
void      initialize_sony_xc77_matrox_parms (camera_parameters *pcp)
{
    /*strcpy (camera_type, "Sony XC77 + Matrox");*/

    pcp->Ncx = 768;       /* [sel]        */
    pcp->Nfx = 512;       /* [pix]        */
    pcp->dx = 0.011;      /* [mm/sel]     */
    pcp->dy = 0.013;      /* [mm/sel]     */
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   /* [mm/pix]     */
    pcp->dpy = pcp->dy;     /* [mm/pix]     */
    pcp->Cx = 512 / 2;        /* [pix]        */
    pcp->Cy = 480 / 2;        /* [pix]        */
    pcp->sx = 1.0;        /* []           */
}


/************************************************************************/
void      initialize_sony_xc57_androx_parms (camera_parameters *pcp)
{
    /*strcpy (camera_type, "Sony XC57 + Androx");*/

    pcp->Ncx = 510;               /* [sel]        */
    pcp->Nfx = 512;               /* [pix]        */
    pcp->dx = 0.017;              /* [mm/sel]     */
    pcp->dy = 0.013;              /* [mm/sel]     */
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   /* [mm/pix]     */
    pcp->dpy = pcp->dy;             /* [mm/pix]     */
    pcp->Cx = 512 / 2;            /* [pix]        */
    pcp->Cy = 480 / 2;            /* [pix]        */
    pcp->sx = 1.107914;           /* []           */
}


/************************************************************************/
/* John Krumm, 9 November 1993                                          */
/* This assumes that every other row, starting with the second row from */
/* the top, has been removed.  The Xap Shot CCD only has 250 lines, and */
/* it inserts new rows in between the libtsai_real rows to make a full size     */
/* picture.  Its algorithm for making these new rows isn't very good,   */
/* so it's best just to throw them away.                                */
/* The camera's specs give the CCD size as 6.4(V)x4.8(H) mm.            */
/* A call to the manufacturer found that the CCD has 250 rows           */
/* and 782 columns.  It is underscanned to 242 rows and 739 columns.    */
/************************************************************************/
void      initialize_xapshot_matrox_parms (camera_parameters *pcp)
{
    /*strcpy (camera_type, "Canon Xap Shot");*/

    pcp->Ncx = 739;           /* [sel]        */
    pcp->Nfx = 512;           /* [pix]        */
    pcp->dx = 6.4 / 782.0;        /* [mm/sel]     */
    pcp->dy = 4.8 / 250.0;        /* [mm/sel]     */
    pcp->dpx = pcp->dx * pcp->Ncx / pcp->Nfx;   /* [mm/pix]     */
    pcp->dpy = pcp->dy;         /* [mm/pix]     */
    pcp->Cx = 512 / 2;            /* [pix]        */
    pcp->Cy = 240 / 2;            /* [pix]        */
    pcp->sx = 1.027753;   /* from noncoplanar calibration *//* []           */
}
/***********************************************************************\
* This routine solves for the roll, pitch and yaw angles (in radians)   *
* for a given orthonormal rotation matrix (from Richard P. Paul,        *
* Robot Manipulators: Mathematics, Programming and Control, p70).       *
* Note 1, should the rotation matrix not be orthonormal these will not  *
* be the "best fit" roll, pitch and yaw angles.                         *
* Note 2, there are actually two possible solutions for the matrix.     *
* The second solution can be found by adding 180 degrees to Rz before   *
* Ry and Rx are calculated.                                             *
\***********************************************************************/
void solve_RPY_transform (calibration_constants *pcc)
  {
  double    sg,cg;

  pcc->Rz = atan2 (pcc->r4, pcc->r1);
  SINCOS (pcc->Rz, sg, cg);
  pcc->Ry = atan2 (-pcc->r7, pcc->r1 * cg + pcc->r4 * sg);
  pcc->Rx = atan2 (pcc->r3 * sg - pcc->r6 * cg, pcc->r5 * cg - pcc->r2 * sg);
  }


/***********************************************************************\
* This routine simply takes the roll, pitch and yaw angles and fills in *
* the rotation matrix elements r1-r9.                   *
\***********************************************************************/
void apply_RPY_transform (calibration_constants *pcc)
  {
  double    sa,ca,sb,cb,sg,cg;

  SINCOS (pcc->Rx, sa, ca); SINCOS (pcc->Ry, sb, cb); SINCOS (pcc->Rz, sg, cg);

  pcc->r1 = cb * cg;
  pcc->r2 = cg * sa * sb - ca * sg;
  pcc->r3 = sa * sg + ca * cg * sb;
  pcc->r4 = cb * sg;
  pcc->r5 = sa * sb * sg + ca * cg;
  pcc->r6 = ca * sb * sg - cg * sa;
  pcc->r7 = -sb;
  pcc->r8 = cb * sa;
  pcc->r9 = ca * cb;
  } 
/*
void      load_cd_data (fp, cd)
    FILE     *fp;
    struct calibration_data *cd;
*/
void load_cd_data (FILE* fp, calibration_data *pcd)
{
    pcd->point_count = 0;
    while (fscanf (fp, "%lf %lf %lf %lf %lf",
         &(pcd->xw[pcd->point_count]),
         &(pcd->yw[pcd->point_count]),
         &(pcd->zw[pcd->point_count]),
         &(pcd->Xf[pcd->point_count]),
         &(pcd->Yf[pcd->point_count])) != EOF)
    if (pcd->point_count++ >= TSAI_MAX_POINTS) {
       fprintf (stderr, "load_cd_data: too many points, compiled in limit is %d\n", TSAI_MAX_POINTS);
       exit (-1);
   }
}

/*
void      dump_cd_data (fp, cd)
    FILE     *fp;
    struct calibration_data *cd;
*/
void dump_cd_data (FILE* fp, calibration_data *pcd)

{
    int       i;

    for (i = 0; i < pcd->point_count; i++)
      fprintf (fp, "%17.10le %17.10le %17.10le %17.10le %17.10le\n",
         pcd->xw[i],pcd->yw[i],pcd->zw[i],pcd->Xf[i],pcd->Yf[i]);
  }

/*
void      load_cp_cc_data (fp, cp, cc)
    FILE     *fp;
    struct camera_parameters *cp;
    struct calibration_constants *cc;
*/
void load_cp_cc_data (FILE* fp, camera_parameters *cp, calibration_constants *cc)
  {
    double    sa,ca,sb,cb,sg,cg;

    fscanf (fp, "%lf", &(cp->Ncx));
    fscanf (fp, "%lf", &(cp->Nfx));
    fscanf (fp, "%lf", &(cp->dx));
    fscanf (fp, "%lf", &(cp->dy));
    fscanf (fp, "%lf", &(cp->dpx));
    fscanf (fp, "%lf", &(cp->dpy));
    fscanf (fp, "%lf", &(cp->Cx));
    fscanf (fp, "%lf", &(cp->Cy));
    fscanf (fp, "%lf", &(cp->sx));

    fscanf (fp, "%lf", &(cc->f));
    fscanf (fp, "%lf", &(cc->kappa1));
    fscanf (fp, "%lf", &(cc->Tx));
    fscanf (fp, "%lf", &(cc->Ty));
    fscanf (fp, "%lf", &(cc->Tz));
    fscanf (fp, "%lf", &(cc->Rx));
    fscanf (fp, "%lf", &(cc->Ry));
    fscanf (fp, "%lf", &(cc->Rz));

    SINCOS (cc->Rx, sa, ca);
    SINCOS (cc->Ry, sb, cb);
    SINCOS (cc->Rz, sg, cg);

    cc->r1 = cb * cg;
    cc->r2 = cg * sa * sb - ca * sg;
    cc->r3 = sa * sg + ca * cg * sb;
    cc->r4 = cb * sg;
    cc->r5 = sa * sb * sg + ca * cg;
    cc->r6 = ca * sb * sg - cg * sa;
    cc->r7 = -sb;
    cc->r8 = cb * sa;
    cc->r9 = ca * cb;

    fscanf (fp, "%lf", &(cc->p1));
    fscanf (fp, "%lf", &(cc->p2));
}

/*
void      dump_cp_cc_data (fp, cp, cc)
    FILE     *fp;
    struct camera_parameters *cp;
    struct calibration_constants *cc;
*/
void dump_cp_cc_data (FILE* fp, camera_parameters *cp, calibration_constants *cc)
{
    fprintf (fp, "%17.10le\n", cp->Ncx);
    fprintf (fp, "%17.10le\n", cp->Nfx);
    fprintf (fp, "%17.10le\n", cp->dx);
    fprintf (fp, "%17.10le\n", cp->dy);
    fprintf (fp, "%17.10le\n", cp->dpx);
    fprintf (fp, "%17.10le\n", cp->dpy);
    fprintf (fp, "%17.10le\n", cp->Cx);
    fprintf (fp, "%17.10le\n", cp->Cy);
    fprintf (fp, "%17.10le\n", cp->sx);

    fprintf (fp, "%17.10le\n", cc->f);
    fprintf (fp, "%17.10le\n", cc->kappa1);
    fprintf (fp, "%17.10le\n", cc->Tx);
    fprintf (fp, "%17.10le\n", cc->Ty);
    fprintf (fp, "%17.10le\n", cc->Tz);
    fprintf (fp, "%17.10le\n", cc->Rx);
    fprintf (fp, "%17.10le\n", cc->Ry);
    fprintf (fp, "%17.10le\n", cc->Rz);
    fprintf (fp, "%17.10le\n", cc->p1);
    fprintf (fp, "%17.10le\n", cc->p2);
}

/*
void  print_cp_cc_data (fp, cp, cc)
    FILE     *fp;
    struct camera_parameters *cp;
    struct calibration_constants *cc;
*/
void print_cp_cc_data (FILE* fp, camera_parameters *cp, calibration_constants *cc)
  {
  fprintf (fp, "       f = %.6lf  [mm]\n\n", cc->f);
  fprintf (fp, "       kappa1 = %.6le  [1/mm^2]\n\n", cc->kappa1);
  fprintf (fp, "       Tx = %.6lf,  Ty = %.6lf,  Tz = %.6lf  [mm]\n\n", cc->Tx, cc->Ty, cc->Tz);
  fprintf (fp, "       Rx = %.6lf,  Ry = %.6lf,  Rz = %.6lf  [deg]\n\n",
        cc->Rx * 180 / PI, cc->Ry * 180 / PI, cc->Rz * 180 / PI);
  fprintf (fp, "       R\n");
  fprintf (fp, "       %9.6lf  %9.6lf  %9.6lf\n", cc->r1, cc->r2, cc->r3);
  fprintf (fp, "       %9.6lf  %9.6lf  %9.6lf\n", cc->r4, cc->r5, cc->r6);
  fprintf (fp, "       %9.6lf  %9.6lf  %9.6lf\n\n", cc->r7, cc->r8, cc->r9);
  fprintf (fp, "       sx = %.6lf\n", cp->sx);
  fprintf (fp, "       Cx = %.6lf,  Cy = %.6lf  [pixels]\n\n", cp->Cx, cp->Cy);
  fprintf (fp, "       Tz / f = %.6lf\n\n", cc->Tz / cc->f);
  }
void sprint_cp_cc_data (char *str, camera_parameters *cp, calibration_constants *cc)
  {
  char		locstr[255];

  sprintf (str, "f = %.6lf  [mm]\n", cc->f);
  sprintf (locstr, "kappa1 = %.6le  [1/mm^2]\n\n", cc->kappa1); strcat(str,locstr);
  sprintf (locstr, "Tx = %.6lf,  Ty = %.6lf,  Tz = %.6lf  [mm]\n", cc->Tx, cc->Ty, cc->Tz);
  strcat(str,locstr);
  sprintf (locstr, "Rx = %.6lf,  Ry = %.6lf,  Rz = %.6lf  [deg]\n",
        cc->Rx * 180 / PI, cc->Ry * 180 / PI, cc->Rz * 180 / PI);
  strcat(str,locstr);
  sprintf (locstr, "R\n"); strcat(str,locstr);
  sprintf (locstr, "%9.6lf  %9.6lf  %9.6lf\n", cc->r1, cc->r2, cc->r3); strcat(str,locstr);
  sprintf (locstr, "%9.6lf  %9.6lf  %9.6lf\n", cc->r4, cc->r5, cc->r6); strcat(str,locstr);
  sprintf (locstr, "%9.6lf  %9.6lf  %9.6lf\n\n", cc->r7, cc->r8, cc->r9); strcat(str,locstr);
  sprintf (locstr, "sx = %.6lf\n", cp->sx); strcat(str,locstr);
  sprintf (locstr, "Cx = %.6lf,  Cy = %.6lf  [pixels]\n", cp->Cx, cp->Cy); strcat(str,locstr);
  sprintf (locstr, "Tz / f = %.6lf\n", cc->Tz / cc->f); strcat(str,locstr);
  }

