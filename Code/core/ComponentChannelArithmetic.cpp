#include "ComponentChannelArithmetic.h"
#define THISCLASS ComponentChannelArithmetic

#include <opencv2/highgui.hpp>
#include "DisplayEditor.h"


THISCLASS::ComponentChannelArithmetic(SwisTrackCore * stc):
	Component(stc, wxT("ChannelArithmetic")),
	mExpression(NULL),
	mDisplayOutput(wxT("Output"), wxT("After channel arithmetic")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentChannelArithmetic() {
	if (mExpression) delete mExpression;
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	wxString expression = GetConfigurationString(wxT("Expression"), wxT(""));
	if (mExpression) delete mExpression;

	mExpression = new Expression();
	mRed = mExpression->CreateNode("R");
	mGreen = mExpression->CreateNode("G");
	mBlue = mExpression->CreateNode("B");
	mError = wxString(mExpression->Parse(expression.ToStdString()));
	mResult = mExpression->GetOrCreateNode("out");
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input Image"));
		return;
	}

	// Split into channels
	cv::Mat channels[3];
	cv::split(inputImage, channels);

	// Execute
	mRed->Set(channels[0]);
	mGreen->Set(channels[1]);
	mBlue->Set(channels[2]);
	mExpression->ResetExecution();
	mResult->Execute();

	// Set the output
	cv::Mat outputImage = mResult->GetMatrix();
	mCore->mDataStructureImageGray.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

