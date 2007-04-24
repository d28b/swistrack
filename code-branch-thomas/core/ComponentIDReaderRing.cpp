#include "ComponentIDReaderRing.h"
#define THISCLASS ComponentIDReaderRing

#include <sstream>
#include <fstream>
#include <cmath>
#include <wx/stopwatch.h>
#define PI (3.14159265358979)

THISCLASS::ComponentIDReaderRing(SwisTrackCore *stc):
		Component(stc, "IDReaderRing"),
		mRingRadiusInner(3), mRingRadiusOuter(5), mObjectList(0),
		mRingValuesMax(0), mRingCount(0), mRingAngles(0), mRingValues(0),
		mCodeLength(0), mBinValues(0), mBinCounts(0),
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
	wxString filename=GetConfigurationString("ObjectListFileName", "");
	mObjectList=new ObjectList(filename.c_str());
	if (mObjectList->mError!="") {
		AddError(mObjectList->mError);
		return;
	}

	// Check if all objects have the same code length
	mCodeLength=-1;
	ObjectList::tObjectList::iterator ito=mObjectList->mObjects.begin();
	while (ito!=mObjectList->mObjects.end()) {
		int thiscodelength=ito->chips.size();
		if (mCodeLength==-1) {mCodeLength=thiscodelength;}
		if (mCodeLength!=thiscodelength) {
			AddError("All codes must have the same length (same number of chips).");
			return;
		}
		ito++;
	}

	// And check if there are any codes longer than 0
	if (mCodeLength<1) {
		AddError("No codes defined.");
		return;
	}

	// Allocate the bins
	delete mBinValues; mBinValues=new float[mCodeLength];
	delete mBinCounts; mBinCounts=new int[mCodeLength];

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
	delete mRingAngles; mRingAngles=new float[mRingValuesMax];
	delete mRingValues; mRingValues=new int[mRingValuesMax];
}

void THISCLASS::OnStep() {
	IplImage *img=mCore->mDataStructureImageGray.mImage;

	int fileid=0;
	DataStructureParticles::tParticleVector::iterator it=mCore->mDataStructureParticles.mParticles->begin();
	while (it!=mCore->mDataStructureParticles.mParticles->end()) {
		// Determine the position of the ring
		float cx=it->mCenter.x+0.5;  // FIXME: for some reason mCenter.x is always integer
		float cy=it->mCenter.y+0.5;  // FIXME
		float x1=floor(cx-mRingRadiusOuter);
		float y1=floor(cy-mRingRadiusOuter);
		float x2=ceil(cx+mRingRadiusOuter);
		float y2=ceil(cy+mRingRadiusOuter);

		// Retrieve all pixels on the ring
		//wxStopWatch sw;
		mRingCount=0;
		int sum=0;
		unsigned char *data_linestart=(unsigned char *)img->imageData + img->widthStep*(int)y1 + (int)x1;
		for (float y=y1+0.5; y<=y2; y+=1) {
			unsigned char *data=data_linestart;
			for (float x=x1+0.5; x<=x2; x+=1) {
				float d2=(x-cx)*(x-cx)+(y-cy)*(y-cy);
				if ((d2<=mRingRadiusOuter2) && (d2>=mRingRadiusInner2)) {
					mRingAngles[mRingCount]=atan2f(y-cy, x-cx)/(2*PI);
					mRingValues[mRingCount]=(int)*data;
					sum+=mRingValues[mRingCount];
					mRingCount++;
					assert(mRingCount<mRingValuesMax);
				} else {
					*data=255;
				}
				data++;
			}
			data_linestart+=img->widthStep;
		}
		//mStepDuration=sw.Time();

		//fileid++;
		//std::ostringstream oss;
		//oss << "pixels" << fileid << ".txt";
		//std::ofstream ofs(oss.str().c_str());
		int mean=sum/mRingCount;
		for (int i=0; i<mRingCount; i++) {
			//ofs << mRingAngles[i] << "\t" << mRingValues[i] << std::endl;
			mRingValues[i]-=mean;
		}

		//std::ostringstream oss1;
		//oss1 << "stats" << fileid << ".txt";
		//std::ofstream ofs1(oss1.str().c_str());

		// Prepare the maximum variables
		float max_sum=0;
		float max_shift=0;
		int max_rotation=0;
		ObjectList::tObject *max_object=0;

		// Correlate with all barcodes
		float shift=1/(float)mCodeLength;
		while (shift>0) {
			// Prepare bins
			RingToBins(shift);

			// Check each object
			ObjectList::tObjectList::iterator ito=mObjectList->mObjects.begin();
			while (ito!=mObjectList->mObjects.end()) {
				// For each rotation of the code
				for (int rotation=0; rotation<mCodeLength; rotation++) {
					// Correlate
					float sum=0;
					for (int i=0; i<mCodeLength; i++) {
						int r=i-rotation;
						while (r<0) {r+=mCodeLength;}
						sum+=mBinValues[i]*ito->chips[r];
					}
					//ofs1 << shift << "\t" << rotation << "\t" << sum << std::endl;

					// New max?
					if (sum>max_sum) {
						max_sum=sum;
						max_shift=shift;
						max_rotation=rotation;
						max_object=&*ito;
					}
				}

				ito++;
			}

			// Shift by one degree
			shift-=(float)(1./360.);
		}

		// Take the object with the highest correlation
		if (max_object) {
			it->mID=max_object->objectid;
			it->mIDCovariance=max_sum;  // FIXME: divide by stddev(signal)
			it->mOrientation=(max_shift+(float)max_rotation/(float)mCodeLength)*2*PI+max_object->angle;
		}

		it++;
	}

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mParticles=mCore->mDataStructureParticles.mParticles;
	mDisplayImageOutput.mTopLeft=cvPoint2D32f(0, 0);
	mDisplayImageOutput.mBottomRight=cvPoint2D32f(img->width, img->height);
	mDisplayImageOutput.mImage=img;
	std::ostringstream oss;
	oss << "Detected particles, " << mCore->mDataStructureImageBinary.mImage->width << "x" << mCore->mDataStructureImageBinary.mImage->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::RingToBins(float shift) {
	// Set all bins to 0
	for (int i=0; i<mCodeLength; i++) {
		mBinValues[i]=0;
		mBinCounts[i]=0;
	}

	// Add all ring values to the right bins
	for (int i=0; i<mRingCount; i++) {
		int bin=(int)floor((mRingAngles[i]-shift)*mCodeLength);
		while (bin<0) {bin+=mCodeLength;}
		while (bin>=mCodeLength) {bin-=mCodeLength;}
		mBinValues[bin]+=mRingValues[i];
		mBinCounts[bin]++;
	}

	// Normalize
	for (int i=0; i<mCodeLength; i++) {
		mBinValues[i]=mBinValues[i]/mBinCounts[i];
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
	delete mRingAngles; mRingAngles=0;
	delete mRingValues; mRingValues=0;

	delete mBinValues; mBinValues=0;
	delete mBinCounts; mBinCounts=0;

	delete mObjectList; mObjectList=0;
}