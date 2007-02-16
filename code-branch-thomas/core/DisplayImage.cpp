#include "DisplayImage.h"
#define THISCLASS DisplayImage

#include <algorithm>

void THISCLASS::Subscribe(DisplayImageSubscriberInterface *disi) {
	mSubscribers.push_back(disi);
}

void THISCLASS::Unsubscribe(DisplayImageSubscriberInterface *disi) {
	tSubscriberList::iterator it=find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it==mSubscribers.end()) {return;}
	mSubscribers.erase(it);
}

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
