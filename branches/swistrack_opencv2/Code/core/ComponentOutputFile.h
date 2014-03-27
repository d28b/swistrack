#ifndef HEADER_ComponentOutputFile
#define HEADER_ComponentOutputFile

#include <vector>
#include <map>
#include <cv.h>
#include <highgui.h>
#include <fstream>
#include "Component.h"

//! An output component writing the trajectory data in tab seperated files.
class ComponentOutputFile: public Component {

public:
	//! Constructor.
	ComponentOutputFile(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputFile();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentOutputFile(mCore);
	}

	static void WriteParticle(std::fstream & fileStream, const Particle & particle);


private:
	struct structOutputFile {
		std::fstream fileStream;
		int trackID;
	};

	// Map track ID -> file.
	typedef std::map<int, structOutputFile*> tFilesMap;

	wxString mFileNamePrefix;		//!< Prefix of file names
	Display mDisplayOutput;			//!< The DisplayImage showing the output of this component.
	tFilesMap mFiles;				//!< Vector containing the FileName

	//! Writes the file header.
	static void WriteHeader(std::fstream & fileStream);
	//! Writes one data record.
	void WriteData(structOutputFile *outputFile);
};

#endif

