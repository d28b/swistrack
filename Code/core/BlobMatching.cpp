#include "BlobMatching.h"
#define THISCLASS BlobMatching

THISCLASS::BlobMatching(): mMaxDistance(10.0), mCenter(0.5), mParticles() {
}

void THISCLASS::ReadConfiguration(Component * component) {
	mMaxDistance = component->GetConfigurationDouble(wxT("MaxDistance"), 10.0);
	mCenter = component->GetConfigurationDouble(wxT("Center"), 0.5);
}

void THISCLASS::Match(DataStructureParticles::tParticleVector particleList1, DataStructureParticles::tParticleVector particleList2) {
	mParticles.clear();

	for (auto & particle1 : particleList1) {
		// Select blob of color 2 which is closest
		Particle * minParticle = NULL;
		double minDistance2 = mMaxDistance * mMaxDistance;
		for (auto & particle2 : particleList2) {
			float dx = particle1.mCenter.x - particle2.mCenter.x;
			float dy = particle1.mCenter.y - particle2.mCenter.y;
			float distance2 = dx * dx + dy * dy;
			if (distance2 >= minDistance2) continue;
			minParticle = &particle2;
			minDistance2 = distance2;
		}

		if (! minParticle) continue;

		// Create particle with this combination of blobs
		Particle particle;
		particle.mArea = minParticle->mArea + particle1.mArea;
		particle.mCenter.x = particle1.mCenter.x + (minParticle->mCenter.x - particle1.mCenter.x) * mCenter;
		particle.mCenter.y = particle1.mCenter.y + (minParticle->mCenter.y - particle1.mCenter.y) * mCenter;
		particle.mOrientation = atan2(minParticle->mCenter.y - particle1.mCenter.y, minParticle->mCenter.x - particle1.mCenter.x);
		particle.mFrameNumber = particle1.mFrameNumber;
		particle.mFrameTimestamp = particle1.mFrameTimestamp;
		mParticles.push_back(particle);
	}
}
