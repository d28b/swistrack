#ifndef HEADER_ComponentOutputBufferedFileAVI
#define HEADER_ComponentOutputBufferedFileAVI

#include <vector>
#include <opencv2/core.hpp>
#include "Component.h"

//! An input component that reads an AVI file using the CV library.
class ComponentOutputBufferedFileAVI: public Component {

public:
	//! Constructor.
	ComponentOutputBufferedFileAVI(SwisTrackCore * stc);
	//! Destructor.
	~ComponentOutputBufferedFileAVI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentOutputBufferedFileAVI(mCore);
	}

private:
	//! Input channel
	enum eInputChannel {
		COLOR,
		GRAYSCALE,
		BINARY
	};

	enum eInputChannel mInputChannel;	//!< (configuration) Selected input channel.

	std::queue<cv::Mat> mBuffer;		//!< Frame buffer.
	unsigned int mBufferSize;			//!< (configuration) Maximum buffer size.
	bool mKeepLast;						//!< (configuration) Keep the last (rather than the first) images.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif
