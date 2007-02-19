#include "DisplayImageParticles.h"
#define THISCLASS DisplayImageParticles

DisplayImageParticles(const std::string &name, const std::string &displayname):
		DisplayImage(name, displayname),
		mImage(0), mTopLeft(CvPoint(0, 0)), mBottomRight(CvPoint(16, 9)), mParticles(0)  {

}

~DisplayImageParticles() {
}

IplImage *THISCLASS::CreateImage(int maxwidth, int maxheight) {
	if (mImage==0) {
		// Calculate the maximum size
		CvSize size=CalculateMaxSize(mImage->width, mImage->height, maxwidth, maxheight);
	
		// Create a resized copy of the image
		IplImage* img=cvCreateImage(size, IPL_DEPTH_8U, 3);
		cvResize(mImage, img);
		if (mNewParticles==0) {return img;}
	} else {
		// Calculate the maximum size
		CvSize size=CalculateMaxSize((mBottomRight.x-mTopLeft.x)/(mBottomRight.y-mTopLeft.y), maxwidth, maxheight);

		IplImage* img=cvCreateImage(size, IPL_DEPTH_8U, 3);
		memset(img->imageData, 255, img->imageSize);
	}

	// Draw particles
	CvPoint curve1[1];
	DataStructureParticles::tParticleVector::iterator it=mNewParticles.begin();
	while (it!=mNewParticles.end()) {
		double x=it->mCenter.x;
		double y=it->mCenter.y;
		cvRectangle(img, cvPoint(x-2, y-2), cvPoint(x+2, y+2), cvScalar(192,0,0), 1);
	}
	return img;
}
