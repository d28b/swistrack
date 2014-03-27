#ifndef HEADER_ComponentAdaptiveBackgroundSubtractionMedian
#define HEADER_ComponentAdaptiveBackgroundSubtractionMedian

#include <cv.h>
#include "Component.h"

#include <vector>

//! A component subtracting a fixed background from a color image.
class ComponentAdaptiveBackgroundSubtractionMedian: public Component {

public:
	//! Constructor.
	ComponentAdaptiveBackgroundSubtractionMedian(SwisTrackCore *stc);
	//! Destructor.
	~ComponentAdaptiveBackgroundSubtractionMedian();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentAdaptiveBackgroundSubtractionMedian(mCore);
	}

	void UpdateBackgroundModel(IplImage * inputImage);

private:
	IplImage * mBackgroundModel;
	IplImage *mOutputImage;
	Display mDisplayOutput;	
};

#endif

