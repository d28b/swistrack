#include "Utility.h"
#define THISCLASS Utility
#include <stdio.h>
#include <math.h>

double THISCLASS::SquareDistance(CvPoint2D32f p1, CvPoint2D32f p2) {
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	return dx*dx + dy*dy;
}

double THISCLASS::Distance(CvPoint2D32f p1, CvPoint2D32f p2) 
{
  return pow(SquareDistance(p1, p2), 0.5);
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

/*
 // untested. 
  CvBox2D THISCLASS::RectToBox(CvRect rect) {

  CvBox2D out;
  out.center = cvPoint2D32f(rect.x + width / 2.0,
			    rect.y + height / 2.0);
  out.size = cvPoint2D32f(rect.width, rect.height);
  out.angle = 0;
  return out;
}
*/

IplImage * THISCLASS::DrawHistogram1D(CvHistogram * hist, IplImage * hist_image) {

  if (hist_image == NULL) {
    hist_image = cvCreateImage(cvSize(320,200), 8, 1);
  }
  int sizes[CV_MAX_DIM];
  int dims = cvGetDims( hist->bins, sizes);
  assert(dims == 1);
  int hist_size = sizes[0];
  cvSet( hist_image, cvScalarAll(255), 0 );
  int bin_w = cvRound((double)hist_image->width/hist_size);
  for(int i = 0; i < hist_size; i++ ) {
    cvRectangle( hist_image, cvPoint(i*bin_w, hist_image->height),
		 cvPoint((i+1)*bin_w, hist_image->height - cvRound(cvGetReal1D(hist->bins,i))),
		 cvScalarAll(0), -1, 8, 0 );
  }
  return hist_image;
}

double THISCLASS::toMillis(wxDateTime ts) 
{
  return ts.GetTicks() * 1000ULL + ts.GetMillisecond();
}
wxString THISCLASS::toMillisString(wxDateTime ts) {
  wxString millis;
  millis << ts.GetMillisecond();
  millis.Pad(3 - millis.Length(), '0', false);

  wxString out;
  out << ts.GetTicks();
  out << millis;
  return out;
}
