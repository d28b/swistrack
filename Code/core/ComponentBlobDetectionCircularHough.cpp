#include "ComponentBlobDetectionCircularHough.h"
#define THISCLASS ComponentBlobDetectionCircularHough

#include "DisplayEditor.h"
#include <algorithm>

THISCLASS::ComponentBlobDetectionCircularHough(SwisTrackCore * stc):
	Component(stc, wxT("BlobDetectionCircularHough")),
	mParticles(),
	mDisplayOutput(wxT("Output"), wxT("Circular particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionCircularHough() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	AccumulatorResolution = GetConfigurationInt(wxT("AccumulatorResolution"), 1);
	MinDist = GetConfigurationInt(wxT("MinDist"), 50.);
	EdgeDetectionThreshold = GetConfigurationInt(wxT("EdgeDetectionThreshold"), 200);
	CenterDetectionThreshold = GetConfigurationInt(wxT("CenterDetectionThreshold"), 10);
	SelectionBySize = GetConfigurationBool(wxT("SelectionBySize"), false);
	MinRadius = GetConfigurationInt(wxT("MinRadius"), 10);
	MaxRadius = GetConfigurationInt(wxT("MaxRadius"), 50);
	LimitNumberOfCircles = GetConfigurationBool(wxT("LimitNumberOfCircles"), false);
	MaxNumberOfCircles = GetConfigurationInt(wxT("MaxNumberOfCircles"), 20);

	// Check for errors in configurations
	if (AccumulatorResolution < 1) AccumulatorResolution = 1;
	if (AccumulatorResolution > 2) AccumulatorResolution = 2;
	if (MinDist < 1) MinDist = 1;
	if (EdgeDetectionThreshold < 1) EdgeDetectionThreshold = 1;
	if (CenterDetectionThreshold < 1) CenterDetectionThreshold = 1;
	if (MinRadius <= 0) MinRadius = 1;
	if (MaxRadius <= MinRadius) MaxRadius = MinRadius + 1;
	if (MaxNumberOfCircles <= 0) MaxNumberOfCircles = 1;

	if (! SelectionBySize) {
		MinRadius = 1;
		MaxRadius = 1000;
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Detect circles
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(inputImage, circles, cv::HOUGH_GRADIENT, AccumulatorResolution, MinDist, EdgeDetectionThreshold, CenterDetectionThreshold, MinRadius, MaxRadius);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);

	// We clear the output vector
	mParticles.clear();

	int useCircles = circles.size();
	if (LimitNumberOfCircles && useCircles > MaxNumberOfCircles) useCircles = MaxNumberOfCircles;

	for (int i = 0; i < useCircles; i++) {
		cv::Vec3f circle = circles[i];
		Particle particle;
		particle.mCenter.x = circle[0];
		particle.mCenter.y = circle[1];
		particle.mArea = M_PI * circle[2] * circle[2];
		mParticles.push_back(particle);
	}

	// Set these particles
	mCore->mDataStructureParticles.mParticles = &mParticles;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (! de.IsActive()) return;
	de.SetParticles(&mParticles);

	// Prepare a color preview
	cv::Mat previewImage;
	cv::Mat inputImages[3] = {inputImage, inputImage, inputImage};
	cv::merge(inputImages, 3, previewImage);

	for (int i = 0; i < useCircles; i++) {
		cv::Vec3f circle = circles[i];
		cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
		int radius = cvRound(circle[2]);
		cv::circle(previewImage, center, radius, cv::Scalar(255, 0, 0), 2);
	}

	de.SetMainImage(previewImage);
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
}

