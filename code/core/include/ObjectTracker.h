// ObjectTracker.h: interface for the ObjectTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTTRACKER_H__A89773DA_9D7C_4C09_84A2_465536FBD002__INCLUDED_)
#define AFX_OBJECTTRACKER_H__A89773DA_9D7C_4C09_84A2_465536FBD002__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tracker.h"

#include <libxml++/libxml++.h>

class Tracker;
class DataLogger;

class ObjectTracker  
{
public:
	void ClearCoverageImage();
	IplImage* GetRawImagePointer();
	CvPoint2D32f* GetTargetPos(int id);
	CvPoint2D32f* GetParticlePos(int id);
	int GetNumberofTracks();
	int GetNumberofParticles();
	void SetPos(int id,CvPoint2D32f *p);
	void RefreshCoverage();
	void ToggleMaskDisplay();

	IplImage* GetCoveragePointer();
	
	void SetDisplay(int display_vid);
	double GetFPS();
	void SetVisualisation(int style);
	int GetStatus();

	IplImage* GetBinaryPointer();
	IplImage* GetImagePointer();
	void SetParameters();
	int Start();
	double GetProgress(int kind);
	
   
	/** Constructor */
	ObjectTracker(xmlpp::Element* cfgRoot);

	/** Destructor */
	 ~ObjectTracker();

	CvPoint2D32f* GetPos(int id);
	void Stop();
	/** Runs a single step */
	int Step();

protected:
	int stop;

private:
     /** Initializes Components */
    int InitMask();
	void InitTracker();

	/** Tracker class */
	Tracker* tracker;

	/** Datalogger class */
	DataLogger* datalogger;

	/** Color image */
	TrackingImage* trackingimg;
	int pause;
    xmlpp::Element* cfgRoot;
};

#endif // !defined(AFX_OBJECTTRACKER_H__A89773DA_9D7C_4C09_84A2_465536FBD002__INCLUDED_)
