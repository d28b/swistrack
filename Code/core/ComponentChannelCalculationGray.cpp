#include "ComponentChannelCalculationGray.h"
#define THISCLASS ComponentChannelCalculationGray

#include <opencv2/core.hpp>
#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentChannelCalculationGray(SwisTrackCore * stc):
	Component(stc, wxT("ChannelCalculationGray")),
	mContext(NULL),
	mDisplayOutput(wxT("Output"), wxT("After channel calculation")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentChannelCalculationGray() {
	if (mContext) delete mContext;
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	wxString expression = GetConfigurationString(wxT("Expression"), wxT(""));
	if (mContext) delete mContext;
	mOutput = NULL;

	mContext = new Expression::Context();
	mContext->AddNumber("pi", M_PI);
	mContext->AddNumber("e", M_E);
	mZeros = mContext->AddMatrix("zeros", cv::Mat());
	mOnes = mContext->AddMatrix("ones", cv::Mat());
	mBlack = mContext->AddMatrix("black", cv::Mat());
	mWhite = mContext->AddMatrix("white", cv::Mat());
	mInput = mContext->AddMatrix("C", cv::Mat());

	std::string parseError = mContext->Parse(expression.ToStdString());
	if (! parseError.empty())
		return AddError(wxT("Parse error: ") + parseError);

	mOutput = mContext->GetNode("out");
	if (! mOutput)
		return AddError(wxT("Missing output. Assign the result to 'out = ...'."));

	std::string typeError = mOutput->Prepare();
	if (! typeError.empty())
		return AddError(wxT("Type error: ") + typeError);
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageGray.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Set the channels
	mInput->Set(inputImage);

	// Update the size of the helper matrices if necessary
	if (! ImageTools::EqualSize(inputImage, mZeros->GetFloatMatrix())) {
		cv::Mat zeros = cv::Mat::zeros(inputImage.size(), CV_32FC1);
		mZeros->Set(zeros);
		mOnes->Set(zeros + 1);
		mBlack->Set(zeros);
		mWhite->Set(zeros + 255);
	}

	// Calculate
	mOutput->Execute();

	// Set the output
	cv::Mat outputImage = mOutput->GetMatrix();
	mCore->mDataStructureImageGray.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

