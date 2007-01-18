#ifndef HEADER_DisplayImageStandard
#define HEADER_DisplayImageStandard

#include <string>
#include "DisplayImage.h"

class DisplayImageStandard {

public:
	//! The image to use for display.
	IplImage *mNewImage;

	//! Constructor.
	DisplayImageStandard(Component *c, const std::string &name, const std::string &displayname): mDisplayImage(c, name, displayname) {}
	//! Destructor.
	~DisplayImageStandard() {}

	// DisplayImage methods
	IplImage *CreateImage(int maxwidth, int maxheight);
};

#endif