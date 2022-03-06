#include "ComponentBlobDetectionTwoColors.h"
#define THISCLASS ComponentBlobDetectionTwoColors

#include "DisplayEditor.h"

THISCLASS::ComponentBlobDetectionTwoColors(SwisTrackCore * stc):
	Component(stc, wxT("BlobDetectionTwoColors")),
	mColor1(0x0000ff, wxT("Binary1"), wxT("After subtraction of color 1 and thresholding")),
	mColor2(0xff0000, wxT("Binary2"), wxT("After subtraction of color 2 and thresholding")),
	mBlobMatching(),
	mDisplayOutput(wxT("Output"), wxT("Particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);
	AddDisplay(&mColor1.mDisplayBinary);
	AddDisplay(&mColor2.mDisplayBinary);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionTwoColors() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mColor1.mColor = GetConfigurationColor(wxT("Color1_Color"), cv::Scalar(255, 0, 0));
	mColor1.mThresholdB = GetConfigurationInt(wxT("Color1_ThresholdB"), 32);
	mColor1.mThresholdG = GetConfigurationInt(wxT("Color1_ThresholdG"), 32);
	mColor1.mThresholdR = GetConfigurationInt(wxT("Color1_ThresholdR"), 32);
	mColor1.mBlobDetection.ReadConfiguration(this, wxT("Color1_"));

	mColor2.mColor = GetConfigurationColor(wxT("Color2_Color"), cv::Scalar(0, 0, 255));
	mColor2.mThresholdB = GetConfigurationInt(wxT("Color2_ThresholdB"), 32);
	mColor2.mThresholdG = GetConfigurationInt(wxT("Color2_ThresholdG"), 32);
	mColor2.mThresholdR = GetConfigurationInt(wxT("Color2_ThresholdR"), 32);
	mColor2.mBlobDetection.ReadConfiguration(this, wxT("Color2_"));

	mBlobMatching.ReadConfiguration(this);
}

void THISCLASS::OnStep() {
	// Get and check input image
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Detect the blobs of the two colors
	mColor1.FindColorBlobs(inputImage, mCore->mDataStructureInput.mFrameTimestamp, mCore->mDataStructureInput.mFrameNumber);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
	mColor2.FindColorBlobs(inputImage, mCore->mDataStructureInput.mFrameTimestamp, mCore->mDataStructureInput.mFrameNumber);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);

	// Match blobs of color 1 to blobs of color 2
	mBlobMatching.Match(mColor1.mBlobDetection.mParticles, mColor2.mBlobDetection.mParticles);

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

void THISCLASS::Color::FindColorBlobs(cv::Mat inputImage, wxDateTime frameTimestamp, int frameNumber) {
	int targetB = (int) mColor.val[0];
	int targetG = (int) mColor.val[1];
	int targetR = (int) mColor.val[2];

	// Create a distance image
	cv::Mat binaryImage(inputImage.rows, inputImage.cols, CV_8UC1);
	for (int y = 0; y < inputImage.rows; y++) {
		unsigned char * inputLine = inputImage.ptr(y);
		unsigned char * binaryLine = binaryImage.ptr(y);
		for (int x = 0; x < inputImage.cols; x++) {
			int b = inputLine[x * 3 + 0];
			int g = inputLine[x * 3 + 1];
			int r = inputLine[x * 3 + 2];
			int db = abs(b - targetB);
			int dg = abs(g - targetG);
			int dr = abs(r - targetR);
			binaryLine[x] = dr <= mThresholdR && dg <= mThresholdG && db <= mThresholdB ? 0 : 255;
		}
	}

	// Do blob detection
	mBlobDetection.FindBlobs(binaryImage, frameTimestamp, frameNumber);

	// Set the binary DisplayImage
	DisplayEditor deBinary(&mDisplayBinary);
	if (deBinary.IsActive()) {
		deBinary.SetParticles(&mBlobDetection.mParticles);
		deBinary.SetMainImage(binaryImage);
	}
}
