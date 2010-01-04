#include "ComponentIDReaderRing.h"
#define THISCLASS ComponentIDReaderRing

#include <fstream>
#include <cmath>
#define PI (3.14159265358979)
#include "DisplayEditor.h"

THISCLASS::ComponentIDReaderRing(SwisTrackCore *stc):
		Component(stc, wxT("IDReaderRing")),
		mRingRadiusInner(3), mRingRadiusOuter(5), mObjectList(0),
		mRingValuesMax(0), mRingCount(0), mRingAngles(0), mRingValues(0),
		mCodeLength(0), mBinValues(0), mBinCounts(0),
		mDisplayOutput(wxT("Output"), wxT("Particles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentIDReaderRing() {
}

void THISCLASS::OnStart() {
	// Read the object list
	wxString filename_string = GetConfigurationString(wxT("ObjectListFileName"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	mObjectList = new ObjectList(filename);
	if (mObjectList->mError != wxT("")) {
		AddError(mObjectList->mError);
		return;
	}

	// Check if all objects have the same code length
	mCodeLength = -1;
	ObjectList::tObjectList::iterator ito = mObjectList->mObjects.begin();
	while (ito != mObjectList->mObjects.end()) {
		int thiscodelength = ito->chips.size();
		if (mCodeLength == -1) {
			mCodeLength = thiscodelength;
		}
		if (mCodeLength != thiscodelength) {
			AddError(wxT("All codes must have the same length (same number of chips)."));
			return;
		}
		ito++;
	}

	// And check if there are any codes longer than 0
	if (mCodeLength < 1) {
		AddError(wxT("No codes defined."));
		return;
	}

	// Allocate the bins
	delete mBinValues;
	mBinValues = new float[mCodeLength];
	delete mBinCounts;
	mBinCounts = new int[mCodeLength];

	// Reload the other settings
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mRingRadiusInner = GetConfigurationDouble(wxT("RingRadiusInner"), 6);
	mRingRadiusOuter = GetConfigurationDouble(wxT("RingRadiusOuter"), 12);
	mRingRadiusInner2 = mRingRadiusInner * mRingRadiusInner;
	mRingRadiusOuter2 = mRingRadiusOuter * mRingRadiusOuter;

	// Check for stupid configurations
	if (mRingRadiusInner > mRingRadiusOuter) {
		AddError(wxT("The inner radius must be smaller than the outer radius."));
	}

	// Allocate enough space
	mRingValuesMax = (int)((2 * mRingRadiusOuter + 1) * (2 * mRingRadiusOuter + 1) - (2 * mRingRadiusInner - 1) * (2 * mRingRadiusInner - 1));
	if (mRingValuesMax < 0) {
		mRingValuesMax = 1;
	}
	delete mRingAngles;
	mRingAngles = new float[mRingValuesMax];
	delete mRingValues;
	mRingValues = new int[mRingValuesMax];
}

void THISCLASS::OnStep() {
	IplImage *img = mCore->mDataStructureImageGray.mImage;
	if (! img) {
		AddError(wxT("No image available on the grayscale channel. You may want to add a 'Conversion to Grayscale' component."));
		return;
	}

	//int fileid=0;
	DataStructureParticles::tParticleVector::iterator it = mCore->mDataStructureParticles.mParticles->begin();
	while (it != mCore->mDataStructureParticles.mParticles->end()) {
		// Determine the position of the ring
		float cx = it->mCenter.x + 0.5;  // FIXME: for some reason mCenter.x is always integer
		float cy = it->mCenter.y + 0.5;  // FIXME
		float x1 = floor(cx - mRingRadiusOuter);
		float y1 = floor(cy - mRingRadiusOuter);
		float x2 = ceil(cx + mRingRadiusOuter);
		float y2 = ceil(cy + mRingRadiusOuter);
		if (x1 < 0) {
			x1 = 0;
		}
		if (y1 < 0) {
			y1 = 0;
		}
		if (x2 > img->width) {
			x2 = img->width;
		}
		if (y2 > img->height) {
			y2 = img->height;
		}

		// Retrieve all pixels on the ring
		//wxStopWatch sw;
		mRingCount = 0;
		int sum = 0;
		unsigned char *data_linestart = (unsigned char *)img->imageData + img->widthStep * (int)y1 + (int)x1;
		for (float y = y1 + 0.5; y <= y2; y += 1) {
			unsigned char *data = data_linestart;
			for (float x = x1 + 0.5; x <= x2; x += 1) {
				float d2 = (x - cx) * (x - cx) + (y - cy) * (y - cy);
				if ((d2 <= mRingRadiusOuter2) && (d2 >= mRingRadiusInner2)) {
					mRingAngles[mRingCount] = atan2f(y - cy, x - cx) / (2 * PI);
					mRingValues[mRingCount] = (int) * data;
					sum += mRingValues[mRingCount];
					mRingCount++;
					assert(mRingCount < mRingValuesMax);
				} else {
					*data = 255;
				}
				data++;
			}
			data_linestart += img->widthStep;
		}
		//mStepDuration=sw.Time();

		//fileid++;
		//std::ostringstream oss;
		//oss << "pixels" << fileid << ".txt";
		//std::ofstream ofs(oss.str().c_str());
		if (mRingCount > 0) {
			int mean = sum / mRingCount;
			for (int i = 0; i < mRingCount; i++) {
				//ofs << mRingAngles[i] << "\t" << mRingValues[i] << std::endl;
				mRingValues[i] -= mean;
			}
		}

		//std::ostringstream oss1;
		//oss1 << "stats" << fileid << ".txt";
		//std::ofstream ofs1(oss1.str().c_str());

		// Prepare the maximum variables
		float max_sum = 0;
		float max_shift = 0;
		int max_rotation = 0;
		ObjectList::Object *max_object = 0;

		// Correlate with all barcodes
		float shift = 1 / (float)mCodeLength;
		while (shift > 0) {
			// Prepare bins
			RingToBins(shift);

			// Check each object
			ObjectList::tObjectList::iterator ito = mObjectList->mObjects.begin();
			while (ito != mObjectList->mObjects.end()) {
				// For each rotation of the code
				for (int rotation = 0; rotation < mCodeLength; rotation++) {
					// Correlate
					float sum = 0;
					for (int i = 0; i < mCodeLength; i++) {
						int r = i - rotation;
						while (r < 0) {
							r += mCodeLength;
						}
						sum += mBinValues[i] * ito->chips[r];
					}
					//ofs1 << shift << "\t" << rotation << "\t" << sum << std::endl;

					// New max?
					if (sum > max_sum) {
						max_sum = sum;
						max_shift = shift;
						max_rotation = rotation;
						max_object = &*ito;
					}
				}

				ito++;
			}

			// Shift by one degree
			shift -= (float)(1. / 360.);
		}

		// Take the object with the highest correlation
		if (max_object) {
			it->mID = max_object->objectid;
			it->mIDCovariance = max_sum;  // FIXME: divide by stddev(signal)
			it->mOrientation = (max_shift + (float)max_rotation / (float)mCodeLength) * 2 * PI + max_object->angle;
		}

		it++;
	}

	// Let the Display know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(mCore->mDataStructureParticles.mParticles);
		de.SetMainImage(img);
	}
}

void THISCLASS::RingToBins(float shift) {
	// Set all bins to 0
	for (int i = 0; i < mCodeLength; i++) {
		mBinValues[i] = 0;
		mBinCounts[i] = 0;
	}

	// Add all ring values to the right bins
	for (int i = 0; i < mRingCount; i++) {
		int bin = (int)floor((mRingAngles[i] - shift) * mCodeLength);
		while (bin < 0) {
			bin += mCodeLength;
		}
		while (bin >= mCodeLength) {
			bin -= mCodeLength;
		}
		mBinValues[bin] += mRingValues[i];
		mBinCounts[bin]++;
	}

	// Normalize
	for (int i = 0; i < mCodeLength; i++) {
		mBinValues[i] = mBinValues[i] / mBinCounts[i];
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
	delete mRingAngles;
	mRingAngles = 0;
	delete mRingValues;
	mRingValues = 0;

	delete mBinValues;
	mBinValues = 0;
	delete mBinCounts;
	mBinCounts = 0;

	delete mObjectList;
	mObjectList = 0;
}
