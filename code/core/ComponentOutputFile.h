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


private:
	struct structOutputFile
	{
		std::fstream fileStream;
		int trackID;

	};
	typedef std::map<int, structOutputFile*> filesMap;
	void writeData(structOutputFile *outputFile);
	void writeHeader(structOutputFile *outputFile);
	wxString mDirectoryName;			//!< Name of the directory to save
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
	filesMap mFiles;			//!< Vector containing the FileName
};

#endif

