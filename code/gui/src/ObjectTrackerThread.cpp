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
		cs->Enter();
		int status = ot->GetStatus();
		cs->Leave();
		if (TestDestroy() == true) {
			return NULL;
		}
		if (status == RUNNING) {
			cs->Enter();
			ot->Step();
			cs->Leave();
		}
		Sleep(10);
		
		if (TestDestroy() == true) {
			return NULL;
		}
	}
	return NULL;
}