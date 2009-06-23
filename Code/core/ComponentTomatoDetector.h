#ifndef HEADER_ComponentTomatoDetector
#define HEADER_ComponentTomatoDetector

#include <cv.h>
#include "Component.h"
#include "DataStructureParticles.h"
extern "C" {
#include "camshift_wrapper.h"
}


//! A tomato tracking component using various methods.
class ComponentTomatoDetector: public Component {

public:
	//! Constructor.
	ComponentTomatoDetector(SwisTrackCore *stc);
	//! Destructor.
	~ComponentTomatoDetector();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	int mChannelMain;					//!< The channel to subtract from.
	int mChannelSubtract;					//!< The channel to subtract.
	Component *Create() {
		return new ComponentTomatoDetector(mCore);
	}
	
private:
		
	//plImage *outputimage;				//!< The image created by this component.
	Display mDisplayOutput;

	int GrayThreshold;						//!< (configuration) Threshold value.
	int ErosionIterations;					//!< (configuration) The number of erosions to apply to the binary image.
	int selected_image;
	int MinAreaBlob;	//!< (configuration) The minimum area of a blob.
	int MaxAreaBlob;	//!< (configuration) The maximum area of a blob.
	unsigned int MaxNumberBlob;	//!< (configuration) The maximum number of blobs that are to detect.
	double MinCompactnessBlob;	//!< (configuration) The minimum Compactness of a blob.
	double MaxCompactnessBlob;	//!< (configuration) The maximum Compactness of a blob.
	DataStructureParticles::tParticleVector mParticles;		//!< The list of detected particles. This is the list given to DataStructureParticles.
	DataStructureParticles::tParticleVector FinalParticles;	
	//! Returns the compactness of a contour. Used by the blob detection algorithm.
	double GetContourCompactness(const void* contour);
	double MaxParticleArea;
	double MinParticleArea;
	IplImage *inputimage;
	IplImage *outputimage;
	IplImage *tempimagecolor;	
	IplImage *tempimagegray;
	IplImage *tempimagebinary;
	IplImage *particle_image;
	IplImage *tempimagebinary2;

};

#endif

