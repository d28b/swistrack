#include "DisplayImageStandard.h"
#define THISCLASS DisplayImageStandard

IplImage *THISCLASS::CreateImage(int maxwidth, int maxheight) {
	if (mImage==0) {return 0;}

	// Calculate the maximum size
	CvSize size=CalculateMaxSize(mImage->width, mImage->height, maxwidth, maxheight);

	// Create a resized copy of the image
	IplImage* img=cvCreateImage(size, IPL_DEPTH_8U, 3);
	cvResize(mImage, img);
	return img;
}
