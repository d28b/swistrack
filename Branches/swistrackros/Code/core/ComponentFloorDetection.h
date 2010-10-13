#ifndef HEADER_ComponentFloorDetection
#define HEADER_ComponentFloorDetection

#include <cv.h>
#include "Component.h"
#include <queue>

//! A component that detects the floor by sampling a trapazoidal region directly in front of the robot
class ComponentFloorDetection: public Component {

public:
	//! Constructor.
	ComponentFloorDetection(SwisTrackCore *stc);
	//! Destructor.
	~ComponentFloorDetection();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentFloorDetection(mCore);
	}

	// New Component methods
	void InitializeData();

private:
	
	CvHistogram* combinedHueHist;
	CvHistogram* combinedIntHist;
	int mHueT;
	int mIntT;
	int lowerBound;
	bool showT;
	Display mDisplayOutput;			   //!< The DisplayImage showing the output of this component.
};

#endif

