#ifndef HEADER_ComponentColorSwapper
#define HEADER_ComponentColorSwapper

#include <opencv2/core.hpp>
#include "Component.h"
#include "DataStructureParticles.h"

//! A basic tracking component that takes pre-filtered particales and
// adds them to the nearest track.
class ComponentColorSwapper: public Component {

public:
	//! Constructor.
	ComponentColorSwapper(SwisTrackCore * stc);
	//! Destructor.
	~ComponentColorSwapper();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentColorSwapper(mCore);
	}

private:
	class BGR {
	public:
		unsigned char b;
		unsigned char g;
		unsigned char r;

		BGR(wxColour color): b(color.Blue()), g(color.Green()), r(color.Red()) { }
	};

	BGR mSourceColor;
	BGR mTargetColor;
	Display mDisplayOutput;
};

#endif

