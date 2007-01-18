#ifndef HEADER_DisplayImageSubscriberInterface
#define HEADER_DisplayImageSubscriberInterface

#include <string>

class DisplayImageSubscriberInterface {

public:
	//! Constructor.
	DisplayImageSubscriberInterface() {}
	//! Destructor.
	~DisplayImageSubscriberInterface() {}

	//! Called when a display image changes.
	virtual void OnDisplayImageChanged(DisplayImage *displayimage) = 0;
};

#endif