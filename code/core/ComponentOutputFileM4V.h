#ifndef HEADER_ComponentOutputFileM4V
#define HEADER_ComponentOutputFileM4V

#include "Component.h"

#ifdef USE_XVID
#include <string>
#include <cv.h>
#include <xvid.h>
#include <wx/file.h>

//! An output component that writes an M4V file using the Xvid library.
class ComponentOutputFileM4V: public Component {

public:
	//! Constructor.
	ComponentOutputFileM4V(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputFileM4V();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentOutputFileM4V(mCore);}

	double GetProgressPercent();
	double GetProgressMSec();
	int GetProgressFrameNumber();
	double GetFPS();

private:
	//! Keep in memory feature
	enum eKeepInMemory {
		cKeepInMemory_None,
		cKeepInMemory_Raw,
		cKeepInMemory_Compressed
	};

	//! Keep in memory feature
	enum eInputChannel {
		cInputChannel_None,
		cInputChannel_Color,
		cInputChannel_Grayscale,
		cInputChannel_Binary
	};

	void *mM4VHandle;					//!< M4V handle.
	unsigned char *mM4VBuffer;			//!< The memory buffer with the compressed frames.
	wxFile mFile;

	int mFrameRate;						//!< (configuration) The frame rate of the output file.
	enum eInputChannel mInputChannel;	//!< (configuration) Selected input channel.
	enum eKeepInMemory mKeepInMemory;	//!< (configuration) Keep in memory feature.
	int mKeepInMemoryFrameCount;		//!< (configuration) Number of uncompressed frames to keep in memory (raw mode).
	int mKeepInMemoryBufferSize;		//!< (configuration) Size of the memory buffer in bytes (compressed mode).
	std::string mFilename;				//!< (configuration) Output file name.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

	//! Opens the video file.
	void M4VOpen(IplImage* image);
	//! Writes one frame.
	void M4VWriteFrame(IplImage* image);
	//! Closes the video file.
	void M4VClose();
};

#else // USE_XVID

class ComponentOutputFileM4V: public Component {

public:
	ComponentOutputFileM4V(SwisTrackCore *stc): Component(stc, "OutputFileM4V") {Initialize();}
	~ComponentOutputFileM4V() {}

	// Overwritten Component methods
	void OnStart() {AddError("XVID support was not compiled into this executable.");}
	void OnReloadConfiguration() {AddError("GigE support was not compiled into this executable.");}
	void OnStep() {AddError("XVID support was not compiled into this executable.");}
	void OnStepCleanup() {}
	void OnStop() {AddError("XVID support was not compiled into this executable.");}
	Component *Create() {return new ComponentOutputFileM4V(mCore);}
};

#endif // USE_XVID

#endif

