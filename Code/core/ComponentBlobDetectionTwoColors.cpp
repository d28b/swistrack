#include "ComponentBlobDetectionTwoColors.h"
#define THISCLASS ComponentBlobDetectionTwoColors

#include "DisplayEditor.h"
#define PI 3.14159265358979

THISCLASS::ComponentBlobDetectionTwoColors(SwisTrackCore *stc):
		Component(stc, wxT("BlobDetectionTwoColors")),
		mMaxNumberOfParticles(10), mMaxDistance(10),mOutputParams(wxT("avg")),
		mColor1(0x0000ff, wxT("Color1"), wxT("After subtraction of color 1"), wxT("Binary1"), wxT("After subtraction of color 1 and thresholding")),
		mColor2(0xff0000, wxT("Color2"), wxT("After subtraction of color 2"), wxT("Binary2"), wxT("After subtraction of color 2 and thresholding")),
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

THISCLASS::~ComponentBlobDetectionTwoColors() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mMaxNumberOfParticles = GetConfigurationInt(wxT("mMaxNumberOfParticles"), 10);
	mMaxDistance = GetConfigurationDouble(wxT("MaxDistance"), 10.);
	mOutputParams = GetConfigurationString(wxT("OutputParams"), wxT("avg"));
	mColor1.mColor = GetConfigurationInt(wxT("Color1_Color"), 0xff);
	mColor1.mThresholdB = GetConfigurationInt(wxT("Color1_ThresholdB"), 32);
	mColor1.mThresholdG = GetConfigurationInt(wxT("Color1_ThresholdG"), 32);
	mColor1.mThresholdR = GetConfigurationInt(wxT("Color1_ThresholdR"), 32);
	mColor1.mSelectionByArea = GetConfigurationBool(wxT("Color1_SelectionByArea"), false);
	mColor1.mAreaMin = GetConfigurationInt(wxT("Color1_AreaMin"), 1);
	mColor1.mAreaMax = GetConfigurationInt(wxT("Color1_AreaMax"), 1000);
	mColor1.mSelectionByCompactness = GetConfigurationBool(wxT("Color1_SelectionByCompactness"), false);
	mColor1.mCompactnessMin = GetConfigurationDouble(wxT("Color1_CompactnessMin"), 1.);
	mColor1.mCompactnessMax = GetConfigurationDouble(wxT("Color1_CompactnessMax"), 1000.);
	mColor2.mColor = GetConfigurationInt(wxT("Color2_Color"), 0xff0000);
	mColor2.mThresholdB = GetConfigurationInt(wxT("Color2_ThresholdB"), 32);
	mColor2.mThresholdG = GetConfigurationInt(wxT("Color2_ThresholdG"), 32);
	mColor2.mThresholdR = GetConfigurationInt(wxT("Color2_ThresholdR"), 32);
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
			if (mOutputParams == wxT("blob1")) { // blob #1
				newparticle.mArea = (*i).mArea;
				newparticle.mCenter.x = (*i).mCenter.x;
				newparticle.mCenter.y = (*i).mCenter.y;
				newparticle.mCompactness = (*i).mCompactness;
				newparticle.mOrientation = (*i).mOrientation;
			}
			else if (mOutputParams == wxT("blob2")) { // blob #2
				newparticle.mArea = (*k_min).mArea;
				newparticle.mCenter.x = (*k_min).mCenter.x;
				newparticle.mCenter.y = (*k_min).mCenter.y;
				newparticle.mCompactness = (*k_min).mCompactness;
				newparticle.mOrientation = (*k_min).mOrientation;
			}
			else if (mOutputParams == wxT("avg")) { // combination
				newparticle.mArea = (*k_min).mArea + (*i).mArea;
				newparticle.mCenter.x = ((*k_min).mCenter.x + (*i).mCenter.x) * 0.5;
				newparticle.mCenter.y = ((*k_min).mCenter.y + (*i).mCenter.y) * 0.5;
				newparticle.mCompactness = ((*k_min).mCompactness + (*i).mCompactness) * 0.5;
				newparticle.mOrientation = atan2((*k_min).mCenter.y - (*i).mCenter.y, (*k_min).mCenter.x - (*i).mCenter.x);
			}
			else AddError(wxT("Invalid Output Parameter Type"));

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
	// Take a copy of the input image
	IplImage *inputimage = cvCloneImage(colorimage);

	// Subtract the color
	CvScalar color_scalar;
	color_scalar.val[0] = (double)((color.mColor & 0xff0000) >> 16);
	color_scalar.val[1] = (double)((color.mColor & 0xff00) >> 8);
	color_scalar.val[2] = (double)(color.mColor & 0xff);
	cvAbsDiffS(inputimage, inputimage, color_scalar);

	// Split the image into channels
	IplImage* imagechannels[3];
	imagechannels[0] = cvCreateImage(cvGetSize(inputimage), 8, 1);
	imagechannels[1] = cvCreateImage(cvGetSize(inputimage), 8, 1);
	imagechannels[2] = cvCreateImage(cvGetSize(inputimage), 8, 1);
	cvSplit(inputimage, imagechannels[0], imagechannels[1], imagechannels[2], NULL);

	// Threshold each channel
	cvThreshold(imagechannels[0], imagechannels[0], color.mThresholdB, 255, CV_THRESH_BINARY_INV);
	cvThreshold(imagechannels[1], imagechannels[1], color.mThresholdG, 255, CV_THRESH_BINARY_INV);
	cvThreshold(imagechannels[2], imagechannels[2], color.mThresholdR, 255, CV_THRESH_BINARY_INV);

	// Combine channels (ch0 & ch1 -> ch0, ch0 & ch2 -> ch0)
	cvAnd(imagechannels[0], imagechannels[1], imagechannels[0]);
	cvAnd(imagechannels[0], imagechannels[2], imagechannels[0]);

	// Set the binary DisplayImage
	DisplayEditor de_binary(&color.mDisplayBinary);
	if (de_binary.IsActive()) {
		de_binary.SetMainImage(imagechannels[0]);
	}

	// Do blob detection
	FindBlobs(imagechannels[0], color);

	// Set the color DisplayImage
	DisplayEditor de_color(&color.mDisplayColor);
	if (de_color.IsActive()) {
		de_color.SetParticles(&color.mParticles);
		de_color.SetMainImage(inputimage);
	}

	// Fill the particles of the binary display image
	if (de_binary.IsActive()) {
		de_binary.SetParticles(&color.mParticles);
	}

	// Release temporary images
	cvReleaseImage(&inputimage);
	cvReleaseImage(&imagechannels[0]);
	cvReleaseImage(&imagechannels[1]);
	cvReleaseImage(&imagechannels[2]);
}

void THISCLASS::FindBlobs(IplImage *inputimage, cColor &color) {
	// Init blob extraxtion
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvContourScanner blobs = cvStartFindContours(inputimage, storage, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	// We clear the output vector
	color.mParticles.clear();

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
//				double centralmoment = cvGetCentralMoment(&moments, 2, 0) - cvGetCentralMoment(&moments, 0, 2);
//				newparticle.mOrientation = atan(2 * cvGetCentralMoment(&moments, 1, 1) / (centralmoment + sqrt(centralmoment * centralmoment + 4 * cvGetCentralMoment(&moments, 1, 1) * cvGetCentralMoment(&moments, 1, 1))));

  			// compute orientation from moments
  			if (moments.mu20 == moments.mu02) {
  				if (moments.mu11 == 0) newparticle.mOrientation = 0;
  				else if (moments.mu11 > 0) newparticle.mOrientation = PI/4;
  				else newparticle.mOrientation = -PI/4;
  			}
  			else if (moments.mu20 > moments.mu02)
  				newparticle.mOrientation = atan(2*moments.mu11 / (moments.mu20-moments.mu02))/2;
  			else
  				newparticle.mOrientation = atan(2*moments.mu11 / (moments.mu20-moments.mu02))/2 + PI/2;
				
				// Fill unused values
				newparticle.mID = -1;
				newparticle.mIDCovariance = -1;

				// Insert the particle at the right place, such that the list remains sorted (note that one could use a heap here to lower the complexity)
				std::vector<Particle>::iterator j;
				for (j = color.mParticles.begin(); (j != color.mParticles.end()) && (newparticle.mArea < (*j).mArea); j++);
				color.mParticles.insert(j, newparticle);

				// Remove particles if we have too many of them
				while (color.mParticles.size() > mMaxNumberOfParticles) {
					// Remove the smallest one
					color.mParticles.pop_back();
				}
			}
		}

		// Release the contour
		cvRelease((void**)&contour);
	}

	// Finalize blob extraction
	cvEndFindContours(&blobs);
	cvReleaseMemStorage(&storage);
}
