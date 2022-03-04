#include "ComponentOutputImageStatisticsBinary.h"
#define THISCLASS ComponentOutputImageStatisticsBinary

#include "DisplayEditor.h"

THISCLASS::ComponentOutputImageStatisticsBinary(SwisTrackCore * stc):
	Component(stc, wxT("OutputImageStatisticsBinary")),
	mCalculateNonZero(true), mCalculateSum(true), mCalculateMeanStdDev(true), mCalculateMinMax(true),
	mDisplayOutput(wxT("Output"), wxT("Image Statistics: Input Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputImageStatisticsBinary() {
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
	cv::Mat inputImage = mCore->mDataStructureImageBinary.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No image on selected input."));
		return;
	}

	// Calculate non-zero elements
	if (mCalculateNonZero) {
		int nonZero = cv::countNonZero(inputImage);
		CommunicationMessage m(wxT("NONZERO"));
		m.AddInt(nonZero);
		mCore->mCommunicationInterface->Send(&m);
	}

	// Calculate sum
	if (mCalculateSum) {
		cv::Scalar sum = cv::sum(inputImage);
		CommunicationMessage m(wxT("SUM"));
		m.AddDouble(sum.val[0]);
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
		mCore->mCommunicationInterface->Send(&m);
	}

	// Calculate min and max
	if (mCalculateMinMax) {
		double minValue;
		double maxValue;
		cv::Point minLocation;
		cv::Point maxLocation;
		cv::minMaxLoc(inputImage, &minValue, &maxValue, &minLocation, &maxLocation);
		CommunicationMessage m(wxT("MINMAX"));
		m.AddDouble(minValue);
		m.AddInt(minLocation.x);
		m.AddInt(minLocation.y);
		m.AddDouble(maxValue);
		m.AddInt(maxLocation.x);
		m.AddInt(maxLocation.y);
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
