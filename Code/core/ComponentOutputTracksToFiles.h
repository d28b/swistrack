#ifndef HEADER_ComponentOutputTracksToFiles
#define HEADER_ComponentOutputTracksToFiles

#include <map>
#include <opencv2/core.hpp>
#include <fstream>
#include "Component.h"

class ComponentOutputTracksToFiles: public Component {

public:
	//! Constructor.
	ComponentOutputTracksToFiles(SwisTrackCore * stc);
	//! Destructor.
	~ComponentOutputTracksToFiles();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentOutputTracksToFiles(mCore);
	}

private:
	class State {
	public:
		bool open;
		std::fstream file;
		int lastFrameNumber;
		bool seen;

		State();
		~State();

		void OpenIfNecessary(SwisTrackCore * core, int trackID);
		void AddPoint(const TrackPoint & point);
		void Close();
	};

	// Open files by track ID.
	std::map<int, State> mState;
};

#endif

