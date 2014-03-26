#ifndef HEADER_ComponentOutputImageStatisticsColor
#define HEADER_ComponentOutputImageStatisticsColor

#include "Component.h"

#include <wx/string.h>
#include <cv.h>
#include <wx/file.h>

//! An output component that writes an M4V file using the Xvid library.
class ComponentOutputImageStatisticsColor: public Component {

public:
	//! Constructor.
	ComponentOutputImageStatisticsColor(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputImageStatisticsColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputImageStatisticsColor(mCore);
	}

private:
	bool mCalculateNonZero;				//!< (configuration) Calculate the number of non-zero elements of the image.
	bool mCalculateSum;					//!< (configuration) Calculate the sum of the image.
	bool mCalculateMeanStdDev;			//!< (configuration) Calculate mean and std deviation of the image.
	bool mCalculateMinMax;				//!< (configuration) Calculate min/max of the image.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif
