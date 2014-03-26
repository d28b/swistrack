#ifndef HEADER_ComponentBlobDetectionRedGreen
#define HEADER_ComponentBlobDetectionRedGreen

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBlobDetectionRedGreen: public Component {

public:
	//! Constructor.
	ComponentBlobDetectionRedGreen(SwisTrackCore *stc);
	//! Destructor.
	~ComponentBlobDetectionRedGreen();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentBlobDetectionRedGreen(mCore);
	}

private:
	//! A structure holding all information related to the extraction of one color.
	class cColor {
	public:
		int mChannelMain;			//!< The channel to subtract from.
		int mChannelSubtract;		//!< The channel to subtract.
		int mThreshold;				//!< (configuration) Threshold.
		bool mSelectionByArea; 		//!< (configuration) If the selection is based on the blob area
		int mAreaMin;				//!< (configuration) The minimum area of a blob.
		int mAreaMax;				//!< (configuration) The maximum area of a blob.
		bool mSelectionByCompactness; //!< (configuration) If the selection is based on the blob Compactness
		double mCompactnessMin;		//!< (configuration) The minimum compactness of a blob.
		double mCompactnessMax;		//!< (configuration) The maximum compactness of a blob.

		DataStructureParticles::tParticleVector mParticles;	//!< The list of detected particles.
		Display mDisplayColor;		//!< The Display showing the resulting color image.
		Display mDisplayBinary;		//!< The Display showing the thresholded image.

		cColor(int channelmain, int channelsubtract, const wxString &display_name_color, const wxString &display_title_color, const wxString &display_name_binary, const wxString &display_title_binary):
				mChannelMain(channelmain), mChannelSubtract(channelsubtract), mThreshold(50),
				mSelectionByArea(false), mAreaMin(0), mAreaMax(1000),
				mSelectionByCompactness(false), mCompactnessMin(0), mCompactnessMax(1000),
				mParticles(),
				mDisplayColor(display_name_color, display_title_color), mDisplayBinary(display_name_binary, display_title_binary) {};
	};

	unsigned int mMaxNumberOfParticles;	//!< (configuration) The maximum number of blobs that are to detect.
	double mMaxDistance;				//!< (configuration) The maximum distance (in pixels) between the two color blobs that make one robot.
	cColor mColor1;						//!< (configuration) Red color.
	cColor mColor2;						//!< (configuration) Green color.
	DataStructureParticles::tParticleVector mParticles;	//!< The list of detected particles. This is the list given to DataStructureParticles.
	Display mDisplayOutput;								//!< The Display showing the last acquired image and the particles.

	//! Returns the compactness of a contour. Used by the blob detection algorithm.
	double GetContourCompactness(const void* contour);
	//! Finds blobs of a specific color on a color image.
	void FindColorBlobs(IplImage *colorimage, cColor &color);
	//! Find blobs on a binary image (changes the binary image).
	void FindBlobs(IplImage *inputimage, cColor &color);
};

#endif

