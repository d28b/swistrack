#ifndef _PARTICLEFILTER_
#define _PARTICLEFILTER_

#include "Component.h"
#include "Segmenter.h"
#include "TrackingImage.h"

/** \struct particle
* \brief stores parameters and track id of a particle
* A particle allows for storing its center (sub-pixel accuracy), its size, compactness, orientation, color, and the id 
* of its associated track.
*/
struct particle{
	/** Particle center */
	CvPoint2D32f p;  
	/** Particle color */
	double color;	  
	/** Particle area */
	double area;
	/** The contour's compactness (area/circumference ratio) */
	double compactness;
	/** The contour's orientation */
	double orientation;
	int id;          // associated track
};

class ParticleFilter : public Component
{
public:
	ParticleFilter(xmlpp::Element* cfgRoot,TrackingImage* trackingimg);
	virtual ~ParticleFilter();

	void SetParameters();
	void GetParticlesFromContours();
	vector<particle>* Step();

	// Interface to 'Segmenter' class //
	int GetStatus();
	int init(int overhead=2);
	double GetProgress(int kind);
	double GetFPS();
	void RefreshCoverage();

private:

	Segmenter* segmenter;
	vector<particle> particles;
	TrackingImage* trackingimg;

	/** Maximum area of blob  [pix^2] */
	int max_area;				
	/** Minimum area of blob  [pix^2] */
	int min_area;				
	
	
};

#endif 