#include "ComponentOutputFile.h"
#define THISCLASS ComponentOutputFile

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
	DataStructureTracks::tTrackVector *mTracks;
	mTracks = mCore->mDataStructureTracks.mTracks;
	if (! mTracks)
	{
		AddError(wxT("No Track"));
		return;
	}

	//For each track, write data in the corresponding output file
	DataStructureTracks::tTrackVector::iterator it = mTracks->begin();
	while (it != mTracks->end())
	{
		bool noCorrespondingFile = true;
		filesVector::iterator it2 = mFilesVector.begin();
		while (it2 != mFilesVector.end())
		{
			//There is an existing file opened
			if (it->mID == (*it2)->trackID)
			{
				//Write the data to the file
				writeData(*it2);
				noCorrespondingFile = false;
				break;
			}
			it2++;
		}
		//There was no open filed to write the data, create a file
		if (noCorrespondingFile)
		{
			structOutputFile *newOutputFile = new structOutputFile;
			newOutputFile->trackID = it->mID;
			wxString tmpFileName = mDirectoryName;
			tmpFileName += wxString::Format(wxT("track_%d.txt"), it->mID);
			(newOutputFile->fileStream).open(tmpFileName.mb_str(wxConvISO8859_1), std::fstream::out | std::fstream::trunc);

			if (!(newOutputFile->fileStream).is_open())
			{
				AddError(wxT("Unable to open one of the output file"));
				return;
			}
			mFilesVector.push_back(newOutputFile);
			writeHeader(newOutputFile);
			writeData(newOutputFile);
		}
		it++;
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
	filesVector::iterator it = mFilesVector.begin();
	while (it != mFilesVector.end())
	{
		((*it)->fileStream).close();
		it++;
	}
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
			if (mCore->mDataStructureInput.mFrameTimestamp != wxInvalidDateTime) {
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

