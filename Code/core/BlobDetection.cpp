#include "BlobDetection.h"
#define THISCLASS BlobDetection

THISCLASS::BlobDetection():
	mThreshold(32), mMaxNumber(0),
	mSelectionByArea(false), mMinArea(0), mMaxArea(1000),
	mSelectionByCompactness(false), mMinCompactness(0), mMaxCompactness(1000),
	mSelectionByOrientation(false), mMinOrientation(0), mMaxOrientation(1000),
	mSelectedContours(), mParticles() {
}

void THISCLASS::ReadConfiguration(Component * component, const wxString prefix) {
	mThreshold = component->GetConfigurationInt(prefix + wxT("Threshold"), 32);
	mMaxNumber = component->GetConfigurationInt(prefix + wxT("MaxNumber"), 10);
	mSelectionByArea = component->GetConfigurationBool(prefix + wxT("SelectionByArea"), false);
	mMinArea = component->GetConfigurationInt(prefix + wxT("MinArea"), 1);
	mMaxArea = component->GetConfigurationInt(prefix + wxT("MaxArea"), 1000);
	mSelectionByCompactness = component->GetConfigurationBool(prefix + wxT("SelectionByCompactness"), false);
	mMinCompactness = component->GetConfigurationDouble(prefix + wxT("MinCompactness"), 1);
	mMaxCompactness = component->GetConfigurationDouble(prefix + wxT("MaxCompactness"), 1000);
	mSelectionByOrientation = component->GetConfigurationBool(prefix + wxT("SelectionByOrientation"), false);
	mMinOrientation = component->GetConfigurationDouble(prefix + wxT("MinOrientation"), -90);
	mMaxOrientation = component->GetConfigurationDouble(prefix + wxT("MaxOrientation"), 90);

	if (mMaxNumber < 0) mMaxNumber = 0;
	if (mMinArea > mMaxArea) component->AddError(wxT("The area selection is empty."));
	if (mMinCompactness > mMaxCompactness) component->AddError(wxT("The area selection is empty."));
}

void THISCLASS::FindBlobs(cv::Mat inputImage, wxDateTime frameTimestamp, int frameNumber) {
	// Blob extraction
	cv::Point offset(0, 0);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(inputImage, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE, offset);

	// Select blobs
	mParticles.clear();
	mSelectedContours.clear();
	if (hierarchy.size() < 1) return;

	for (int child = hierarchy[0][FIRST_CHILD]; child >= 0; child = hierarchy[child][NEXT_SIBLING]) {
		auto & contour = contours[child];
		Particle particle;
		particle.mID = -1;
		particle.mIDCovariance = -1;

		// Particle area
		cv::Moments moments = cv::moments(contour);
		//printf("contour %f\n", moments.m00);
		particle.mArea = moments.m00;
		particle.mCenter.x = (float)(offset.x + (moments.m10 / moments.m00 + 0.5));  // moments using Green theorem
		particle.mCenter.y = (float)(offset.y + (moments.m01 / moments.m00 + 0.5));  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem
		particle.mFrameTimestamp = frameTimestamp;
		particle.mFrameNumber = frameNumber;

		// Selection by area
		if (mSelectionByArea && (particle.mArea < mMinArea || particle.mArea > mMaxArea)) continue;

		// Selection by compactness
		double arcLength = cv::arcLength(contour, true);
		particle.mCompactness = fabs(12.56 * cv::contourArea(contour) / (arcLength * arcLength));
		if (mSelectionByCompactness && (particle.mCompactness < mMinCompactness || particle.mCompactness > mMaxCompactness)) continue;

		// Orientation
		if (moments.mu20 == moments.mu02) {
			if (moments.mu11 == 0) particle.mOrientation = 0;
			else if (moments.mu11 > 0) particle.mOrientation = M_PI / 4;
			else particle.mOrientation = -M_PI / 4;
		} else if (moments.mu20 > moments.mu02) {
			particle.mOrientation = atan(2 * moments.mu11 / (moments.mu20 - moments.mu02)) / 2;
		} else {
			particle.mOrientation = atan(2 * moments.mu11 / (moments.mu20 - moments.mu02)) / 2 + M_PI / 2;
		}

		// Selection by orientation
		if (mSelectionByOrientation) {
			double orientation = particle.mOrientation - mMinOrientation;
			while (orientation < 0) orientation += M_PI * 2;
			while (orientation >= M_PI * 2) orientation -= M_PI * 2;
			if (orientation > mMaxOrientation) continue;
		}

		mParticles.push_back(particle);
		mSelectedContours.push_back(contour);
	}

	// Sort by area
	std::sort(mParticles.begin(), mParticles.end(), Particle::CompareArea);

	// Keep the mMaxNumber biggest particles only
	if (mMaxNumber)
		while (mParticles.size() > mMaxNumber)
			mParticles.pop_back();
}

void THISCLASS::DrawContours(cv::Mat outputImage) {
	cv::Scalar color(255, 255, 255);
	for (unsigned int i = 0; i < mSelectedContours.size(); i++) {
		auto contour = mSelectedContours[i];
		cv::drawContours(outputImage, mSelectedContours, i, color, cv::FILLED);
	}
}
