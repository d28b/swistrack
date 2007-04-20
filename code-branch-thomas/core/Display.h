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
friend class DisplayEditor;
friend class SwisTrackCore;

public:
	//! The list type of subscribers.
	typedef std::list<DisplaySubscriberInterface *> tSubscriberList;

	// Internal display information
	Component *mComponent;			//!< The associated component
	std::string mName;				//!< The internal name of this display.
	std::string mDisplayName;		//!< The name displayed to the user.
	tSubscriberList mSubscribers;	//!< The list of subscribers.

	// General information
	int mFrameNumber;				//!< The frame number.
	std::string mAnnotation1;		//!< Annotation 1, usually displayed below the image.
	std::string mAnnotation2;		//!< Annotation 2, usually displayed below the image.
	std::list<std::string> mErrors;	//!< This holds more information display errors (e.g. wrong image format, ...).

	// Display window
	CvPoint *mOffset;				//!< The offset at which the display starts.
	CvSize *mSize;					//!< The size of the display.

	// Images
	double mImagePixelSize;			//!< The size of a pixel in the image.
	IplImage *mImage;				//!< The main image. This is always assumed to be at position (0, 0);
	IplImage *mMaskImage;			//!< The mask image, if any.
	
	// Particles
	DataStructureParticles::tParticleVector *mParticles;		//!< The particles in pixel coordinates.
	DataStructureParticles::tParticleVector *mParticlesWorld;	//!< The particles in world coordinates.

	//! Constructor.
	Display(const std::string &name, const std::string &displayname);
	//! Destructor.
	~Display();

	//! Adds a subscriber.
	void Subscribe(DisplaySubscriberInterface *disi);
	//! Removes a subscriber.
	void Unsubscribe(DisplaySubscriberInterface *disi);

protected:
	//! This is called by SwisTrackCore when the subscribers shall be notified. These subscribers will call CreateImage() to fetch the image.
	void OnChanged();

};

#endif