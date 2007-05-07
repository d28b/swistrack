#ifndef HEADER_Display
#define HEADER_Display

class Display;

#include <string>
#include <list>
#include "cv.h"
#include "DisplaySubscriberInterface.h"
#include "DataStructureParticles.h"
#include "Component.h"

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
	int mFramesCount;				//!< The number of frames.
	std::string mAnnotation1;		//!< Annotation 1, usually displayed below the image.
	std::string mAnnotation2;		//!< Annotation 2, usually displayed below the image.
	ErrorList mErrors;				//!< This holds more information display errors (e.g. wrong image format, ...).

	// Display window
	CvPoint mTopLeft;				//!< The offset at which the display starts. (TODO: implement this in the renderer)
	CvSize mSize;					//!< The size of the display.

	// Images
	double mImagePixelSize;			//!< The size of a pixel in the image. (not implemented and probably not needed)
	IplImage *mMainImage;			//!< The main image. This is always assumed to be at position (0, 0).
	IplImage *mMaskImage;			//!< The mask image, if any.
	
	// Particles
	DataStructureParticles::tParticleVector mParticles;			//!< The particles in pixel coordinates.
	DataStructureParticles::tParticleVector mParticlesWorld;	//!< The particles in world coordinates.

	//! Constructor.
	Display(const std::string &name, const std::string &displayname);
	//! Destructor.
	~Display();

	//! Adds a subscriber.
	void Subscribe(DisplaySubscriberInterface *disi);
	//! Removes a subscriber.
	void Unsubscribe(DisplaySubscriberInterface *disi);

	//! Returns if the this display is active (i.e. someone wants its contents).
	void SetActive() {mActive=true;}

protected:
	bool mActive;	//<! Whether the display is active or not. (A display needs to be activated in the OnDisplayBeforeStep method.)
	bool mChanged;	//<! Whether the display has changed in the last step.

	//! This is called by SwisTrackCore before a step. This will check if any of the subscribers needs an image.
	void OnBeforeStep();
	//! This is called by SwisTrackCore after a step. It will notify the subscribers if a new image is available.
	void OnAfterStep();

};

#endif