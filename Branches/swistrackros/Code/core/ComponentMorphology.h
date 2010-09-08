#ifndef HEADER_ComponentMorphology
#define HEADER_ComponentMorphology

#include <cv.h>
#include <string>
#include "Component.h"
#include "DataStructureParticles.h"

//! Multi-purpose Morphology component.
class ComponentMorphology: public Component {

public:

	//! Constructor.
	ComponentMorphology(SwisTrackCore *stc);
	//! Destructor.
	~ComponentMorphology();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	int operation;
	int iterations;
	int mChannelMain;					//!< The channel to subtract from.
	int mChannelSubtract;					//!< The channel to subtract.
	Component *Create() {
		return new ComponentMorphology(mCore);
	}
private:
	IplImage *tempimage;	
	IplImage *tempimage2;
	IplImage *outputimage;				//!< The image created by this component.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

