#ifndef HEADER_ComponentBlobDetectionTwoColors
#define HEADER_ComponentBlobDetectionTwoColors

#include <opencv2/core.hpp>
#include "Component.h"
#include "BlobDetection.h"
#include "BlobMatching.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBlobDetectionTwoColors: public Component {

public:
	//! Constructor.
	ComponentBlobDetectionTwoColors(SwisTrackCore * stc);
	//! Destructor.
	~ComponentBlobDetectionTwoColors();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentBlobDetectionTwoColors(mCore);
	}

private:
	//! A structure holding all information related to the extraction of one color.
	class Color {
	public:
		cv::Scalar mColor;				//!< (configuration) The color to subtract.
		int mThresholdR;				//!< (configuration) Threshold for the red channel.
		int mThresholdG;				//!< (configuration) Threshold for the green channel.
		int mThresholdB;				//!< (configuration) Threshold for the blue channel.
		BlobDetection mBlobDetection;	//!< (configuration) Blob detection.

		Display mDisplayBinary;			//!< The Display showing the thresholded image.

		Color(int color, const wxString &displayNameBinary, const wxString &displayTitleBinary):
			mColor(color), mThresholdR(32), mThresholdG(32), mThresholdB(32), mBlobDetection(),
			mDisplayBinary(displayNameBinary, displayTitleBinary) {};

		//! Finds blobs of a specific color on a color image.
		void FindColorBlobs(cv::Mat inputImage, wxDateTime frameTimestamp, int frameNumber);
	};

	Color mColor1;						//!< (configuration) Color 1.
	Color mColor2;						//!< (configuration) Color 2.
	BlobMatching mBlobMatching;			//!< (configuration) Blob matching.
	Display mDisplayOutput;				//!< The Display showing the last acquired image and the particles.
};

#endif

