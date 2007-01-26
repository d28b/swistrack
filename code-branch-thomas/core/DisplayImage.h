#ifndef HEADER_DisplayImage
#define HEADER_DisplayImage

class DisplayImage;

#include <string>
#include <list>
#include "cv.h"
#include "DisplayImageSubscriberInterface.h"

//! The base class for images that are shown in the GUI or stored as videos.
class DisplayImage {

public:
	//! The list type of subscribers.
	typedef std::list<DisplayImageSubscriberInterface *> tSubscribers;

	std::string mName;			//!< The internal name.
	std::string mDisplayName;	//!< The name displayed to the user.
	std::string mAnnotation1;	//!< Annotation 1, usually displayed below the image.
	std::string mAnnotation2;	//!< Annotation 2, usually displayed below the image.
	tSubscribers mSubscribers;	//!< The list of subscribers.

	//! Constructor.
	DisplayImage(const std::string &name, const std::string &displayname): mName(name), mDisplayName(displayname) {}
	//! Destructor.
	~DisplayImage() {}

	//! Called to draw the display image. This function is called once for each subscriber, with the desired width/height. The returned IplImage must be released using ReleaseImage.
	virtual IplImage *CreateImage(int maxwidth, int maxheight) = 0;
	//! Releases an image.
	void ReleaseImage(IplImage *image);

	//! Calculates the maximum size.
	CvSize CalculateMaxSize(int srcwidth, int srcheight, int maxwidth, int maxheight);
};

#endif