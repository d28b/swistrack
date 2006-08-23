#include "ParticleFilter.h"



/** \brief Returns the compacteness of a contour
 *
 * Compactness is a contour feature defined as 4*pi*Area/Perimeter^2
 * It is maximum (1) for a circle, 0.78 for a square and \f[\frac{\pi\alpha}{(1+\alpha)^2}\f]
 * for a rectangle where on side is \f[(\alpha \leq 1) \f] smaller than the other.
 *
 * \param contour: a CvSeq* containing the contour
 * \result Returns the compactness 
 */
double  ParticleFilter::GetContourCompactness( const void* contour)
{
 double l = cvArcLength(contour,CV_WHOLE_SEQ,1);
 return fabs(12.56*cvContourArea(contour)/(l*l));	
}  

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
		throw "[ParticleFilter::ParticleFilter] Particlefilter mode undefined (/CFG/COMPONENTS/INPUT)";
	
	mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/PARTICLEFILTER","mode");
	
	switch(mode){
		case 0 : 
			{
				CreateExceptionIfEmpty(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MINAREA");					
				CreateExceptionIfEmpty(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MAXAREA");
				CreateExceptionIfEmpty(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MAXNUMBER");
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
		case 0 : 
			{
				min_area = GetIntValByXPath(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MINAREA");
				max_area = GetIntValByXPath(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MAXAREA");
				max_number = GetIntValByXPath(cfgRoot,"/CFG/PARTICLEFILTER[@mode='0']/MAXNUMBER");
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
void ParticleFilter::GetParticlesFromContours()
{
	switch(mode)
	{
	case 0:
		{
			//Control if max_number is not to small
			if (max_number<=0)
				throw "[ParticleFilter::GetParticlesFromContours] Max Number of blob must be greater or equal to 1";
			//We get the binary image from the segmenter, we make a copy because the image is modified during blobs extraction
			IplImage* src_tmp = cvCloneImage(segmenter->GetBinaryImage());
			//We clear the ouput vector
			particles.clear();
			particle tmpParticle; //Used to put the calculated value in memory
			CvMoments moments; //Used to calculate the moments
			std::vector<particle>::iterator j;//Iterator used to stock the particles by size
			//We allocate memory to extract the contours from the binary image
			CvMemStorage* storage = cvCreateMemStorage(0);
			CvSeq* contour = 0;
			//Init blob extraxtion						
			CvContourScanner blobs = cvStartFindContours(src_tmp,storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
			
			// This is used to correct the position in case of ROI
			CvRect rectROI;
			if(src_tmp->roi != NULL)
				rectROI = cvGetImageROI(src_tmp);
			else
			{
				rectROI.x = 0;
				rectROI.y = 0;
			}

			while((contour=cvFindNextContour(blobs))!=NULL)
			{
				//calculating the moments
				cvMoments(contour,&moments);
				//Calculating Particle Area
				tmpParticle.area=moments.m00;				

				//Selection based on area
				if ((tmpParticle.area<=max_area)&&(tmpParticle.area>=min_area))
				{
					//Check if we have already enough particles
					if (particles.size()==max_number)
					{
						//If the particle is bigger than le smaller particle stored, stored it, else do nothing
						if (tmpParticle.area>(particles.back()).area)
						{
							//Find the place were it must be inserted, sorted by size
							for(j=particles.begin();(j!=particles.end())&&(tmpParticle.area <(*j).area);j++);
							
							//Caculate the important values						
							(tmpParticle.p).x=(float)(rectROI.x + (moments.m10/moments.m00+0.5));  // moments using Green theorema
							(tmpParticle.p).y=(float)(rectROI.y + (moments.m01/moments.m00+0.5));  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem
							tmpParticle.compactness=GetContourCompactness(contour);
							tmpParticle.id=-1;
							tmpParticle.orientation=0;

							//Insert the particle
							particles.insert(j,tmpParticle);
							//remove the smallest one
							particles.pop_back();
						}
					}
					else //The particle is added at the correct place
					{
						//Find the place were it must be inserted, sorted by size
						for(j=particles.begin();(j!=particles.end())&&(tmpParticle.area <(*j).area);j++);
						
						//Caculate the important values						
						(tmpParticle.p).x=(float)(rectROI.x + (moments.m10/moments.m00+0.5));  // moments using Green theorema
						(tmpParticle.p).y=(float)(rectROI.y + (moments.m01/moments.m00+0.5));  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem
						tmpParticle.compactness=GetContourCompactness(contour);
						tmpParticle.id=-1;
						tmpParticle.orientation=0;

						//Insert the particle
						particles.insert(j,tmpParticle);
					}
				}
			}
			//If we need to display the particles
			if(trackingimg->GetDisplay())
			{
				for(j=particles.begin();j!=particles.end();j++)
				{
					trackingimg->DrawCircle(cvPoint((int)(((*j).p).x),(int)(((*j).p).y)),CV_RGB(0,255,0));
					//trackingimg->Cover(cvPoint((int)(((*j).p).x),(int)(((*j).p).y)),CV_RGB(255-((j*).area),0,0),2);
					trackingimg->Cover(cvPoint((int)(((*j).p).x),(int)(((*j).p).y)),CV_RGB(255,0,0),2);
				}
			}
			cvReleaseImage(&src_tmp);
			cvReleaseMemStorage(&storage);
			cvRelease((void**)&contour);
		}
		break;
	default: throw "[ParticleFilter::GetParticlesFromContours()] Mode not implemented.";
	}
}