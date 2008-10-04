#ifndef HEADER_DisplaySubscriberInterface
#define HEADER_DisplaySubscriberInterface

class DisplaySubscriberInterface;

#include <wx/string.h>
#include "Display.h"

//! This is the interface for subscribers to Display classes.
class DisplaySubscriberInterface {

public:
	//! Constructor.
	DisplaySubscriberInterface() {}
	//! Destructor.
	virtual ~DisplaySubscriberInterface() {}

	//! Called when subscribing to a Display.
	virtual void OnDisplaySubscribe(Display *display) = 0;
	//! Called when unsubscribing from a Display.
	virtual void OnDisplayUnsubscribe(Display *display) = 0;
	//! Called to ask whether the display is active and data should be collected to draw it.
	virtual void OnDisplayBeforeStep(Display *display) = 0;
	//! Called when a display changes.
	virtual void OnDisplayChanged(Display *display) = 0;
};

#endif
