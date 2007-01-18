#include "DisplayImageStandard.h"
#define THISCLASS DisplayImageStandard

IplImage *THISCLASS::CreateImage(int maxwidth, int maxheight) {
	if (mNewImage==0) {return 0;}

	// Calculate the maximum size
	CvSize size=CalculateMaxSize(mNewImage->width, mNewImage->height, maxwidth, maxheight);

	// Create a resized copy of the image
	IplImage* img=cvCreateImage(size, IPL_DEPTH_8U, 3);
	cvResize(mNewImage, img);
	return img;
}
