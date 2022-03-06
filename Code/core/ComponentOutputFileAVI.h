#ifndef HEADER_ComponentOutputFileAVI
#define HEADER_ComponentOutputFileAVI

#include <vector>
#include <opencv2/core.hpp>
#include "Component.h"

//! An input component that reads an AVI file using the CV library.
class ComponentOutputFileAVI: public Component {

public:
	//! Constructor.
	ComponentOutputFileAVI(SwisTrackCore * stc);
	//! Destructor.
	~ComponentOutputFileAVI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentOutputFileAVI(mCore);
	}

private:
	//! Input channel
	enum eInputChannel {
		COLOR,
		GRAYSCALE,
		BINARY
	};

	enum eInputChannel mInputChannel;		//!< (configuration) Selected input channel.

	cv::VideoWriter mWriter;				//!< Video writer.
	std::string mFileName;					//!< (configuration) Name of the saved AVI.
	int mFrameRate;							//!< (configuration) The frame rate of the output AVI.
	int mCodec;								//!< (configuration) Codec.

	Display mDisplayOutput;					//!< The DisplayImage showing the output of this component.
};

#endif

