#include "Display.h"
#define THISCLASS Display

#include <algorithm>

THISCLASS::Display(const wxString &name, const wxString &displayname):
		mComponent(0), mName(name), mDisplayName(displayname), mSubscribers(), mStepCounter(-1),
		mFrameNumber(-1), mFramesCount(-1), mTime(wxDateTime::UNow()), mAnnotation(), mErrors(),
		mTopLeft(cvPoint(0, 0)), mSize(cvSize(0, 0)),
		mMainImage(0), mMaskImage(0), mParticles(), mTrajectories(false),
		mActive(false), mChanged(false) {

}

THISCLASS::~Display() {
	// Unsubscribe all interfaces
	while (! mSubscribers.empty()) {
		Unsubscribe(*(mSubscribers.begin()));
	}
}

void THISCLASS::Subscribe(DisplaySubscriberInterface *disi) {
	// Return if this interface is in the subscriber list already
	tSubscriberList::iterator it = find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it != mSubscribers.end()) {
		return;
	}

	// Otherwise, add the interface
	mSubscribers.push_back(disi);
	disi->OnDisplaySubscribe(this);
}

void THISCLASS::Unsubscribe(DisplaySubscriberInterface *disi) {
	// Look up the interface and return if not found
	tSubscriberList::iterator it = find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it == mSubscribers.end()) {
		return;
	}

	// Remove the interface
	disi->OnDisplayUnsubscribe(this);
	mSubscribers.erase(it);
}

void THISCLASS::OnBeforeStep() {
	// By default, the display is inactive
	mActive = false;

	// Call the subscribers to check if the want an image
	tSubscriberList::iterator it = mSubscribers.begin();
	while (it != mSubscribers.end()) {
		(*it)->OnDisplayBeforeStep(this);
		it++;
	}
}

void THISCLASS::OnAfterStep() {
	// Proceed only if the display changed (this flag is set by the DisplayEditor)
	if (! mChanged) {
		return;
	}

	// Otherwise, let the subscribers know that we have a new image
	tSubscriberList::iterator it = mSubscribers.begin();
	while (it != mSubscribers.end()) {
		(*it)->OnDisplayChanged(this);
		it++;
	}

	mChanged = false;
}
