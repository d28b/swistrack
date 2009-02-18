#ifndef HEADER_ComponentCalibrationTSAI
#define HEADER_ComponentCalibrationTSAI

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"
#include "ConfigurationXML.h"
#include "libtsai.h"
#include <vector>



//! A component that calculates world coordinates of the particles with a linear model. Basically, each corner of the camera is mapped to world coordinates and all points inside the image are linearly interpolated.
class ComponentCalibrationTSAI: public Component , public ConfigurationXML{

public:
	//! Constructor.
	ComponentCalibrationTSAI(SwisTrackCore *stc);
	//! Destructor.
	~ComponentCalibrationTSAI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentCalibrationTSAI(mCore);
	}

private:
	//! A structure containing the position values of the calibration points
	struct CalibrationPoint {
		double xWorld;
		double yWorld;
		double xImage;
		double yImage;
	};

	std::vector<CalibrationPoint>	calibrationPointList;		//!< List containing the calibration points.
	Display mDisplayOutput;									//!< The Display showing the particles.

	void ReadPoint(wxXmlNode *node);
	CvPoint2D32f Image2World(CvPoint2D32f imageCoordinates);
	CvPoint2D32f World2Image(CvPoint2D32f worldCoordinates);
	calibration_data calibrationData;
	calibration_constants calibrationConstants;
	camera_parameters cameraParameters;

};
#endif

