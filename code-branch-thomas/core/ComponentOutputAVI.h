#ifndef HEADER_ComponentOutputAVI
#define HEADER_ComponentOutputAVI

#ifdef WIN32 // mask as quick&dirty hack!!!

#include "Component.h"

//! A component that generates an AVI movie.
/*!
	This component writes an AVI file using CvAVIWriter (only available on Win32). The file is written when started in productive mode only.
	The frames written are taken from a DisplayImage. Hence, any intermediate image can be stored. The DisplayImage may be changed (or even disabled) while the component is running in productive mode.
*/
class ComponentOutputAVI: public Component {

public:
	//! Constructor.
	ComponentOutputAVI(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputAVI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentOutputAVI(mCore);}

private:
	std::string mFileName;			//!< (configuration) File name of the AVI file to write.
	double mPlaybackSpeedFPS;		//!< (configuration) Playback speed FPS.
	int mFrameRate;					//!< (configuration) Record every mFrameRate image.
	CvSize mMaxImageSize;			//!< (configuration) The maximum image size. (Images are scaled proportionally, with neither side exceeding these values.)

	CvAVIWriter* mAVIWriter;		//!< AVI writer object.
	int mFrameCounter;				//!< Counts the number of frames until mFrameRate.
	bool mErrorAVIFile;				//!< The video file could not be opened.
	int mWrittenFramesCount;		//!< The number of written frames.
	DisplayImage *mCurrentDisplayImage;
};

#endif

#endif

