#ifndef HEADER_DisplayImage
#define HEADER_DisplayImage

class DisplayImage;

#include <string>
#include <list>
#include "cv.h"
#include "DisplaySubscriberInterface.h"

//! The base class for images that are shown in the GUI or stored as videos.
class DisplayImage {
friend class SwisTrackCore;

public:
	//! The list type of subscribers.
	typedef std::list<DisplaySubscriberInterface *> tSubscriberList;

	std::string mName;			//!< The internal name.
	std::string mDisplayName;	//!< The name displayed to the user.
	std::string mAnnotation1;	//!< Annotation 1, usually displayed below the image.
	std::string mAnnotation2;	//!< Annotation 2, usually displayed below the image.
	tSubscriberList mSubscribers;	//!< The list of subscribers.

	//! Constructor.
	DisplayImage(const std::string &name, const std::string &displayname) {}
	//! Destructor.
	~DisplayImage() {}

	//! Adds a subscriber.
	void Subscribe(DisplaySubscriberInterface *disi) {}
	//! Removes a subscriber.
	void Unsubscribe(DisplaySubscriberInterface *disi) {}

	//! Called to draw the display image. This function is called once for each subscriber, with the desired width/height. The returned IplImage must be released using ReleaseImage.
	virtual IplImage *CreateImage(int maxwidth, int maxheight) = 0;
	//! Releases an image.
	void ReleaseImage(IplImage *image) {}

	//! Calculates the maximum size given the size of the source image. The returned size is either the size of the source image, or a scaled down rectangle (to fit into maxwidth/maxheight) with the same proportions.
	CvSize CalculateMaxSize(int srcwidth, int srcheight, int maxwidth, int maxheight) {}
	//! Calculates the maximum size given a ratio=width/height. The returned size is the largest rectangle with this width/heigth ratio that fits into the maxwidth/maxheight rectangle.
	CvSize CalculateMaxSize(double ratio, int maxwidth, int maxheight) {}

protected:
	//! This is called by SwisTrackCore when the subscribers shall be notified. These subscribers will call CreateImage() to fetch the image.
	void OnChanged() {}

};

#endif