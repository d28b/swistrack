#ifndef HEADER_DisplayImageStandard
#define HEADER_DisplayImageStandard

class DisplayImageStandard;

#include <string>
#include "DisplayImage.h"

class DisplayImageStandard: public DisplayImage {

public:
	//! The image to use for display.
	IplImage *mNewImage;

	//! Constructor.
	DisplayImageStandard(const std::string &name, const std::string &displayname): DisplayImage(name, displayname) {}
	//! Destructor.
	~DisplayImageStandard() {}

	// DisplayImage methods
	IplImage *CreateImage(int maxwidth, int maxheight);
};

#endif