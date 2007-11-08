#include "ComponentConvertToGray.h"
#define THISCLASS ComponentConvertToGray

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentConvertToGray(SwisTrackCore *stc):
		Component(stc, "ConvertToGray"),
		mOutputImage(0),
		mDisplayOutput("Output", "After conversion to grayscale") {

	// Data structure relations
	mCategory=&(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertToGray() {
}

void THISCLASS::OnStart() 
{
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() 
{
	channel=GetConfigurationInt("Channel", 0);
	strcpy(channelColorSeq,"BGR");
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureInput.mImage;
	if (! inputimage) {return;}
	
	try {
		// We convert the input image to black and white
		if (inputimage->nChannels==3) 
		{
			// 3 color case, we convert to gray or select one channel
			PrepareOutputImage(inputimage);
			if ((channel>0) && (channel<=3)) 
			{
				// Correct the channel sequence
				if (strncmp(inputimage->channelSeq,channelColorSeq,3))		
				{				
					for (int i=1;i<4;i++)		
						if (inputimage->channelSeq[i-1]==channelColorSeq[channel-1])
						{
							channel=i;
							break;
						}
					strcpy(channelColorSeq,inputimage->channelSeq);					
				}
				cvSetImageCOI(inputimage, channel);
				cvCopy(inputimage, mOutputImage);
			} else {
				cvCvtColor(inputimage, mOutputImage, CV_BGR2GRAY);
			}
			mCore->mDataStructureImageGray.mImage=mOutputImage;
		} else if (inputimage->nChannels==2) {
			// Packed YUV422
			PrepareOutputImage(inputimage);
			CvtYUV422ToGray(inputimage, mOutputImage);
			mCore->mDataStructureImageGray.mImage=mOutputImage;
		} else if (inputimage->nChannels==1) {
			// Already in Gray
			mCore->mDataStructureImageGray.mImage=inputimage;
		} else {
			// Other cases, we take the first channel
			PrepareOutputImage(inputimage);
			cvCvtPixToPlane(inputimage, mOutputImage, NULL, NULL, NULL);
			mCore->mDataStructureImageGray.mImage=mOutputImage;
		}
	} catch(...) {
		AddError("Conversion to gray failed.");
	}

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageGray.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageGray.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}

void THISCLASS::CvtYUV422ToGray(IplImage *inputimage, IplImage *outputimage) {
	unsigned char *crl=(unsigned char *)inputimage->imageData;
	unsigned char *cwl=(unsigned char *)outputimage->imageData;
	for (int y=0; y<inputimage->height; y++) {
		unsigned char *cr=crl;
		unsigned char *cw=cwl;
		for (int x=0; x<inputimage->width; x+=2) {
			cr++;
			int y1=(int)*cr; cr++;
			cr++;
			int y2=(int)*cr; cr++;

			*cw=y1; cw++;
			*cw=y2; cw++;
		}
		crl+=inputimage->widthStep;
		cwl+=outputimage->widthStep;
	}
}
