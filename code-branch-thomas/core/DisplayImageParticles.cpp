#include "DisplayImageParticles.h"
#define THISCLASS DisplayImageParticles

#include <sstream>
#include <highgui.h>

THISCLASS::DisplayImageParticles(const std::string &name, const std::string &displayname):
		DisplayImage(name, displayname),
		mImage(0), mTopLeft(cvPoint2D32f(0, 0)), mBottomRight(cvPoint2D32f(16, 9)), mParticles(0)  {

}

THISCLASS::~DisplayImageParticles() {
}

IplImage *THISCLASS::CreateImage(int maxwidth, int maxheight) {
	IplImage* img;
	if (mImage) {
		// Calculate the maximum size
		CvSize size=CalculateMaxSize(mImage->width, mImage->height, maxwidth, maxheight);
	
		// Create a resized copy of the image
		img=cvCreateImage(size, IPL_DEPTH_8U, mImage->nChannels);
		cvResize(mImage, img);
		if (mParticles==0) {return img;}
	} else {
		// Calculate the maximum size
		CvSize size=CalculateMaxSize((mBottomRight.x-mTopLeft.x)/(mBottomRight.y-mTopLeft.y), maxwidth, maxheight);

		img=cvCreateImage(size, IPL_DEPTH_8U, 3);
		memset(img->imageData, 255, img->imageSize);
	}

	// Initialize font
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4);

	// Draw particles
	double scaling=(double)img->width / (mBottomRight.x-mTopLeft.x);
	DataStructureParticles::tParticleVector::iterator it=mParticles->begin();
	while (it!=mParticles->end()) {
		int x=(int)floor(it->mCenter.x*scaling+0.5);
		int y=(int)floor(it->mCenter.y*scaling+0.5);
		cvRectangle(img, cvPoint(x-2, y-2), cvPoint(x+2, y+2), cvScalar(192, 0, 0), 1);

		std::ostringstream oss;
		oss << "(" << it->mID << ")";
		cvPutText(img, oss.str().c_str(), cvPoint(x+12, y+10), &font, cvScalar(255, 0, 0));
		it++;
	}
	return img;
}
