#ifndef HEADER_ComponentColorGaussianBlur
#define HEADER_ComponentColorGaussianBlur

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentColorGaussianBlur: public Component {

public:
	//! Constructor.
	ComponentColorGaussianBlur(SwisTrackCore * stc);
	//! Destructor.
	~ComponentColorGaussianBlur();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentColorGaussianBlur(mCore);
	}

private:
	double mSigmaX;				//!< (configuration) Sigma X.
	double mSigmaY;				//!< (configuration) Sigma Y.
	int mSizeX;					//!< (configuration) Window size X.
	int mSizeY;					//!< (configuration) Window size Y.

	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
};

#endif

