#include "Display.h"
#define THISCLASS Display

#include <algorithm>

THISCLASS::Display(const wxString & name, const wxString & displayname):
	mComponent(0), mName(name), mDisplayName(displayname), mSubscribers(), mStepCounter(-1),
	mFrameNumber(-1), mFramesCount(-1), mTime(wxDateTime::UNow()), mAnnotation(), mErrors(),
	mTopLeft(0, 0), mSize(0, 0),
	mMainImage(), mMaskImage(), mParticles(), mTrajectories(false),
	mActive(false), mChanged(false) {

}

THISCLASS::~Display() {
	// Unsubscribe all interfaces
	while (! mSubscribers.empty())
		Unsubscribe(*(mSubscribers.begin()));
}

void THISCLASS::Subscribe(DisplaySubscriberInterface * disi) {
	// Return if this interface is in the subscriber list already
	tSubscriberList::iterator it = find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it != mSubscribers.end()) return;

	// Otherwise, add the interface
	mSubscribers.push_back(disi);
	disi->OnDisplaySubscribe(this);
}

void THISCLASS::Unsubscribe(DisplaySubscriberInterface * disi) {
	// Look up the interface and return if not found
	tSubscriberList::iterator it = find(mSubscribers.begin(), mSubscribers.end(), disi);
	if (it == mSubscribers.end()) return;

	// Remove the interface
	disi->OnDisplayUnsubscribe(this);
	mSubscribers.erase(it);
}

void THISCLASS::OnBeforeStep() {
	// By default, the display is inactive
	mActive = false;

	// Call the subscribers to check if the want an image
	for (auto subscriber : mSubscribers)
		subscriber->OnDisplayBeforeStep(this);
}

void THISCLASS::OnAfterStep() {
	// Proceed only if the display changed (this flag is set by the DisplayEditor)
	if (! mChanged) return;

	// Otherwise, let the subscribers know that we have a new image
	for (auto subscriber : mSubscribers)
		subscriber->OnDisplayChanged(this);

	mChanged = false;
}
