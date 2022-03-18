#include "ComponentOutputParticlesToFile.h"
#define THISCLASS ComponentOutputParticlesToFile
using namespace std;
#include <iostream>
#include <errno.h>
#include <fstream>
#include "DisplayEditor.h"
#include "Utility.h"

THISCLASS::ComponentOutputParticlesToFile(SwisTrackCore * stc):
	Component(stc, wxT("OutputParticlesToFile")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureParticles));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputParticlesToFile() {
}

void THISCLASS::OnStart() {
	wxFileName filename = mCore->GetRunFileName("particles.csv");
	mFile.open(filename.GetFullPath().ToStdString(), std::fstream::out | std::fstream::trunc);
	if (! mFile.is_open()) {
		AddError(wxT("Unable to open 'particles.csv'."));
		return;
	}

	mFile
		<< "% Frame number" << "\t"
		<< "Time" << "\t"
		<< "X (image)" << "\t" << "Y (image)" << "\t"
		<< "X (world)" << "\t" << "Y (world)" << "\t"
		<< "Area" << "\t"
		<< "Orientation" << "\t"
		<< "Compactness" << "\t"
		<< std::endl;
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	auto particles = mCore->mDataStructureParticles.mParticles;
	if (! particles) {
		AddError(wxT("No particles."));
		return;
	}

	// Write all particles
	for (auto & particle : *particles) {
		int ms = particle.mFrameTimestamp.GetMillisecond();
		char d3 = '0' + (ms % 10);
		ms /= 10;
		char d2 = '0' + (ms % 10);
		ms /= 10;
		char d1 = '0' + (ms % 10);

		mFile
	        << particle.mFrameNumber << "\t"
	        << particle.mFrameTimestamp.FormatISOCombined().ToStdString() << '.' << d1 << d2 << d3 << "\t"
	        << particle.mCenter.x << "\t" << particle.mCenter.y << "\t"
	        << particle.mWorldCenter.x << "\t" << particle.mWorldCenter.y << "\t"
	        << particle.mArea << "\t"
	        << particle.mOrientation << "\t"
	        << particle.mCompactness
	        << std::endl;
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	mFile.close();
}
