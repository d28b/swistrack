#ifndef HEADER_ComponentMinCostFlowTracking
#define HEADER_ComponentMinCostFlowTracking

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"


#include "MinCostFlow.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentMinCostFlowTracking: public Component {

public:
	//! Constructor.
	ComponentMinCostFlowTracking(SwisTrackCore *stc);
	//! Destructor.
	~ComponentMinCostFlowTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentMinCostFlowTracking(mCore);
	}

private:
	DataStructureTracks::tTrackMap mTracks;
	wxDateTime mStartTimestamp;

	map<MinCostFlow::Graph::vertex_descriptor, Particle> 
	  mObservations;
	Display mDisplayOutput;	

	MinCostFlow mFlow;

	MinCostFlow::Graph mGraph;
	MinCostFlow::Graph::vertex_descriptor sourceVertex, sinkVertex;

	double p_entr;
	double p_exit;
	double beta_i;

	double mMaxSquareDistanceForSameTrack;
	double mMaxDifferenceInArea;
	int mMaxTrackCount;

	wxTimeSpan mWindowSize;

	double p_link(Particle x_i, Particle x_j);

	void ProcessWindow();
	void ResetGraph();
	void AddTransitionEdges();

	void OutputTracks(const MinCostFlow::Graph & graph);
	
	int mNextTrackId;




};

#endif

