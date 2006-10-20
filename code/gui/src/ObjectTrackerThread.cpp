#include "ObjectTrackerThread.h"
#include "SwisTrack.h"
#include "Canvas.h"
#include "AviWriter.h"
#include "SocketServer.h"
#include "SettingsDialog.h"
#include "SwisTrackPanel.h"
#include "InterceptionPanel.h"
#include "GUIApp.h"
#include <string.h>

ObjectTrackerThread::ObjectTrackerThread(ObjectTracker* _ot, wxCriticalSection* _cs)
: wxThread(), ot(_ot), cs(_cs) {

}

ObjectTrackerThread::ObjectTrackerThread(ObjectTrackerThread & that) : wxThread() {

}

ObjectTrackerThread::~ObjectTrackerThread() {
}


void* ObjectTrackerThread::Entry() {
	int status = 0;
	while (true) {
		{
			wxCriticalSectionLocker(*cs);	
			status = ot->GetStatus();
		}
		if (TestDestroy() == true) {
			return NULL;
		}
		if (status == RUNNING) {
			wxCriticalSectionLocker(*cs);	
			ot->Step();
		}
		Sleep(10);
		
		if (TestDestroy() == true) {
			return NULL;
		}
	}
	return NULL;
}