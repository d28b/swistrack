#include "ComponentPiezoControl.h"
#define THISCLASS ComponentPiezoControl

#include <highgui.h>
#include "DisplayEditor.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>



using namespace cv;
using namespace std;

THISCLASS::ComponentPiezoControl(SwisTrackCore *stc):
		Component(stc, wxT("PiezoControl")) {

	mCategory = &(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureCommands));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentPiezoControl() {
}

void THISCLASS::write_serial_port (char *Buffer, int BytesToWrite)

{
  int AmountWritten = 0;
  int StillToWrite;
  if (serial_fd<=0){
	  printf("piezo serial port not available\n");
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
	  printf("piezo serial port not available\n");
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


void THISCLASS::initialisePiezo(wxString Device) {
	int PortSpeed=B115200;

	if (serial_fd>0) { // serial port already open, close first
		printf("Closing serial device %s \n",device.mb_str().data());
		close(serial_fd);
	}
	printf("Opening serial device %s \n", device.mb_str().data());

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
	printf("success. ");
}

void THISCLASS::OnStart() {
	
	device = GetConfigurationString(wxT("SerialDevicePiezo"), wxT("/dev/ttyUSB0"));

	initialisePiezo(device);
	
	// Load other parameters
	OnReloadConfiguration();
	
	return;
}

void THISCLASS::setPiezo(int frequency, int amplitude) {
	char str[15];
	sprintf(str, "%d", frequency);
	print_serial("dds ");
	print_serial(str);
	print_serial("\nrdac ");

	sprintf(str, "%d", amplitude);
	print_serial(str);
	print_serial("\n");
}


void THISCLASS::OnReloadConfiguration() {
	device = GetConfigurationString(wxT("SerialDeviceValves"), wxT("/dev/ttyACM0"));
	
	mode  = GetConfigurationInt(wxT("Mode"), 0);
	
	if (GetConfigurationBool(wxT("Activate"), false)) {
		frequency1 = GetConfigurationInt(wxT("f1"), 0);
		amplitude1 = GetConfigurationInt(wxT("a1"), 0);
		frequency2 = GetConfigurationInt(wxT("f2"), 0);
		amplitude2 = GetConfigurationInt(wxT("a2"), 0);
		
	} else {
		frequency1 = 0;
		amplitude1 = 0;
		frequency2 = 0;
		amplitude2 = 0;
		mode=-1;
		
	}
	switch (mode) {
	case 0:
		printf("assembly\n");
		setPiezo(frequency1, amplitude1);
		break;
	case 1:
		printf("assembly\n");
		setPiezo(frequency2, amplitude2);
		break;

	default:
		setPiezo(0,0);
		break;	
	}
}

void THISCLASS::OnStep() {
	
	if (mode==2) { // if automatic mode chosen
		int piezo_mode=mCore->mDataStructureCommands.mCommands->front().mParamValue;
		int ri=rand()% 10;
		switch (piezo_mode) {
		case -1:
			
			if (ri <2){
				rfreq=39000+(rand()%3000);
			} else 
			if (ri <7){
				rfreq=49000+(rand()%2000);
			} else{
				rfreq=58000+(rand()%4000);				
			}
			setPiezo(rfreq ,amplitude2);
			printf("switch piezo off\n");
			break;
		case 0:
			setPiezo(0 ,0);
			printf("switch piezo off\n");
			break;
		
		case 1:
			setPiezo(frequency1, amplitude1);
			printf("set piezo to assembly\n");
			break;
		case 2:
			printf("set piezo to disassembly\n");
			setPiezo(frequency2, amplitude2);
			break;
		default:
			setPiezo(0,0);
			break;	
		}

	}
	
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

