#include "DisplayImage.h"
#define THISCLASS DisplayImage

#include <algorithm>

THISCLASS::DisplayImage(const std::string &name, const std::string &displayname):
		mName(name), mDisplayName(displayname) {

}

THISCLASS::~DisplayImage() {
	// Unsubscribe all interfaces
	while (! mSubscribers.empty()) {
		Unsubscribe(*(mSubscribers.begin()));
	}
}

void THISCLASS::Subscribe(DisplayImageSubscriberInterface *disi) {
	// Return if this interface is in the subscriber list already
	tSubscriberList::iterator it=find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it!=mSubscribers.end()) {return;}

	// Otherwise, add the interface
	mSubscribers.push_back(disi);
	disi->OnDisplayImageSubscribe(this);
}

void THISCLASS::Unsubscribe(DisplayImageSubscriberInterface *disi) {
	// Look up the interface and return if not found
	tSubscriberList::iterator it=find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it==mSubscribers.end()) {return;}

	// Remove the interface
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
