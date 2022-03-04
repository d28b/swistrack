#include "ComponentOutputImageStatisticsColor.h"
#define THISCLASS ComponentOutputImageStatisticsColor

#include "DisplayEditor.h"

THISCLASS::ComponentOutputImageStatisticsColor(SwisTrackCore * stc):
	Component(stc, wxT("OutputImageStatisticsColor")),
	mCalculateNonZero(true), mCalculateSum(true), mCalculateMeanStdDev(true), mCalculateMinMax(true),
	mDisplayOutput(wxT("Output"), wxT("Image Statistics: Input Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputImageStatisticsColor() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mCalculateNonZero = GetConfigurationBool(wxT("CalculateNonZero"), true);
	mCalculateSum = GetConfigurationBool(wxT("CalculateSum"), true);
	mCalculateMeanStdDev = GetConfigurationBool(wxT("CalculateMeanStdDev"), true);
	mCalculateMinMax = GetConfigurationBool(wxT("CalculateMinMax"), true);
}

void THISCLASS::OnStep() {
	// Get the input image
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No image on selected input."));
		return;
	}

	// Split into channels if necessary
	cv::Mat channels[3];
	if (mCalculateNonZero || mCalculateMinMax)
		cv::split(inputImage, channels);

	// Calculate non-zero elements
	if (mCalculateNonZero) {
		CommunicationMessage m(wxT("NONZERO"));
		for (int i = 0; i < 3; i++)
			m.AddInt(cv::countNonZero(channels[i]));
		mCore->mCommunicationInterface->Send(&m);
	}

	// Calculate sum
	if (mCalculateSum) {
		cv::Scalar sum = cv::sum(inputImage);
		CommunicationMessage m(wxT("SUM"));
		m.AddDouble(sum.val[0]);
		m.AddDouble(sum.val[1]);
		m.AddDouble(sum.val[2]);
		mCore->mCommunicationInterface->Send(&m);
	}

	// Calculate mean and standard deviation
	if (mCalculateMeanStdDev) {
		cv::Scalar mean;
		cv::Scalar stdDev;
		cv::meanStdDev(inputImage, mean, stdDev);
		CommunicationMessage m(wxT("MEANSTDDEV"));
		m.AddDouble(mean.val[0]);
		m.AddDouble(stdDev.val[0]);
		m.AddDouble(mean.val[1]);
		m.AddDouble(stdDev.val[1]);
		m.AddDouble(mean.val[2]);
		m.AddDouble(stdDev.val[2]);
		mCore->mCommunicationInterface->Send(&m);
	}

	// Calculate min and max
	if (mCalculateMinMax) {
		double minValue;
		double maxValue;
		cv::Point minLocation;
		cv::Point maxLocation;
		CommunicationMessage m(wxT("MINMAX"));
		for (int i = 0; i < 3; i++) {
			cv::minMaxLoc(channels[i], &minValue, &maxValue, &minLocation, &maxLocation);
			m.AddDouble(minValue);
			m.AddInt(minLocation.x);
			m.AddInt(minLocation.y);
			m.AddDouble(maxValue);
			m.AddInt(maxLocation.x);
			m.AddInt(maxLocation.y);
		}

		mCore->mCommunicationInterface->Send(&m);
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(inputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
