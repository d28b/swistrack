#ifndef HEADER_ComponentBackgroundSubtractionGray
#define HEADER_ComponentBackgroundSubtractionGray

#include "cv.h"
#include "Component.h"

//! A component subtracting a fixed background from a grayscale image.
class ComponentBackgroundSubtractionGray: public Component {

public:
	//! Constructor.
	ComponentBackgroundSubtractionGray(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBackgroundSubtractionGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBackgroundSubtractionGray(mCore);}

private:
	IplImage *mBackgroundImage;
	double mBackgroundImageMean;
	bool mCorrectMean;
};

#endif

