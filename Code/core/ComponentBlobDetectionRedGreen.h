#ifndef HEADER_ComponentBlobDetectionRedGreen
#define HEADER_ComponentBlobDetectionRedGreen

#include <opencv2/core.hpp>
#include "Component.h"
#include "DataStructureParticles.h"
#include "BlobDetection.h"
#include "BlobMatching.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentBlobDetectionRedGreen: public Component {

public:
	//! Constructor.
	ComponentBlobDetectionRedGreen(SwisTrackCore * stc);
	//! Destructor.
	~ComponentBlobDetectionRedGreen();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentBlobDetectionRedGreen(mCore);
	}

private:
	//! A structure holding all information related to the extraction of one color.
	class Color {
	public:
		int mThreshold;					//!< (configuration) The threshold to create the binary image.
		BlobDetection mBlobDetection;	//!< (configuration) Blob detection.

		Display mDisplayColor;			//!< The Display showing the resulting color image.
		Display mDisplayBinary;			//!< The Display showing the thresholded image.

		Color(const wxString & displayNameColor, const wxString & displayTitleColor, const wxString & displayNameBinary, const wxString & displayTitleBinary):
			mThreshold(0), mBlobDetection(),
			mDisplayColor(displayNameColor, displayTitleColor),
			mDisplayBinary(displayNameBinary, displayTitleBinary) {};

		//! Finds blobs of a specific color on a color image.
		void FindColorBlobs(cv::Mat colorImage, wxDateTime frameTimestamp, int frameNumber);
	};

	Color mColorRed;					//!< (configuration) Red color.
	Color mColorGreen;					//!< (configuration) Green color.
	BlobMatching mBlobMatching;			//!< (configuration) Blob matching.
	Display mDisplayOutput;				//!< The Display showing the last acquired image and the particles.
};

#endif

