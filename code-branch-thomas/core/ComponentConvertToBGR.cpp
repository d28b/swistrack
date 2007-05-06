#include "ComponentConvertToBGR.h"
#define THISCLASS ComponentConvertToBGR

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentConvertToBGR(SwisTrackCore *stc):
		Component(stc, "ConvertToBGR"),
		mOutputImage(0),
		mDisplayOutput("Output", "After conversion to color (BGR)") {

	// Data structure relations
	mCategory=&(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureImageBGR));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentConvertToBGR() {
}

void THISCLASS::OnStart() {
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureInput.mImage;
	if (! inputimage) {return;}
	
	try {
		// We convert the input image in black and white
		if (inputimage->nChannels==3) {
			// Already in BGR
			//PrepareOutputImage(inputimage);
			//cvSplit(inputimage, mOutputImage0, mOutputImage1, mOutputImage2, NULL);
			//cvMerge(mOutputImage2, mOutputImage1, mOutputImage0, NULL, inputimage);
			//mCore->mDataStructureImageBGR.mImage=inputimage;

			//cvFlip(inputimage, NULL, 0);
			mCore->mDataStructureImageBGR.mImage=inputimage;
		} else if (inputimage->nChannels==2) {
			// Packed YUV422
			PrepareOutputImage(inputimage);
			CvtYUV422ToBGR(inputimage, mOutputImage);
			mCore->mDataStructureImageBGR.mImage=mOutputImage;
		} else if (inputimage->nChannels==1) {
			// Gray, convert to BGR
			PrepareOutputImage(inputimage);
			cvCvtColor(inputimage, mOutputImage, CV_GRAY2BGR);
			mCore->mDataStructureImageBGR.mImage=mOutputImage;
		} else {
			// Other cases, we take the first channel and transform it in BGR
			PrepareOutputImage(inputimage);
			cvCvtPixToPlane(inputimage, mOutputImage, NULL, NULL, NULL);
			cvCvtColor(mOutputImage, mOutputImage, CV_GRAY2BGR);
			mCore->mDataStructureImageBGR.mImage=mOutputImage;
		}
	} catch(...) {
		AddError("Conversion to gray failed.");
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageBGR.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureImageBGR.mImage=0;
}

void THISCLASS::OnStop() {
	if (mOutputImage) {cvReleaseImage(&mOutputImage);}
}

void THISCLASS::CvtYUV422ToBGR(IplImage *inputimage, IplImage *outputimage) {
	unsigned char *crl=(unsigned char *)inputimage->imageData;
	unsigned char *cwl=(unsigned char *)outputimage->imageData;
	for (int y=0; y<inputimage->height; y++) {
		unsigned char *cr=crl;
		unsigned char *cw=cwl;
		for (int x=0; x<inputimage->width; x+=2) {
			int u=(int)*cr; cr++;
			int y1=(int)*cr; cr++;
			int v=(int)*cr; cr++;
			int y2=(int)*cr; cr++;

			// Pixel 1
			int c = y1 - 16;
			int d = u - 128;
			int e = v - 128;

			int r = ( 298 * c + 409 * e + 128) >> 8;
			int g = ( 298 * c - 100 * d - 208 * e + 128) >> 8;
			int b = ( 298 * c + 516 * d + 128) >> 8;

			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			if (r < 0) r = 0;
			if (g < 0) g = 0;
			if (b < 0) b = 0;

			*cw=(unsigned char)b; cw++;
			*cw=(unsigned char)g; cw++;
			*cw=(unsigned char)r; cw++;

			// Pixel 2
			c = y2 - 16;

			r = ( 298 * c + 409 * e + 128) >> 8;
			g = ( 298 * c - 100 * d - 208 * e + 128) >> 8;
			b = ( 298 * c + 516 * d + 128) >> 8;

			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			if (r < 0) r = 0;
			if (g < 0) g = 0;
			if (b < 0) b = 0;

			*cw=(unsigned char)b; cw++;
			*cw=(unsigned char)g; cw++;
			*cw=(unsigned char)r; cw++;
		}
		crl+=inputimage->widthStep;
		cwl+=outputimage->widthStep;
	}
	outputimage->dataOrder=0;
}
