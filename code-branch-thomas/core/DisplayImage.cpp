#include "DisplayImage.h"
#define THISCLASS DisplayImage

#include <algorithm>

void THISCLASS::Subscribe(DisplayImageSubscriberInterface *disi) {
	mSubscribers.push_back(disi);
	disi->OnDisplayImageSubscribe(this);
}

void THISCLASS::Unsubscribe(DisplayImageSubscriberInterface *disi) {
	tSubscriberList::iterator it=find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it==mSubscribers.end()) {return;}
	disi->OnDisplayImageUnsubscribe(this);
	mSubscribers.erase(it);
}

void THISCLASS::OnChanged() {
	tSubscriberList::iterator it=mSubscribers.begin();
	while (it!=mSubscribers.end()) {
		(*it)->OnDisplayImageChanged(this);
		it++;
	}
}

CvSize THISCLASS::CalculateMaxSize(int srcwidth, int srcheight, int maxwidth, int maxheight) {
	double rw=(double)(maxwidth)/(double)(srcwidth);
	double rh=(double)(maxheight)/(double)(srcheight);
	double r=1;
	if (r>rw) {r=rw;}
	if (r>rh) {r=rh;}
	return cvSize((int)floor(srcwidth*r+0.5), (int)floor(srcheight*r+0.5));
}

CvSize THISCLASS::CalculateMaxSize(double ratio, int maxwidth, int maxheight) {
	double width=(double)(maxheight)*ratio;
	if (width<maxwidth) {
		return cvSize((int)floor(width+0.5), maxheight);
	}

	double height=(double)(maxwidth)/ratio;
	return cvSize(maxwidth, (int)floor(height+0.5));
}

void THISCLASS::ReleaseImage(IplImage *image) {
	cvReleaseImage(&image);
}
