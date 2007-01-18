#ifndef HEADER_DisplayImageSubscriberInterface
#define HEADER_DisplayImageSubscriberInterface

class DisplayImageSubscriberInterface;

#include <string>
#include "DisplayImage.h"

class DisplayImageSubscriberInterface {

public:
	//! Constructor.
	DisplayImageSubscriberInterface() {}
	//! Destructor.
	~DisplayImageSubscriberInterface() {}

	//! Called when a display image changes.
	virtual void OnDisplayImageChanged(DisplayImage *di) = 0;
};

#endif