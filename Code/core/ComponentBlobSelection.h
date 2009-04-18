#ifndef HEADER_ComponentBlobSelection
#define HEADER_ComponentBlobSelection

#include <cv.h>
#include "Component.h"

//! A component that selects blobs that have a certain size
class ComponentBlobSelection: public Component {

public:
	//! Constructor.
	ComponentBlobSelection(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBlobSelection();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentBlobSelection(mCore);
	}

private:
	int mMinArea;	//!< (configuration) The minimum area of a blob.
	int mMaxArea;	//!< (configuration) The maximum area of a blob.
	bool mAreaSelection; //!< (configuration) If the selection is based on the blob area
	double mMinCompactness;	//!< (configuration) The minimum Compactness of a blob.
	double mMaxCompactness;	//!< (configuration) The maximum Compactness of a blob.
	bool mCompactnessSelection; //!< (configuration) If the selection is based on the blob Compactness
	double mMinOrientation;	//!< (configuration) The minimum Compactness of a blob.
	double mMaxOrientation;	//!< (configuration) The maximum Compactness of a blob.
	bool mOrientationSelection; //!< (configuration) If the selection is based on the blob Orientation
	IplImage* mOutputImage; //!< Output image

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

