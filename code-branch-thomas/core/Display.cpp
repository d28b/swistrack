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



// OLD
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

// OLD
CvSize THISCLASS::CalculateMaxSize(double ratio, int maxwidth, int maxheight) {
	// Find out the largest width we can use
	double w=(double)(maxheight)*ratio;
	if (w>maxwidth) {w=maxwidth;}

	// Make width a multiple of 4 (because of byte alignment)
	w=floor(w/4)*4;
	double h=floor(w/ratio+0.5);
	return cvSize((int)w, (int)h);
}

// OLD
void THISCLASS::ReleaseImage(IplImage *image) {
	cvReleaseImage(&image);
}
