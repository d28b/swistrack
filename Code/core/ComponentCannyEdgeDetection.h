#ifndef HEADER_ComponentCannyEdgeDetection
#define HEADER_ComponentCannyEdgeDetection

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that applies a constant threshold to a color image, there is only one threshold for all channels.  The output is a binary image.
class ComponentCannyEdgeDetection: public Component {

public:
	//! Constructor.
	ComponentCannyEdgeDetection(SwisTrackCore * stc);
	//! Destructor.
	~ComponentCannyEdgeDetection();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentCannyEdgeDetection(mCore);
	}

private:
	double mThreshold1;
	double mThreshold2;
	Display mDisplayOutput;
};

#endif

