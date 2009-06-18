#include "ComponentBlobDetectionCircularHough.h"
#define THISCLASS ComponentBlobDetectionCircularHough

#include "DisplayEditor.h"
#define PI 3.14159265358979
#include <algorithm>


//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Current working code//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/*
THISCLASS::ComponentBlobDetectionCircularHough(SwisTrackCore *stc):
		Component(stc, wxT("BlobDetectionCircularHough")),
		MaxNumberOfCircles(10), mMaxDistance(50),
		mColor1(2, 1, wxT("Color1"), wxT("Red"), wxT("Binary1"), wxT("Red: after thresholding")),
		mColor2(1, 2, wxT("Color2"), wxT("Green"), wxT("Binary2"), wxT("Green: after thresholding")),
		mParticles(),
		mDisplayOutput(wxT("Output"), wxT("Particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionCircularHough() {}

void THISCLASS::OnStart() 
{
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() 
{
	MaxNumberOfCircles = GetConfigurationInt(wxT("MaxNumberOfCircles"), 10);
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
	if (MaxNumberOfCircles < 1)
		AddError(wxT("The maximum number of particles must be greater or equal to 1."));
	if (mMaxDistance < 0)
		AddError(wxT("The maximum distane must be equal to 0."));
	if (mColor1.mAreaMin > mColor1.mAreaMax)
		AddError(wxT("The minimum area must be smaller than the maximum area."));
	if (mColor2.mAreaMin > mColor2.mAreaMax)
		AddError(wxT("The minimum area must be smaller than the maximum area."));
	if (mColor1.mCompactnessMin > mColor1.mCompactnessMax)
		AddError(wxT("The minimum compactness must be small than the maximum compactness."));
	if (mColor2.mCompactnessMin > mColor2.mCompactnessMax)
		AddError(wxT("The minimum compactness must be small than the maximum compactness."));
}

void THISCLASS::OnStep() 
{
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	IplImage *outputimage = mCore->mDataStructureImageColor.mImage;	
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError(wxT("The input image is not a gray image."));
		return;
	}
	
	CvMemStorage* storage = cvCreateMemStorage(0);
    	CvSeq* circles = cvHoughCircles(inputimage, storage, 
        CV_HOUGH_GRADIENT, 1, inputimage->height/8, 200, 25,10,inputimage->width);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);


	// We clear the output vector
	mParticles.clear();

    	int i;
    	for (i = 0; i < circles->total; i++) 
    	{
        	float* p = (float*)cvGetSeqElem( circles, i );
		// cvCircle - draws circle(image, center, radius, color, thickness)        	 
		// x=cvRound(p[0])
		// y=cvRound(p[1])
		// radius=cvRound(p[2])
         	cvCircle( outputimage, cvPoint(cvRound(p[0]),cvRound(p[1])), 
             	cvRound(p[2]), CV_RGB(255,0,0), -1 );
		Particle newparticle;
		newparticle.mArea = 3.1415*p[2]*p[2];
		newparticle.mCenter.x = p[0];
		newparticle.mCenter.y = p[1];
		newparticle.mOrientation = 1;
		mParticles.push_back(newparticle);

        }
	// Set these particles
	mCore->mDataStructureParticles.mParticles = &mParticles;
	
	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(&mParticles);
		de.SetMainImage(outputimage);
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
			if (color.mParticles.size() > MaxNumberOfCircles) {
				break;
			}
		}
	}
}*/

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Prototipying code//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

THISCLASS::ComponentBlobDetectionCircularHough(SwisTrackCore *stc):
		Component(stc, wxT("BlobDetectionCircularHough")),
		mParticles(),
		mDisplayOutput(wxT("Output"), wxT("Particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionCircularHough() {}

void THISCLASS::OnStart() 
{
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() 
{
	// Acumulator resolution for Circular Hough Transform	
	dp = GetConfigurationInt(wxT("dp"), 1);
	// Minimum distance between circles
	MinDist = GetConfigurationInt(wxT("MinDist"), 50.);
	// Canny Edge Detection threshold
	Param1 = GetConfigurationInt(wxT("Param1"), 200);
	// Circle Center Detection threshold
	Param2 = GetConfigurationInt(wxT("Param2"), 10);
	// Thresholding circle radius
	SelectionBySize = GetConfigurationBool(wxT("SelectionBySize"), false);
	// Minimum circle radius
	MinRadius = GetConfigurationInt(wxT("MinRadius"), 10);
	// Maximum circle radius
	MaxRadius = GetConfigurationInt(wxT("MaxRadius"), 50);
	// Limit number of circles
	LimitNumberofCircles = GetConfigurationBool(wxT("LimitNumberofCircles"), false);
	// Maximum number of circles
	MaxNumberofCircles = GetConfigurationInt(wxT("MaxNumberofCircles"), 20);

	// Check for errors in configurations
	//if (dp != 1 || dp != 2)
	//	AddError(wxT("The acumulator resolution must be 1 or 2."));
	if (MinDist <= 0)
		MinDist = 1;
	if (Param1 <= 0)
		Param1 = 1;
	if (Param2 <= 0)
		Param2 = 1;
	if (MinRadius <= 0)
		MinRadius = 1;
	if (MaxRadius <= MinRadius)
		MaxRadius = MinRadius + 1;
	if (MaxNumberofCircles <= 0)
		MaxNumberofCircles = 1;
}

void THISCLASS::OnStep() 
{
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	IplImage *outputimage = mCore->mDataStructureImageColor.mImage;	
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError(wxT("The input image is not a gray image."));
		return;
	}
	if (!SelectionBySize)
	{
		MinRadius = 1;
		MaxRadius = 1000;
	}
	CvMemStorage* storage = cvCreateMemStorage(0);
    	CvSeq* circles = cvHoughCircles(inputimage, storage, 
        CV_HOUGH_GRADIENT, dp, MinDist, Param1, Param2,MinRadius,MaxRadius);
	mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);


	// We clear the output vector
	mParticles.clear();

    	int i;
	int j = 1;    	
	for (i = 0; i < circles->total; i++) 
    	{
        	float* p = (float*)cvGetSeqElem( circles, i );
		// cvCircle - draws circle(image, center, radius, color, thickness)        	 
		// x=cvRound(p[0])
		// y=cvRound(p[1])
		// radius=cvRound(p[2])
         	cvCircle( outputimage, cvPoint(cvRound(p[0]),cvRound(p[1])), 
             	cvRound(p[2]), CV_RGB(255,0,0), 2 );
		Particle newparticle;
		newparticle.mCenter.x = p[0];
		newparticle.mCenter.y = p[1];
		newparticle.mArea = 3.14159265*p[2]*p[2];
		if (LimitNumberofCircles)
		{
			if (j > MaxNumberofCircles)
				break;		
		}
		j++;
	}

	// Set these particles
	mCore->mDataStructureParticles.mParticles = &mParticles;
	
	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(&mParticles);
		de.SetMainImage(outputimage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
}
/*
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
			if (color.mParticles.size() > MaxNumberOfCircles) {
				break;
			}
		}
	}
}
*/
