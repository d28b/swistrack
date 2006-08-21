// ObjectTracker.h: interface for the ObjectTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTTRACKER_H__A89773DA_9D7C_4C09_84A2_465536FBD002__INCLUDED_)
#define AFX_OBJECTTRACKER_H__A89773DA_9D7C_4C09_84A2_465536FBD002__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <libxml++/libxml++.h>
#include "cxcore.h"
#include "Segmenter.h" // we include this only for getting the enum values "PAUSE, etc" used in SocketServer
#include "Track.h" // we include this only for getting the enum values "TRAJCROSS, etc" used in SocketServer

//
class Calibration;
class Output;

class ObjectTracker  
{
public:
	CvPoint2D32f* GetTargetPos(int id);
	CvPoint2D32f* GetParticlePos(int id);
	CvPoint2D32f* GetPos(int id);

	IplImage* GetRawImagePointer();
	IplImage* GetCoveragePointer();
	IplImage* GetBinaryPointer();
	IplImage* GetImagePointer();
	
	int GetNumberofTracks();
	int GetNumberofParticles();
	int GetStatus();

	double GetFPS();
	double GetProgress(int kind);

	void ClearCoverageImage();	
	void SetPos(int id,CvPoint2D32f *p);
	void RefreshCoverage();
	void ToggleMaskDisplay();
	void SetDisplay(int display_vid);
	void SetVisualisation(int style);
	void SetParameters();
	
	int Start();
	void Stop();
	/** Runs a single step */
	int Step();

	/** Constructor */
	ObjectTracker(xmlpp::Element* cfgRoot);
	/** Destructor */
	 ~ObjectTracker();


protected:
	int stop;

private:
     /** Initializes Components */
    int InitMask();
	void InitTracker();

	/** Tracker class */
	Calibration* calibration;

	/** Datalogger class */
	Output* datalogger;

	/** Color image */
//	TrackingImage* trackingimg;
	int pause;
    xmlpp::Element* cfgRoot;
};

#endif // !defined(AFX_OBJECTTRACKER_H__A89773DA_9D7C_4C09_84A2_465536FBD002__INCLUDED_)
