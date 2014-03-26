#include "ComponentOutputImageStatisticsGray.h"
#define THISCLASS ComponentOutputImageStatisticsGray

#include "DisplayEditor.h"

THISCLASS::ComponentOutputImageStatisticsGray(SwisTrackCore *stc):
		Component(stc, wxT("OutputImageStatisticsGray")),
		mCalculateNonZero(true), mCalculateSum(true), mCalculateMeanStdDev(true), mCalculateMinMax(true),
		mDisplayOutput(wxT("Output"), wxT("Image Statistics: Input Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputImageStatisticsGray() {
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
	IplImage* inputimage = mCore->mDataStructureImageGray.mImage;
	if (! inputimage) {
		AddError(wxT("No image on selected input."));
		return;
	}

	// Calculate non-zero elements
	if (mCalculateNonZero) {
		int non_zero= cvCountNonZero(inputimage);
		CommunicationMessage m(wxT("STATS_NONZERO"));
		m.AddInt(non_zero);
		mCore->mCommunicationInterface->Send(&m);
	}

	// Calculate sum
	if (mCalculateSum) {
		CvScalar sum= cvSum(inputimage);
		CommunicationMessage m(wxT("STATS_SUM"));
		m.AddDouble(sum.val[0]);
		mCore->mCommunicationInterface->Send(&m);
	}

	// Calculate mean and standard deviation
	if (mCalculateMeanStdDev) {
		CvScalar mean;
		CvScalar std_dev;
		cvAvgSdv(inputimage, &mean, &std_dev, NULL);
		CommunicationMessage m(wxT("STATS_MEANSTDDEV"));
		m.AddDouble(mean.val[0]);
		m.AddDouble(std_dev.val[0]);
		mCore->mCommunicationInterface->Send(&m);
	}

	// Calculate min and max
	if (mCalculateMinMax) {
		double min_val;
		double max_val;
		cvMinMaxLoc(inputimage, &min_val, &max_val, NULL, NULL, NULL);
		CommunicationMessage m(wxT("STATS_MINMAX"));
		m.AddDouble(min_val);
		m.AddDouble(max_val);
		mCore->mCommunicationInterface->Send(&m);
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(inputimage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
