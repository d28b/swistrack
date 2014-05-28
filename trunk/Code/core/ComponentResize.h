#ifndef HEADER_ComponentResize
#define HEADER_ComponentResize

#include <cv.h>
#include "Component.h"
using namespace cv;

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentResize: public Component {

public:
	//! Constructor.
	ComponentResize(SwisTrackCore *stc);
	//! Destructor.
	~ComponentResize();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentResize(mCore);
	}

private:

	int ROI_x, ROI_y, ROI_width, ROI_height;
	bool mConfigurationChanged;
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
	IplImage *mDisplayOutputImage;
	bool mGenerateOutput; 		// configuration parameter specifies if an magnitude image will be calculated for display (takes extra time)
};

#endif

