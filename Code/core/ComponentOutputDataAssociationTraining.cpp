#include "ComponentOutputDataAssociationTraining.h"
#define THISCLASS ComponentOutputDataAssociationTraining

THISCLASS::ComponentOutputDataAssociationTraining(SwisTrackCore *stc):
		Component(stc, wxT("OutputDataAssociationTraining")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureParticles));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputDataAssociationTraining() {
}

void THISCLASS::OnStart() {
  mFileName = GetConfigurationString(wxT("FileName"), wxT(""));
}

void THISCLASS::OnStep() {

}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
