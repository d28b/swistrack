#ifndef HEADER_ComponentCalibrationFileTSAI
#define HEADER_ComponentCalibrationFileTSAI

#include <opencv2/core.hpp>
#include "Component.h"
#include "DataStructureParticles.h"
#include "ConfigurationXML.h"
#include <vector>

//! A component that calculates world coordinates of the particles with a linear model. Basically, each corner of the camera is mapped to world coordinates and all points inside the image are linearly interpolated.
class ComponentCalibrationFileTSAI: public Component, public ConfigurationXML {

public:
	//! Constructor.
	ComponentCalibrationFileTSAI(SwisTrackCore * stc);
	//! Destructor.
	~ComponentCalibrationFileTSAI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
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

	class RotationMatrix {
		double a[9];

	public:
		double get(int row, int col) {
			return a[row * 3 + col];
		}
		void set(int row, int col, double value) {
			a[row * 3 + col] = value;
		}
	};

	struct CalibrationParameters {
		double dx;			//!< Camera pixel size x
		double dy;			//!< Camera pixel size y
		double sx;			//!< Uncertainty factor on dx versus dy
		double k1;			//!< First term coefficient of radial distorsion
		double f;			//!< Lens focal length
		cv::Point2d C;		//!< Optical center position on the CCD array
		cv::Point3d T;		//!< Translation vector
		RotationMatrix R;	//!< Rotation matrix
	};

	std::vector<CalibrationPoint> calibrationPointList;		//!< List containing the calibration points.
	Display mDisplayOutput;									//!< The Display showing the particles.
	CalibrationParameters mCalParam;						//!< Calibration parameters

	void ReadPoint(wxXmlNode * node);
	void Read3DMatrix(wxXmlNode * node);
	double rot(int a, int b);
	cv::Point2f Image2World(cv::Point2f imageCoordinates);
};
#endif

