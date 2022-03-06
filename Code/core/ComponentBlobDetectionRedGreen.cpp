#include "ComponentBlobDetectionRedGreen.h"
#define THISCLASS ComponentBlobDetectionRedGreen

#include "DisplayEditor.h"
#include <algorithm>

THISCLASS::ComponentBlobDetectionRedGreen(SwisTrackCore * stc):
	Component(stc, wxT("BlobDetectionRedGreen")),
	mColorRed(wxT("ColorRed"), wxT("Red"), wxT("BinaryRed"), wxT("Red after thresholding")),
	mColorGreen(wxT("ColorGreen"), wxT("Green"), wxT("BinaryGreen"), wxT("Green after thresholding")),
	mBlobMatching(),
	mDisplayOutput(wxT("Output"), wxT("Particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);
	AddDisplay(&mColorRed.mDisplayColor);
	AddDisplay(&mColorRed.mDisplayBinary);
	AddDisplay(&mColorGreen.mDisplayColor);
	AddDisplay(&mColorGreen.mDisplayBinary);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionRedGreen() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mColorRed.mThreshold = GetConfigurationInt(wxT("Color1_Threshold"), 20);
	mColorGreen.mThreshold = GetConfigurationInt(wxT("Color2_Threshold"), 20);

	mColorRed.mBlobDetection.ReadConfiguration(this, wxT("Color1_"));
	mColorGreen.mBlobDetection.ReadConfiguration(this, wxT("Color2_"));

	mBlobMatching.ReadConfiguration(this);
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Split the image into channels
	cv::Mat channels[3];
	cv::split(inputImage, channels);
	cv::Mat redChannel = channels[2] - channels[1];
	cv::Mat greenChannel = channels[1] - channels[2];
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);

	// Detect the blobs of the two colors
	mColorRed.FindColorBlobs(redChannel, mCore->mDataStructureInput.mFrameTimestamp, mCore->mDataStructureInput.mFrameNumber);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
	mColorGreen.FindColorBlobs(greenChannel, mCore->mDataStructureInput.mFrameTimestamp, mCore->mDataStructureInput.mFrameNumber);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);

	// Match blobs of color 1 to blobs of color 2
	mBlobMatching.Match(mColorRed.mBlobDetection.mParticles, mColorGreen.mBlobDetection.mParticles);

	// Set these particles
	mCore->mDataStructureParticles.mParticles = &mBlobMatching.mParticles;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(&mBlobMatching.mParticles);
		de.SetMainImage(inputImage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

void THISCLASS::Color::FindColorBlobs(cv::Mat channel, wxDateTime frameTimestamp, int frameNumber) {
	// Set the color DisplayImage
	DisplayEditor deColor(&mDisplayColor);
	if (deColor.IsActive()) deColor.SetMainImage(channel);

	// Threshold the main channel
	cv::Mat thresholded;
	cv::threshold(channel, thresholded, mThreshold, 255, cv::THRESH_BINARY_INV);

	// Set the binary DisplayImage
	DisplayEditor deBinary(&mDisplayBinary);
	if (deBinary.IsActive()) deBinary.SetMainImage(thresholded);

	// Do blob detection
	mBlobDetection.FindBlobs(thresholded, frameTimestamp, frameNumber);

	// Fill the particles display images
	if (deColor.IsActive()) deColor.SetParticles(&mBlobDetection.mParticles);
	if (deBinary.IsActive()) deBinary.SetParticles(&mBlobDetection.mParticles);
}
