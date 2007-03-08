#include "ComponentIDReaderRing.h"
#define THISCLASS ComponentIDReaderRing

#include <sstream>
#include <cmath>
#define PI (3.14159265358979)

THISCLASS::ComponentIDReaderRing(SwisTrackCore *stc):
		Component(stc, "IDReaderRing"),
		mRingRadiusInner(3), mRingRadiusOuter(5), mCodes(),
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
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mRingRadiusInner=GetConfigurationDouble("RingRadiusInner", 6);
	mRingRadiusOuter=GetConfigurationDouble("RingRadiusOuter", 12);
	
	mChips=14;
	mCodes.push_back(5323);

	// Check for stupid configurations
	if (mRingRadiusInner>mRingRadiusOuter) {
		AddError("The inner radius must be smaller than the outer radius.");
	}
}

void THISCLASS::OnStep() {
	IplImage *img=mCore->mDataStructureImageGray.mImage;

	std::ostringstream debugtext;

	float maxsum=0;
	DataStructureParticles::tParticleVector::iterator it=mCore->mDataStructureParticles.mParticles->begin();
	while (it!=mCore->mDataStructureParticles.mParticles->end()) {
		float cx=it->mCenter.x+1;
		float cy=it->mCenter.y+1;
		float x1=floor(cx-mRingRadiusOuter);
		float y1=floor(cy-mRingRadiusOuter);
		float x2=ceil(cx+mRingRadiusOuter);
		float y2=ceil(cy+mRingRadiusOuter);

		float sa[1000];
		int sv[1000];
		int si=0;
		unsigned char *data_linestart=(unsigned char *)img->imageData + img->widthStep*(int)y1 + (int)x1;
		for (float y=y1+0.5; y<=y2; y+=1) {
			unsigned char *data=data_linestart;
			for (float x=x1+0.5; x<=x2; x+=1) {
				float d=sqrt((x-cx)*(x-cx)+(y-cy)*(y-cy));
				if ((d<=mRingRadiusOuter) && (d>=mRingRadiusInner)) {
					sa[si]=atan2f(y-cy, x-cx)/(2*PI)*mChips;
					sv[si]=(int)*data;
					si++;
				} else {
					*data=255;
				}
				data++;
			}
			data_linestart+=img->widthStep;
		}

		float chips[14];
		//int code=3245;
		int code=5323;
		for (int i=0; i<mChips; i++) {
			chips[i]=(code & 1 ? 1 : -1);
			code=(code >> 1);
		}

		float shift=0;

		while (shift<mChips) {
			int sum=0;
			for (int i=0; i<si; i++) {
				int bin=(int)floor(sa[i]-shift);
				while (bin<0) {bin+=mChips;}
				while (bin>=mChips) {bin-=mChips;}
				sum+=sv[i]*chips[bin];
			}

			it->mID=sum;
			debugtext << ", " << sum;
			if (sum>maxsum) {maxsum=sum;}
			shift+=0.1;
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
