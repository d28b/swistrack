#ifndef HEADER_ComponentCalibrationLinear
#define HEADER_ComponentCalibrationLinear

#include "cv.h"
#include "Component.h"
#include "DataStructureParticles.h"
#include <vector>

//! A component that calculates world coordinates of the particles with a linear model. Basically, each corner of the camera is mapped to world coordinates and all points inside the image are linearly interpolated.
class ComponentCalibrationLinear: public Component {

public:
	//! Constructor.
	ComponentCalibrationLinear(SwisTrackCore *stc);
	//! Destructor.
	~ComponentCalibrationLinear();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentCalibrationLinear(mCore);}

	void Transform(Particle *p);

private:
	CvPoint2D32f mWorldTopLeft;			//!< (configuration) The world coordinates of the corresponding corner.
	CvPoint2D32f mWorldTopRight;		//!< (configuration) The world coordinates of the corresponding corner.
	CvPoint2D32f mWorldBottomLeft;		//!< (configuration) The world coordinates of the corresponding corner.
	CvPoint2D32f mWorldBottomRight;		//!< (configuration) The world coordinates of the corresponding corner.
	CvPoint2D32f mCameraTopLeft;		//!< (configuration) The coordinates of the corresponding corner on the camera image.
	CvPoint2D32f mCameraBottomRight;	//!< (configuration) The coordinates of the corresponding corner on the camera image.

	Display mDisplayOutput;				//!< The Display showing the particles.
};

#endif

