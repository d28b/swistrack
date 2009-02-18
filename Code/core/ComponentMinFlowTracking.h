#ifndef HEADER_ComponentMinFlowTracking
#define HEADER_ComponentMinFlowTracking

#include <cv.h>
#include <vector>
using namespace std;

#include "Component.h"
#include "DataStructureParticles.h"


double squareDistance(CvPoint2D32f p1, CvPoint2D32f p2);

class DetectionResponse {
  Particle mParticle;
 public:
   DetectionResponse(Particle p) {
    mParticle = p;
  }
  // appearance
};

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentMinFlowTracking: public Component {

public:
	//! Constructor.
	ComponentMinFlowTracking(SwisTrackCore *stc);
	//! Destructor.
	~ComponentMinFlowTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentMinFlowTracking(mCore);
	}

private:
	void Track() ;
	DataStructureTracks::tTrackMap mTracks;
	
	vector<DetectionResponse> mObservations;
	
	int mWindowSize;
	int mFrameCount;
	
	Display mDisplayOutput;
	
};

#endif

