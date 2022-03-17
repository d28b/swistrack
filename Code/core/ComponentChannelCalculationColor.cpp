#include "ComponentChannelCalculationColor.h"
#define THISCLASS ComponentChannelCalculationColor

#include <opencv2/core.hpp>
#include "DisplayEditor.h"
#include "ImageTools.h"

THISCLASS::ComponentChannelCalculationColor(SwisTrackCore * stc):
	Component(stc, wxT("ChannelCalculationColor")),
	mContext(NULL),
	mDisplayOutput(wxT("Output"), wxT("After channel calculation")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryProcessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageColor));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentChannelCalculationColor() {
	if (mContext) delete mContext;
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	wxString expression = GetConfigurationString(wxT("Expression"), wxT(""));
	if (mContext) delete mContext;
	mOutputRed = NULL;
	mOutputGreen = NULL;
	mOutputBlue = NULL;

	mContext = new Expression::Context();
	mContext->AddNumber("pi", M_PI);
	mContext->AddNumber("e", M_E);
	mZeros = mContext->AddMatrix("zeros", cv::Mat());
	mOnes = mContext->AddMatrix("ones", cv::Mat());
	mBlack = mContext->AddMatrix("black", cv::Mat());
	mWhite = mContext->AddMatrix("white", cv::Mat());
	mRed = mContext->AddMatrix("R", cv::Mat());
	mGreen = mContext->AddMatrix("G", cv::Mat());
	mBlue = mContext->AddMatrix("B", cv::Mat());

	std::string parseError = mContext->Parse(expression.ToStdString());
	if (! parseError.empty())
		return AddError(wxT("Parse error: ") + parseError);

	mOutputRed = mContext->GetNode("Rout");
	mOutputGreen = mContext->GetNode("Gout");
	mOutputBlue = mContext->GetNode("Bout");
	if (! mOutputRed) mOutputRed = mRed;
	if (! mOutputGreen) mOutputGreen = mGreen;
	if (! mOutputBlue) mOutputBlue = mBlue;

	std::string typeErrorRed = mOutputRed->Prepare();
	if (! typeErrorRed.empty())
		return AddError(wxT("Type error: ") + typeErrorRed);

	std::string typeErrorGreen = mOutputGreen->Prepare();
	if (! typeErrorGreen.empty())
		return AddError(wxT("Type error: ") + typeErrorGreen);

	std::string typeErrorBlue = mOutputBlue->Prepare();
	if (! typeErrorBlue.empty())
		return AddError(wxT("Type error: ") + typeErrorBlue);

	if (mOutputRed->resultType != Expression::Node::MATRIX)
		return AddError(wxT("Rout must be a matrix."));
	if (mOutputGreen->resultType != Expression::Node::MATRIX)
		return AddError(wxT("Gout must be a matrix."));
	if (mOutputBlue->resultType != Expression::Node::MATRIX)
		return AddError(wxT("Bout must be a matrix."));
}

void THISCLASS::OnStep() {
	cv::Mat inputImage = mCore->mDataStructureImageColor.mImage;
	if (inputImage.empty()) {
		AddError(wxT("No input image."));
		return;
	}

	// Set the channels
	cv::Mat channels[3];
	cv::split(inputImage, channels);
	mBlue->Set(channels[0]);
	mGreen->Set(channels[1]);
	mRed->Set(channels[2]);

	// Update the size of the helper matrices if necessary
	if (! ImageTools::EqualSize(inputImage, mZeros->GetFloatMatrix())) {
		cv::Mat zeros = cv::Mat::zeros(inputImage.size(), CV_32FC1);
		mZeros->Set(zeros);
		mOnes->Set(zeros + 1);
		mBlack->Set(zeros);
		mWhite->Set(zeros + 255);
	}

	// Calculate
	mOutputRed->Execute();
	mOutputGreen->Execute();
	mOutputBlue->Execute();

	// Set the output
	cv::Mat outputChannels[3] = {
		mOutputRed->GetMatrix(),
		mOutputGreen->GetMatrix(),
		mOutputBlue->GetMatrix()
	};
	cv::Mat outputImage;
	cv::merge(outputChannels, 3, outputImage);
	mCore->mDataStructureImageColor.mImage = outputImage;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) de.SetMainImage(outputImage);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

