#ifndef HEADER_ComponentPiezoControl
#define HEADER_ComponentPiezoControl

#include <cv.h>
#include "Component.h"
   
   
//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentPiezoControl: public Component {

public:
	//! Constructor.
	ComponentPiezoControl(SwisTrackCore *stc);
	//! Destructor.
	~ComponentPiezoControl();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentPiezoControl(mCore);
	}

private:
	void initialisePiezo(wxString Device);
	wxString device;
	int serial_fd;
	void write_serial(char output);
	void write_serial_port(char *Buffer, int BytesToWrite);
	void print_serial(const char* string);
	void setPiezo(int frequency, int amplitude);
	int rfreq;
	int frequency1;
	int amplitude1;
	
	int frequency2;
	int amplitude2;
	int mode;
	bool active;

};

#endif

