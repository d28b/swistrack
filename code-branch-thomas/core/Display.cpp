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
