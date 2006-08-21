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
#include "Output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ObjectTracker::ObjectTracker(xmlpp::Element* cfgRoot) : cfgRoot(cfgRoot)
{
	stop=1;
	pause=0;
	tracker=NULL;
	trackingimg=NULL;
	datalogger=NULL;

    if(!cfgRoot){
        throw "[ObjectTracker::ObjectTracker] No configuration specified";
        }

    trackingimg = new TrackingImage();
    tracker=new Tracker(this,cfgRoot,trackingimg);
	datalogger=new Output(this,cfgRoot);
   
}

ObjectTracker::~ObjectTracker()
{
	if(tracker) delete(tracker);
	if(trackingimg) delete(trackingimg);
	if(datalogger) delete(datalogger);
}



/** \brief Stops tracking
* Call this function to stop the segmenter. Image and video file are released. 
* Make sure, you call InitVideo(...) before you start again.
*/
void ObjectTracker::Stop(){
	
	stop=1;
	if(tracker) delete(tracker);
	tracker=NULL;
	if(trackingimg) delete(trackingimg);
	trackingimg=NULL;
}

/*! Given the trajectory, GetPos yields (asynchronously) the latest object position. For getting synchronous data,
*   the ProcessData() method has to be overridden.
*
* \param id : the id number of the trajectory you are interested in
* \return the latest point on the trajectory specified by id
*/
CvPoint2D32f* ObjectTracker::GetPos(int id)
{
	if(tracker)
		return(tracker->GetPos(id));
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
	status=tracker->Step();	
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
	if(tracker)
		return(tracker->GetProgress(kind));
	else
		return 0;
}


/** \brief Starts tracking
*
* Starts tracking only if video and background image were specified using InitVideo(...).
*/
int ObjectTracker::Start()
{
	if(!tracker || !trackingimg){
        throw "InitVideo has not been executed properly";
		return(GENERIC_ERROR);
		}
	stop=0;
	InitTracker();
	return(OK);
//	TrackingThread();
}


/** \brief Sets tracking parameters
* Allows for changing parameters used by the segmenter (bin_threshold, min_area) and the
* data association algorithm (max_area, max_speed).
* Call this function whenever you want during tracking.
*/

void ObjectTracker::SetParameters()
{
	if(tracker)
		tracker->SetParameters();
}

/** \brief Returns a pointer to the current image
*
* Calls TrackingImage::GetImagePointer()
* \return A pointer to the current image
*/
IplImage* ObjectTracker::GetImagePointer()
{
	if(trackingimg)
		return(trackingimg->GetImagePointer());
	else
		return(NULL);
}

/** \brief Returns a pointer to the current binary image
*
* Calls TrackingImage::GetBinaryPointer()
* \return A pointer to the current binary image
*/
IplImage* ObjectTracker::GetBinaryPointer()
{
	if(trackingimg)
		return(trackingimg->GetBinaryPointer());
	else
		return(NULL);
}


/** \brief Returns segmenter status
*
* \return
* enum status_code
*/
int ObjectTracker::GetStatus()
{
	if(!stop)
		return(tracker->GetStatus());
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
if(tracker)
	tracker->SetVisualisation(style);
}

/** \brief Returns FPS (frames per second) of the current video
*
* \return FPS
*/
double ObjectTracker::GetFPS()
{
	if(tracker)
		return(tracker->GetFPS());
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
	if(trackingimg)
		trackingimg->SetDisplay(display_vid);
}


/** \brief Initializes tracker and the trajectories
*
* This method is used internally and calls otSearching for every frame until all objects have been found.
*/
void ObjectTracker::InitTracker()
{
  int status = tracker->init();
  int trials=0;
  while(!stop && status==SEARCHING){
	  trials++;
      status=tracker->init();
      if(trials>1000) throw "Could not distinguish the specified number of objects within the first 1000 frames";
	  }
  if(status==FINISHED){
      throw "Could not distinguish the specified number of objects before the video stream ended";
      }
}


IplImage* ObjectTracker::GetCoveragePointer()
{
	if(trackingimg)
		return(trackingimg->GetCoveragePointer());
	else
		return(NULL);
}

int ObjectTracker::InitMask()
{
if(tracker)
return(tracker->InitMask());
	else
return(0);
	
}

void ObjectTracker::ToggleMaskDisplay()
{
	if(tracker) tracker->ToggleMaskDisplay();
}

void ObjectTracker::RefreshCoverage()
{
	tracker->RefreshCoverage();
}

void ObjectTracker::SetPos(int id, CvPoint2D32f *p)
{
	tracker->SetPos(id,p);
}

CvPoint2D32f* ObjectTracker::GetTargetPos(int id){
	if(tracker) return(tracker->GetTargetPos(id));
	else return(0);
}

CvPoint2D32f* ObjectTracker::GetParticlePos(int id){
	if(tracker) return(tracker->GetParticlePos(id));
		else return(0);
}

int ObjectTracker::GetNumberofTracks(){
	if(tracker) return(tracker->GetNumberofTracks());
		else return(0);
}

int ObjectTracker::GetNumberofParticles(){
	if(tracker) return(tracker->GetNumberofParticles());
		else return(0);
}


/** \brief Returns a pointer to the current frame without overlaid information
*
* Calls TrackingImage::GetRawImagePointer()
* \return A pointer to the current raw image
*/
IplImage* ObjectTracker::GetRawImagePointer()
{
	if(trackingimg)
		return(trackingimg->GetRawImagePointer());
	else
		return(NULL);
}

void ObjectTracker::ClearCoverageImage()
{   
	if(trackingimg) trackingimg->ClearCoverageImage();
}
