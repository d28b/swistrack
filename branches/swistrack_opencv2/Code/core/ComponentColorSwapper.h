#ifndef HEADER_ComponentColorSwapper
#define HEADER_ComponentColorSwapper

#include <cv.h>
//#include <cvaux.hpp>
extern "C" {
#include "camshift_wrapper.h"
}
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentColorSwapper: public Component {

public:
	//! Constructor.
	ComponentColorSwapper(SwisTrackCore *stc);
	//! Destructor.
	~ComponentColorSwapper();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentColorSwapper(mCore);
	}

private:
	IplImage * mOutputImage;
	wxColour mSourceColor;
	wxColour mTargetColor;
	Display mDisplayOutput;

	
};

#endif

