/*
 *  ComponentOutputSQLite.h
 *  
 *
 *  Created by Rony Kubat on 6/6/09.
 *  Copyright 2009 MIT Media Lab - Cognitive Machines. All rights reserved.
 *
 */

#ifndef HEADER_ComponentOutputSQLite
#define HEADER_ComponentOutputSQLite

#include "Component.h"

#ifdef USE_SQLITE3

#include "SwisTrackCore.h"
extern "C"  {
    #include <sqlite3.h>
}

//SQLITE_EXTENSION_INIT1;

class ComponentOutputSQLite: public Component {
    
public:
    ComponentOutputSQLite(SwisTrackCore* stc);
    ~ComponentOutputSQLite();
    
    void OnStart();
    void OnReloadConfiguration();
    void OnStep();
    void OnStepCleanup();
    void OnStop();
    
    Component* Create() { return new ComponentOutputSQLite(mCore); }
    
    double GetProgressPercent();
    double GetProgressMSec();
    int    GetProgressFrameNumber();
    double GetFPS();
    
private:
    bool CreateTables();                         //!< returns true on success.
    bool ClearIfNecessary();                     //!< returns true on success.
    bool WriteConfiguration();                   //!< returns true on success.
    bool AttemptInsert(sqlite3_stmt* statement); //!< returns true on success.
    
private:
    sqlite3*        mDB;
    sqlite3_stmt*   mInsertStatement;
    bool            mReady;
    
    wxString        mDirectoryName;         //!< The directory to save
    wxString        mFileName;
    bool            mClearOnStart;
    Display         mDisplayOutput;			//!< The DisplayImage showing the output of this component.

};

#else // USE_SQLITE3

class ComponentOutputSQLite: public Component {

public:
	ComponentOutputSQLite(SwisTrackCore *stc): Component(stc, wxT("OutputSQLite")) {
		Initialize();
	}
	~ComponentOutputSQLite() {}

	// Overwritten Component methods
	void OnStart() {
		AddError(wxT("SQLite support was not compiled into this executable."));
	}
	void OnReloadConfiguration() {
		AddError(wxT("SQLite support was not compiled into this executable."));
	}
	void OnStep() {
		AddError(wxT("SQLite support was not compiled into this executable."));
	}
	void OnStepCleanup() {}
	void OnStop() {
		AddError(wxT("SQLite support was not compiled into this executable."));
	}
	Component *Create() {
		return new ComponentOutputSQLite(mCore);
	}
};

#endif // USE_SQLITE3

#endif
