#include "ComponentOutputFile.h"
#define THISCLASS ComponentOutputFile
using namespace std;
#include <iostream>
#include <errno.h>
#include <fstream>
#include "DisplayEditor.h"
#include "Utility.h"

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
	mFileNamePrefix = GetConfigurationString(wxT("FileNamePrefix"), wxT(""));
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// If there is no track, stop
	DataStructureTracks::tTrackMap *mTracks;
	mTracks = mCore->mDataStructureTracks.mTracks;
	if (! mTracks) {
		AddError(wxT("No tracks."));
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

			// Put file name together
			wxString filename_str = mFileNamePrefix + wxString::Format(wxT("_%08d.txt"), it->first);
			wxFileName filename = mCore->GetRunFileName(filename_str);
			if (! filename.IsOk()) {
				AddError(wxT("Invalid output file prefix."));
				return;
			}

			newOutputFile->fileStream.open(filename.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);

			if (! newOutputFile->fileStream.is_open()) {
				wxString msg;
				msg << wxT("Unable to open one of the output files: ");
				msg << filename.GetFullPath() << wxT(" ");
				msg << wxString::FromAscii(strerror(errno));
				AddError(msg);
				return;
			}
			mFiles[it->first] = newOutputFile;
			WriteHeader(newOutputFile->fileStream);
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
	while (it != mFiles.end()) {
		((it->second)->fileStream).close();
		delete it->second;
		it++;
	}
	mFiles.clear();
}

void THISCLASS::WriteHeader(std::fstream & fileStream) {
	fileStream
	<< "%Frame Number" << "\t"
	<< "x (image)" << "\t" << "y (image)" << "\t"
	<< "x (world)" << "\t" << "y (world)" << "\t"
	<< "Area" << "\t"
	<< "Orientation" << "\t"
	<< "Compactness" << "\t"
	<< "TimeStamp (ms)" << std::endl;
}
void THISCLASS::WriteParticle(std::fstream & fileStream, const Particle & p) {
	// Write the needed data to the file
	fileStream
	<< p.mFrameNumber << "\t"
	<< p.mCenter.x << "\t" << p.mCenter.y << "\t"
	<< p.mWorldCenter.x << "\t" << p.mWorldCenter.y << "\t"
	<< p.mArea << "\t"
	<< p.mOrientation << "\t"
	<< p.mCompactness;

	if (p.mTimestamp.IsValid()) {
		fileStream << "\t" << Utility::toMillisString(p.mTimestamp).ToAscii();
	} else  {
		fileStream << "\t -1"; // write -1 in place of an invalid timestamp
	}

	fileStream << std::endl;
}

void THISCLASS::WriteData(structOutputFile *outputFile) {
	// This was originally designed assuming each particles were for only one frame.
	// ComponentMinCostFlow tracking sends particles for many frames at once.
	// This loop still works if particles for each track are sorted in time.
	// So that's part of the spec now.
	//Search for the corresponding particle
	DataStructureParticles::tParticleVector *particles = mCore->mDataStructureParticles.mParticles;
	if (! particles) {
		AddError(wxT("There are no particles"));
		return;
	}

	DataStructureParticles::tParticleVector::iterator it = particles->begin();
	while (it != particles->end()) {
		// Correct ID is found
		if (it->mID == outputFile->trackID) {
			WriteParticle(outputFile->fileStream, *it);
			/*// Write the needed data to the file
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
			  return;*/
		}
		it++;
	}
}

