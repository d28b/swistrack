#ifndef HEADER_ComponentCalibrationOpenCV
#define HEADER_ComponentCalibrationOpenCV

#include <opencv2/core.hpp>
#include "Component.h"
#include "DataStructureParticles.h"
#include "ConfigurationXML.h"
#include <vector>

//! A component that calculates world coordinates of the particles with a linear model. Basically, each corner of the camera is mapped to world coordinates and all points inside the image are linearly interpolated.
class ComponentCalibrationOpenCV: public Component, public ConfigurationXML {


public:
	//! Constructor.
	ComponentCalibrationOpenCV(SwisTrackCore * stc);
	//! Destructor.
	~ComponentCalibrationOpenCV();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentCalibrationOpenCV(mCore);
	}

	void Transform(Particle * p);

private:
	//! A structure containing the position values of the calibration points
	struct CalibrationPoint {
		double xWorld;
		double yWorld;
		double xImage;
		double yImage;
	};

	std::vector<CalibrationPoint> calibrationPointList;			//!< List containing the calibration points.

	Display mDisplayOutput;										//!< The Display showing the particles.

	void ReadPoint(wxXmlNode * node);
	cv::Point2f Image2World(cv::Point2f p);

	cv::Mat intrinsic_matrix;
	cv::Mat distortion_coeffs;
	cv::Mat rotation_vectors;
	cv::Mat rotation_matrix;
	cv::Mat rotation_matrix_inverse;
	cv::Mat translation_vectors;
	cv::Mat translation_tr;
	cv::Mat intrinsic_transfer;
	cv::Mat mapx;
	cv::Mat mapy;
};
#endif

