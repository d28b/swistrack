#ifndef HEADER_ComponentSimulationParticles
#define HEADER_ComponentSimulationParticles

#include "Component.h"
#include "DataStructureParticles.h"
#include "SimulationParticles.h"
#include <cv.h>
#include <vector>

//! A component that simulates moving particles. The particle movements are read from a file with NMEA records. Only particles within a certain rectangular area (virtual camera) will be injected.
class ComponentSimulationParticles: public Component {

public:
	//! Constructor.
	ComponentSimulationParticles(SwisTrackCore *stc);
	//! Destructor.
	~ComponentSimulationParticles();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentSimulationParticles(mCore);
	}

private:
	CvPoint2D32f mCameraOrigin;		//!< (configuration) The origin of the virtual camera.
	float mCameraRotation;			//!< (configuration) The rotation of the virtual camera in rad.
	float mCameraPixelSize;			//!< (configuration) The pixel size of the virtual camera. Note that pixels are supposed to be square.
	CvSize2D32f mCameraSize;		//!< (configuration) The width and height (in pixels) of the virtual camera.

	SimulationParticles *mSimulationParticles;				//!< The frames with the particle.
	int mFrameNumber;										//!< The current frame number.
	DataStructureParticles::tParticleVector mParticles;		//!< The list of currently injected particles.

	Display mDisplayOutput;									//!< The Display showing the particles.
};

#endif

