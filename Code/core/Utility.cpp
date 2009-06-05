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

void THISCLASS::IntegrateHistogram(CvHistogram * dest, CvHistogram * newSample) {
  int size1[CV_MAX_DIM], size2[CV_MAX_DIM], total = 1;

  int dims1 = cvGetDims( dest->bins, size1 );
  int dims2 = cvGetDims( newSample->bins, size2 );
  assert(dims1 == dims2);
  for(int i = 0; i < dims1; i++ ) {
    assert(size1[i] == size2[i]);
    total *= size1[i];
  }
  
  union { float* fl; uchar* ptr; } v;
  v.fl = 0;
  float *destBins, *newSampleBins;
  
  cvGetRawData(dest->bins, &v.ptr);
  destBins = v.fl;

  cvGetRawData(newSample->bins, &v.ptr);
  newSampleBins = v.fl;

  for(int i = 0; i < total; i++ ) {
    destBins[i] += newSampleBins[i];
  }
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


