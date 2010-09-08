#ifndef HEADER_ComponentRosTest
#define HEADER_ComponentRosTest

#include <vector>
#include <map>
#include <cv.h>
#include <highgui.h>
#include <fstream>
#include "Component.h"
#include "ros/ros.h"
#include "swistrack/ParticleTrack.h"

//! An output component writing the trajectory data in tab seperated files.
class ComponentRosTest: public Component {

public:
	//! Constructor.
	ComponentRosTest(SwisTrackCore *stc);
	//! Destructor.
	~ComponentRosTest();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentRosTest(mCore);
	}

	void WriteParticle(const Particle & particle);


private:

	wxString mFileNamePrefix;		//!< Prefix of file names
	Display mDisplayOutput;			//!< The DisplayImage showing the output of this component.

	ros::Publisher track_pub; // Publishes tracks to ROS.

	void WriteData(int ID);
};

#endif

