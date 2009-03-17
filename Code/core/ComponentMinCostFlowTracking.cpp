#include "ComponentMinCostFlowTracking.h"
#define THISCLASS ComponentMinCostFlowTracking

#include "DisplayEditor.h"
#include "Utility.h"
#include <iostream>
#include <set>
#include "MinCostFlow.h"
#include <cv.h>
#include <cassert>
using namespace std;

THISCLASS::ComponentMinCostFlowTracking(SwisTrackCore *stc):
		Component(stc, wxT("MinCostFlowTracking")),
		mDisplayOutput(wxT("Output"), wxT("Dynamic Tracking"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	testFlow();

	mStartTimestamp = wxInvalidDateTime;
	//exit(-1);
}

THISCLASS::~ComponentMinCostFlowTracking()
{
}

void THISCLASS::OnStart()
{
	THISCLASS::OnReloadConfiguration();

	mGraph.clear();
	struct MinCostFlow::VertexProps vProps;
	
	vProps.name="source";
	sourceVertex = add_vertex(vProps, mGraph);
	
	vProps.name="sink";
	sinkVertex = add_vertex(vProps, mGraph);
}

void THISCLASS::OnReloadConfiguration()
{
}

double THISCLASS::p_link(Particle x_i, Particle x_j) {
  double colorSim = cvCompareHist(x_i.mColorModel, x_j.mColorModel, 
				  CV_COMP_BHATTACHARYYA); 
  // zero is completely similar, 1 is completely different.

  double distance = Utility::SquareDistance(x_i.mCenter, x_j.mCenter);
  
  
  if (distance > mMinSquareDistanceForSameTrack ) {
    return 0;
  } else if (x_i.mTimestamp > x_j.mTimestamp) {
    return 0;
  } else {
    return 1 - colorSim;
  }
}

void THISCLASS::OnStep()
{

  DataStructureParticles::tParticleVector * particles = 
    mCore->mDataStructureParticles.mParticles;

  if (particles == NULL) {
    return;
  }


  if (mStartTimestamp == wxInvalidDateTime) {
    mStartTimestamp = mCore->mDataStructureInput.FrameTimestamp();
  }
  double C_i = beta_i * (1 - beta_i);
  double C_en_i = - log(p_entr);
  double C_ex_i = - log(p_exit);

  
  for (DataStructureParticles::tParticleVector::iterator pIt = 
	 particles->begin(); pIt != particles->end(); pIt++) {
    struct MinCostFlow::VertexProps vProps;
    vProps.name = pIt->mID;


    MinCostFlow::Graph::vertex_descriptor u_i = add_vertex(vProps, mGraph);
    
    MinCostFlow::Graph::vertex_descriptor v_i = add_vertex(vProps, mGraph);
    mObservations[u_i] = *pIt;
    struct MinCostFlow::EdgeProps eProps;

    // Observation edge
    eProps.cost = C_i;
    eProps.capacity = 1;
    add_edge(u_i, v_i, eProps, mGraph);

    // enter edge
    eProps.cost = C_en_i;
    eProps.capacity = 1;
    add_edge(sourceVertex, u_i, eProps, mGraph);

    // exit edge
    eProps.cost = C_ex_i;
    eProps.capacity = 1;
    add_edge(v_i, sinkVertex, eProps, mGraph);

    // make transition edges later. 
  }
  
  wxTimeSpan diff = mStartTimestamp.Subtract(mCore->mDataStructureInput.FrameTimestamp());
  if (diff.GetSeconds() >= mWindowSizeSeconds) {
    ProcessWindow();
  }
}
void THISCLASS::ProcessWindow() {
  AddTransitionEdges();
  
}
void THISCLASS::AddTransitionEdges() { 
  for (map<MinCostFlow::Graph::vertex_descriptor, Particle>::iterator i = 
	 mObservations.begin(); i != mObservations.end(); i++) {
    // FIXME - loop over every pair once. 
    for (map<MinCostFlow::Graph::vertex_descriptor, Particle>::iterator j = 
	   mObservations.begin(); j != mObservations.end(); j++) {
      double p_lnk = p_link(mObservations[i->first], mObservations[j->first]);
      if (p_lnk != 0) {
	MinCostFlow::Graph::vertex_descriptor u_i, v_i, u_j, v_j;

	MinCostFlow::Graph::edge_descriptor e_i, e_j;

	u_i = i->first;
	assert(out_degree(u_i, mGraph) == 1);
	e_i = *(out_edges(u_i, mGraph).first);
	v_i = target(e_i, mGraph);


	u_j = j->first;
	assert(out_degree(u_j, mGraph) == 1);
	e_j = *(out_edges(u_j, mGraph).first);
	v_j = target(e_j, mGraph);
	MinCostFlow::EdgeProps eProps;
	eProps.cost = p_lnk;
	eProps.flow = 1;
	add_edge(v_i, u_j, eProps, mGraph);
	  
      }
    }
  }
}
  void THISCLASS::OnStepCleanup() {

}

void THISCLASS::OnStop() {


}
