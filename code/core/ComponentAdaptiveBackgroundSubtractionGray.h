#ifndef HEADER_ComponentAdaptiveBackgroundSubtractionGray
#define HEADER_ComponentAdaptiveBackgroundSubtractionGray

#include <cv.h>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentAdaptiveBackgroundSubtractionGray: public Component {

public:
	//! Constructor.
	ComponentAdaptiveBackgroundSubtractionGray(SwisTrackCore *stc);
	//! Destructor.
	~ComponentAdaptiveBackgroundSubtractionGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentAdaptiveBackgroundSubtractionGray(mCore);
	}

private:
	enum eMode {
		sMode_AbsDiff,
		sMode_SubImageBackground,
		sMode_SubBackgroundImage,
	};

	IplImage *mBackgroundImage;			//!< The background image.
	CvScalar mBackgroundImageMean;		//!< The mean of the background image.
	bool mCorrectMean;					//!< (configuration) Whether to correct for the mean or not.
	float mUpdateProportion;			//!< (configuration) Proportion of the input image added to the background
	eMode mMode;						//!< (configuration) The subtraction mode.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

