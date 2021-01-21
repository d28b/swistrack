#ifndef libtsai_h        
#define libtsai_h
//****************************************************************************/
// Filename :   $RCSfile: libtsai.h,v $
// Version  :   $Revision: 1.1.1.1 $
//
// Author   :   $Author: yuri $
//              Bergomi Dave, Reg Willson, Thierry Zimmerman
//              Ecole Polytechnique Federale de Lausanne
//              IPR-LPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2003/12/01 16:11:18 $


/** \file libtsai.h

    Header of CAL_MAIN.C, ECALMAIN.C, CAL_TRAN.C, CAL_EVAL.C, CAL_UTIL.C
*/

/** This is a Brief description of libtsai library

    Purpose  : This file contains routines for calibrating Tsai's 11 parameter 
	           camera model, routines for error evaluation, routines for 
			   transforming between coordinate systems and routines 
			   for dumping, loading and printing the data structures used
			   in this library
               
    
    \author $Author: yuri $
    \version $Revision: 1.1.1.1 $
    \date $Date: 2003/12/01 16:11:18 $
*/

// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include "matrix.h"
#include "f2c.h"
#include "cal_defs.h"

/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

// Maximum number of data points allowed
#define TSAI_MAX_POINTS 100

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

/**
* Camera parameters are usually the fixed parameters of the given camera
* system, typically obtained from manufacturers specifications.			
*																				
* Cx and Cy (the center of radial lens distortion), may be calibrated			
* separately or as part of the coplanar/noncoplanar calibration.				
* The same with sx (x scale uncertainty factor).								           
*/
typedef struct {
    double    Ncx;      /**< [sel]     Number of sensor elements in camera's x direction */
    double    Nfx;      /**< [pix]     Number of pixels in frame grabber's x direction   */
    double    dx;       /**< [mm/sel]  X dimension of camera's sensor element (in mm)    */
    double    dy;       /**< [mm/sel]  Y dimension of camera's sensor element (in mm)    */
    double    dpx;      /**< [mm/pix]  Effective X dimension of pixel in frame grabber   */
    double    dpy;      /**< [mm/pix]  Effective Y dimension of pixel in frame grabber   */
    double    Cx;       /**< [pix]     Z axis intercept of camera coordinate system      */
    double    Cy;       /**< [pix]     Z axis intercept of camera coordinate system      */
    double    sx;       /**< []        Scale factor to compensate for any error in dpx   */
} camera_parameters ;


/** 
* Calibration constants are the model constants that are determined from the	
* calibration data.																
*/
typedef struct {
    double    f;                                /**< [mm]         */
    double    kappa1;                           /**< [1/mm^2]     */
    double    p1,p2;                            /**< [1/mm]       */
    double    Tx,Ty,Tz;                         /**< [mm]         */
    double    Rx,Ry,Rz;                         /**< [rad]		*/
    double    r1,r2,r3,r4,r5,r6,r7,r8,r9;       /**< []           */
} calibration_constants ;

/**
* Calibration data consists of the (x,y,z) world coordinates of a feature		
* point  (in mm) and the corresponding coordinates (Xf,Yf) (in pixels) of the	
* feature point in the image.  Point count is the number of points in the data	
* set.																			
*                    															
*                        														
* Coplanar calibration:       													
*                          														
* For coplanar calibration the z world coordinate of the data must be zero.		
* In addition, for numerical stability the coordinates of the feature points	
* should be placed at some arbitrary distance from the origin (0,0,0) of the	
* world coordinate system.  Finally, the plane containing the feature points	
* should not be parallel to the imaging plane.  A relative angle of 30 degrees	
* is recommended.																
*																				
*																				
* Noncoplanar calibration:														
*																				
* For noncoplanar calibration the data must not lie in a single plane.			
*																				
*/
typedef struct
  {
  int       point_count;
  double    xw[TSAI_MAX_POINTS];
  double    yw[TSAI_MAX_POINTS];
  double    zw[TSAI_MAX_POINTS];
  double    Xf[TSAI_MAX_POINTS];
  double    Yf[TSAI_MAX_POINTS];
  } calibration_data;

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/

//prototypes of CAL_MAIN.C

#ifdef __cplusplus
extern "C"
{
#endif

  /** simple coplanar implementation of Tsai's algorithm
   * 
   * only the f, Tz and kappa1 parameters are optimized
   * and all of the calibration points lie in a single plane
   *
   * \param *pcd: calibration data
   * \param *pcc: calibration constants
   * \param *pcp: calibration parameters
   * \result f, Tz and kappa1 optimized
   */
void coplanar_calibration(calibration_data *pcd,
						  calibration_constants *pcc,
						  camera_parameters *pcp);

  /** complete coplanar implementation of Tsai's algorithm
   * 
   * all parameters are optimized
   * and all of the calibration points lie in a single plane
   *
   * \param *pcd: calibration data
   * \param *pcc: calibration constants
   * \param *pcp: calibration parameters
   * \result all parameters in *pcc optimized
   */
void coplanar_calibration_with_full_optimization(calibration_data *pcd,
												 calibration_constants *pcc,
												 camera_parameters *pcp);

  /** simple non-coplanar implementation of Tsai's algorithm
   * 
   * only the f, Tz and kappa1 parameters are optimized
   * and the calibration points do not lie in a single plane
   *
   * \param *pcd: calibration data
   * \param *pcc: calibration constants
   * \param *pcp: calibration parameters
   * \result f, Tz and kappa1 optimized
   */
void noncoplanar_calibration(calibration_data *pcd,
							 calibration_constants *pcc,
							 camera_parameters *pcp);

  /** complete non-coplanar implementation of Tsai's algorithm
   * 
   * all parameters are optimized
   * and the calibration points do not lie in a single plane
   *
   * \param *pcd: calibration data
   * \param *pcc: calibration constants
   * \param *pcp: calibration parameters
   * \result all parameters in *pcc optimized
   */
void noncoplanar_calibration_with_full_optimization(calibration_data *pcd,
													calibration_constants *pcc,
													camera_parameters *pcp);

// error evaluation routines
void print_error_stats(FILE* fp, calibration_data *pcd,
					   calibration_constants cc,
					   camera_parameters cp);

void sprint_error_stats(char *str, calibration_data *pcd,
						calibration_constants cc,
						camera_parameters cp);

//prototypes of ECALMAIN.C

  /** coplanar implementation of Tsai's algorithm estimating extrinsic parameters
   * 
   * only Rx, Ry, Rz, Tx, Ty and Tz parameters are optimized,
   * all of the calibration points lie in a single plane,
   * the inputs are the precalibrated parameters kappa1, Cx, Cy and sx
   *
   * \param *pcd: calibration data
   * \param *pcc: calibration constants
   * \param *pcp: calibration parameters
   * \result Rx, Ry, Rz, Tx, Ty, Tz (extrinsic aprameters)
   */
void coplanar_extrinsic_parameter_estimation(calibration_data *pcd,
											 calibration_constants *pcc,
											 camera_parameters *pcp);

  /** non-coplanar implementation of Tsai's algorithm estimating extrinsic parameters
   * 
   * only Rx, Ry, Rz, Tx, Ty and Tz parameters are optimized,
   * the calibration points do not lie in a single plane
   * the inputs are the precalibrated parameters kappa1, Cx, Cy and sx
   *
   * \param *pcd: calibration data
   * \param *pcc: calibration constants
   * \param *pcp: calibration parameters
   * \result Rx, Ry, Rz, Tx, Ty, Tz (extrinsic aprameters)
   */
void noncoplanar_extrinsic_parameter_estimation(calibration_data *pcd,
												calibration_constants *pcc,
												camera_parameters *pcp);

// prototypes of CAL_TRAN.C

  /** converts from undistorted to distorted sensor coordinates
   * 
   * This routine converts from undistorted to distorted sensor coordinates.
   * The technique involves algebraically solving the cubic polynomial
   * Ru = Rd * (1 + kappa1 * Rd**2) using the Cardan method.
   *
   * Note: for kappa1 < 0 the distorted sensor plane extends out to a maximum
   *       barrel distortion radius of  Rd = sqrt (-1/(3 * kappa1)).
   *
   * \param cc: calibration constants
   * \param Xu: undistorted X coordinate
   * \param Yu: undistorted Y coordinate
   * \param *Xd: distorted X coordinate
   * \param *Yd: distorted Y coordinate
   * \result *Xd and *Yd distorted coordinates
   */
void undistorted_to_distorted_sensor_coord(calibration_constants cc,
										   double Xu,
										   double Yu,
										   double *Xd,
										   double *Yd);

  /** converts from distorted to undistorted sensor coordinates
   * 
   * This routine converts from distorted to undistorted sensor coordinates.
   * The technique involves algebraically solving the cubic polynomial
   * Ru = Rd * (1 + kappa1 * Rd**2) using the Cardan method.
   *
   * Note: for kappa1 < 0 the distorted sensor plane extends out to a maximum
   *       barrel distortion radius of  Rd = sqrt (-1/(3 * kappa1)).
   *
   * \param cc: calibration constants
   * \param Xd: distorted X coordinate
   * \param Yd: distorted Y coordinate  
   * \param *Xu: undistorted X coordinate
   * \param *Yu: undistorted Y coordinate
   * \result *Xu and *Yu undistorted coordinates
   */
void distorted_to_undistorted_sensor_coord(calibration_constants cc,
										   double Xd,
										   double Yd,
										   double* Xu,
										   double* Yu);

  /** converts from undistorted to distorted image coordinates
   * 
   * This routine converts from undistorted to distorted image coordinates.
   * The technique involves algebraically solving the cubic polynomial
   * Ru = Rd * (1 + kappa1 * Rd**2) using the Cardan method.
   *
   * Note: for kappa1 < 0 the distorted sensor plane extends out to a maximum
   *       barrel distortion radius of  Rd = sqrt (-1/(3 * kappa1)).
   *
   * \param cc: calibration constants
   * \param cp: calibration parameters
   * \param Xfu: undistorted X coordinate
   * \param Yfu: undistorted Y coordinate
   * \param *Xfd: distorted X coordinate
   * \param *Yfd: distorted Y coordinate
   * \result *Xfd and *Yfd distorted coordinates
   */
void undistorted_to_distorted_image_coord(calibration_constants cc,
										  camera_parameters cp,
										  double Xfu,
										  double Yfu,
										  double* Xfd,
										  double* Yfd);

  /** converts from distorted to undistorted image coordinates
   * 
   * This routine converts from distorted to undistorted image coordinates.
   * The technique involves algebraically solving the cubic polynomial
   * Ru = Rd * (1 + kappa1 * Rd**2) using the Cardan method.
   *
   * Note: for kappa1 < 0 the distorted sensor plane extends out to a maximum
   *       barrel distortion radius of  Rd = sqrt (-1/(3 * kappa1)).
   *
   * \param cc: calibration constants
   * \param cp: calibration parameters
   * \param Xfd: distorted X coordinate
   * \param Yfd: distorted Y coordinate  
   * \param *Xfu: undistorted X coordinate
   * \param *Yfu: undistorted Y coordinate
   * \result *Xfu and *Yfu undistorted coordinates
   */
void distorted_to_undistorted_image_coord(calibration_constants cc,
										  camera_parameters cp,
										  double Xfd,
										  double Yfd,
										  double* Xfu,
										  double* Yfu);

  /** converts from world to image coordinates
   * 
   * This routine takes the position of a point in world coordinates [mm]
   * and determines the position of its image in image coordinates [pix].
   *
   * \param cc: calibration constants
   * \param cp: calibration parameters
   * \param xw: world X coordinate
   * \param yw: world Y coordinate  
   * \param zw: world Z coordinate
   * \param *Xf: image X coordinate
   * \param *Yf: image Y coordinate
   * \result *Xf and *Yf image coordinates
   */
void world_coord_to_image_coord(calibration_constants cc,
								camera_parameters cp,
								double xw,
								double yw,
								double zw,
								double* Xf,
								double *Yf);

  /** converts from image to world coordinates
   * 
   * This routine performs an inverse perspective projection to determine
   * the position of a point in world coordinates that corresponds to a
   * given position in image coordinates. To constrain the inverse
   * projection to a single point the routine requires a Z world
   * coordinate for the point in addition to the X and Y image coordinates.
   *
   * \param cc: calibration constants
   * \param cp: calibration parameters
   * \param Xf: image X coordinate
   * \param Yf: image Y coordinate
   * \param zw: world Z coordinate
   * \param *xw: world X coordinate
   * \param *yw: world Y coordinate  
   * \result *xw and *yw world coordinates
   */
void image_coord_to_world_coord(calibration_constants cc,
								camera_parameters cp,
								double Xfd,
								double Yfd,
								double zw, 
								double* xw,
								double* yw);

  /** converts from world to camera coordinates
   * 
   * This routine takes the position of a point in world coordinates [mm]
   * and determines its position in camera coordinates [mm].
   *
   * \param cc: calibration constants
   * \param xw: world X coordinate
   * \param yw: world Y coordinate  
   * \param zw: world Z coordinate
   * \param *xc: camera X coordinate
   * \param *yc: camera Y coordinate
   * \param *zc: camera Z coordinate
   * \result *xc, *yc and *zc camera coordinates
   */
void world_coord_to_camera_coord(calibration_constants cc,
								 double xw,
								 double yw,
								 double zw,
								 double *xc,
								 double* yc,
								 double* zc);

  /** converts from world to camera coordinates
   * 
   * This routine takes the position of a point in camera coordinates [mm]
   * and determines its position in world coordinates [mm].
   *
   * \param cc: calibration constants
   * \param xc: camera X coordinate
   * \param yc: camera Y coordinate
   * \param zc: camera Z coordinate
   * \param *xw: world X coordinate
   * \param *yw: world Y coordinate  
   * \param *zw: world Z coordinate
   * \result *xw, *yw and *zw world coordinates
   */
void camera_coord_to_world_coord(calibration_constants cc,
								 double xc,
								 double yc,
								 double zc,
								 double* xw,
								 double* yw,
								 double* zw);

// prototypes of CAL_EVAL.C

  /** error in distorted image coordinates between the measured location and the image projected through the calibrated model
   * 
   * This routine calculates the mean, standard deviation, max, and sum-of-squared
   * error of the magnitude of the error, in distorted image coordinates, between
   * the measured location of a feature point in the image plane and the image of
   * the 3D feature point as projected through the calibrated model.
   * The calculation is for all of the points in the calibration data set.
   *
   * \param *pcd: calibration data
   * \param cc: calibration constants
   * \param cp: camera parameters
   * \param *mean: mean error
   * \param *stddev: standard deviation error
   * \param *max: max error
   * \param *sse: sum-of-squared error
   * \result *mean, *stddev, *max and *sse
   */
void distorted_image_plane_error_stats(calibration_data *pcd,
									   calibration_constants cc,
									   camera_parameters cp,
									   double* mean,
									   double* stddev,
									   double* max,
									   double* sse);

  /** error in undistorted image coordinates between the measured location and the image projected through the calibrated model
   * 
   * This routine calculates the mean, standard deviation, max, and sum-of-squared
   * error of the magnitude of the error, in undistorted image coordinates, between
   * the measured location of a feature point in the image plane and the image of
   * the 3D feature point as projected through the calibrated model.
   * The calculation is for all of the points in the calibration data set.
   *
   * \param *pcd: calibration data
   * \param cc: calibration constants
   * \param cp: camera parameters
   * \param *mean: mean error
   * \param *stddev: standard deviation error
   * \param *max: max error
   * \param *sse: sum-of-squared error
   * \result *mean, *stddev, *max and *sse
   */
void undistorted_image_plane_error_stats(calibration_data *pcd,
										 calibration_constants cc,
										 camera_parameters cp,
										 double* mean,
										 double* stddev,
										 double* max,
										 double* sse);

/*******************************************************************************\
* This routine calculates the mean, standard deviation, max, and sum-of-squared *
* error of the distance of closest approach (i.e. 3D error) between the point   *
* in object space and the line of sight formed by back projecting the measured  *
* 2D coordinates out through the camera model.                                  *
* The calculation is for all of the points in the calibration data set.         *
\*******************************************************************************/

  /** error of the distance of closest approach between a point in object space and the line of sight
   * 
   * This routine calculates the mean, standard deviation, max, and sum-of-squared
   * error of the distance of closest approach (i.e. 3D error) between the point
   * in object space and the line of sight formed by back projecting the measured
   * 2D coordinates out through the camera model.
   * The calculation is for all of the points in the calibration data set.
   *
   * \param *pcd: calibration data
   * \param cc: calibration constants
   * \param cp: camera parameters
   * \param *mean: mean error
   * \param *stddev: standard deviation error
   * \param *max: max error
   * \param *sse: sum-of-squared error
   * \result *mean, *stddev, *max and *sse
   */
void object_space_error_stats(calibration_data *pcd,
							  calibration_constants cc,
							  camera_parameters cp,
							  double* mean,
							  double* stddev,
							  double* max,
							  double* sse);

/*******************************************************************************\
* This routine performs an error measure proposed by Weng in IEEE PAMI,         *
* October 1992.                                                                 *
\*******************************************************************************/

  /** Weng error mesure
   * 
   * This routine performs an error measure proposed by Weng in IEEE PAMI,
   * October 1992. 
   *
   * \param *pcd: calibration data
   * \param cc: calibration constants
   * \param cp: camera parameters
   * \param *mean: mean error
   * \param *stddev: standard deviation error
   * \result *mean and *stddev
   */
void normalized_calibration_error (calibration_data *pcd, 
								   calibration_constants cc,
								   camera_parameters cp,
								   double* mean, 
								   double* stddev);

// prototypes of CAL_UTIL.C

 /** initialise camera parameters for Teli CS5130P + Matrox
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for Teli CS5130P + Matrox
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_teli_matrox_parms(camera_parameters *pcp);

 /** initialise camera parameters for Sony XC-999P + Matrox
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for Sony XC-999P + Matrox
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_sony_xc999p_matrox_parms(camera_parameters *pcp);

 /** initialise camera parameters for Sony DXC-750P + Matrox
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for Sony DXC-750P + Matrox
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_sony_dxc750p_matrox_parms(camera_parameters *pcp);

 /** initialise camera parameters for Photometrics Star I
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for Photometrics Star I
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_photometrics_parms(camera_parameters *pcp);

 /** initialise camera parameters for General Imaging MOS5300 + Matrox
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for General Imaging MOS5300 + Matrox
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_general_imaging_mos5300_matrox_parms(camera_parameters *pcp);

 /** initialise camera parameters for Panasonic GP-MF702 + Matrox
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for Panasonic GP-MF702 + Matrox
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_panasonic_gp_mf702_matrox_parms(camera_parameters *pcp);

 /** initialise camera parameters for Sony XC75 + Matrox
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for Sony XC75 + Matrox
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_sony_xc75_matrox_parms(camera_parameters *pcp);

 /** initialise camera parameters for Sony XC77 + Matrox
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for Sony XC77 + Matrox
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_sony_xc77_matrox_parms(camera_parameters *pcp);

 /** initialise camera parameters for Sony XC57 + Androx
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for Sony XC57 + Androx
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_sony_xc57_androx_parms(camera_parameters *pcp);

 /** initialise camera parameters for Canon Xap Shot
   * 
   * initialise Ncx, Nfx, dx, dy, dpx, dpy, Cx, Cy and sx
   * parameters for Canon Xap Shot
   *
   * \param *pcp: camera parameters
   * \result *pcp camera parameters
   */
void initialize_xapshot_matrox_parms(camera_parameters *pcp);

 /** solves for the roll, pitch and yaw angles for a given orthonormal rotation matrix
   * 
   * This routine solves for the roll, pitch and yaw angles (in radians)
   * for a given orthonormal rotation matrix (from Richard P. Paul,
   * Robot Manipulators: Mathematics, Programming and Control, p70). 
   * Note 1, should the rotation matrix not be orthonormal these will not
   * be the "best fit" roll, pitch and yaw angles. 
   * Note 2, there are actually two possible solutions for the matrix. 
   * The second solution can be found by adding 180 degrees to Rz before
   * Ry and Rx are calculated.    
   *
   * \param *pcc: calibration constants (
   * \result Rx,Ry and Rz
   */
void solve_RPY_transform(calibration_constants *pcc);

 /** creates the rotation matrix from the roll, pitch and yaw angles
   * 
   * This routine simply takes the roll, pitch and yaw angles and fills in
   * the rotation matrix elements r1-r9.
   *
   * \param *pcc: calibration constants
   * \result r1, r2, r3, r4, r5, r6, r7, r8 and r9
   */
void apply_RPY_transform(calibration_constants *pcc);

// IO functions
void load_cd_data(FILE* fp,
				  calibration_data *pcd);

void dump_cd_data(FILE* fp,
				  calibration_data *pcd);

void load_cp_cc_data(FILE* fp,
					 camera_parameters *cp,
					 calibration_constants *cc);

void dump_cp_cc_data(FILE* fp,
					 camera_parameters *cp,
					 calibration_constants *cc);

void print_cp_cc_data(FILE* fp,
					  camera_parameters *cp,
					  calibration_constants *cc);

void sprint_cp_cc_data(char* str,
					   camera_parameters *cp,
					   calibration_constants *cc);

#ifdef __cplusplus
}
#endif

/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/**
	This file contains the prototipes of the routines for calibrating 
	Tsai's 11 parameter camera model.
	The camera model is based on the pin hole model of 3D-2D perspective  
	projection with 1st order radial lens distortion.  The model consists of
	internal (also called intrinsic or interior) camera parameters:
                                                                             
		f       - effective focal length of the pin hole camera                 
        kappa1  - 1st order radial lens distortion coefficient                  
        Cx, Cy  - coordinates of center of radial lens distortion               
				(also used as the piercing point of the camera coordinate 
				frame's Z axis with the camera's sensor plane)               
        sx      - uncertainty factor for scale of horizontal scanline           
                                                                               
	and 6 external (also called extrinsic or exterior) camera parameters:         
                                                                               
        Rx, Ry, Rz, Tx, Ty, Tz  - rotational and translational components of    
                                  the transform between the world's coordinate  
                                  frame and the camera's coordinate frame.      
                                                                                
	Data for model calibration consists of the (x,y,z) world coordinates of a     
	feature point (in mm) and the corresponding coordinates (Xf,Yf) (in pixels)   
	of the feature point in the image.  Two types of calibration are available:   
                                                                               
        coplanar     - all of the calibration points lie in a single plane      
        non-coplanar - the calibration points do not lie in a single plane      
                                                                               
	This file contains routines for two levels of calibration.  The first level   
	of calibration is a direct implementation of Tsai's algorithm in which only   
	the f, Tz and kappa1 parameters are optimized for.  The routines are:         
                                                                                
        coplanar_calibration ()                                                 
        noncoplanar_calibration ()                                              
                                                                                
	The second level of calibration optimizes for everything.  This level is      
	very slow but provides the most accurate calibration.  The routines are:      
                                                                                
        coplanar_calibration_with_full_optimization ()                          
        noncoplanar_calibration_with_full_optimization ()                       
	
	The following two routines:                                             
                                                                              
        coplanar_extrinsic_parameter_estimation ()                                                                                                        
        noncoplanar_extrinsic_parameter_estimation ()                           
                                                                               
	which are used respectively for coplanar and non-coplanar calibration data
	estimate only the extrinsic parameters of the camera
                                                                               
	Initial estimates for the extrinsic camera parameters are determined using a  
	modification of the first stage of Tsai's algorithm.  These estimates are     
	then refined using iterative non-linear optimization.                         

	The following routines are used for measuring the accuracy of a calibrated
	camera model:          

        distorted_image_plane_error_stats ()                                    
        undistorted_image_plane_error_stats ()                                  
        object_space_error_stats ()                                             
        normalized_calibration_error ()                                         

	Routines are also provided for transforming between the different coordinate
	systems used in Tsai's perspective projection camera model.  
	The routines are:                                                                         
 
        world_coord_to_image_coord ()                                           
        image_coord_to_world_coord ()                                           
        world_coord_to_camera_coord ()                                          
        camera_coord_to_world_coord ()                                          
        distorted_to_undistorted_sensor_coord ()                                
        undistorted_to_distorted_sensor_coord ()                                
        distorted_to_undistorted_image_coord ()                                 
        undistorted_to_distorted_image_coord ()                                  

	Routines are also provided for initializing camera parameter variables        
	for ten of our camera/frame grabber systems. These routines are:            
		
		initialize_teli_matrox_parms ()
		initialize_sony_xc999p_matrox_parms ()
		initialize_sony_dxc750p_matrox_parms ()
        initialize_photometrics_parms ()                                        
        initialize_general_imaging_mos5300_matrox_parms ()                      
        initialize_panasonic_gp_mf702_matrox_parms ()                           
        initialize_sony_xc75_matrox_parms ()                                    
        initialize_sony_xc77_matrox_parms ()                                    
        initialize_sony_xc57_androx_parms ()                                    
		initialize_xapshot_matrox_parms ()

	External routines
	-----------------
                                                                               
	Nonlinear optimization for these camera calibration routines is performed     
	by the MINPACK lmdif subroutine.  lmdif uses a modified Levenberg-Marquardt   
	with a jacobian calculated by a forward-difference approximation.             
	The MINPACK FORTRAN routines were translated into C generated using f2c.      
                                                                               
	Matrix operations (inversions, multiplications, etc.) are also provided by    
	external routines.                                                            
                                                                               
                                                                               
	Extra notes                                                                   
	-----------                                                                   
                                                                              
	An explanation of the basic algorithms and description of the variables       
	can be found in several publications, including:                              
                                                                              
	"An Efficient and Accurate Camera Calibration Technique for 3D Machine        
	Vision", Roger Y. Tsai, Proceedings of IEEE Conference on Computer Vision    
	and Pattern Recognition, Miami Beach, FL, 1986, pages 364-374.               
                                                                               
	and                                                                          
                                                                              
	"A versatile Camera Calibration Technique for High-Accuracy 3D Machine        
	Vision Metrology Using Off-the-Shelf TV Cameras and Lenses", Roger Y. Tsai,  
	IEEE Journal of Robotics and Automation, Vol. RA-3, No. 4, August 1987,      
	pages 323-344.
*/
#endif
