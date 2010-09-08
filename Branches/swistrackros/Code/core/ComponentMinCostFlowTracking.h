#ifndef HEADER_ComponentMinCostFlowTracking
#define HEADER_ComponentMinCostFlowTracking

#include "Component.h"

#ifdef USE_BOOST
#include <cv.h>
#include "DataStructureParticles.h"
#include "MinCostFlow.h"

//! A tracking component that uses the algorithm described in 
//! Global Data Association for Multi-Object Tracking Using Network Flows
//! by Li Zhang, Yuan Li and Ramakant Nevatia
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
	DataStructureParticles::tParticleVector mParticles;
	wxDateTime mStartTimestamp;

	map<MinCostFlow::Graph::vertex_descriptor, Particle> 
	  mObservations;
	Display mDisplayOutput;	

	MinCostFlow mFlow;

	MinCostFlow::Graph mGraph;
	MinCostFlow::Graph::vertex_descriptor sourceVertex, sinkVertex; // representing source and sink in the paper
	
	// transformed source and sink  needed for implementation. 
	// we keep it around instead of adding and removing these from the graph
	// because that invalidates other vertex descriptors in bgl.
	MinCostFlow::Graph::vertex_descriptor tSourceVertex, tSinkVertex; 

	double p_entr;
	double p_exit;
	double beta_i;

	double mMaxSquareDistanceForSameTrack;
	double mMinimumAreaParticle;
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

#else // USE_BOOST

class ComponentMinCostFlowTracking: public Component {

public:
	ComponentMinCostFlowTracking(SwisTrackCore *stc): Component(stc, wxT("MinCostFlowTracking")) {
		Initialize();
	}
	~ComponentMinCostFlowTracking() {}

	// Overwritten Component methods
	void OnStart() {
		AddError(wxT("This component was not compiled in this executable."));
	}
	void OnReloadConfiguration() {
		AddError(wxT("This component was not compiled in this executable."));
	}
	void OnStep() {
		AddError(wxT("This component was not compiled in this executable."));
	}
	void OnStepCleanup() {}
	void OnStop() {
		AddError(wxT("This component was not compiled in this executable."));
	}
	Component *Create() {
		return new ComponentMinCostFlowTracking(mCore);
	}
};

#endif // USE_BOOST

#endif
