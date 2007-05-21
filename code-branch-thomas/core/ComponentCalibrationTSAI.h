#ifndef HEADER_ComponentCalibrationTSAI
#define HEADER_ComponentCalibrationTSAI

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"
#include "ConfigurationXML.h"
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
	Component *Create() {return new ComponentCalibrationTSAI(mCore);}

	void Transform(Particle *p);

private:
	//! A structure containing the position values of the calibration points
	struct CalibrationPoint
	{
		double xWorld;
		double yWorld;
		double xImage;
		double yImage;
	};
	std::vector<CalibrationPoint>	calibrationPointList;		//!< List containing the calibration points.
	Display mDisplayOutput;									//!< The Display showing the particles.
	void ReadPoint(wxXmlNode *node);


};
#endif

