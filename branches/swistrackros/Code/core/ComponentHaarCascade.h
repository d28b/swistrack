#ifndef HEADER_ComponentHaarCascade
#define HEADER_ComponentHaarCascade

#include <cv.h>
#include "Component.h"
#include <queue>

// A component that finds uses a haar cascade xml file to 
// detect a target
class ComponentHaarCascade: public Component {

public:
	//! Constructor.
	ComponentHaarCascade(SwisTrackCore *stc);
	//! Destructor.
	~ComponentHaarCascade();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();


	// Function prototype for detecting and drawing an object from an image
	void detect_and_draw( IplImage* image );
	float RankFace(IplImage* inputimage);


	Component *Create() {
		return new ComponentHaarCascade(mCore);
	}

	// New Component methods

private:
	IplImage* templateImage;
	//IplImage* mOutputImage;
	Display mDisplayOutput;			   //!< The DisplayImage showing the output of this component.

	wxFileName cascade_name;
	CvMemStorage* storage; // Memory for calculations
	CvHaarClassifierCascade* cascade;
	int optlen;

};

#endif

