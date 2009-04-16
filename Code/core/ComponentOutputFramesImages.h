#ifndef HEADER_ComponentOutputFramesImages
#define HEADER_ComponentOutputFramesImages

#include <vector>
#include <cv.h>
#include <highgui.h>
#include "Component.h"

//! An input component that reads an AVI file using the CV library.
class ComponentOutputFramesImages: public Component {

public:
	enum eFileType { // see cvLoadImage for supported file types
		FileTypePNG = 0,
		FileTypeBMP = 1,
		FileTypeJPEG = 2,
	};
	//! Constructor.
	ComponentOutputFramesImages(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputFramesImages();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputFramesImages(mCore);
	}

private:
	//! Input channel
	enum eInputChannel {
		cInputChannel_None,
		cInputChannel_Color,
		cInputChannel_Grayscale,
		cInputChannel_Binary
	};

	enum eInputChannel mInputChannel;	//!< (configuration) Selected input channel.
	wxFileName mFileName;				//!< (configuration) directory and filename prefix
	eFileType mFileType;				//!< (configuration) directory and filename prefix
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

