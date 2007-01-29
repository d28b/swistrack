#include "ComponentSimulateParticles.h"
#define THISCLASS ComponentSimulateParticles

THISCLASS::ComponentSimulateParticles(SwisTrackCore *stc):
		Component(stc, "SimulationParticles"),
		mCameraOrigin(0, 0), mCameraRotation(0), mCameraPixelSize(1), mCameraSize(640, 480),
		mPositionNoise(0), mAngleNoise(1),
		mSimulationParticles(), mParticles() {

	// Data structure relations
	mDisplayName="Particle simulation";
	mCategory=&(mCore->mCategoryBlobDetection);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
}

THISCLASS::~ComponentSimulateParticles() {
}

void THISCLASS::OnStart() {
	bool serious=mCore->IsStartedInSeriousMode();

	// Read the file (if the filename changed or if we are in serious mode)
	std::string filename=GetConfigurationString("File", "");
	if (serious || (mSimulationParticles->mFileName!=filename)) {
		if (! mSimulationParticles->Read(filename)) {
			std::ostringstream oss;
			oss << "The file \'" << file << "\' could not be read";
			AddError(oss.str());
		}
	}

	// Check whether we have a file or not
	if (mSimulationParticles.mFileName=="") {
		AddError("No simulation file loaded.");			
	} else {
		std::ostringstream oss;
		oss << "\'" << mSimulationParticles.mFileName << "\': " << mSimulationParticles.mFrames.count() << " frames";
		AddError(oss.str());
	}

	// Load other values
	OnReloadConfiguration();

	// Start the simulation at the first frame
	SimulationParticles::tFrame *frame=mSimulationParticles.FirstFrame();
	mFrameNumber=frame.number-1;

	// Data structure initialization
	mCore->mDataStructureInput.mFrameNumber=0;
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnReloadConfiguration() {
	mCameraOrigin.x=GetConfigurationDouble("CameraOrigin.x", 0);
	mCameraOrigin.y=GetConfigurationDouble("CameraOrigin.y", 0);
	mCameraRotation=GetConfigurationDouble("CameraRotation", 0);
	mCameraPixelSize=GetConfigurationDouble("CameraPixelSize", 1);
	mCameraSize.x=GetConfigurationInt("CameraSize.x", 640);
	mCameraSize.y=GetConfigurationInt("CameraSize.y", 480);
	mPositionNoiseStdDev=GetConfigurationDouble("PositionNoiseStdDev", 0);
	mAngleNoiseStdDev=GetConfigurationDouble("AngleNoiseStdDev", 0);

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
	SimulationParticles::tFrame *frame=mSimulationParticles.NextFrame(mFrameNumber);
	
	// Copy the particles of the current frame
	mParticles.clear();
	mParticles.insert(frame.particles.begin(), frame.particles.end());

	// Add noise
	Random r;
	DataStructureParticles::tParticleVector::iterator it=mParticles.begin();
	while (it!=mParticles.end()) {
		it->mCenter.x+=r.Normal(0, mPositionNoiseStdDev);
		it->mCenter.y+=r.Normal(0, mPositionNoiseStdDev);
		it->mOrientation+=r.Normal(0, mAngleNoiseStdDev);
		it++;
	}

	// Set these particles
	mCore->mDataStructureInput.mFrameNumber=mFrameNumber;
	mCore->mDataStructureParticles.mParticles=&mParticles;
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureInput.mFrameNumber=0;
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
}
