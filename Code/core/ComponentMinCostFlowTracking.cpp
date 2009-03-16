#include "ComponentMinCostFlowTracking.h"
#define THISCLASS ComponentMinCostFlowTracking

#include "DisplayEditor.h"
#include "Utility.h"
#include <iostream>
#include <set>
#include "MinCostFlow.h"

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
	exit(-1);
}

THISCLASS::~ComponentMinCostFlowTracking()
{
}

void THISCLASS::OnStart()
{
	THISCLASS::OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()
{
}

void THISCLASS::OnStep()
{

}

void THISCLASS::OnStepCleanup() {

}

void THISCLASS::OnStop() {


}
