#include "ComponentFilterParticles.h"
#define THISCLASS ComponentFilterParticles

#include "DisplayEditor.h"
#include "Utility.h"
#include <limits>
#include <iostream>
#include <set>

using namespace std;

THISCLASS::ComponentFilterParticles(SwisTrackCore * stc):
	Component(stc, wxT("FilterParticles")),
	mParticles(),
	mDisplayOutput(wxT("Output"), wxT("Particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	Initialize();
}

THISCLASS::~ComponentFilterParticles() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mMaxParticleArea = GetConfigurationDouble(wxT("MaxParticleArea"), 100);
	mMinParticleArea = GetConfigurationDouble(wxT("MinParticleArea"), 100);
}

void THISCLASS::OnStep() {
	mParticles.clear();
	for (auto particle : *mCore->mDataStructureParticles.mParticles) {
		if (particle.mArea < mMinParticleArea) continue;
		if (particle.mArea > mMaxParticleArea) continue;
		mParticles.push_back(particle);
	}

	mCore->mDataStructureParticles.mParticles = &mParticles;
	DisplayEditor de(&mDisplayOutput);
	if (! de.IsActive()) return;

	cv::Mat outputImage = mCore->mDataStructureInput.mImage.clone();
	for (auto particle : *mCore->mDataStructureParticles.mParticles)
		cv::circle(outputImage, cv::Point(particle.mCenter), 10, CV_RGB(255, 0, 0), 3, cv::LINE_AA, 0);

	for (auto particle : mParticles)
		cv::circle(outputImage, cv::Point(particle.mCenter), 10, CV_RGB(0, 255, 0), 3, cv::LINE_AA, 0);

	//de.SetParticles(&mParticles);
	de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}



