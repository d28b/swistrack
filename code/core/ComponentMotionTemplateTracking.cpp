#include "ComponentMotionTemplateTracking.h"
#define THISCLASS ComponentMotionTemplateTracking
using namespace std;
#include <iostream>
#include <set>

#include "DisplayEditor.h"

#include "utils.h"

#include <limits>

THISCLASS::ComponentMotionTemplateTracking(SwisTrackCore *stc):
		Component(stc, wxT("MotionTemplateTracking")),
		mNextTrackId(0), mParticles(), mOutputImage(0), 
		MHI_DURATION(1), MAX_TIME_DELTA(0.5),
		MIN_TIME_DELTA(0.05), N(4), buf(0), last(0), mhi(0),
		orient(0), mask(0), segmask(0), storage(0), firstTimestamp(-1),
		mDisplayOutput(wxT("Output"), wxT("Tracking"))
		
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	//AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentMotionTemplateTracking()
{

}

void THISCLASS::OnStart()
{
  THISCLASS::OnReloadConfiguration(); 

}

void THISCLASS::OnReloadConfiguration()
{

  mInitialWindowSize = GetConfigurationInt(wxT("InitialWindowSize"), 60);
  mFrameKillThreshold = GetConfigurationInt(wxT("FrameKillThreshold"), 10);

  mMinNewTrackDistanceSquared = 
    pow(GetConfigurationDouble(wxT("MinNewTrackDistance"), 100), 2);
  mTrackDistanceKillThresholdSquared = 
    pow(GetConfigurationDouble(wxT("TrackDistanceKillThreshold"), 10), 2);

  mMaximumNumberOfTrackers = GetConfigurationInt(wxT("MaximumNumberOfTrackers"), 4);

  mDiffThreshold = GetConfigurationInt(wxT("DiffThreshold"), 30);
}
void THISCLASS::AddNewTracks(IplImage * inputImage) {

  if (mTracks.size() >= mMaximumNumberOfTrackers) {
    // already have enough trackers, so punt. 
    return;
  }

  if (mCore->mDataStructureParticles.mParticles == NULL) {
    AddError(wxT("No input particles!"));
    return;
  }
  const DataStructureParticles::tParticleVector & particles = *mCore->mDataStructureParticles.mParticles;
  for (DataStructureParticles::tParticleVector::const_iterator pIt = 
	 particles.begin();
       pIt != particles.end(); pIt++) {
    assert(pIt->mID == -1); // (particle should not be associated)
    float minSquareDist = std::numeric_limits<float>::max();
    Track * closestTrack = NULL;
    for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
	 i != mTracks.end(); i++) {
      float distance = squareDistance(i->second.trajectory.back(), 
				      pIt->mCenter);
      if (distance < minSquareDist) {
	minSquareDist = distance;
	closestTrack = &i->second;
      }
    }
    if (closestTrack == NULL || minSquareDist > mMinNewTrackDistanceSquared) {
      int id = mNextTrackId++;
      mTracks[id] = Track(id);
      mTracks[id].AddPoint(pIt->mCenter, 
			   mCore->mDataStructureInput.mFrameNumber);
      Particle p;
      p.mCenter = pIt->mCenter;
      p.mID = id;
      p.mArea = pIt->mArea;
      p.mCompactness = pIt->mCompactness;
      p.mOrientation = pIt->mOrientation;
      mParticles.push_back(p);
      
      //mTrackers[id] = camshift();
      //createTracker(&mTrackers[id], inputImage);
      //setVmin(&mTrackers[id], mVmin);
      //setSmin(&mTrackers[id], mSmin);
      //CvRect start = rectByCenter(pIt->mCenter.x, 
      //pIt->mCenter.y,
      //mInitialWindowSize, mInitialWindowSize);
      //startTracking(&mTrackers[id], inputImage, &start);
    }
  }
}

void THISCLASS::FilterTracks() 
{
  set<int> trackIdsToErase;  
  for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
       i != mTracks.end(); i++) {
    Track & track = i->second;
    int frameDifference = mCore->mDataStructureInput.mFrameNumber - track.LastUpdateFrame();
    if ( frameDifference >= mFrameKillThreshold) {
	  trackIdsToErase.insert(i->first);
    }
  }


  for (DataStructureTracks::tTrackMap::iterator i = mTracks.begin();
       i != mTracks.end(); i++) {
    Track & track1 = i->second;
    for (DataStructureTracks::tTrackMap::iterator j = mTracks.begin();
	 j != mTracks.end(); j++) {
      Track & track2 = j->second;
      double cost = squareDistance(track1.trajectory.back(), track2.trajectory.back());
      if (track1.mID != track2.mID && cost < mTrackDistanceKillThresholdSquared) {
	// kill a track - keep the older one
	if (track1.mID < track2.mID) {
	  trackIdsToErase.insert(track2.mID);
	} else {
	  trackIdsToErase.insert(track1.mID);
	  break;
	}
      }
    }
  }
  for (set<int>::iterator i = trackIdsToErase.begin();
       i != trackIdsToErase.end(); i++) {
    EraseTrack(*i);
  }
  
}


void THISCLASS::OnStep()
{	

  wxTimeSpan timeSinceLastFrame = mCore->mDataStructureInput.TimeSinceLastFrame();
  if (timeSinceLastFrame.IsLongerThan(wxTimeSpan::Seconds(5))) {
    cout << "Clearing tracks because there was a gap: " << timeSinceLastFrame.Format().ToAscii() << endl;
    ClearTracks();
  }
  IplImage *inputImage = mCore->mDataStructureImageColor.mImage;
  if (! inputImage) {
    AddError(wxT("No input image."));
    return;
  }
  mParticles.clear();
  /*UpdateTrackers(inputImage);
  
  AddNewTracks(inputImage);
  FilterTracks();

  // update the tracks store locally to this component
  mCore->mDataStructureTracks.mTracks = &mTracks;
  mCore->mDataStructureParticles.mParticles = &mParticles;
  // Let the DisplayImage know about our image
  */
  if (mOutputImage == NULL) {
    mOutputImage = cvCreateImage( cvSize(inputImage->width,
					 inputImage->height), 8, 3 );
    cvZero( mOutputImage );
    mOutputImage->origin = inputImage->origin;
  }  
  wxDateTime ts = mCore->mDataStructureInput.FrameTimestamp();
  double timestamp = ts.GetTicks() + ts.GetMillisecond() / 1000.0;
  update_mhi(inputImage, mOutputImage, timestamp, mDiffThreshold);
  mCore->mDataStructureParticles.mParticles = &mParticles;
  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {



    de.SetMainImage(mOutputImage);

  }
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
   cvReleaseImage(&mOutputImage);
   mOutputImage = 0;
   ClearTracks();
}

void THISCLASS::ClearTracks() {
  mTracks.clear();
   for (std::map<int, camshift>::iterator i = mTrackers.begin();
	i != mTrackers.end(); i++) {    
     releaseTracker(&i->second);
   }
   mTrackers.clear();

}
void THISCLASS::EraseTrack(int id) {
  mTracks.erase(id);
  releaseTracker(&mTrackers[id]);
  mTrackers.erase(id);
}


// parameters:
//  img - input video frame
//  dst - resultant motion picture
//  args - optional parameters
void  THISCLASS::update_mhi( IplImage* img, IplImage* dst, double timestampIn,
			     int diff_threshold )
{
  //double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
  if (firstTimestamp == -1) {
    firstTimestamp = timestampIn - 0.01;
  }
  double timestamp = timestampIn - firstTimestamp;
  printf("ts: %.3f\n", timestamp);
  //double timestamp = timestampIn;
  
    CvSize size = cvSize(img->width,img->height); // get current frame size
    int i, idx1 = last, idx2;
    IplImage* silh;
    CvSeq* seq;
    CvRect comp_rect;
    double count;
    double angle;
    CvPoint center;
    double magnitude;          
    CvScalar color;

    // allocate images at the beginning or
    // reallocate them if the frame size is changed
    if( !mhi || mhi->width != size.width || mhi->height != size.height ) {
        if( buf == 0 ) {
            buf = (IplImage**)malloc(N*sizeof(buf[0]));
            memset( buf, 0, N*sizeof(buf[0]));
        }
        
        for( i = 0; i < N; i++ ) {
            cvReleaseImage( &buf[i] );
            buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
            cvZero( buf[i] );
        }
        cvReleaseImage( &mhi );
        cvReleaseImage( &orient );
        cvReleaseImage( &segmask );
        cvReleaseImage( &mask );
        
        mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        cvZero( mhi ); // clear MHI at the beginning
        orient = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        segmask = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        mask = cvCreateImage( size, IPL_DEPTH_8U, 1 );
    }

    cvCvtColor( img, buf[last], CV_BGR2GRAY ); // convert frame to grayscale

    idx2 = (last + 1) % N; // index of (last - (N-1))th frame
    last = idx2;

    silh = buf[idx2];
    cvAbsDiff( buf[idx1], buf[idx2], silh ); // get difference between frames
    
    cvThreshold( silh, silh, diff_threshold, 1, CV_THRESH_BINARY ); // and threshold it
    cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); // update MHI

    // convert MHI to blue 8u image
    cvCvtScale( mhi, mask, 255./MHI_DURATION,
                (MHI_DURATION - timestamp)*255./MHI_DURATION );
    cvZero( dst );
    cvCvtPlaneToPix( mask, 0, 0, 0, dst );

    // calculate motion gradient orientation and valid orientation mask
    cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );
    
    if( !storage )
        storage = cvCreateMemStorage(0);
    else
        cvClearMemStorage(storage);
    
    // segment motion: get sequence of motion components
    // segmask is marked motion components map. It is not used further
    seq = cvSegmentMotion( mhi, segmask, storage, timestamp, MAX_TIME_DELTA );

    // iterate through the motion components,
    // One more iteration (i == -1) corresponds to the whole image (global motion)
    mParticles.clear();
    Particle tmpParticle; // Used to put the calculated value in memory

    for( i = -1; i < seq->total; i++ ) {

        if( i < 0 ) { // case of the whole image
            comp_rect = cvRect( 0, 0, size.width, size.height );
            color = CV_RGB(255,255,255);
            magnitude = 100;
        }
        else { // i-th motion component
            comp_rect = ((CvConnectedComp*)cvGetSeqElem( seq, i ))->rect;
            if( comp_rect.width + comp_rect.height < 100 ) // reject very small components
                continue;
            color = CV_RGB(255,0,0);
            magnitude = 30;
        }

        // select component ROI
        cvSetImageROI( silh, comp_rect );
        cvSetImageROI( mhi, comp_rect );
        cvSetImageROI( orient, comp_rect );
        cvSetImageROI( mask, comp_rect );

        // calculate orientation
        angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
        angle = 360.0 - angle;  // adjust for images with top-left origin

        count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI

        cvResetImageROI( mhi );
        cvResetImageROI( orient );
        cvResetImageROI( mask );
        cvResetImageROI( silh );

        // check for the case of little motion
        if( count < comp_rect.width*comp_rect.height * 0.05 )
            continue;

        // draw a clock with arrow indicating the direction
        center = cvPoint( (comp_rect.x + comp_rect.width/2),
                          (comp_rect.y + comp_rect.height/2) );
	
	tmpParticle.mArea = comp_rect.width * comp_rect.height;
	tmpParticle.mCenter.x = center.x;
	tmpParticle.mCenter.y = center.y;
	mParticles.push_back(tmpParticle);
	

        cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
        cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
                cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );
    }
}
