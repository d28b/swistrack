#ifndef HEADER_ComponentOutputFileM4V
#define HEADER_ComponentOutputFileM4V

#include <string>
#include <cv.h>
#include <xvid.h>
#include "Component.h"
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
	void *mM4VHandle;					//!< M4V handle.
	unsigned char *mM4VBuffer;			//!< The memory buffer used to compress frames.
	//xvid_enc_zone_t mM4VZones[64];		//!< Memory for the zones.
	wxFile mFile;

	int mFrameRate;						//!< The frame rate of the output file.
	int mInputSelection;				//!< Selected input channel.
	std::string mFilename;				//!< Output file name.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.

	//! Opens the video file.
	void M4VOpen(IplImage* image);
	//! Writes one frame.
	void M4VWriteFrame(IplImage* image);
	//! Closes the video file.
	void M4VClose();
};

#endif

