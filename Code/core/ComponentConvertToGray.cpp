#include "ComponentConvertToGray.h"
#define THISCLASS ComponentConvertToGray

#include "DisplayEditor.h"

THISCLASS::ComponentConvertToGray(SwisTrackCore *stc):
		Component(stc, wxT("ConvertToGray")),
		mOutputImage(0),
		mDisplayOutput(wxT("Output"), wxT("After conversion to grayscale")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertToGray() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	mChannel = GetConfigurationInt(wxT("Channel"), 0);
	memcpy(mChannelColorSeq, wxT("BGR "), 4);
}

void THISCLASS::OnStep() {
	IplImage *inputimage = mCore->mDataStructureInput.mImage;
	if (! inputimage) {
		return;
	}

	try {
		// We convert the input image to black and white
		if (inputimage->nChannels == 3) {
			// The input image is a color image
			PrepareOutputImage(inputimage);

			if ((mChannel > 0) && (mChannel <= 3)) {
				// If the user selected one channel, copy that channel

				// If the channel sequence in the image changed, follow the channel
				if (memcmp(inputimage->channelSeq, mChannelColorSeq, 3)) {
					for (int i = 0;i < 3;i++) {
						if (inputimage->channelSeq[i] == mChannelColorSeq[mChannel]) {
							mChannel = i;
							break;
						}
					}
					memcpy(mChannelColorSeq, inputimage->channelSeq, 3);
				}

				// Copy
				switch(mChannel)
				{				
				case 1:
					cvSplit(inputimage, mOutputImage, NULL, NULL, NULL);
					break;
				case 2:
					cvSplit(inputimage, NULL, mOutputImage, NULL, NULL);
					break;
				case 3:
					cvSplit(inputimage, NULL, NULL, mOutputImage, NULL);
					break;
				default:
					AddError(wxT("Conversion to gray failed."));
					break;
				}
			} else {
				// Otherwise, convert to gray using the standard procedure
				cvCvtColor(inputimage, mOutputImage, CV_BGR2GRAY);
			}
			mCore->mDataStructureImageGray.mImage = mOutputImage;
		} else if (inputimage->nChannels == 1) {
			// The input image is in gray already, so just take that very same image
			mCore->mDataStructureImageGray.mImage = inputimage;
		} else {
			// Other cases (should never happen), we take the first channel
			PrepareOutputImage(inputimage);
			cvSplit(inputimage, mOutputImage, NULL, NULL, NULL);
			mCore->mDataStructureImageGray.mImage = mOutputImage;
		}
	} catch (...) {
		AddError(wxT("Conversion to gray failed."));
	}

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageGray.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageGray.mImage = 0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}
}
