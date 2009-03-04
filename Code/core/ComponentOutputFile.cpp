#include "ComponentOutputFile.h"
#define THISCLASS ComponentOutputFile
using namespace std;
#include <iostream>
#include <fstream>
#include "DisplayEditor.h"

THISCLASS::ComponentOutputFile(SwisTrackCore *stc):
		Component(stc, wxT("OutputFile")),
		mDisplayOutput(wxT("Output"), wxT("After tracking"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);
	AddDataStructureRead(&(mCore->mDataStructureTracks));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputFile() {
}

void THISCLASS::OnStart() {
	mDirectoryName = GetConfigurationString(wxT("DirectoryName"), wxT(""));
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// If there is no track, stop
	DataStructureTracks::tTrackMap *mTracks;
	mTracks = mCore->mDataStructureTracks.mTracks;
	if (! mTracks) {
		AddError(wxT("No Track"));
		return;
	}

	// For each track, write data in the corresponding output file
	DataStructureTracks::tTrackMap::iterator it = mTracks->begin();
	while (it != mTracks->end()) {
		if (mFiles.find(it->first) != mFiles.end()) {
			// Write the data to the file
			WriteData(mFiles[it->first]);
		} else {
			structOutputFile *newOutputFile = new structOutputFile;
			newOutputFile->trackID = it->first;
			wxString tmpFileName = mDirectoryName;
			tmpFileName += wxString::Format(wxT("track_%08d.txt"), it->first);
			(newOutputFile->fileStream).open(tmpFileName.mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);

			if (!(newOutputFile->fileStream).is_open()) {
				AddError(wxT("Unable to open one of the output file"));
				return;
			}
			mFiles[it->first] = newOutputFile;
			WriteHeader(newOutputFile);
			WriteData(newOutputFile);
		}
		it++;
	}

	// If a track disappeared, close the file. (Otherwise we'd eventually run out of open files.)
	for (std::map<int, structOutputFile*>::iterator i = mFiles.begin(); i != mFiles.end(); i++) {
		if (mTracks->find(i->first) == mTracks->end()) {
			mFiles[i->first]->fileStream.close();
			delete mFiles[i->first];
			mFiles.erase(i->first);
		}
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	// Close all files
	tFilesMap::iterator it = mFiles.begin();
	while (it != mFiles.end())
	{
		((it->second)->fileStream).close();
		delete it->second;
		it++;
	}
	mFiles.clear();
}

void THISCLASS::WriteHeader(structOutputFile *outputFile) {
	// Write the header of the file
	outputFile->fileStream
	// Frame number
	<< "%Frame Number" << "\t"
	// Center in image coordinates
	<< "x (image)" << "\t" << "y (image)" << "\t"
	// Center in world coordinates
	<< "x (world)" << "\t" << "y (world)" << "\t"
	// Area
	<< "Area" << "\t"
	// Orientation
	<< "Orientation" << "\t"
	// Compactness
	<< "Compactness" << std::endl;
}

void THISCLASS::WriteData(structOutputFile *outputFile) {
	//Search for the corresponding particle
	DataStructureParticles::tParticleVector *particles = mCore->mDataStructureParticles.mParticles;
	if (! particles)
	{
		AddError(wxT("There are no particles"));
		return;
	}

	DataStructureParticles::tParticleVector::iterator it = particles->begin();
	while (it != particles->end())
	{
		// Correct ID is found
		if (it->mID == outputFile->trackID)
		{
			// Write the needed data to the file
			outputFile->fileStream
			// Frame number
			<< mCore->mDataStructureInput.mFrameNumber << "\t"
			// Center (image coordinates)
			<< it->mCenter.x << "\t" << it->mCenter.y << "\t"
			// Center (world coordinates)
			<< it->mWorldCenter.x << "\t" << it->mWorldCenter.y << "\t"
			// Area
			<< it->mArea << "\t"
			// Orientation
			<< it->mOrientation << "\t"
			// Compactness
			<< it->mCompactness;
			if (mCore->mDataStructureInput.FrameTimestamp().IsValid()) {
				wxString date;
				wxString millis;
				millis << mCore->mDataStructureInput.FrameTimestamp().GetMillisecond();
				millis.Pad(3 - millis.Length(), '0', false);
				outputFile->fileStream
				<< "\t" << mCore->mDataStructureInput.FrameTimestamp().GetTicks()
				<< millis.ToAscii();
			}
			outputFile->fileStream << std::endl;
			return;
		}
		it++;
	}
}
