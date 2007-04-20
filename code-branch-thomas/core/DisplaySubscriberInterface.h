#ifndef HEADER_DisplaySubscriberInterface
#define HEADER_DisplaySubscriberInterface

class DisplaySubscriberInterface;

#include <string>
#include "Display.h"

//! This is the interface for subscribers to Display classes.
class DisplaySubscriberInterface {

public:
	//! Constructor.
	DisplaySubscriberInterface() {}
	//! Destructor.
	~DisplaySubscriberInterface() {}

	//! Called when subscribing to a Display.
	virtual void OnDisplaySubscribe(Display *di) = 0;
	//! Called when a display changes.
	virtual void OnDisplayChanged(Display *di) = 0;
	//! Called when unsubscribing from a Display.
	virtual void OnDisplayUnsubscribe(Display *di) = 0;
};

#endif