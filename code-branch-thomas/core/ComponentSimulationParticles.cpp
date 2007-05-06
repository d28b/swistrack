#include "ComponentSimulationParticles.h"
#define THISCLASS ComponentSimulationParticles

#include <sstream>
#include "DisplayEditor.h"
#include "Random.h"

THISCLASS::ComponentSimulationParticles(SwisTrackCore *stc):
		Component(stc, "SimulationParticles"),
		mCameraOrigin(cvPoint2D32f(0, 0)), mCameraRotation(0), mCameraPixelSize(1), mCameraSize(cvSize2D32f(640, 480)),
		mPositionNoiseStdDev(0), mAngleNoiseStdDev(1),
		mSimulationParticles(0), mParticles(),
		mDisplayOutput("Output", "Particle Simulation: Output") {

	// Data structure relations
	mCategory=&(mCore->mCategoryBlobDetection);
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
	bool productive=mCore->IsStartedInProductiveMode();

	// Read the file (if the filename changed or if we are in productive mode)
	std::string filename=GetConfigurationString("File", "");
	if (productive || (mSimulationParticles==0) || (mSimulationParticles->GetFileName()!=filename)) {
		delete mSimulationParticles;
		mSimulationParticles=new SimulationParticles(filename);
		if (! mSimulationParticles->IsOpen()) {
			std::ostringstream oss;
			oss << "The file \'" << filename << "\' could not be read.";
			AddError(oss.str());
		} else {
			std::ostringstream oss;
			oss << "File \'" << mSimulationParticles->GetFileName() << "\' loaded.";
			AddInfo(oss.str());
		}
	}

	// Load other values
	OnReloadConfiguration();

	// Start the simulation at the first frame that is available in the file
	SimulationParticles::tFrame *frame=mSimulationParticles->FirstFrame();
	if (frame) {
		mFrameNumber=frame->number-1;
	} else {
		mFrameNumber=0;
	}

	// Data structure initialization
	mCore->mDataStructureInput.mFrameNumber=0;
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnReloadConfiguration() {
	mCameraOrigin.x=(float)GetConfigurationDouble("CameraOrigin.x", 0);
	mCameraOrigin.y=(float)GetConfigurationDouble("CameraOrigin.y", 0);
	mCameraRotation=(float)GetConfigurationDouble("CameraRotation", 0);
	mCameraPixelSize=(float)GetConfigurationDouble("CameraPixelSize", 1);
	mCameraSize.width=(float)GetConfigurationDouble("CameraSize.w", 640);
	mCameraSize.height=(float)GetConfigurationDouble("CameraSize.h", 480);
	mPositionNoiseStdDev=(float)GetConfigurationDouble("PositionNoiseStdDev", 0);
	mAngleNoiseStdDev=(float)GetConfigurationDouble("AngleNoiseStdDev", 0);

	// Check for stupid configurations
	if (mCameraPixelSize<=0) {
		AddError("The pixel size must be bigger than 0.");
	}
	if (mPositionNoiseStdDev<0) {
		AddError("The position noise must be zero or positive.");
	}
	if (mAngleNoiseStdDev<0) {
		AddError("The angle noise must be zero or positive.");
	}
}

void THISCLASS::OnStep() {
	mFrameNumber++;
	SimulationParticles::tFrame *frame=mSimulationParticles->GetFutureFrameByNumber(mFrameNumber);

	// Clear the particles
	mParticles.clear();

	// Add those particles that are in the range of the camera
	Random r;  // This adds 4 memory leaks (init of static variables) !!!
	DataStructureParticles::tParticleVector::iterator it=frame->particles.begin();
	while (it!=frame->particles.end()) {
		float sx=(it->mCenter.x-mCameraOrigin.x)/mCameraPixelSize;
		float sy=(it->mCenter.y-mCameraOrigin.y)/mCameraPixelSize;
		float x=sx*cos(mCameraRotation)-sy*sin(mCameraRotation);
		float y=sx*sin(mCameraRotation)+sy*cos(mCameraRotation);

		if ((x>=0) && (y>=0) && (x<mCameraSize.width) && (y<mCameraSize.height)) {
			Particle p;
			p.mID=it->mID;
			p.mCenter.x=x+(float)r.Normal(0, mPositionNoiseStdDev);
			p.mCenter.y=y+(float)r.Normal(0, mPositionNoiseStdDev);
			p.mOrientation=it->mOrientation+(float)r.Normal(0, mAngleNoiseStdDev);
			mParticles.push_back(p);
		}

		it++;
	}

	// Set these particles
	mCore->mDataStructureInput.mFrameNumber=mFrameNumber;
	mCore->mDataStructureParticles.mParticles=&mParticles;

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		//de.SetMode(DisplayEditor::sModeWorldCoordinates);
		de.SetArea(0, 0, mCameraSize.width, mCameraSize.height);
		de.SetParticles(&mParticles);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mFrameNumber=0;
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
}
