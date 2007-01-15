#ifndef HEADER_DisplayImage
#define HEADER_DisplayImage

#include <string>
#include "cv.h"

class DisplayImage {

public:
	//! The image.
	IplImage *mImage;
	//! The internal name.
	std::string mName;
	//! The name displayed to the user.
	std::string mDisplayName;

	//! The list type of subscribers.
	typedef std::list<DisplayImageSubscriberInterface *> tSubscribers;
	//! The list of subscribers.
	tSubscribers mSubscribers;

	//! Constructor.
	DisplayImage(const std::string &name, const std::string &displayname): mName(name), mDisplayName(displayname) {}
	//! Destructor.
	~DisplayImage() {}

	//! Called to draw the display image. Note that this function is only called if there is at least one subscriber.
	OnPaint() = 0;
};

#endif