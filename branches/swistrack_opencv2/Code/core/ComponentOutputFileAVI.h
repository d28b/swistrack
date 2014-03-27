#ifndef HEADER_ComponentOutputFileAVI
#define HEADER_ComponentOutputFileAVI

#include <vector>
#include <cv.h>
#include <highgui.h>
#include "Component.h"

//! An input component that reads an AVI file using the CV library.
class ComponentOutputFileAVI: public Component {

public:
	//! Constructor.
	ComponentOutputFileAVI(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputFileAVI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputFileAVI(mCore);
	}

private:
	//! Input channel
	enum eInputChannel {
		cInputChannel_None,
		cInputChannel_Color,
		cInputChannel_Grayscale,
		cInputChannel_Binary
	};

	CvVideoWriter* mWriter;				//!< Pointer to AVI sequence.
	int mFrameRate;						//!< (configuration) The frame rate of the output AVI.
	enum eInputChannel mInputChannel;	//!< (configuration) Selected input channel.
	wxFileName mFileName;				//!< (configuration) Name of the saved AVI.
	wxString mCodecString;				//!< (configuration) Codec code
	int mFrameBufferCount;				//!< (configuration) Number of frames to buffer before writing to disk.
	IplImage** mFrameBuffer;			//!< The frame buffer.
	int mFrameBufferWriteCounter;		//!< The number of frames in the frame buffer.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

	void BufferedFrames_Allocate(int count);
	void BufferedFrames_Add(IplImage *inputimage);
	void BufferedFrames_Write();
};

#endif

