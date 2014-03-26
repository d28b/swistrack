#include "ComponentSimulationParticles.h"
#define THISCLASS ComponentSimulationParticles

#include "DisplayEditor.h"

THISCLASS::ComponentSimulationParticles(SwisTrackCore *stc):
		Component(stc, wxT("SimulationParticles")),
		mCameraOrigin(cvPoint2D32f(0, 0)), mCameraRotation(0), mCameraPixelSize(1), mCameraSize(cvSize2D32f(640, 480)),
		mSimulationParticles(0), mParticles(),
		mDisplayOutput(wxT("Output"), wxT("Particle Simulation: Output")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentSimulationParticles() {
	delete mSimulationParticles;
}

void THISCLASS::OnStart() {
	bool production = mCore->IsStartedInProductionMode();

	// Read the file (if the filename changed or if we are in production mode)
	wxString filename_string = GetConfigurationString(wxT("File"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	if (production || (mSimulationParticles == 0) || (mSimulationParticles->GetFileName() != filename)) {
		delete mSimulationParticles;
		mSimulationParticles = new SimulationParticles(filename);
		if (! mSimulationParticles->IsOpen()) {
			AddError(wxT("The file \'") + filename.GetFullPath() + wxT("\' could not be read."));
		} else {
			AddInfo(wxT("File \'") + mSimulationParticles->GetFileName().GetFullPath() + wxT("\' loaded."));
		}
	}

	// Load other values
	OnReloadConfiguration();

	// Start the simulation at the first frame that is available in the file
	SimulationParticles::Frame *frame = mSimulationParticles->FirstFrame();
	if (frame) {
		mFrameNumber = frame->number - 1;
	} else {
		mFrameNumber = 0;
	}

	// Data structure initialization
	mCore->mDataStructureInput.mFrameNumber = 0;
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnReloadConfiguration() {
	mCameraOrigin.x = (float)GetConfigurationDouble(wxT("CameraOrigin.x"), 0);
	mCameraOrigin.y = (float)GetConfigurationDouble(wxT("CameraOrigin.y"), 0);
	mCameraRotation = (float)GetConfigurationDouble(wxT("CameraRotation"), 0);
	mCameraPixelSize = (float)GetConfigurationDouble(wxT("CameraPixelSize"), 0.01);
	mCameraSize.width = (float)GetConfigurationDouble(wxT("CameraSize.w"), 640);
	mCameraSize.height = (float)GetConfigurationDouble(wxT("CameraSize.h"), 480);

	// Check for stupid configurations
	if (mCameraPixelSize <= 0) {
		AddError(wxT("The pixel size must be bigger than 0."));
		mCameraPixelSize = 0.01f;
	}
}

void THISCLASS::OnStep() {
	mFrameNumber++;
	SimulationParticles::Frame *frame = mSimulationParticles->GetFutureFrameByNumber(mFrameNumber);

	// Clear the particles
	mParticles.clear();

	// Add all particles which are in the range of the camera
	DataStructureParticles::tParticleVector::iterator it = frame->particles.begin();
	while (it != frame->particles.end()) {
		float sx = (it->mWorldCenter.x - mCameraOrigin.x) / mCameraPixelSize;
		float sy = (it->mWorldCenter.y - mCameraOrigin.y) / mCameraPixelSize;
		float x = sx * cos(mCameraRotation) - sy * sin(mCameraRotation);
		float y = sx * sin(mCameraRotation) + sy * cos(mCameraRotation);

		if ((x >= 0) && (y >= 0) && (x < mCameraSize.width) && (y < mCameraSize.height)) {
			Particle p;
			p.mID = it->mID;
			p.mCenter.x = x;
			p.mCenter.y = y;
			p.mOrientation = it->mOrientation;
			p.mWorldCenter.x = 0;
			p.mWorldCenter.y = 0;
			mParticles.push_back(p);
		}

		it++;
	}

	// Set these particles
	mCore->mDataStructureInput.mFrameNumber = mFrameNumber;
	mCore->mDataStructureParticles.mParticles = &mParticles;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		//de.SetMode(DisplayEditor::sModeWorldCoordinates);
		de.SetArea(0, 0, mCameraSize.width, mCameraSize.height);
		de.SetParticles(&mParticles);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mFrameNumber = 0;
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
}
