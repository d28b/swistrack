#ifndef HEADER_ComponentOutputFramesImages
#define HEADER_ComponentOutputFramesImages

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "Component.h"

//! An input component that reads an AVI file using the CV library.
class ComponentOutputFramesImages: public Component {

public:
	enum eFileType {
		FileTypePNG = 0,
		FileTypeBMP = 1,
		FileTypeJPEG = 2,
	};

	//! Constructor.
	ComponentOutputFramesImages(SwisTrackCore * stc);
	//! Destructor.
	~ComponentOutputFramesImages();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentOutputFramesImages(mCore);
	}

private:
	//! Input channel
	enum eInputChannel {
		COLOR,
		GRAYSCALE,
		BINARY
	};

	enum eInputChannel mInputChannel;	//!< (configuration) Selected input channel.
	std::string mFileBase;				//!< (configuration) Base file name.
	std::string mFileExtension;			//!< (configuration) File extension.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

