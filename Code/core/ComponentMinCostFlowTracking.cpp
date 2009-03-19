#include "ComponentMinCostFlowTracking.h"
#define THISCLASS ComponentMinCostFlowTracking

#include "DisplayEditor.h"
#include "Utility.h"
#include <iostream>
#include <set>
#include "MinCostFlow.h"
#include <cv.h>
#include <cassert>
#include <sstream>
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


	//exit(-1);
}

THISCLASS::~ComponentMinCostFlowTracking()
{
}

void THISCLASS::ResetGraph() {
  mGraph.clear();
  struct MinCostFlow::VertexProps vProps;
  
  vProps.name="source";
  sourceVertex = add_vertex(vProps, mGraph);
  
  vProps.name="sink";
  sinkVertex = add_vertex(vProps, mGraph);

  mObservations.clear();
  
}

void THISCLASS::OnStart()
{
  THISCLASS::OnReloadConfiguration();
  
  ResetGraph();
  mMaxSquareDistanceForSameTrack = pow(GetConfigurationDouble(wxT("MaxDistanceForSameTrack"), 10), 2);
  
  mMaxDifferenceInArea = GetConfigurationDouble(wxT("MaxDifferenceInAreaForSameTrack"), 50);

  mWindowSize = wxTimeSpan::Seconds(GetConfigurationInt(wxT("WindowSizeSeconds"), 10));

  mStartTimestamp = wxInvalidDateTime;
}

void THISCLASS::OnReloadConfiguration()
{
}

double THISCLASS::p_link(Particle x_i, Particle x_j) {
  double distance = Utility::SquareDistance(x_i.mCenter, x_j.mCenter);
  double area = fabs(x_i.mArea - x_j.mArea);
  if (x_i.mColorModel == NULL || x_j.mColorModel == NULL) {
    return 0;
  } else if (distance > mMaxSquareDistanceForSameTrack ) {
    return 0;
  } else if (area > mMaxDifferenceInArea) {
    return 0;
  } else if (x_i.mTimestamp > x_j.mTimestamp) {
    return 0;
  } else {
    double colorSim = cvCompareHist(x_i.mColorModel, x_j.mColorModel, 
				    CV_COMP_BHATTACHARYYA); 
    // zero is completely similar, 1 is completely different.



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
    ostringstream path;
    path << pIt->mTimestamp.Format().ToAscii() << " " << pIt->mCenter.x << "," << pIt->mCenter.y;
    vProps.name = path.str();


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
  
  wxTimeSpan diff = mCore->mDataStructureInput.FrameTimestamp().Subtract(mStartTimestamp);
  cout << "Window size: " << mWindowSize.Format().ToAscii() << endl; 
  cout << "       diff: " << diff.Format().ToAscii() << endl; 
  if (diff.IsLongerThan(mWindowSize)) {
    ProcessWindow();
    mStartTimestamp = mCore->mDataStructureInput.FrameTimestamp();
  }
}
void THISCLASS::ProcessWindow() {
  cout << "Running min cost flow!" << endl;
  AddTransitionEdges();
  mFlow.minCostFlow(&mGraph);

  OutputTracks(mGraph);

  mGraph.clear();
}

void THISCLASS::OutputTracks(const MinCostFlow::Graph & graph) {
  
  
}


void THISCLASS::AddTransitionEdges() { 
  for (map<MinCostFlow::Graph::vertex_descriptor, Particle>::iterator i = 
	 mObservations.begin(); i != mObservations.end(); i++) {
    // FIXME - loop over every pair once. 
    Particle & x_i = i->second;
    for (map<MinCostFlow::Graph::vertex_descriptor, Particle>::iterator j = 
	   mObservations.begin(); j != mObservations.end(); j++) {
      Particle & x_j = j->second;
      double p_lnk = p_link(x_i, x_j);
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
