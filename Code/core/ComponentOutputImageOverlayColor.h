#ifndef HEADER_ComponentOutputImageOverlayColor
#define HEADER_ComponentOutputImageOverlayColor

#include <cv.h>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentOutputImageOverlayColor: public Component {

public:
	//! Constructor.
	ComponentOutputImageOverlayColor(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputImageOverlayColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputImageOverlayColor(mCore);
	}

private:
	enum eMode {
		sMode_Addition,
		sMode_Subtraction,
		sMode_Multiplication,
	};

	IplImage *mFinalImage;			//!< The image being constructed.
	wxFileName mFileName;			//!< (configuration) directory and filename prefix
	eMode mMode;					//!< (configuration) The combination mode.

	Display mDisplayOutput;			//!< The DisplayImage showing the output of this component.
};

#endif

