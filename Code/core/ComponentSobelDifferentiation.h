#ifndef HEADER_ComponentSobelDifferentiation
#define HEADER_ComponentSobelDifferentiation

#include <opencv2/opencv.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component tdifferentiates the image with Sobel().
class ComponentSobelDifferentiation: public Component {

public:
	//! Constructor.
	ComponentSobelDifferentiation(SwisTrackCore * stc);
	//! Destructor.
	~ComponentSobelDifferentiation();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentSobelDifferentiation(mCore);
	}

private:
	int mOrderX;				//!< (configuration) Order X.
	int mOrderY;				//!< (configuration) Order Y.
	int mKernelSize;			//!< (configuration) Kernel size.
	Display mDisplayOutput;		//!< The Display showing the last acquired image and the particles.

};

#endif

