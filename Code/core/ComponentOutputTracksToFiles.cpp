#include "ComponentOutputTracksToFiles.h"
#define THISCLASS ComponentOutputTracksToFiles
using namespace std;
#include <iostream>
#include <errno.h>
#include <fstream>
#include "DisplayEditor.h"
#include "Utility.h"

THISCLASS::ComponentOutputTracksToFiles(SwisTrackCore * stc):
	Component(stc, wxT("OutputTracksToFiles")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureTracks));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputTracksToFiles() {
}

void THISCLASS::OnStart() {
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// If there is no track, stop
	auto tracks = mCore->mDataStructureTracks.mTracks;
	if (! tracks) {
		AddError(wxT("No tracks."));
		return;
	}

	// For each track, write data in the corresponding output file
	for (auto & entry : *tracks) {
		auto & state = mState[entry.first];
		state.seen = true;
		if (entry.second.mTrajectory.empty()) continue;
		state.OpenIfNecessary(mCore, entry.first);
		state.AddPoint(entry.second.mTrajectory.back());
	}

	// Close files of obsolete tracks
	for (auto it = mState.begin(); it != mState.end(); ) {
		if (! it->second.seen) {
			it->second.Close();
			it = mState.erase(it);
		} else {
			it->second.seen = false;
			it++;
		}
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	// Close all files
	for (auto & entry : mState)
		entry.second.Close();

	mState.clear();
}

THISCLASS::State::State(): open(false), file(), lastFrameNumber(-1), seen(true) {
}

THISCLASS::State::~State() {
}

void THISCLASS::State::OpenIfNecessary(SwisTrackCore * core, int trackID) {
	if (open) return;
	open = true;

	wxString filenameString = wxString::Format(wxT("track%08d.csv"), trackID);
	wxFileName filename = core->GetRunFileName(filenameString);
	file.open(filename.GetFullPath().ToStdString(), std::fstream::out | std::fstream::trunc);

	if (! file.is_open()) return;
	file << "% Frame number" << "\t" << "x" << "\t" << "y" << std::endl;
}

void THISCLASS::State::AddPoint(const TrackPoint & point) {
	if (! file.is_open()) return;
	if (point.frameNumber == lastFrameNumber) return;

	file << point.frameNumber << "\t" << point.x << "\t" << point.y << std::endl;
	lastFrameNumber = point.frameNumber;
}

void THISCLASS::State::Close() {
	if (! open) return;
	file.close();
}
