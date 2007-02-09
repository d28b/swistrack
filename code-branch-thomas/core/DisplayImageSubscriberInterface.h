#ifndef HEADER_DisplayImageSubscriberInterface
#define HEADER_DisplayImageSubscriberInterface

class DisplayImageSubscriberInterface;

#include <string>
#include "DisplayImage.h"

//! This is the interface for subscribers to DisplayImage classes.
class DisplayImageSubscriberInterface {

public:
	//! Constructor.
	DisplayImageSubscriberInterface() {}
	//! Destructor.
	~DisplayImageSubscriberInterface() {}

	//! Called when subscribing to a DisplayImage.
	virtual void OnDisplayImageSubscribe(DisplayImage *di) = 0;
	//! Called when a display image changes.
	virtual void OnDisplayImageChanged(DisplayImage *di) = 0;
	//! Called when unsubscribing from a DisplayImage.
	virtual void OnDisplayImageUnsubscribe(DisplayImage *di) = 0;
};

#endif