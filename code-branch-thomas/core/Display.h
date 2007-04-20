#ifndef HEADER_Display
#define HEADER_Display

class Display;

#include <string>
#include <list>
#include "cv.h"
#include "DisplaySubscriberInterface.h"
#include "DataStructureParticles.h"

//! The class holding all information necessary to provide a nice image of an intermediate step to the user.
class Display {
friend class DisplayConstructor;
friend class SwisTrackCore;

public:
	//! The list type of subscribers.
	typedef std::list<DisplaySubscriberInterface *> tSubscriberList;

	// Internal display information
	std::string mName;				//!< The internal name of this display.
	std::string mDisplayName;		//!< The name displayed to the user.
	tSubscriberList mSubscribers;	//!< The list of subscribers.

	// General information
	int mFrameNumber;				//!< The frame number.
	std::string mAnnotation1;		//!< Annotation 1, usually displayed below the image.
	std::string mAnnotation2;		//!< Annotation 2, usually displayed below the image.

	// Images
	IplImage *mImage;			//!< The image to display, if any.
	std::string mImageError;	//!< If the image cannot be displayed, this holds more information about the error.
	IplImage *mMaskImage;		//!< The mask image, if any.

	// Particles
	DataStructureParticles::tParticleVector *mParticles;		//!< The particles in pixel coordinates.
	DataStructureParticles::tParticleVector *mParticlesWorld;	//!< The particles in world coordinates.

	//! Constructor.
	Display(const std::string &name, const std::string &displayname);
	//! Destructor.
	~Display();

	//! Renders and returns the image.
	IplImage *GetDisplayImage(double scalingfactor);

	//! Adds a subscriber.
	void Subscribe(DisplaySubscriberInterface *disi);
	//! Removes a subscriber.
	void Unsubscribe(DisplaySubscriberInterface *disi);

	//! Called to draw the display image. This function is called once for each subscriber, with the desired width/height. The returned IplImage must be released using ReleaseImage.
	IplImage *CreateImage(double scalingfactor);
	//! Releases an image.
	void ReleaseImage(IplImage *image);

	//! Calculates the size of the image.
	CvSize CalculateSize(double scalingfactor);

	//! Calculates the maximum size given the size of the source image. The returned size is either the size of the source image, or a scaled down rectangle (to fit into maxwidth/maxheight) with the same proportions.
	CvSize CalculateMaxSize(int srcwidth, int srcheight, int maxwidth, int maxheight);
	//! Calculates the maximum size given a ratio=width/height. The returned size is the largest rectangle with this width/heigth ratio that fits into the maxwidth/maxheight rectangle.
	CvSize CalculateMaxSize(double ratio, int maxwidth, int maxheight);

protected:

	//! This is called by SwisTrackCore when the subscribers shall be notified. These subscribers will call CreateImage() to fetch the image.
	void OnChanged();

};

#endif