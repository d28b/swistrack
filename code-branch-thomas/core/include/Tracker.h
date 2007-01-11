// tracker.h: interface for the tracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACKER_H__AB256777_016A_4984_A1F1_8AC6C1D31D05__INCLUDED_)
#define AFX_TRACKER_H__AB256777_016A_4984_A1F1_8AC6C1D31D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mask.h"
#include "ParticleFilter.h"
#include "TrackingImage.h"
#include "Track.h"
#include "ObjectTracker.h"
#include "OldComponent.h"


class ObjectTracker;

class Tracker : public OldComponent
	{
	public:
		CvPoint2D32f* GetTargetPos(int id);
		CvPoint2D32f* GetParticlePos(int id);
		int GetNumberofTracks();
		int GetNumberofParticles();
		void SetPos(int id,CvPoint2D32f *p);
		int GetIdByPos(CvPoint2D32f *p);
		void RefreshCoverage();
		double GetDist(CvPoint2D32f* p1, CvPoint2D32f* p2);
		CvPoint2D32f* GetCritPoint(int id);
		int GetRestingTrajectories(vector<int>* restingtraj);
		void SetCritPoint(int id);
		void ToggleMaskDisplay();
		int InitMask();
		double GetFPS();
		void SetVisualisation(int style);
		double GetProgress(int kind);
		CvPoint2D32f* GetPos(int id);
		int init();
		/** performs one tracking step */
		int Step();
		/** returns status */
		int GetStatus();
		/** Constructor */
        Tracker(Calibration* parent, xmlpp::Element* cfgRoot, TrackingImage* trackingimg);
		/** Destructor */
		virtual ~Tracker();
		/** Tracking image */
		TrackingImage* trackingimg;
        void SetParameters();
		
	private:
		vector<int> oldshared;
		vector<int> sharedage;
		vector<int> restingtraj;

		int CountSharedTrajectories(vector<int>* shared);
		double Fitness(particle* pa, particle* pb);
		void DrawTrajectories();
		/** Convert list of contours into particles */
		//void GetParticlesFromContours(vector<resultContour>* contours);
		/** Add a point to a track */
		void AddPoint(int i, CvPoint2D32f p);
		/** Calculates cost between two points */
		double GetCost(int id,CvPoint2D32f p);
		/** Deletes all used (associated) particles from the list of particles */
		void CleanParticles();
		void FindFreeParticles(int* fp, int* ap);
		void AssociateParticlesToCompetitors(int max_speed);
		void DataAssociation();
		/** Adds competitor into list, makes sure that entries are not redundant */
		void AddCompetitor(int c);
		void AddLostItem(int c);
		
		Calibration* parent;
		/** Particlefilter class */
		ParticleFilter* particlefilter;
		/** Mask class */
		Mask* mask;
		/** Info about objects being tracked */
		vector<Track> targets;
		/** Potential trajectories */
		vector<Track> ptargets;
		/** status */
		int status;
		/** particles from segmenter*/
		vector<particle>* particles;	 
		
		/** Maximum speed of objects to track */
		int max_speed;	
		/** 'Average' speed of objects to track */
		int avg_speed;

		/** Number of objects to track */
		int nr_objects;
		/** Vector containing the id number of objects competing for tracks */
		vector<int> competitors;
		/** Vector containing the id number of lost items */
		vector<int> lostitems;
		/** Drawing style */
		int style;	
		/** If set to one, trajectories can share a particle, otherwise not (suitable for marker based tracking) */
		int sharedtrajectories;
	};

#endif // !defined(AFX_TRACKER_H__AB256777_016A_4984_A1F1_8AC6C1D31D05__INCLUDED_)
