#ifndef HEADER_DisplayImageParticles
#define HEADER_DisplayImageParticles

class DisplayImageParticles;

#include <string>
#include "DisplayImage.h"

//! A DisplayImage that just copies an existing image.
class DisplayImageParticles: public DisplayImage {

public:
	IplImage *mImage;										//!< The image to use for display. This class will create a copy of this image upon request.
	CvPoint2D32f mTopLeft;									//!< The coordinates of the top left corner.
	CvPoint2D32f mBottomRight;								//!< The coordinates of the bottom right corner.
	DataStructureParticles::tParticleVector *mParticles;	//!< The particles to display.

	//! Constructor.
	DisplayImageParticles(const std::string &name, const std::string &displayname): DisplayImage(name, displayname) {}
	//! Destructor.
	~DisplayImageParticles() {}

	// DisplayImage methods
	IplImage *CreateImage(int maxwidth, int maxheight);
};

#endif