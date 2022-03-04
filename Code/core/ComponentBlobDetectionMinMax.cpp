#include "ComponentBlobDetectionMinMax.h"
#define THISCLASS ComponentBlobDetectionMinMax

#include "DisplayEditor.h"

THISCLASS::ComponentBlobDetectionMinMax(SwisTrackCore * stc):
	Component(stc, wxT("BlobDetectionMinMax")),
	mBlobDetection(),
	mDisplayOutput(wxT("Output"), wxT("Particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionMinMax() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mBlobDetection.ReadConfiguration(this, wxT(""));
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageBinary.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Find particles
	mBlobDetection.FindBlobs(inputImage, mCore->mDataStructureInput.mFrameTimestamp, mCore->mDataStructureInput.mFrameNumber);
	mCore->mDataStructureParticles.mParticles = &mBlobDetection.mParticles;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (! de.IsActive()) return;

	cv::Mat outputImage = cv::Mat::zeros(inputImage.rows, inputImage.cols, CV_8UC3);
	mBlobDetection.DrawContours(outputImage);

	de.SetParticles(&mBlobDetection.mParticles);
	de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
