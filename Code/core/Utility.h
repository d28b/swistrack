#ifndef HEADER_Utility
#define HEADER_Utility

#include <opencv2/opencv.hpp>
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
	static double SquareDistance(cv::Point2f p1, cv::Point2f p2);
	static double Distance(cv::Point2f p1, cv::Point2f p2) ;

	static wxString toMillisString(wxDateTime ts);
	static double toMillis(wxDateTime ts);

	//! Creates a rectangle from center, width and height.
	static cv::Rect RectByCenter(int center_x, int center_xy, int width, int height);
};

#endif
