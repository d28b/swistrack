#ifndef HEADER_ComponentCalibrationLinear
#define HEADER_ComponentCalibrationLinear

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"
#include "ConfigurationXML.h"
#include <vector>



//! A component that calculates world coordinates of the particles with a linear model. Basically, each corner of the camera is mapped to world coordinates and all points inside the image are linearly interpolated.
class ComponentCalibrationLinear: public Component, public ConfigurationXML {

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
	Component *Create() {
		return new ComponentCalibrationLinear(mCore);
	}

	void Transform(Particle *p);

private:
	//! A structure containing the position values of the calibration points
	struct CalibrationPoint {
		double xWorld;
		double yWorld;
		double xImage;
		double yImage;
	};

	std::vector<CalibrationPoint> calibrationPointList;			//!< List containing the calibration points.
	float cameraMatrix[12];										//!< Matrix for the coordinate transformation.
	Display mDisplayOutput;										//!< The Display showing the particles.

	void ReadPoint(wxXmlNode *node);
	CvPoint2D32f Image2World(CvPoint2D32f p);
};
#endif

