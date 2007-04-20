#include "Display.h"
#define THISCLASS Display

#include <algorithm>
#include <sstream>
#include <cmath>
#define PI 3.14159265358979

THISCLASS::Display(const std::string &name, const std::string &displayname):
		mName(name), mDisplayName(displayname) {

}

THISCLASS::~Display() {
	// Unsubscribe all interfaces
	while (! mSubscribers.empty()) {
		Unsubscribe(*(mSubscribers.begin()));
	}
}

void THISCLASS::Subscribe(DisplaySubscriberInterface *disi) {
	// Return if this interface is in the subscriber list already
	tSubscriberList::iterator it=find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it!=mSubscribers.end()) {return;}

	// Otherwise, add the interface
	mSubscribers.push_back(disi);
	disi->OnDisplaySubscribe(this);
}

void THISCLASS::Unsubscribe(DisplaySubscriberInterface *disi) {
	// Look up the interface and return if not found
	tSubscriberList::iterator it=find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it==mSubscribers.end()) {return;}

	// Remove the interface
	disi->OnDisplayUnsubscribe(this);
	mSubscribers.erase(it);
}

void THISCLASS::OnChanged() {
	tSubscriberList::iterator it=mSubscribers.begin();
	while (it!=mSubscribers.end()) {
		(*it)->OnDisplayChanged(this);
		it++;
	}
}

CvSize THISCLASS::CalculateSize(double scalingfactor) {
	double w=(double)(mImage->width)*scalingfactor;
	double h=(double)(mImage->height)*scalingfactor;
	return cvSize((int)floor(w+0.5), (int)floor(h+0.5));
}

IplImage *THISCLASS::CreateImage(double scalingfactor) {
	// Return the cached image if possible
	if (mCachedViewScalingFactor==scalingfactor) {
		return mCachedViewImage;
	}

	// Delete the old cached view image
	cvReleaseImage(&mCachedViewImage);

	// Create a resized copy of the image
	CvSize size=CalculateSize(scalingfactor);
	mCachedViewImage=cvCreateImage(size, IPL_DEPTH_8U, mImage->nChannels);
	if (mImage) {
		cvResize(mImage, mCachedViewImage);
	} else {
		memset(mCachedViewImage->imageData, 255, mCachedViewImage->imageSize);
	}

	// Initialize font
	if (mParticles) {
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4);

		// Draw particles
		DataStructureParticles::tParticleVector::iterator it=mParticles->begin();
		while (it!=mParticles->end()) {
			int x=(int)floor(it->mCenter.x*scalingfactor+0.5);
			int y=(int)floor(it->mCenter.y*scalingfactor+0.5);
			cvRectangle(mCachedViewImage, cvPoint(x-2, y-2), cvPoint(x+2, y+2), cvScalar(192, 0, 0), 1);

			float c=cosf(it->mOrientation)*20+(float)0.5;
			float s=sinf(it->mOrientation)*20+(float)0.5;
			cvLine(mCachedViewImage, cvPoint(x, y), cvPoint(x+(int)floorf(c), y+(int)floorf(s)), cvScalar(192, 0, 0), 1);

			std::ostringstream oss;
			oss << it->mID << " (" << it->mOrientation/PI*180. << ") [" << it->mIDCovariance << "]";
			cvPutText(mCachedViewImage, oss.str().c_str(), cvPoint(x+12, y+10), &font, cvScalar(255, 0, 0));
			it++;
		}
	}

	return mCachedViewImage;
}


CvSize THISCLASS::CalculateMaxSize(int srcwidth, int srcheight, int maxwidth, int maxheight) {
	// Find out the smallest ratio such that the image fits into [maxwidth, maxheight]
	double rw=(double)(maxwidth)/(double)(srcwidth);
	double rh=(double)(maxheight)/(double)(srcheight);
	double r=1;
	if (r>rw) {r=rw;}
	if (r>rh) {r=rh;}

	// Make width a multiple of 4 (because of byte alignment)
	double w=floor((double)srcwidth*r/4)*4;
	double h=floor((double)srcheight/(double)srcwidth*w+0.5);
	return cvSize((int)w, (int)h);
}

CvSize THISCLASS::CalculateMaxSize(double ratio, int maxwidth, int maxheight) {
	// Find out the largest width we can use
	double w=(double)(maxheight)*ratio;
	if (w>maxwidth) {w=maxwidth;}

	// Make width a multiple of 4 (because of byte alignment)
	w=floor(w/4)*4;
	double h=floor(w/ratio+0.5);
	return cvSize((int)w, (int)h);
}

void THISCLASS::ReleaseImage(IplImage *image) {
	cvReleaseImage(&image);
}
