#include "DisplayImage.h"
#define THISCLASS DisplayImage

CvSize THISCLASS::CalculateMaxSize(int srcwidth, int srcheight, int maxwidth, int maxheight) {
	double rw=(double)(maxwidth)/(double)(srcwidth);
	double rh=(double)(maxheight)/(double)(srcheight);
	double r=1;
	if (r>rw) {r=rw;}
	if (r>rh) {r=rh;}
	return cvSize((int)floor(srcwidth*r+0.5), (int)floor(srcheight*r+0.5));
}

void THISCLASS::ReleaseImage(IplImage *image) {
	cvReleaseImage(&image);
}
