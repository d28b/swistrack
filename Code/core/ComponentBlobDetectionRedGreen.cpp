#include "ComponentBlobDetectionRedGreen.h"
#define THISCLASS ComponentBlobDetectionRedGreen

#include "DisplayEditor.h"
#define PI 3.14159265358979
#include <algorithm>

THISCLASS::ComponentBlobDetectionRedGreen(SwisTrackCore *stc):
		Component(stc, wxT("BlobDetectionRedGreen")),
		mMaxNumberOfParticles(10), mMaxDistance(50),
		mColor1(2, 1, wxT("Color1"), wxT("Red"), wxT("Binary1"), wxT("Red: after thresholding")),
		mColor2(1, 2, wxT("Color2"), wxT("Green"), wxT("Binary2"), wxT("Green: after thresholding")),
		mParticles(),
		mDisplayOutput(wxT("Output"), wxT("Particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);
	AddDisplay(&mColor1.mDisplayColor);
	AddDisplay(&mColor1.mDisplayBinary);
	AddDisplay(&mColor2.mDisplayColor);
	AddDisplay(&mColor2.mDisplayBinary);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionRedGreen() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mMaxNumberOfParticles = GetConfigurationInt(wxT("mMaxNumberOfParticles"), 10);
	mMaxDistance = GetConfigurationDouble(wxT("MaxDistance"), 10.);
	mColor1.mThreshold = GetConfigurationInt(wxT("Color1_Threshold"), 32);
	mColor1.mSelectionByArea = GetConfigurationBool(wxT("Color1_SelectionByArea"), false);
	mColor1.mAreaMin = GetConfigurationInt(wxT("Color1_AreaMin"), 1);
	mColor1.mAreaMax = GetConfigurationInt(wxT("Color1_AreaMax"), 1000);
	mColor1.mSelectionByCompactness = GetConfigurationBool(wxT("Color1_SelectionByCompactness"), false);
	mColor1.mCompactnessMin = GetConfigurationDouble(wxT("Color1_CompactnessMin"), 1.);
	mColor1.mCompactnessMax = GetConfigurationDouble(wxT("Color1_CompactnessMax"), 1000.);
	mColor2.mThreshold = GetConfigurationInt(wxT("Color2_Threshold"), 32);
	mColor2.mSelectionByArea = GetConfigurationBool(wxT("Color2_SelectionByArea"), false);
	mColor2.mAreaMin = GetConfigurationInt(wxT("Color2_AreaMin"), 1);
	mColor2.mAreaMax = GetConfigurationInt(wxT("Color2_AreaMax"), 1000);
	mColor2.mSelectionByCompactness = GetConfigurationBool(wxT("Color2_SelectionByCompactness"), false);
	mColor2.mCompactnessMin = GetConfigurationDouble(wxT("Color2_CompactnessMin"), 1.);
	mColor2.mCompactnessMax = GetConfigurationDouble(wxT("Color2_CompactnessMax"), 1000.);

	// Check for stupid configurations
	if (mMaxNumberOfParticles < 1) {
		AddError(wxT("The maximum number of particles must be greater or equal to 1."));
	}

	if (mMaxDistance < 0) {
		AddError(wxT("The maximum distane must be equal to 0."));
	}

	if (mColor1.mAreaMin > mColor1.mAreaMax) {
		AddError(wxT("The minimum area must be smaller than the maximum area."));
	}

	if (mColor2.mAreaMin > mColor2.mAreaMax) {
		AddError(wxT("The minimum area must be smaller than the maximum area."));
	}

	if (mColor1.mCompactnessMin > mColor1.mCompactnessMax) {
		AddError(wxT("The minimum compactness must be small than the maximum compactness."));
	}

	if (mColor2.mCompactnessMin > mColor2.mCompactnessMax) {
		AddError(wxT("The minimum compactness must be small than the maximum compactness."));
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageColor.mImage;
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels != 3) {
		AddError(wxT("The input image is not a color image."));
		return;
	}
	if (memcmp(inputimage->channelSeq, "BGR", 3)) {
		AddWarning(wxT("The input image is not a BGR image. The result may be unexpected."));
	}

	// Detect the blobs of the two colors
	FindColorBlobs(inputimage, mColor1);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
	FindColorBlobs(inputimage, mColor2);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);

	// We clear the output vector
	mParticles.clear();

	// Match blobs of color 1 to blobs of color 2
	for (std::vector<Particle>::iterator i = mColor1.mParticles.begin(); i != mColor1.mParticles.end(); i++) {
		// Select blob of color 2 which is closest
		std::vector<Particle>::iterator k_min = mColor2.mParticles.end();
		float k_min_distance2 = pow((float)mMaxDistance, 2.0f);
		for (std::vector<Particle>::iterator k = mColor2.mParticles.begin(); k != mColor2.mParticles.end(); k++) {
			float distance2 = pow((*i).mCenter.x - (*k).mCenter.x, 2.0f) + pow((*i).mCenter.y - (*k).mCenter.y, 2.0f);

			if (distance2 < k_min_distance2) {
				k_min = k;
				k_min_distance2 = distance2;
			}
		}

		// Create particle with this combination of blobs
		if (k_min != mColor2.mParticles.end()) {
			Particle newparticle;
			newparticle.mArea = (*k_min).mArea + (*i).mArea;
			newparticle.mCenter.x = ((*k_min).mCenter.x + (*i).mCenter.x) * 0.5;
			newparticle.mCenter.y = ((*k_min).mCenter.y + (*i).mCenter.y) * 0.5;
			newparticle.mOrientation = atan2((*k_min).mCenter.y - (*i).mCenter.y, (*k_min).mCenter.x - (*i).mCenter.x);
			newparticle.mFrameNumber = mCore->mDataStructureInput.mFrameNumber;
			newparticle.mTimestamp = mCore->mDataStructureInput.FrameTimestamp();
			mParticles.push_back(newparticle);
		}
	}

	// Set these particles
	mCore->mDataStructureParticles.mParticles = &mParticles;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(&mParticles);
		de.SetMainImage(inputimage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
}

double THISCLASS::GetContourCompactness(const void* contour) {
	double l = cvArcLength(contour, CV_WHOLE_SEQ, 1);
	return fabs(12.56*cvContourArea(contour) / (l*l));
}

void THISCLASS::FindColorBlobs(IplImage *colorimage, cColor &color) {
	// Split the image into channels
	IplImage* imagechannels[3];
	imagechannels[0] = cvCreateImage(cvGetSize(colorimage), 8, 1);
	imagechannels[1] = cvCreateImage(cvGetSize(colorimage), 8, 1);
	imagechannels[2] = cvCreateImage(cvGetSize(colorimage), 8, 1);
	cvSplit(colorimage, imagechannels[0], imagechannels[1], imagechannels[2], NULL);

	// Subtraction
	cvSub(imagechannels[color.mChannelMain], imagechannels[color.mChannelSubtract], imagechannels[0]);

	// Set the color DisplayImage
	DisplayEditor de_color(&color.mDisplayColor);
	if (de_color.IsActive()) {
		de_color.SetMainImage(imagechannels[0]);
	}

	// Threshold the main channel
	cvThreshold(imagechannels[0], imagechannels[0], color.mThreshold, 255, CV_THRESH_BINARY_INV);

	// Set the binary DisplayImage
	DisplayEditor de_binary(&color.mDisplayBinary);
	if (de_binary.IsActive()) {
		de_binary.SetMainImage(imagechannels[0]);
	}

	// Do blob detection
	FindBlobs(imagechannels[0], color);

	// Fill the particles display images
	if (de_color.IsActive()) {
		de_color.SetParticles(&color.mParticles);
	}
	if (de_binary.IsActive()) {
		de_binary.SetParticles(&color.mParticles);
	}

	// Release temporary images
	cvReleaseImage(&imagechannels[0]);
	cvReleaseImage(&imagechannels[1]);
	cvReleaseImage(&imagechannels[2]);
}

void THISCLASS::FindBlobs(IplImage *inputimage, cColor &color) {
	// Init blob extraxtion
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvContourScanner blobs = cvStartFindContours(inputimage, storage, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	DataStructureParticles::tParticleVector particles;

	// Iterate over blobs
	while (1) {
		// Get next contour (if one exists)
		CvSeq* contour = cvFindNextContour(blobs);
		if (! contour) {
			break;
		}

		// Compute the moments
		CvMoments moments;
		cvMoments(contour, &moments);

		// Compute particle position
		Particle newparticle;
		newparticle.mArea = moments.m00;
		newparticle.mCenter.x = (float)(moments.m10 / moments.m00 + 0.5);  // moments using Green theorem
		newparticle.mCenter.y = (float)(moments.m01 / moments.m00 + 0.5);  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem

		// Selection based on area
		if ((color.mSelectionByArea == false) || ((newparticle.mArea <= color.mAreaMax) && (newparticle.mArea >= color.mAreaMin))) {
			newparticle.mCompactness = GetContourCompactness(contour);
			if ((color.mSelectionByCompactness == false) || ((newparticle.mCompactness > color.mCompactnessMin) && (newparticle.mCompactness < color.mCompactnessMax))) {
				double centralmoment = cvGetCentralMoment(&moments, 2, 0) - cvGetCentralMoment(&moments, 0, 2);
				newparticle.mOrientation = atan(2 * cvGetCentralMoment(&moments, 1, 1) / (centralmoment + sqrt(centralmoment * centralmoment + 4 * cvGetCentralMoment(&moments, 1, 1) * cvGetCentralMoment(&moments, 1, 1))));

				// Fill unused values
				newparticle.mID = -1;
				newparticle.mIDCovariance = -1;

				// Add the particle
				particles.push_back(newparticle);
			}
		}

		// Release the contour
		cvRelease((void**)&contour);
	}

	// Finalize blob extraction
	cvEndFindContours(&blobs);
	cvReleaseMemStorage(&storage);

	// Sort the particles
	std::sort(particles.begin(), particles.end(), Particle::CompareArea);

	// We clear the output vector
	color.mParticles.clear();

	// Keep only the largest particles, and only the biggest particle within a certain range
	float mindistance2 = pow((float)mMaxDistance, 2.0f) * 0.5;
	std::vector<Particle>::iterator i1;
	for (i1 = particles.begin(); i1 != particles.end(); i1++) {
		// Check if there is a particle nearby already
		std::vector<Particle>::iterator i2 = i1;
		for (i2 = color.mParticles.begin(); i2 != color.mParticles.end(); i2++) {
			float distance2 = pow(i1->mCenter.x - i2->mCenter.x, 2.0f) + pow(i1->mCenter.y - i2->mCenter.y, 2.0f);
			if (distance2 < mindistance2) {
				break;
			}
		}

		// If not, add the particle and check if we have enough
		if (i2 == color.mParticles.end()) {
			color.mParticles.push_back(*i1);
			if (color.mParticles.size() > mMaxNumberOfParticles) {
				break;
			}
		}
	}
}
