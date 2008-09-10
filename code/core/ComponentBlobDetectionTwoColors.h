#ifndef HEADER_ComponentBlobDetectionTwoColors
#define HEADER_ComponentBlobDetectionTwoColors

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBlobDetectionTwoColors: public Component {

public:
	//! Constructor.
	ComponentBlobDetectionTwoColors(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBlobDetectionTwoColors();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentBlobDetectionTwoColors(mCore);}

private:
	unsigned int mMaxNumberOfParticles;	//!< (configuration) The maximum number of blobs that are to detect.
	double mMaxDistance;		//!< (configuration) The maximum distance (in pixels) between the two color blobs that make one robot.
	int mColor1;				//!< (configuration) Color 1.
	int mColor2;				//!< (configuration) Color 2.
	int mThresholdB;			//!< (configuration) Threshold for the blue channel.
	int mThresholdG;			//!< (configuration) Threshold for the green channel.
	int mThresholdR;			//!< (configuration) Threshold for the red channel.
	bool mSelectionByArea; 		//!< (configuration) If the selection is based on the blob area
	int mAreaMin;				//!< (configuration) The minimum area of a blob.
	int mAreaMax;				//!< (configuration) The maximum area of a blob.
	bool mSelectionByCompactness; //!< (configuration) If the selection is based on the blob Compactness
	double mCompactnessMin;		//!< (configuration) The minimum compactness of a blob.
	double mCompactnessMax;		//!< (configuration) The maximum compactness of a blob.
	DataStructureParticles::tParticleVector mParticles;	//!< The list of detected particles. This is the list given to DataStructureParticles.
	Display mDisplayColor1;								//!< The Display showing the image processed with the first color.
	Display mDisplayColor2;								//!< The Display showing the image processed with the second color.
	Display mDisplayOutput;								//!< The Display showing the last acquired image and the particles.

	//! Returns the compactness of a contour. Used by the blob detection algorithm.
	double GetContourCompactness(const void* contour);
	//! Finds blobs of a specific color on a color image.
	void FindColorBlobs(IplImage *colorimage, int color, DataStructureParticles::tParticleVector &particlevector, Display &display);
	//! Find blobs on a binary image (changes the binary image).
	void FindBlobs(IplImage *inputimage, DataStructureParticles::tParticleVector &particlevector);
};

#endif

