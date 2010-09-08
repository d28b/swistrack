#ifndef HEADER_ComponentGrayMask
#define HEADER_ComponentGrayMask

#include <cv.h>
#include "Component.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentGrayMask: public Component {

public:
	//! Constructor.
	ComponentGrayMask(SwisTrackCore *stc);
	//! Destructor.
	~ComponentGrayMask();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentGrayMask(mCore);
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

