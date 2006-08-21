// ObjectTracker.cpp: implementation of the ObjectTracker class.
//
//////////////////////////////////////////////////////////////////////
/*! 
 * \class ObjectTracker
 * \brief ObjectTracker: Object tracking in video frames
 *
*/

/* \class ObjectTracker
* \brief the main tracking class providing an interface to your application
*/

#include "ObjectTracker.h"
#include "Calibration.h"
#include "Output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ObjectTracker::ObjectTracker(xmlpp::Element* cfgRoot) : cfgRoot(cfgRoot)
{
	stop=1;
	pause=0;
	calibration=NULL;
//	trackingimg=NULL;
	datalogger=NULL;

    if(!cfgRoot){
        throw "[ObjectTracker::ObjectTracker] No configuration specified";
        }

    calibration=new Calibration(this,cfgRoot);
	datalogger=new Output(this,cfgRoot);
   
}

ObjectTracker::~ObjectTracker()
{
	if(calibration) delete(calibration);
	if(datalogger) delete(datalogger);
}



/** \brief Stops tracking
* Call this function to stop the segmenter. Image and video file are released. 
* Make sure, you call InitVideo(...) before you start again.
*/
void ObjectTracker::Stop(){
	
	stop=1;
	if(calibration) delete(calibration);
	calibration=NULL;
//	if(trackingimg) delete(trackingimg);
//	trackingimg=NULL;
}

/*! Given the trajectory, GetPos yields (asynchronously) the latest object position. For getting synchronous data,
*   the ProcessData() method has to be overridden.
*
* \param id : the id number of the trajectory you are interested in
* \return the latest point on the trajectory specified by id
*/
CvPoint2D32f* ObjectTracker::GetPos(int id)
{
	if(calibration)
		return(calibration->GetPos(id));
	else
		return(NULL);
}

/** \brief Performs a single tracking step
* This function is internally used by Start(). It calls the function ProcessData() after
* each step. If the segmentation takes longer than 250ms, calling the Stop() method might yield
* a problem as it tries to free the memory the segmenter is using while it is still running.
*
* \return the segmenter status
*
*/
int ObjectTracker::Step()
{
	int status;
	status=calibration->Step();	
	datalogger->WriteRow();
	return(status);
}


/** \brief Gives AVI progress
* 
* \param kind : format of the progress
*               - 1 = progress in milliseconds
*               - 2 = progress in frames
*               - 3 = progress in percent
* \result Returns progress in the format specified by 'kind'
*
*/ 
double ObjectTracker::GetProgress(int kind)
{
	if(calibration)
		return(calibration->GetProgress(kind));
	else
		return 0;
}


/** \brief Starts tracking
*
* Starts tracking only if video and background image were specified using InitVideo(...).
*/
int ObjectTracker::Start()
{
	if(calibration){
	stop=0;
	InitTracker();
	return(OK);
	}
	else
		return(0);
}


/** \brief Sets tracking parameters
* Allows for changing parameters used by the segmenter (bin_threshold, min_area) and the
* data association algorithm (max_area, max_speed).
* Call this function whenever you want during tracking.
*/

void ObjectTracker::SetParameters()
{
	if(calibration)
		calibration->SetParameters();
}

/** \brief Returns a pointer to the current image
*
* Calls TrackingImage::GetImagePointer()
* \return A pointer to the current image
*/
IplImage* ObjectTracker::GetImagePointer()
{
		return(calibration->GetImagePointer());
}

/** \brief Returns a pointer to the current binary image
*
* Calls TrackingImage::GetBinaryPointer()
* \return A pointer to the current binary image
*/
IplImage* ObjectTracker::GetBinaryPointer()
{
		return(calibration->GetBinaryPointer());
}


/** \brief Returns segmenter status
*
* \return
* enum status_code
*/
int ObjectTracker::GetStatus()
{
	if(!stop)
		return(calibration->GetStatus());
	else
		return(STOPPED);
}

/** \brief Changes display options
*
* \param style
* - TRAJCROSS		: Draws a cross and id for every object
* - TRAJNONUMBER	: Draws a cross without id
* - TRAJFULL		: Draws a full trajectory with cross and id	
* - TRAJNOCROSS		: Draws only the trajectory
*/
void ObjectTracker::SetVisualisation(int style)
{
if(calibration)
	calibration->SetVisualisation(style);
}

/** \brief Returns FPS (frames per second) of the current video
*
* \return FPS
*/
double ObjectTracker::GetFPS()
{
	if(calibration)
		return(calibration->GetFPS());
	else
		return(0);
}

/** \brief Allows to turn display on and off
*
* Turning the display off saves some computation, as trajectories and so on are not
* drawn anymore.
*
* \param display_vid : set to '1' for display and '0' otherwise
*/
void ObjectTracker::SetDisplay(int display_vid)
{
	if(calibration)
		calibration->SetDisplay(display_vid);
}


/** \brief Initializes calibration and the trajectories
*
* This method is used internally and calls otSearching for every frame until all objects have been found.
*/
void ObjectTracker::InitTracker()
{
  int status = calibration->InitTracker();
  int trials=0;
  while(!stop && status==SEARCHING){
	  trials++;
      status=calibration->InitTracker();
      if(trials>1000) throw "Could not distinguish the specified number of objects within the first 1000 frames";
	  }
  if(status==FINISHED){
      throw "Could not distinguish the specified number of objects before the video stream ended";
      }
}


IplImage* ObjectTracker::GetCoveragePointer()
{
	return(calibration->GetCoveragePointer());
}

int ObjectTracker::InitMask()
{
if(calibration)
return(calibration->InitMask());
	else
return(0);
	
}

void ObjectTracker::ToggleMaskDisplay()
{
	if(calibration) calibration->ToggleMaskDisplay();
}

void ObjectTracker::RefreshCoverage()
{
	calibration->RefreshCoverage();
}

void ObjectTracker::SetPos(int id, CvPoint2D32f *p)
{
	calibration->SetPos(id,p);
}

CvPoint2D32f* ObjectTracker::GetTargetPos(int id){
	if(calibration) return(calibration->GetTargetPos(id));
	else return(0);
}

CvPoint2D32f* ObjectTracker::GetParticlePos(int id){
	if(calibration) return(calibration->GetParticlePos(id));
		else return(0);
}

int ObjectTracker::GetNumberofTracks(){
	if(calibration) return(calibration->GetNumberofTracks());
		else return(0);
}

int ObjectTracker::GetNumberofParticles(){
	if(calibration) return(calibration->GetNumberofParticles());
		else return(0);
}


/** \brief Returns a pointer to the current frame without overlaid information
*
* Calls TrackingImage::GetRawImagePointer()
* \return A pointer to the current raw image
*/
IplImage* ObjectTracker::GetRawImagePointer()
{
		return(calibration->GetRawImagePointer());
}

void ObjectTracker::ClearCoverageImage()
{   
	if(calibration) calibration->ClearCoverageImage();
}
