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
	Component *Create() {
		return new ComponentBlobDetectionTwoColors(mCore);
	}

private:
	//! A structure holding all information related to the extraction of one color.
	class cColor {
	public:
		int mColor;					//!< (configuration) The color to subtract.
		int mThresholdR;			//!< (configuration) Threshold for the red channel.
		int mThresholdG;			//!< (configuration) Threshold for the green channel.
		int mThresholdB;			//!< (configuration) Threshold for the blue channel.
		bool mSelectionByArea; 		//!< (configuration) If the selection is based on the blob area
		int mAreaMin;				//!< (configuration) The minimum area of a blob.
		int mAreaMax;				//!< (configuration) The maximum area of a blob.
		bool mSelectionByCompactness; //!< (configuration) If the selection is based on the blob Compactness
		double mCompactnessMin;		//!< (configuration) The minimum compactness of a blob.
		double mCompactnessMax;		//!< (configuration) The maximum compactness of a blob.

		DataStructureParticles::tParticleVector mParticles;	//!< The list of detected particles.
		Display mDisplayColor;		//!< The Display showing the resulting color image.
		Display mDisplayBinary;		//!< The Display showing the thresholded image.

		cColor(int color, const wxString &display_name_color, const wxString &display_title_color, const wxString &display_name_binary, const wxString &display_title_binary):
				mColor(color), mThresholdR(32), mThresholdG(32), mThresholdB(32),
				mSelectionByArea(false), mAreaMin(0), mAreaMax(1000),
				mSelectionByCompactness(false), mCompactnessMin(0), mCompactnessMax(1000),
				mParticles(),
				mDisplayColor(display_name_color, display_title_color), mDisplayBinary(display_name_binary, display_title_binary) {};
	};

	unsigned int mMaxNumberOfParticles;	//!< (configuration) The maximum number of blobs that are to detect.
	double mMaxDistance;		//!< (configuration) The maximum distance (in pixels) between the two color blobs that make one robot.
	cColor mColor1;				//!< (configuration) Color 1.
	cColor mColor2;				//!< (configuration) Color 2.
	DataStructureParticles::tParticleVector mParticles;	//!< The list of detected particles. This is the list given to DataStructureParticles.
	Display mDisplayOutput;								//!< The Display showing the last acquired image and the particles.
	wxString mOutputParams;				//!< (configuration) Selects the blob for output parameters (1st, 2nd, avg)

	//! Returns the compactness of a contour. Used by the blob detection algorithm.
	double GetContourCompactness(const void* contour);
	//! Finds blobs of a specific color on a color image.
	void FindColorBlobs(IplImage *colorimage, cColor &color);
	//! Find blobs on a binary image (changes the binary image).
	void FindBlobs(IplImage *inputimage, cColor &color);
};

#endif

