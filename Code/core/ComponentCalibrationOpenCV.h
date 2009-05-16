#ifndef HEADER_ComponentCalibrationOpenCV
#define HEADER_ComponentCalibrationOpenCV

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"
#include "ConfigurationXML.h"
#include <vector>



//! A component that calculates world coordinates of the particles with a linear model. Basically, each corner of the camera is mapped to world coordinates and all points inside the image are linearly interpolated.
class ComponentCalibrationOpenCV: public Component, public ConfigurationXML {


public:
	//! Constructor.
	ComponentCalibrationOpenCV(SwisTrackCore *stc);
	//! Destructor.
	~ComponentCalibrationOpenCV();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentCalibrationOpenCV(mCore);
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

	Display mDisplayOutput;										//!< The Display showing the particles.

	void ReadPoint(wxXmlNode *node);
	CvPoint2D32f Image2World(CvPoint2D32f p);


///
 // will be initialized by the prepare undistortion mapping function
	CvMat* intrinsic_matrix;
	CvMat* distortion_coeffs;
	CvMat* rotation_vectors;
	CvMat* rotation_matrix;
	CvMat* rotation_matrix_inverse;
	CvMat* translation_vectors;
	CvMat* translation_tr;
	CvMat* intrinsic_transfer;
	CvMat* mapx; 
	CvMat* mapy;
// they should be released at destructor
};
#endif

