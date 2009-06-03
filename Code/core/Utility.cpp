#include "Utility.h"
#define THISCLASS Utility

double THISCLASS::SquareDistance(CvPoint2D32f p1, CvPoint2D32f p2) {
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	return dx*dx + dy*dy;
}

CvRect THISCLASS::RectByCenter(int centerX, int centerY, int width, int height) {
	int lowerLeftx = centerX - width / 2;
	int lowerLefty = centerY - width / 2;
	return cvRect(lowerLeftx, lowerLefty, width, height);
}


/*CvBox2D THISCLASS::RectToBox(CvRect rect) {

  CvBox2D out;
  out.center = cvPoint2D32f(rect.x + width / 2.0,
			    rect.y + height / 2.0);
  out.size = cvPoint2D32f(rect.width, rect.height);
  out.angle = 0;
  return out;
}
*/
