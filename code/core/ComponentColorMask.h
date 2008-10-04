#ifndef HEADER_ComponentColorMask
#define HEADER_ComponentColorMask

#include <cv.h>
#include "Component.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentColorMask: public Component {

public:
	//! Constructor.
	ComponentColorMask(SwisTrackCore *stc);
	//! Destructor.
	~ComponentColorMask();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentColorMask(mCore);
	}

private:
	enum eMode {
		cMode_BlackBlack,
		cMode_WhiteWhite,
		cMode_WhiteBlack,
		cMode_BlackWhite
	};

	IplImage *mMaskImage;		//!< The mask image.
	enum eMode mMode;			//!< (configuration) The mode.

	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
};

#endif

