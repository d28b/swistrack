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

void THISCLASS::OnStart()
{
	mDirectoryName = GetConfigurationString(wxT("DirectoryName"), wxT(""));
}

void THISCLASS::OnReloadConfiguration()
{
}

void THISCLASS::OnStep()
{
	//If there is no track, stop
	DataStructureTracks::tTrackMap *mTracks;
	mTracks = mCore->mDataStructureTracks.mTracks;
	if (! mTracks)
	{
		AddError(wxT("No Track"));
		return;
	}

	//For each track, write data in the corresponding output file
	DataStructureTracks::tTrackMap::iterator it = mTracks->begin();
	while (it != mTracks->end())
	{
	  if (mFiles.find(it->first) != mFiles.end()) {
	    //Write the data to the file
	    writeData(mFiles[it->first]);
	  } else {
			structOutputFile *newOutputFile = new structOutputFile;
			newOutputFile->trackID = it->first;
			wxString tmpFileName = mDirectoryName;
			tmpFileName += wxString::Format(wxT("track_%08d.txt"), it->first);
			(newOutputFile->fileStream).open(tmpFileName.mb_str(wxConvISO8859_1), std::fstream::out | std::fstream::trunc);

			if (!(newOutputFile->fileStream).is_open())
			{
				AddError(wxT("Unable to open one of the output file"));
				return;
			}
			mFiles[it->first] = newOutputFile;
			writeHeader(newOutputFile);
			writeData(newOutputFile);
		}
		it++;
	}

	// if a track disappeared, close the file.   Otherwise 
	// we run out of open files eventually.
	for (std::map<int, structOutputFile*>::iterator i = mFiles.begin(); 
	     i != mFiles.end(); i++) {
	  if (mTracks->find(i->first) == mTracks->end()) {
	    mFiles[i->first]->fileStream.close();
	    delete mFiles[i->first];
	    mFiles.erase(i->first);
	  }
	}
	// Set the display
	/*
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
	*/
}

void THISCLASS::OnStepCleanup()
{
}

void THISCLASS::OnStop()
{
	//close all the files
	filesMap::iterator it = mFiles.begin();
	while (it != mFiles.end())
	{
		((it->second)->fileStream).close();
		delete it->second;
		it++;
	}
	mFiles.clear();
}

void THISCLASS::writeHeader(structOutputFile *outputFile)
{
	//Write the header of the file
	outputFile->fileStream
	//Frame number
	<< "%Frame Number" << "\t"
	//image center
	<< "Image Center x" << "\t" << "Image Center y" << "\t"
	//World center
	<< "World Center x" << "\t" << "World Center y" << "\t"
	//Area
	<< "Area" << "\t"
	//Orientation
	<< "Orientation" << "\t"
	//Compactness
	<< "Compactness" << std::endl;
}

void THISCLASS::writeData(structOutputFile *outputFile)
{

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
		//Correct ID is found
		if (it->mID == outputFile->trackID)
		{
			//Write the needed data to the file
			outputFile->fileStream
			//Frame number
			<< mCore->mDataStructureInput.mFrameNumber << "\t"
			//image center
			<< it->mCenter.x << "\t" << it->mCenter.y << "\t"
			//World center
			<< it->mWorldCenter.x << "\t" << it->mWorldCenter.y << "\t"
			//Area
			<< it->mArea << "\t"
			//Orientation
			<< it->mOrientation << "\t"
			//Compactness
			<< it->mCompactness;
			if (mCore->mDataStructureInput.mFrameTimestamp.IsValid()) {
			  wxString date;
			  wxString millis;
			  millis << mCore->mDataStructureInput.mFrameTimestamp.GetMillisecond();
			  millis.Pad(3 - millis.Length(), '0', false);
			  outputFile->fileStream
			    << "\t" << mCore->mDataStructureInput.mFrameTimestamp.GetTicks() 
			    << millis.ToAscii();
			} 
			outputFile->fileStream << std::endl;
			return;
		}
		it++;
	}
}

