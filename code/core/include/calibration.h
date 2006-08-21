#ifndef calibration_h        
#define calibration_h

#include "highgui.h"
#include "libgeom.h"


using namespace std;


/** \class Calibration
* \brief This class provides a transformation for camera calibration
*
* The non-linear transformation from image pixels to world coordinates is approximated by a 
* second-order, non-symmetric polynomial of the form
*
* \f[x = a_{11}u^2+a_{12}v^2+a_{13}u+a_{14}v+a_{15}uv\f]
* \f[y = a_{21}u^2+a_{22}v^2+a_{23}u+a_{24}v+a_{25}uv\f]
*
* with u, v the image pixels, and x, y the coordinates in world space (meters).
* The ten parameters of this-very simple model-are estimated in a least-square fashion from a 
* sufficiently large number of coordinate pairs. Thus, the software has to be provided with a number 
* of image points which positions in the real world are known-for instance by using a pattern of known 
* dimensions that is presented to the software as a bitmap file. Hereby, increasing the number of pairs 
* used in the estimation process over the minimum of ten moderates the effect of noisy measurements during calibration.
* Also, one observes that the precision of the approximation and hence the precision of the prediction 
* is higher in the neighborhood of the training points. By that, we are able to bias the region within 
* the camera frame where we want to minimize the prediction area. 
*/
class Calibration
{

private:

  /** \brief Blobs */
  VisBlobSet spots;
  /** \brief Distortion matrix*/
  CvMat* distortion32f;
  /** \brief Camera matrix*/
  CvMat* cameraMatrix32f;
  /** \brief Calibration matrix */
  CvMat* transformationMatrix32f;

  CvPoint2D32f target,xaxis, yaxis; // coordinates of target
  CvPoint2D32f dxy; // displacement of target from center
  CvPoint2D32f meani,meano,stdi,stdo; // mean and standard deviation of training data

  double alpha; // rotation of target vs. camera frame

 
  
  int FindPoints(CvPoint2D32f* imagePoints32f, CvPoint3D32f* objectPoints32f,VisBlobSet& blobs, int cw, int ch,double wx,double wy,IplImage* img=0);
  int FindRoundPoints(CvPoint2D32f* imagePoints32f, CvPoint3D32f* objectPoints32f,VisBlobSet& blobs, int h, int* nrofpoints, double vd);
  IplImage* bgimg;
  IplImage* calibimg; 

  bool CalibrateTarget();

public:
	void SaveError(int n,CvPoint2D32f* u,CvPoint3D32f* x,char* fname);

	Calibration(char* _bgfname, char* _calfname);

	~Calibration();

  bool CalibratePattern(int cw, int ch,double wx, double wy);
  bool CalibrateRoundPattern(int h, int* nrofpoints, double vd); // h=number of circles, nrofpoints=array(h) with the number of points on each circle, vd=distance between rings

  bool CalibrateCenter();	

  double GetArenaRadius();



  void TestCalibration(char* testpattern,int cw, int ch,double wx, double wy);
  void TestRoundCalibration(char* testpattern,int h, int* nrofpoints, double vd);
 
  CvPoint2D32f ImageToWorld(CvPoint2D32f p, int transform=1);

  CvPoint2D32f WorldToImage(CvPoint2D32f p);

  IplImage* mask;

  CvPoint2D32f  center;
  CvPoint2D32f* arenaboundary;
  char* bgfname;
  char* calfname;

	
};






/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/**

 \todo An Empty() function.
 
 \test See test0() in bidon.cpp

*/
#endif
