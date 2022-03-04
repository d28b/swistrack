#ifndef HEADER_ComponentWeightedSum
#define HEADER_ComponentWeightedSum

#include <opencv2/core.hpp>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentWeightedSum: public Component {

public:
	//! Constructor.
	ComponentWeightedSum(SwisTrackCore * stc);
	//! Destructor.
	~ComponentWeightedSum();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentWeightedSum(mCore);
	}

private:
	double base;				//!< (configuration) Base value.
	double R;					//!< (configuration) Weight of R channel.
	double G;					//!< (configuration) Weight of G channel.
	double B;					//!< (configuration) Weight of B channel.

	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
};

#endif

