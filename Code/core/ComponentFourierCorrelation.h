#ifndef HEADER_ComponentFourierCorrelation
#define HEADER_ComponentFourierCorrelation

#include <cv.h>
#include "Component.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentFourierCorrelation: public Component {

public:
	//! Constructor.
	ComponentFourierCorrelation(SwisTrackCore *stc);
	//! Destructor.
	~ComponentFourierCorrelation();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentFourierCorrelation(mCore);
	}

private:
	cv::Mat mFilterMatrix;
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
	
	Display mPatternPreview;		//!< The DisplayImage showing the output of this component.
	IplImage *mDisplayOutputImage;
	IplImage *mPatternPreviewImage;
	
	cv::Mat mPattern1, mPatternFourierFiltered;
	
	cv::Mat mPatterns;
	
	bool mGenerateOutput; 		// configuration parameter specifies if an magnitude image will be calculated for display (takes extra time)
	bool mConfigChanged;
};

#endif

