#ifndef HEADER_ComponentGraphControl
#define HEADER_ComponentGraphControl

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"
#include "Graph.h"

//! A basic control algorithm that computes the connected components (CC) in the graph associated with the particles (= vertices)
//  and compares the number of particles in the CC with the dimension of the target structure
class ComponentGraphControl: public Component {

public:
	//! Constructor.
	ComponentGraphControl(SwisTrackCore *stc);
	//! Destructor.
	~ComponentGraphControl();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentGraphControl(mCore);
	}

private:        // this needs to be cleared of unused stuff
	void DataAssociation();
	double GetCost(int id, CvPoint2D32f p);
	void AddPoint(int i, CvPoint2D32f p);

	DataStructureParticles::tParticleVector *mParticles;
	DataStructureCommand::tCommandVector mCommands;
	double** distanceArray;
	int maxParticles;
	int ParticleSize;
	// Parameters
	int ClusterSize; //!< (configuration) The number of particles in the target structure
	int ArenaSize;
	int mMaxNumber;		//!< (configuration) The maximum number of objects that are to track.
	int imgCenterX, imgCenterY;
	
	Display mDisplayOutput;									//!< The Display showing the last acquired image and the particles.
	IplImage *mDisplayOutputImage;
	
};

#endif

