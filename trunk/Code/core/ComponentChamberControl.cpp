#include "ComponentChamberControl.h"
#define THISCLASS ComponentChamberControl

#include <highgui.h>
#include "DisplayEditor.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>



using namespace cv;
using namespace std;


#define NUMBER_VALVES 4
#define VALVEPORT "RD"
const int valve_mapping[NUMBER_VALVES] = {0,1,2,3};

THISCLASS::ComponentChamberControl(SwisTrackCore *stc):
		Component(stc, wxT("ChamberControl")) {

	mCategory = &(mCore->mCategoryOutput);
	// Read the XML configuration file
	Initialize();
	
	currentConfig.Device=wxT("");      
	for (int i=0; i<NUMBER_VALVES; i++) currentConfig.valves[i]=false;
	currentConfig.compressor=false;
	currentConfig.pump=false;
	currentConfig.pumpspeed=0.0;
	
	
	serial_fd=0;
}

THISCLASS::~ComponentChamberControl() {
	for (int i=0; i<NUMBER_VALVES; i++) {
		printf("switch valve %i : off \n", i);
		switch_valve(i, false);
	}

	printf("switch compressor: off \n");
	switch_compressor(false);
	
	
	printf("switch pump: off");
	switch_pump(false, 0);
	
	close(serial_fd);
}



void THISCLASS::write_serial_port (char *Buffer, int BytesToWrite)

{
  int AmountWritten = 0;
  int StillToWrite;
  if (serial_fd<=0){
	  printf("serial port not available\n");
	  return;
  }
  
  StillToWrite = BytesToWrite;
  while (StillToWrite > 0) {
    AmountWritten = write(serial_fd, Buffer, StillToWrite);
    if (AmountWritten >= 0) {
      StillToWrite -= AmountWritten;
      Buffer += AmountWritten;
    } 
    if (StillToWrite > 0) {
      usleep(1);
    }
  }
}

void THISCLASS::write_serial(char output)
{
  if (serial_fd<=0){
	  printf("serial port not available\n");
	  return;
  }
  char outbuf=output;
  int AmountWritten = 0;
  while (AmountWritten== 0) {
    AmountWritten = write(serial_fd, &outbuf, 1);
    if (AmountWritten==0) {
      usleep(1);
    }
  }
  
}

void THISCLASS::print_serial(const char* string) {
	const char* s = string;
	while (*s != 0) {
		write_serial(*s);
		s++;
	}
}

void THISCLASS::initialiseChamber(wxString Device) {
	int PortSpeed=B38400;

	if (serial_fd>0) { // serial port already open, close first
		printf("Closing serial device %s \n",currentConfig.Device.mb_str().data());
		close(serial_fd);
	}
	printf("Opening serial device %s \n", Device.mb_str().data());
	currentConfig.Device=Device;
	// open serial port
	serial_fd = open(Device.mb_str(), O_RDWR | O_NOCTTY ); 
	if (serial_fd <0) {
		printf("failed to open serial device %s \n", Device.mb_str().data());
		fflush(stdout);
		return; 
	}
	struct termios newtio;
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = PortSpeed | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | IGNBRK;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;   /* set input mode */
	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 0;   /* blocking read until 0 chars received */
	tcflush(serial_fd, TCIFLUSH);
	tcsetattr(serial_fd,TCSANOW,&newtio);
	printf("success. Sending chamber initialisation... ");
	
	// send initialisation values
	// configure outputs
	print_serial("*CONFIG.TRISC");
	write_serial(0x00);write_serial(0xFF);write_serial(0x0D);
	print_serial("*CONFIG.TRISD");
	write_serial(0x00);write_serial(0xFF);write_serial(0x0D);
	print_serial("*CONFIG.TRISF");
	write_serial(0x00);write_serial(0xFF);write_serial(0x0D);
	
	// enable PWM timer
	print_serial("*CONFIG.TMR");
	write_serial(0x02);write_serial(0x01);write_serial(0x00);write_serial(0x00);write_serial(0x0D);
	
	// configure pressure controller
	print_serial("*CONFIG.PRT100");	write_serial(0x0D);
	print_serial("*CONFIG.PRD20");	write_serial(0x0D);
	
	// enable PWM
	print_serial("RF");
	write_serial(0x80);
	write_serial(0x80);
	write_serial(0x0D);
	// Load other parameters

	
	
	printf("Chamber initialised.\n");
	
}	

void THISCLASS::switch_compressor(bool on_off) {
	if (on_off) {
		print_serial("*CONFIG.PR1");
	} else {
		print_serial("*CONFIG.PR0");
	
	}
	write_serial(0x0D);
	
}

void THISCLASS::switch_valve(int index, bool on_off) {
	print_serial(VALVEPORT);
	int vindex=valve_mapping[index];
	if (on_off) {
		write_serial((1<<vindex)& 0xff);
	} else {
		write_serial(0x00);
	}
	write_serial((1<<vindex)& 0xff);
	write_serial(0x0D);
}

void THISCLASS::switch_pump(bool on_off, int speed) {
	print_serial("CCP");
	write_serial(0x01);
	if (speed<0) {
		write_serial(0xCC);
	} else {
		write_serial(0x4C);
	}
	if (on_off) {
		write_serial(abs(speed) & 1023);
	} else {
		write_serial(0);
	}
	write_serial(0x0D);
	
}

void THISCLASS::OnStart() {

	return;
}

void THISCLASS::OnReloadConfiguration() {
	ChamberConfiguration_t newConfig;
	
	newConfig.Device = GetConfigurationString(wxT("SerialDeviceValves"), wxT("/dev/ttyACM0"));
	
	newConfig.valves[0]=GetConfigurationBool(wxT("Valve1"), false);
	newConfig.valves[1]=GetConfigurationBool(wxT("Valve2"), false);
	newConfig.valves[2]=GetConfigurationBool(wxT("Valve3"), false);
	newConfig.valves[3]=GetConfigurationBool(wxT("Valve4"), false);

	newConfig.pump=GetConfigurationBool(wxT("Pump"), false);
	newConfig.pumpspeed=GetConfigurationDouble(wxT("PumpSpeed"), 0);
	newConfig.compressor=GetConfigurationBool(wxT("Compressor"), false);

	
	if ((newConfig.Device!=currentConfig.Device)||(serial_fd<=0)) {
		initialiseChamber(newConfig.Device);
	}

	for (int i=0; i<NUMBER_VALVES; i++) {
		if (newConfig.valves[i] != currentConfig.valves[i]) {
			printf("switch valve %i : %i \n", i, newConfig.valves[i]);
			switch_valve(i, newConfig.valves[i]);
		}
	}

	if (newConfig.compressor!= currentConfig.compressor) {
		printf("switch compressor: %i \n", newConfig.compressor);
		switch_compressor(newConfig.compressor);
	}
	
	if ((newConfig.pump != currentConfig.pump)|| (newConfig.pumpspeed != currentConfig.pumpspeed)) {
		printf("switch pump: %i speed: %i\n", newConfig.pump, newConfig.pumpspeed);
		switch_pump(newConfig.pump, newConfig.pumpspeed);
	}

	currentConfig=newConfig;
}

void THISCLASS::OnStep() {
	
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

