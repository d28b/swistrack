#ifndef HEADER_ComponentMoveColorToBinary
#define HEADER_ComponentMoveColorToBinary

#include <opencv2/opencv.hpp>
#include "Component.h"

//! A component that converts the color input image binary.
class ComponentMoveColorToBinary: public Component {

public:
	//! Constructor.
	ComponentMoveColorToBinary(SwisTrackCore * stc);
	//! Destructor.
	~ComponentMoveColorToBinary();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentMoveColorToBinary(mCore);
	}

private:
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

};

#endif

