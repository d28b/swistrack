#include "ComponentChannelArithmetic.h"
#define THISCLASS ComponentChannelArithmetic

#include <highgui.h>
#include <sstream>
#include "DisplayEditor.h"


THISCLASS::ComponentChannelArithmetic(SwisTrackCore *stc):
		Component(stc, "ChannelArithmetic"),
		coeff1(0.3), coeff2(0.59), coeff3(0.11),
        channel1(1), channel2(2), channel3(3),
        op1(ADD), op2(ADD),
		mDisplayOutput("Output", "After channel arithmetic")
{
	// Data structure relations
	mCategory=&(mCore->mCategoryPreprocessingColor);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentChannelArithmetic() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() 
{
    coeff1=GetConfigurationDouble("Coeff1", 0.3);
    coeff2=GetConfigurationDouble("Coeff2", 0.59);
    coeff3=GetConfigurationDouble("Coeff3", 0.11);

    channel1=GetConfigurationInt("Channel1", 1);
    channel2=GetConfigurationInt("Channel1", 2);
    channel3=GetConfigurationInt("Channel1", 3);

    switch (GetConfigurationInt("Op1", ADD))
    {
        case ADD: op1=ADD; break;
        case SUB: op1=SUB; break;
        case MUL: op1=MUL; break;
        case DIV: op1=DIV; break;
    }
    switch (GetConfigurationInt("Op2", ADD))
    {
        case ADD: op2=ADD; break;
        case SUB: op2=SUB; break;
        case MUL: op2=MUL; break;
        case DIV: op2=DIV; break;
    }

    strcpy(channelColorSeq,"BGR");
}

void THISCLASS::OnStep() {
	IplImage *inputimage=mCore->mDataStructureImageColor.mImage;	
	//Check the images
	if (!inputimage) 
	{
		AddError("No input Image");
		return;
	}
    else if (inputimage->nChannels !=3)
	{
		AddError("Input image does not have 3 channels.");
		return;
	}

    else 
    {

    try {
        PrepareOutputImage(inputimage);

		//Make a temporary clone of the image in 3 seperate channels
		IplImage* tmpImage[3]; // holds channels in order RGB
		for (int i=0;i<3;i++)
			tmpImage[i]=cvCreateImage(cvGetSize(inputimage),8,1);
        if (memcmp(inputimage->channelSeq,"RGB",3)==0)
		    cvSplit(inputimage,tmpImage[0],tmpImage[1],tmpImage[2],NULL);
        else if (memcmp(inputimage->channelSeq,"BGR",3)==0)
		    cvSplit(inputimage,tmpImage[2],tmpImage[1],tmpImage[0],NULL);
        else AddError("Image neither RGB nor BGR.");

        // begin with channel selected in first dropdown
        cvCopy(tmpImage[channel1-1], mOutputImage);

        // apply first coefficient
        cvConvertScale(mOutputImage,mOutputImage,coeff1);

        // possibly use second & third slots
        if (channel2!=0)
        {
            // apply second coefficient
            cvConvertScale(tmpImage[channel2-1],tmpImage[channel2-1],coeff2);
            switch(op1)
            {   // perform first operation
                case ADD:
                    cvAdd(mOutputImage,tmpImage[channel2-1],mOutputImage);
                    break;
                case SUB:
                    cvSub(mOutputImage,tmpImage[channel2-1],mOutputImage);
                    break;
                case MUL:
                    cvMul(mOutputImage,tmpImage[channel2-1],mOutputImage);
                    break;
                case DIV:
                    cvDiv(mOutputImage,tmpImage[channel2-1],mOutputImage);
                    break;
            }

            if (channel3!=0)
            {
                // apply third coefficient
                cvConvertScale(tmpImage[channel3-1],tmpImage[channel3-1],coeff3);
                switch(op2)
                {   // perform second operation
                    case ADD:
                        cvAdd(mOutputImage,tmpImage[channel3-1],mOutputImage);
                        break;
                    case SUB:
                        cvSub(mOutputImage,tmpImage[channel3-1],mOutputImage);
                        break;
                    case MUL:
                        cvMul(mOutputImage,tmpImage[channel3-1],mOutputImage);
                        break;
                    case DIV:
                        cvDiv(mOutputImage,tmpImage[channel3-1],mOutputImage);
                        break;
                }
            }
        }

        mCore->mDataStructureImageGray.mImage=mOutputImage;
    } catch(...) {
        AddError("Channel arithmetic failed.");
    }

    }

	// Set the display
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

