#include "ComponentWriteParticleTraining.h"
#define THISCLASS ComponentWriteParticleTraining

THISCLASS::ComponentWriteParticleTraining(SwisTrackCore *stc):
		Component(stc, wxT("WriteParticleTraining")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureParticles));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentWriteParticleTraining() {
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
