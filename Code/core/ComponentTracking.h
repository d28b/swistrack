#ifndef HEADER_ComponentTracking
#define HEADER_ComponentTracking

#include <opencv2/opencv.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentTracking: public Component {

public:
	//! Constructor.
	ComponentTracking(SwisTrackCore * stc);
	//! Destructor.
	~ComponentTracking();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentTracking(mCore);
	}

private:
	void DataAssociation();
	double GetCost(int id, cv::Point2f p);
	void AddPoint(int i, cv::Point2f p);
	void AddCompetitor(int c);
	void FindFreeParticles(int * fp, int * ap);
	void CleanParticles();
	void AssociateParticlesToCompetitors(int max_speed);
	int CountSharedTrajectories(std::vector<int>* shared);
	double Distance(cv::Point2f p1, cv::Point2f p2);
	void SetCritPoint(int id);
	cv::Point2f GetCritPoint(int id);
	cv::Point2f GetPos(int id);

	DataStructureParticles::tParticleVector * particles;

	std::vector<int> competitors;				//!< Id numbers of objects competing for tracks.
	DataStructureTracks::tTrackMap ptargets;	//!< Potential trajectories.

	std::vector<int> oldshared;
	std::vector<int> sharedage;
	std::vector<int> restingtraj;
	int avg_speed;								//!< 'Average' speed of objects to track.

	// Parameters

	int mShareTrajectories; 					//!< If set to one, trajectories can share a particle, otherwise not (suitable for marker based tracking)
	int mMaxNumber;								//!< (configuration) The maximum number of objects that are to track.
	double mDistanceGate; 						//!< (configuration) The maximum distance a particle can move during one step.
	DataStructureTracks::tTrackMap mTracks;
	Display mDisplayOutput;						//!< The Display showing the last acquired image and the particles.
};

#endif

