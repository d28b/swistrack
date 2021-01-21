#ifndef HEADER_ComponentChamberControl
#define HEADER_ComponentChamberControl

#include <cv.h>
#include "Component.h"
   
   
//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentChamberControl: public Component {

public:
	//! Constructor.
	ComponentChamberControl(SwisTrackCore *stc);
	//! Destructor.
	~ComponentChamberControl();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentChamberControl(mCore);
	}

private:
	int serial_fd;
	void write_serial(char output);
	void write_serial_port(char *Buffer, int BytesToWrite);
	void print_serial(const char* string);
	void switch_compressor(bool on_off);
	void switch_valve(int index, bool on_off);
	void switch_pump(bool on_off, int speed);
	void initialiseChamber(wxString Device);
	typedef struct ChamberConfiguration_t {
		wxString Device;
		bool valves[4];
		int pressure;
		bool compressor;
		bool pump;
		int pumpspeed;
	} ChamberConfiguration_t;
	ChamberConfiguration_t currentConfig;
};

#endif

