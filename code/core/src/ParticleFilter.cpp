#include "ParticleFilter.h"


ParticleFilter::ParticleFilter(xmlpp::Element* cfgRoot,TrackingImage* trackingimg)
	: trackingimg(trackingimg)
{
	// 0. Copy configuration root to a local variable
	this->cfgRoot = cfgRoot;

	// 1. Instantiate class one level below
	segmenter = new Segmenter(cfgRoot,trackingimg);	
	segmenter->GetStatus();
	
	// 2. Check whether all necessary parameters for this class are available in the configuration

	if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/PARTICLEFILTER","mode"))
		throw "[Segmenter::Segmenter] Particlefilter mode undefined (/CFG/COMPONENTS/INPUT)";
	
	mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/PARTICLEFILTER","mode");
	
	switch(mode){
		case 0 : {
			if(!IsDefined(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MINAREA"))
				throw "[ParticleFilter::ParticleFilter] no parameters for particle filter mode 0 (minimum area) found";

			if(!IsDefined(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MAXAREA"))
				throw "[ParticleFilter::ParticleFilter] no parameters for particle filter mode 0 (maximal area) found";
				 }
				 break;
		default : throw "[ParticleFilter::ParticleFilter] Invalid mode.";
	}
	
	// 4. Fetch parameters from the configuration
	SetParameters();
}	


void ParticleFilter::SetParameters()
{
	switch(mode){
		case 0 : {
				min_area = GetIntValByXPath(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MINAREA");
				max_area = GetIntValByXPath(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MAXAREA");
				 }
				 break;
		default : throw "[ParticleFilter::ParticleFilter] Invalid mode.";
	}

	segmenter->SetParameters();
}

int ParticleFilter::GetStatus()
{
	return segmenter->GetStatus();
}

int ParticleFilter::init(int overhead)
{
	return segmenter->init(overhead);
}

ParticleFilter::~ParticleFilter()
{
	if(segmenter){
		delete(segmenter);
		segmenter=NULL;
		}
}

vector<particle>* ParticleFilter::Step()
{
	int status=segmenter->Step();
	if(status==RUNNING){
		GetParticlesFromContours();
		return(&particles);
	}
	else
		return(0);
}

double ParticleFilter::GetProgress(int kind)
{
	return(segmenter->GetProgress(kind));
}

double ParticleFilter::GetFPS()
{
	return(segmenter->GetFPS());
}

void ParticleFilter::RefreshCoverage()
{
	segmenter->RefreshCoverage();
}

/** Convert list of contours into particles 
* Allows to filter particles according to tracking/segmentation parameters. Currently,
* the function distinguishes normal and large contours (collisions) but does not use this feature.
* While contours store additional parameters (e.g. area, compactness, ...), particles are given
* only by their position, and the object they are currently associated with.
*
* \param contours : list of contours from segmentation
* \param img : trackingimg to store information relevant to the user (optional)
*
* \todo A possible improvement could be to generate more than one particle from
* contours that are too large.
*/
void ParticleFilter::GetParticlesFromContours(){
	
	vector<resultContour>* contours = segmenter->GetContours();
	int numContours = contours->size();

	int numParticles=0;
	particles.clear();
	
	for(int c=0;c<numContours;c++)
		{                             
		if(contours->at(c).area>=min_area && contours->at(c).area<=max_area){ // Only allow blobs that are within the size constraints
			particle P;
			P.p=contours->at(c).center;
			P.compactness=contours->at(c).compactness;
			P.orientation=contours->at(c).orientation;
			P.area=contours->at(c).area;
			P.color=contours->at(c).color;
			P.id=-1; // initially the particle is unassociated
			particles.push_back(P);
			numParticles++;
			if(trackingimg->GetDisplay())
				trackingimg->DrawCircle(cvPoint((int)(contours->at(c).center.x),(int)(contours->at(c).center.y)),CV_RGB(0,255,0));
			//trackingimg->DrawRectangle(&contours->at(c).boundingrect,CV_RGB(0,0,255));
				trackingimg->Cover(cvPoint((int)(contours->at(c).center.x),(int)(contours->at(c).center.y)),CV_RGB(255-P.area,0,0),2);
			}
		}
	}


