#ifndef HEADER_ComponentSimulateParticles
#define HEADER_ComponentSimulateParticles

#include "cv.h"
#include "Component.h"
#include "DataStructureParticles.h"
#include <vector>

//! A component that simulates moving particles. The particle movements are read from a file with NMEA records. Only particles within a certain rectangular area (virtual camera) will be injected.
class ComponentSimulateParticles: public Component {

public:
	//! Constructor.
	ComponentSimulateParticles(SwisTrackCore *stc);
	//! Destructor.
	~ComponentSimulateParticles();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentSimulateParticles(mCore);}

private:
	CvSize mCameraOrigin;		//!< (configuration) The origin of the virtual camera.
	double mCameraRotation;		//!< (configuration) The rotation of the virtual camera in rad.
	double mCameraPixelSize;	//!< (configuration) The pixel size of the virtual camera. Note that pixels are supposed to be square.
	CvSize mCameraSize;			//!< (configuration) The width and height (in pixels) of the virtual camera.
	double mPositionNoise;		//!< (configuration) (unimplemented) The variance of the gaussian noise superimposed to the particle positions.
	double mAngleNoise;			//!< (configuration) (unimplemented) The variance of the gaussian noise superimposed to the particle angles.
	std::strind mFile;			//!< (configuration) The simulation file to be read.

	DataStructureParticles::tParticleVector mParticles;		//!< The list of currently injected particles.
};

#endif

