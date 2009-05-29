#ifndef HEADER_ComponentCalibrationFileTSAI
#define HEADER_ComponentCalibrationFileTSAI

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"
#include "ConfigurationXML.h"
#include <vector>



//! A component that calculates world coordinates of the particles with a linear model. Basically, each corner of the camera is mapped to world coordinates and all points inside the image are linearly interpolated.
class ComponentCalibrationFileTSAI: public Component , public ConfigurationXML{

public:
	//! Constructor.
	ComponentCalibrationFileTSAI(SwisTrackCore *stc);
	//! Destructor.
	~ComponentCalibrationFileTSAI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentCalibrationFileTSAI(mCore);
	}

private:
	//! A structure containing the position values of the calibration points
	struct CalibrationPoint {
		double xWorld;
		double yWorld;
		double xImage;
		double yImage;
	};
	struct calibrationParametersStruct
	{
		double dx;		//!< Camera pixel size x
		double dy;		//!< Camera pixel size y
		double sx;		//!< Uncertainty factor on dx versus dy
		double k1;		//!< First term coefficient of radial distorsion
		double f;		//!< Lens focal length
		CvPoint2D64f C;	//!< Optical center position on the CCD array
		CvPoint3D64f T;	//!< Translation vector
		CvMat* R;		//!< Rotation matrix		
	};

	std::vector<CalibrationPoint>	calibrationPointList;		//!< List containing the calibration points.
	Display mDisplayOutput;										//!< The Display showing the particles.
	calibrationParametersStruct mCalParam;						//!< Calibration parameters

	void ReadPoint(wxXmlNode *node);
	void Read3DMatrix(wxXmlNode *node, CvMat* matrix);
	CvPoint2D32f Image2World(CvPoint2D32f imageCoordinates);	
};
#endif

