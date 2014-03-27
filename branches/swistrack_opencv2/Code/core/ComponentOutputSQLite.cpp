/*
 *  ComponentOutputSQLite.cpp
 *  
 *
 *  Created by Rony Kubat on 6/6/09.
 *  Copyright 2009 MIT Media Lab - Cognitive Machines. All rights reserved.
 *
 */

#include "ComponentOutputSQLite.h" 

#ifdef USE_SQLITE3 
#include "ConfigurationWriterXML.h"
#include "Utility.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <wx/mstream.h>
#include <wx/xml/xml.h>

int ComponentOutputSQLite_beginTransaction(sqlite3* db)  { return sqlite3_exec(db, "BEGIN EXCLUSIVE TRANSACTION;", NULL, NULL, NULL); }
int ComponentOutputSQLite_commitTransaction(sqlite3* db) { return sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, NULL); }

using namespace std;

ComponentOutputSQLite::ComponentOutputSQLite(SwisTrackCore* stc):
        Component(stc, wxT("OutputSQLite")),
        mDB(NULL),
        mInsertStatement(NULL),
        mReady(false),
        mDirectoryName(wxT("/tmp")),
        mFileName(wxT("tracks.sqlite")),
        mClearOnStart(false),
        mDisplayOutput(wxT("Output"), wxT("After tracking"))
{
    mCategory = &(mCore->mCategoryOutput);

    AddDisplay(&mDisplayOutput);
    AddDataStructureRead(&(mCore->mDataStructureTracks));
    
    Initialize();
}


ComponentOutputSQLite::~ComponentOutputSQLite()
{

}




void ComponentOutputSQLite::OnStart()
{
    mDirectoryName = GetConfigurationString(wxT("DirectoryName"), wxT("/tmp"));
    mFileName      = GetConfigurationString(wxT("FileName"),      wxT("tracks.sqlite"));
    mClearOnStart  = GetConfigurationBool  (wxT("ClearAtStart"),  true);    
    
    // setup db
    wxString path;
    path << mDirectoryName << wxT("/") << mFileName;

    int   result   = 0;
    char  dbPath[1024] = "";
    
    strncpy(dbPath, path.ToAscii(), sizeof(dbPath));
    
    /// Open the DB
    result = sqlite3_open(dbPath, &mDB);
    if (SQLITE_OK != result) {
        AddError(wxT("Unable to open database file."));
        return;
    }
    
    if (! CreateTables())       { return; }
    if (! ClearIfNecessary())   { return; }
    if (! WriteConfiguration()) { return; }
    
    /// Prepare the insert statement for points.
    result = sqlite3_prepare_v2(mDB, 
                                "INSERT INTO points (trackID, frameNumber, imageX, imageY, worldX, worldY, area, orientation, compactness, timestamp) VALUES (?,?,?,?,?,?,?,?,?,?);",
                                -1,
                                &mInsertStatement,
                                0);
    if (SQLITE_OK != result) {
        AddError(wxString::FromAscii(sqlite3_errmsg(mDB)));
        return;
    }
    
    mReady = true;
}


bool ComponentOutputSQLite::AttemptInsert(sqlite3_stmt* statement)
{
    bool retry = false;
    do {
        retry = false;
        int result = sqlite3_step(statement);
        
        switch (result) {
            case SQLITE_DONE:   // all is well..
                break;
            case SQLITE_BUSY:
                printf("DB Busy, retrying...\n");
                retry = true;
                usleep(20);
                break;
            case SQLITE_ERROR:  // bad.
                AddError(wxT("SQLITE_ERROR on insert"));
                printf(sqlite3_errmsg(mDB));
                printf("\n");
                return false;                
            case SQLITE_MISUSE: // also bad.
                AddError(wxT("SQLITE_MISUSE on insert"));
                printf(sqlite3_errmsg(mDB));
                printf("\n");
                return false;
            default:            // should never happen...
                printf("Unknown sqlite error code received: %d\n", result);
                return false;
        }
    } while (retry);
    return true;
}



bool ComponentOutputSQLite::CreateTables()
{
    int   result   = SQLITE_OK;
    char* errorMsg = NULL;
    
    result = sqlite3_exec(mDB, 
                          "CREATE TABLE IF NOT EXISTS points (trackID INTEGER, frameNumber INTEGER, imageX REAL, imageY REAL, worldX REAL, worldY REAL, area REAL, orientation REAL, compactness REAL, timestamp REAL);",
                          NULL, /* intentionally ommitted callback function */
                          NULL, /* intentionally ommitted 1st arg to callback function */
                          &errorMsg);
    if (SQLITE_OK != result) {
        if (errorMsg) {
            AddError(wxString::FromAscii(errorMsg));
            sqlite3_free(errorMsg); errorMsg = NULL;
        } else {
            AddError(wxT("Unknown error trying to create points table."));
        }
        return false;
    }
    
    
    result = sqlite3_exec(mDB,
                          "CREATE INDEX trackIDindex on points(trackID asc);",
                          NULL, /* intentionally ommitted callback function */
                          NULL, /* intentionally ommitted 1st arg to callback function */
                          &errorMsg);
    if (SQLITE_OK != result) {
        if (errorMsg) {
            AddError(wxString::FromAscii(errorMsg));
            sqlite3_free(errorMsg); errorMsg = NULL;
        } else {
            AddError(wxT("Unknown error trying to create points index."));
        }
        return false;
    }
    
    
    result = sqlite3_exec(mDB,
                          "CREATE TABLE IF NOT EXISTS configuration (key TEXT, value TEXT);",
                          NULL,
                          NULL,
                          &errorMsg);
    if (SQLITE_OK != result) {
        if (errorMsg) {
            AddError(wxString::FromAscii(errorMsg));
            sqlite3_free(errorMsg); errorMsg = NULL;
        } else {
            AddError(wxT("Unknown error trying to create configuration table."));
        }       
        return false;            
    }
    
    return true;
}


bool ComponentOutputSQLite::ClearIfNecessary()
{
    int   result   = SQLITE_OK;
    char* errorMsg = NULL;

    if (mClearOnStart) {        
        result = sqlite3_exec(mDB, "DELETE FROM points;", NULL, NULL, &errorMsg);
        if (SQLITE_OK != result) {
            if (errorMsg) {
                AddError(wxString::FromAscii(errorMsg));
                sqlite3_free(errorMsg); errorMsg = NULL;
            } else {
                AddError(wxT("Unable to clear previous points from the output file."));
            }            
            return false;
        }
        result = sqlite3_exec(mDB, "DELETE FROM configuration;", NULL, NULL, &errorMsg);
        if (SQLITE_OK != result) {
            if (errorMsg) {
                AddError(wxString::FromAscii(errorMsg));
                sqlite3_free(errorMsg); errorMsg = NULL;
            } else {
                AddError(wxT("Unable to clear previous configuration data from the output file."));
            }            
            return false;
        }        
    }
    return true;
}


bool ComponentOutputSQLite::WriteConfiguration()
{
    int result = SQLITE_OK;
    sqlite3_stmt* configurationInsert = NULL;
    
    result = sqlite3_prepare_v2(mDB, "INSERT INTO configuration (key, value) VALUES (?, ?);", -1, &configurationInsert, 0);
    if (SQLITE_OK != result) {
        AddError(wxString::FromAscii(sqlite3_errmsg(mDB)));
        return false;
    }
    
    /// User name
    {
        sqlite3_bind_text(configurationInsert, 1, "USER",         -1, SQLITE_STATIC);
        sqlite3_bind_text(configurationInsert, 2, getenv("USER"), -1, SQLITE_TRANSIENT);
        if (! AttemptInsert(configurationInsert)) {
            sqlite3_finalize(configurationInsert);
            return false;
        }
        sqlite3_reset(configurationInsert);
    }

    /// Program
    {
        sqlite3_bind_text(configurationInsert, 1, "GENERATOR", -1, SQLITE_STATIC);
        sqlite3_bind_text(configurationInsert, 2, "swistrack", -1, SQLITE_STATIC);
        if (! AttemptInsert(configurationInsert)) {
            sqlite3_finalize(configurationInsert);
            return false;
        }
        sqlite3_reset(configurationInsert);
    }
        
    /// Host name
    {
        char hostname[512] = "";
        gethostname(hostname, 512);
        hostname[511] = 0; // force null termination so sqlite doesn't balk.
        sqlite3_bind_text(configurationInsert, 1, "HOSTNAME", -1, SQLITE_STATIC);
        sqlite3_bind_text(configurationInsert, 2, hostname,   -1, SQLITE_TRANSIENT);
        if (! AttemptInsert(configurationInsert)) {
            sqlite3_finalize(configurationInsert);
            return false;
        }
        sqlite3_reset(configurationInsert);
    }

    /// Start time
    {
        time_t rawtime;
        struct tm* timeinfo;
        char timeString[32] = "";
    
        time (&rawtime);
        timeinfo = localtime(&rawtime);
        asctime_r(timeinfo, timeString);
        timeString[strlen(timeString) - 1] = 0; // get rid of training newline.
        
        sqlite3_bind_text(configurationInsert, 1, "START_TIME", -1, SQLITE_STATIC);
        sqlite3_bind_text(configurationInsert, 2, timeString,   -1, SQLITE_TRANSIENT);
        if (! AttemptInsert(configurationInsert)) {
            sqlite3_finalize(configurationInsert);
            return false;
        }
        sqlite3_reset(configurationInsert);
    }
    
    /// The configuration being run...
    {
        
        ConfigurationWriterXML writer;
        writer.WriteComponents(mCore);

        wxMemoryOutputStream outStream(NULL, 0);
        wxXmlDocument* xmlDocument = writer.GetDocument();
        if (! xmlDocument->Save(outStream)) {
            sqlite3_finalize(configurationInsert);
            return false;
        }
        size_t const length = outStream.GetLength();
        char* configText = (char*) malloc(sizeof(*configText) * length);
        outStream.CopyTo(configText, length);
        
        sqlite3_bind_text(configurationInsert, 1, "CONFIG_XML", -1, SQLITE_STATIC);
        sqlite3_bind_text(configurationInsert, 2, configText, -1, SQLITE_TRANSIENT);
        
        free(configText);
        
        if (! AttemptInsert(configurationInsert)) {
            sqlite3_finalize(configurationInsert);
            return false;
        }
        sqlite3_reset(configurationInsert);        
    }
    
    
    sqlite3_finalize(configurationInsert);
    return true;
}



void ComponentOutputSQLite::OnReloadConfiguration() { }


void ComponentOutputSQLite::OnStep()
{
    if (! mReady) return;
    
    DataStructureTracks::tTrackMap* const tracks = mCore->mDataStructureTracks.mTracks;
    if (! tracks) {
        AddError(wxT("No Track"));
        return;
    }
    
    ComponentOutputSQLite_beginTransaction(mDB);
    
    DataStructureTracks::tTrackMap::iterator track = tracks->begin();
    while (track != tracks->end()) {
        
        int trackID = track->first;
        
        DataStructureParticles::tParticleVector* particles = mCore->mDataStructureParticles.mParticles;
        if (! particles) {
            AddError(wxT("There are no particles"));
            continue;
        }
            
        DataStructureParticles::tParticleVector::iterator particle = particles->begin();
        while (particle != particles->end()) {
            
            if (particle->mID == trackID) {
                
                sqlite3_reset(mInsertStatement);
                
                int column = 1; // silly SQL! binding start at 1, not zero!
                sqlite3_bind_int64 (mInsertStatement, column++, trackID);
                sqlite3_bind_int64 (mInsertStatement, column++, particle->mFrameNumber);
                sqlite3_bind_double(mInsertStatement, column++, particle->mCenter.x);
                sqlite3_bind_double(mInsertStatement, column++, particle->mCenter.y);
                sqlite3_bind_double(mInsertStatement, column++, particle->mWorldCenter.x);
                sqlite3_bind_double(mInsertStatement, column++, particle->mWorldCenter.y);
                sqlite3_bind_double(mInsertStatement, column++, particle->mArea);
                sqlite3_bind_double(mInsertStatement, column++, particle->mOrientation);
                sqlite3_bind_double(mInsertStatement, column++, particle->mCompactness);
                sqlite3_bind_double(mInsertStatement, column++, particle->mTimestamp.IsValid() ? (Utility::toMillis(particle->mTimestamp) * 1000.0) : -1);
                
                AttemptInsert(mInsertStatement);
            }
            
            ++particle;
        }
        
        ++track;
    }
    
    ComponentOutputSQLite_commitTransaction(mDB);
}


void ComponentOutputSQLite::OnStepCleanup() { }


void ComponentOutputSQLite::OnStop()
{
    if (! mReady) return;
    
    // close db
    if (mInsertStatement) {
        sqlite3_finalize(mInsertStatement);
        mInsertStatement = NULL;
    }
    
    if (mDB) {
        sqlite3_close(mDB);
        mDB = NULL;
    }
}


double ComponentOutputSQLite::GetProgressPercent()     { return  0; }
double ComponentOutputSQLite::GetProgressMSec()        { return -1; }
int    ComponentOutputSQLite::GetProgressFrameNumber() { return  0; }
double ComponentOutputSQLite::GetFPS()                 { return -1; }

#endif // USE_SQLITE3
