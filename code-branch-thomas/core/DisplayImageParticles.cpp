#include "DisplayImageParticles.h"
#define THISCLASS DisplayImageParticles

THISCLASS::DisplayImageParticles(const std::string &name, const std::string &displayname):
		DisplayImage(name, displayname),
		mImage(0), mTopLeft(cvPoint2D32f(0, 0)), mBottomRight(cvPoint2D32f(16, 9)), mParticles(0)  {

}

THISCLASS::~DisplayImageParticles() {
}

IplImage *THISCLASS::CreateImage(int maxwidth, int maxheight) {
	IplImage* img;
	if (mImage==0) {
		// Calculate the maximum size
		CvSize size=CalculateMaxSize(mImage->width, mImage->height, maxwidth, maxheight);
	
		// Create a resized copy of the image
		img=cvCreateImage(size, IPL_DEPTH_8U, 3);
		cvResize(mImage, img);
		if (mParticles==0) {return img;}
	} else {
		// Calculate the maximum size
		CvSize size=CalculateMaxSize((mBottomRight.x-mTopLeft.x)/(mBottomRight.y-mTopLeft.y), maxwidth, maxheight);

		img=cvCreateImage(size, IPL_DEPTH_8U, 3);
		memset(img->imageData, 255, img->imageSize);
	}

	// Draw particles
	DataStructureParticles::tParticleVector::iterator it=mParticles->begin();
	while (it!=mParticles->end()) {
		int x=(int)floor(it->mCenter.x+0.5);
		int y=(int)floor(it->mCenter.y+0.5);
		cvRectangle(img, cvPoint(x-2, y-2), cvPoint(x+2, y+2), cvScalar(192,0,0), 1);
	}
	return img;
}
