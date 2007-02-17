#ifndef HEADER_DisplayImageStandard
#define HEADER_DisplayImageStandard

class DisplayImageStandard;

#include <string>
#include "DisplayImage.h"

//! A DisplayImage that just copies an existing image.
class DisplayImageStandard: public DisplayImage {

public:
	IplImage *mNewImage;	//!< The image to use for display. This class will create a copy of this image upon request.

	//! Constructor.
	DisplayImageStandard(const std::string &name, const std::string &displayname): DisplayImage(name, displayname) {}
	//! Destructor.
	~DisplayImageStandard() {}

	// DisplayImage methods
	IplImage *CreateImage(int maxwidth, int maxheight);
};

#endif