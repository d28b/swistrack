#ifndef HEADER_Utility
#define HEADER_Utility

#include <cv.h>
#include <wx/string.h>
#include <wx/datetime.h>

//! General utility functions.
/*!
	The methods of this class are used by a number of other classes.
*/
class Utility {

public:
	//! Constructor.
	Utility() {}
	//! Destructor.
	~Utility() {}

	//! Calculates the square distance.
	static double SquareDistance(CvPoint2D32f p1, CvPoint2D32f p2);
	static double Distance(CvPoint2D32f p1, CvPoint2D32f p2) ;

	static wxString toMillisString(wxDateTime ts);
	static double toMillis(wxDateTime ts);

	//! Creates a rectangle from center, width and height.
	static CvRect RectByCenter(int center_x, int center_xy, int width, int height);
	static CvMat * IpImageToCvMat(IplImage * image) {
	  CvMat * mat = (CvMat *) cvAlloc(sizeof(CvMat));
	  mat = cvGetMat(image, mat);
	  return mat;
	}

	static void IntegrateHistogram(CvHistogram * dest, CvHistogram * newSample);	

	/**
	 * Draws the histogram in the image.
	 * If target is null, creates a new image, otherwise draws in target.
	 * Returns the image that it created, or that was passed in. 
	 */
	static IplImage * DrawHistogram1D(CvHistogram * histogram, IplImage * hist_image=NULL);
};

#endif
