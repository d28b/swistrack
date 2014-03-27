#include "ComponentMinCostFlowTracking.h"
#define THISCLASS ComponentMinCostFlowTracking

#ifdef USE_BOOST

#include "DisplayEditor.h"
#include "Utility.h"
#include "MinCostFlow.h"
#include <iostream>
#include <set>
#include <cv.h>
#include <cassert>
#include <sstream>
#include <limits>

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
	mNextTrackId = 0;
	mTracks.clear();
	MinCostFlow::testFlow();
	
	p_entr = 0.000001;
	p_exit = 0.000001;
	beta_i = 0.3;
	  

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

  MinCostFlow::VertexPair sourceAndSink = MinCostFlow::addSourceAndSink(&mGraph);
  tSourceVertex = sourceAndSink.first;
  tSinkVertex = sourceAndSink.second;
  
  for (map<MinCostFlow::Graph::vertex_descriptor, Particle>::iterator i = 
	 mObservations.begin(); i != mObservations.end(); i++) {
    cvReleaseHist(&(i->second.mColorModel));
  }
  mObservations.clear();
  
}

void THISCLASS::OnStart()
{
  THISCLASS::OnReloadConfiguration();
  
  ResetGraph();
  mMaxSquareDistanceForSameTrack = pow(GetConfigurationDouble(wxT("MaxDistanceForSameTrack"), 10), 2);


  
  mMaxDifferenceInArea = GetConfigurationDouble(wxT("MaxDifferenceInAreaForSameTrack"), 50);
  mMaxTrackCount = GetConfigurationInt(wxT("MaxTrackCount"), 10);

  mWindowSize = wxTimeSpan::Seconds(GetConfigurationInt(wxT("WindowSizeSeconds"), 10));


  mMinimumAreaParticle = pow(GetConfigurationDouble(wxT("MinimumAreaParticle"), 50), 2);
  
  mStartTimestamp = wxInvalidDateTime;
}

void THISCLASS::OnReloadConfiguration()
{
}

double THISCLASS::p_link(Particle x_i, Particle x_j) {
  double distance = Utility::SquareDistance(x_i.mCenter, x_j.mCenter);
  //double area = fabs(x_i.mArea - x_j.mArea);
  if (x_i.mColorModel == NULL || x_j.mColorModel == NULL) {
    return 0;
  } else if (distance > mMaxSquareDistanceForSameTrack ) {
    return 0;
  } else if (x_i.mTimestamp >= x_j.mTimestamp) {
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
  
  mParticles.clear(); // usually a no-op, but necessary if we outputted tracks on the last frame.
  DataStructureParticles::tParticleVector * particles = 
    mCore->mDataStructureParticles.mParticles;

  if (particles == NULL) {
    return;
  }


  if (mStartTimestamp == wxInvalidDateTime) {
    mStartTimestamp = mCore->mDataStructureInput.FrameTimestamp();
  }
  double C_i = log(beta_i * (1 - beta_i));
  double C_en_i = - log(p_entr);
  double C_ex_i = - log(p_exit);

  for (DataStructureParticles::tParticleVector::iterator pIt = 
	 particles->begin(); pIt != particles->end(); pIt++) {

    if (pIt->mArea >= mMinimumAreaParticle) {
      struct MinCostFlow::VertexProps vProps;
      ostringstream path;
      path << pIt->mTimestamp.GetTicks() << "_" << pIt->mCenter.x << "_" << pIt->mCenter.y;
      ostringstream uname, vname;
      uname << "u_i_" << path.str();
      vname << "v_i_" << path.str();
      
      vProps.net_supply = 0;

      vProps.name = uname.str();
      MinCostFlow::Graph::vertex_descriptor u_i = add_vertex(vProps, mGraph);
      
      vProps.name = vname.str();
      MinCostFlow::Graph::vertex_descriptor v_i = add_vertex(vProps, mGraph);
      mObservations[u_i] = *pIt;
      CvHistogram * newHist = NULL;
      cvCopyHist(pIt->mColorModel, &newHist);
      mObservations[u_i].mColorModel = newHist;
      
      struct MinCostFlow::EdgeProps eProps;
      eProps.flow = 0;
      
      // Observation edge
      eProps.cost = C_i * 1000;
      
      eProps.capacity = 1;
      
      add_edge(u_i, v_i, eProps, mGraph);
      
      // enter edge
      eProps.cost = C_en_i * 1000;
      eProps.capacity = 1;
      add_edge(sourceVertex, u_i, eProps, mGraph);
      
      // exit edge
      eProps.cost = C_ex_i * 1000;
      eProps.capacity = 1;
      add_edge(v_i, sinkVertex, eProps, mGraph);

      // make transition edges later. 
    }

  }
  
  wxTimeSpan diff = mCore->mDataStructureInput.FrameTimestamp().Subtract(mStartTimestamp);
  if (diff.IsLongerThan(mWindowSize)) {
    ProcessWindow();
    mStartTimestamp = mCore->mDataStructureInput.FrameTimestamp();
  }

  mCore->mDataStructureTracks.mTracks = &mTracks;
  mCore->mDataStructureParticles.mParticles = &mParticles;

  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {
    de.SetMainImage(mCore->mDataStructureInput.mImage);
    de.SetTrajectories(true);
  }
}
void THISCLASS::ProcessWindow() {
  cout << "Running min cost flow!" << endl;
  AddTransitionEdges();
  MinCostFlow::WriteGraphviz(mGraph, string("graph.dot"));

  double minCost = numeric_limits<double>::max();
  MinCostFlow::Graph minGraph;
  int minTrackCount;
  cout << "Running: " << mMaxTrackCount << endl;
  for (int i = 0; i < mMaxTrackCount; i++) {
    int trackCount = i + 1;
    

    mGraph[sourceVertex].net_supply = trackCount;
    mGraph[sinkVertex].net_supply = -trackCount;
    
    MinCostFlow::updateSourceAndSink(&mGraph, tSourceVertex, tSinkVertex);
    MinCostFlow::initializeGraph(&mGraph);
    MinCostFlow::WriteGraphviz(mGraph, string("graph.dot"));

    MinCostFlow::minCostFlow(&mGraph, tSourceVertex, tSinkVertex);
    double flowCost = MinCostFlow::CostOfFlow(mGraph);    
    cout << "Cost: " << flowCost << endl;
    if (flowCost < minCost) {
      minCost = flowCost;
      minGraph = mGraph;
      minTrackCount = trackCount;
    }
  }

  cout << "Tracks: " << minTrackCount << endl;
  cout << "Graph" << endl;

  OutputTracks(minGraph);

  ResetGraph();
}

void THISCLASS::OutputTracks(const MinCostFlow::Graph & graph) {
  cout << "Net supply: " << graph[sourceVertex].net_supply << endl;
  cout << "Net supply: " << graph[sinkVertex].net_supply << endl;

  graph_traits < MinCostFlow::Graph >::out_edge_iterator ei, ei_end, ej, ej_end;


  cout << "Outputting tracks." << endl;
  for (tie(ei, ei_end) = out_edges(sourceVertex, graph); ei != ei_end; ++ei) {
    if (graph[*ei].flow == 1) {
      int id = mNextTrackId++;

      mTracks[id] = Track(id);
      mTracks[id].SetMaxLength(-1);
      MinCostFlow::Graph::vertex_descriptor u_i, v_i;
      u_i = target(*ei, graph);
      while (u_i != sinkVertex) {
	mObservations[u_i].mID = id;



	mParticles.push_back(mObservations[u_i]);
	// We're deleting the reference to the color histogram since it's not needed any 
	// more by this component.  I don't want to copy it because it's big, and I'm too lazy to
	// implement reference counting.
	// Note that the particles have to appear in the order they appear in the track to make the output file work.
	mParticles.back().mColorModel = NULL;
	mTracks[id].AddPoint(mObservations[u_i].mCenter, mObservations[u_i].mFrameNumber); 
	assert(out_degree(u_i, graph) == 1);
	v_i = target(*(out_edges(u_i, graph).first), graph);
	for (tie(ej, ej_end) = out_edges(v_i, graph); ej != ej_end; ++ej) {
	  if (graph[*ej].flow == 1) {
	    u_i = target(*ej, graph);
	    break;
	  } else {
	    assert(graph[*ej].flow == 0);
	  }
	  
	}
      }
      cout << "Adding new track with length: " << mTracks[id].trajectory.size() << endl;
    } else {
      assert(graph[*ei].flow == 0);
    }
  }
  cout << "Have: " << mTracks.size() << " tracks." << endl;
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
	double C_i_j = - log(p_lnk);
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
	eProps.cost = C_i_j * 1000;
	eProps.capacity = 1;
	add_edge(v_i, u_j, eProps, mGraph);
	  
      }
    }
  }
}
  void THISCLASS::OnStepCleanup() {

}

void THISCLASS::OnStop() {
}

#endif // USE_BOOST
