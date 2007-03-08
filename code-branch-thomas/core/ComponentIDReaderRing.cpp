#include "ComponentIDReaderRing.h"
#define THISCLASS ComponentIDReaderRing

#include <sstream>
#include <cmath>
#define PI (3.14159265358979)

THISCLASS::ComponentIDReaderRing(SwisTrackCore *stc):
		Component(stc, "IDReaderRing"),
		mRingRadiusInner(3), mRingRadiusOuter(5), mObjectList(0),
		mRingValuesMax(0), mAngles(0), mValues(0),
		mDisplayImageOutput("Output", "Particles") {

	// Data structure relations
	mDisplayName="ID reader";
	mCategory=&(mCore->mCategoryBlobDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentIDReaderRing() {
}

void THISCLASS::OnStart() {
	// Read the object list
	filename=GetConfigurationString("ObjectListFileName", "");
	mObjectList=new ObjectList(filename);
	if (mObjectList->mError!="") {
		AddError(mObjectList->mError);
		return;
	}

	// Reload the other settings
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mRingRadiusInner=GetConfigurationDouble("RingRadiusInner", 6);
	mRingRadiusOuter=GetConfigurationDouble("RingRadiusOuter", 12);
	mRingRadiusInner2=mRingRadiusInner*mRingRadiusInner;
	mRingRadiusOuter2=mRingRadiusOuter*mRingRadiusOuter;

	// Check for stupid configurations
	if (mRingRadiusInner>mRingRadiusOuter) {
		AddError("The inner radius must be smaller than the outer radius.");
	}

	// Allocate enough space
	mRingValuesMax=(2*mRingRadiusOuter+1)*(2*mRingRadiusOuter+1)-(2*mRingRadiusInner-1)*(2*mRingRadiusInner-1);
	mAngles=new float[mRingValuesMax];
	mValues=new int[mRingValuesMax];
}

void THISCLASS::OnStep() {
	IplImage *img=mCore->mDataStructureImageGray.mImage;

	std::ostringstream debugtext;

	float maxsum=0;
	DataStructureParticles::tParticleVector::iterator it=mCore->mDataStructureParticles.mParticles->begin();
	while (it!=mCore->mDataStructureParticles.mParticles->end()) {
		// Determine the position of the ring
		float cx=it->mCenter.x+1;  // FIXME
		float cy=it->mCenter.y+1;  // FIXME
		float x1=floor(cx-mRingRadiusOuter);
		float y1=floor(cy-mRingRadiusOuter);
		float x2=ceil(cx+mRingRadiusOuter);
		float y2=ceil(cy+mRingRadiusOuter);

		// Retrieve all pixels on the ring
		int si=0;
		unsigned char *data_linestart=(unsigned char *)img->imageData + img->widthStep*(int)y1 + (int)x1;
		for (float y=y1+0.5; y<=y2; y+=1) {
			unsigned char *data=data_linestart;
			for (float x=x1+0.5; x<=x2; x+=1) {
				float d2=(x-cx)*(x-cx)+(y-cy)*(y-cy);
				if ((d2<=mRingRadiusOuter2) && (d2>=mRingRadiusInner2)) {
					mAngles[si]=atan2f(y-cy, x-cx)/(2*PI);
					mValues[si]=(int)*data;
					si++;
					assert(si<mRingValuesMax);
				} else {
					*data=255;
				}
				data++;
			}
			data_linestart+=img->widthStep;
		}

		// Correlate with all barcodes
		ObjectList::tObjectList::iterator ito=mObjectList.mObjects.begin()
		while (ito!=mObjectList.mObjects.end()) {
			int numchips=ito->chips.size();

			int maxsum=0;
			float maxshift=0;
			
			float shift=0;
			while (shift<1) {
				int sum=0;
				for (int i=0; i<si; i++) {
					int bin=(int)floor((mAngles[i]-shift)*numchips);
					while (bin<0) {bin+=1;}
					while (bin>=1) {bin-=1;}
					sum+=mValues[i]*ito->chips[bin];
				}

				if (sum>maxsum) {maxsum=sum; maxshift=shift;}
				debugtext << ", " << sum;
				if (sum>maxsum) {maxsum=sum;}
				shift+=0.1;
			}
			it->mID=sum;
			
			ito++;
		}
		
		it++;
	}

	

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mParticles=mCore->mDataStructureParticles.mParticles;
	mDisplayImageOutput.mTopLeft=cvPoint2D32f(0, 0);
	mDisplayImageOutput.mBottomRight=cvPoint2D32f(img->width, img->height);
	mDisplayImageOutput.mImage=img;
	std::ostringstream oss;
	oss << "Detected particles, " << mCore->mDataStructureImageBinary.mImage->width << "x" << mCore->mDataStructureImageBinary.mImage->height << ", max " << maxsum << debugtext.str();
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
}
