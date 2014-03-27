#include "ComponentMotionTemplateParticleDetector.h"
#define THISCLASS ComponentMotionTemplateParticleDetector

#include "DisplayEditor.h"
#include "Utility.h"
#include <highgui.h>
#include <limits>
#include <iostream>
#include <set>

using namespace std;

THISCLASS::ComponentMotionTemplateParticleDetector(SwisTrackCore *stc):
		Component(stc, wxT("MotionTemplateParticleDetector")),
		mParticles(), mOutputImage(0),
		mMhiDuration(1),
		N(4), buf(0), last(0), mhi(0),
		orient(0), mask(0), segmask(0), mHsv(0), storage(0), firstTimestamp(-1),
		mForegroundMask(0),
		mDisplayOutput(wxT("Output"), wxT("Tracking"))

{
	// Data structure relations
	mCategory = &(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureInput));
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));

	// This was going to be a tracker when I started writing it
	// but then I realized it was better to use it for particle
	// detection and use DynamicNearestNeighborTracking for tracking.
	AddDataStructureWrite(&(mCore->mDataStructureParticles)); 
	//AddDataStructureWrite(&(mCore->mDataStructureTracks));
	AddDisplay(&mDisplayOutput);
	memset(mInputChannels, 0, 3);
	Initialize();						// Read the XML configuration file
}

THISCLASS::~ComponentMotionTemplateParticleDetector()
{

}

void THISCLASS::OnStart()
{
	THISCLASS::OnReloadConfiguration();

}

void THISCLASS::OnReloadConfiguration()
{


  mDiffThreshold = GetConfigurationInt(wxT("DiffThreshold"), 30);
  mMhiDuration = GetConfigurationDouble(wxT("MhiDuration"), 1);
  mSegmentThreshold = GetConfigurationDouble(wxT("SegmentThreshold"), 0.5);
  mMaxTimeDelta = GetConfigurationDouble(wxT("MaxTimeDelta"), 0.5);
  mMinTimeDelta = GetConfigurationDouble(wxT("MaxTimeDelta"), 0.05);

}

void THISCLASS::OnStep()
{

	wxTimeSpan timeSinceLastFrame = mCore->mDataStructureInput.TimeSinceLastFrame();
	IplImage *inputImage = mCore->mDataStructureInput.mImage;

	if (! inputImage) {
		AddError(wxT("No input image."));
		return;
	}

	if (mForegroundMask == NULL) {
	  mForegroundMask = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);
	}
	for (DataStructureParticles::tParticleVector::iterator pIt = 
	       mParticles.begin(); pIt != mParticles.end(); pIt++) {	
	  cvReleaseHist(&(pIt->mColorModel));
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
	
	update_mhi(inputImage, mOutputImage, mCore->mDataStructureImageBinary.mImage, timestamp, mDiffThreshold,
		   mCore->mDataStructureInput.FrameTimestamp());
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
}



// parameters:
//  img - input video frame
//  dst - resultant motion picture
//  args - optional parameters
void  THISCLASS::update_mhi( IplImage* inputImage, IplImage* dst, IplImage * foregroundMask, double timestampIn,
                             int diff_threshold, wxDateTime frameTimestamp )
{
	//double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
	if (firstTimestamp == -1) {
		firstTimestamp = timestampIn - 0.01;
	}
	double timestamp = timestampIn - firstTimestamp;
	//printf("ts: %.3f\n", timestamp);
	//double timestamp = timestampIn;

	CvSize size = cvSize(inputImage->width, inputImage->height); // get current frame size
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
	if ( !mhi || mhi->width != size.width || mhi->height != size.height ) {
		if ( buf == 0 ) {
			buf = (IplImage**)malloc(N * sizeof(buf[0]));
			memset( buf, 0, N*sizeof(buf[0]));
		}

		for ( i = 0; i < N; i++ ) {
			cvReleaseImage( &buf[i] );
			buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
			cvZero( buf[i] );
		}
		cvReleaseImage( &mhi );
		cvReleaseImage( &orient );
		cvReleaseImage( &segmask );
		cvReleaseImage( &mask );
		cvReleaseImage( &mHsv);
		for (int i = 0; i < 3; i++) {
		  mInputChannels[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
		}

		mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
		cvZero( mhi ); // clear MHI at the beginning
		orient = cvCreateImage( size, IPL_DEPTH_32F, 1 );
		segmask = cvCreateImage( size, IPL_DEPTH_32F, 1 );
		mask = cvCreateImage( size, IPL_DEPTH_8U, 1 );
	}

	cvCvtColor( inputImage, buf[last], CV_BGR2GRAY ); // convert frame to grayscale
	if (mHsv == NULL) {
	  mHsv = cvCloneImage(inputImage);
	}
	cvCvtColor(inputImage, mHsv, CV_BGR2HSV);

	idx2 = (last + 1) % N; // index of (last - (N-1))th frame
	last = idx2;

	silh = buf[idx2];
	cvAbsDiff( buf[idx1], buf[idx2], silh ); // get difference between frames

	cvThreshold( silh, silh, diff_threshold, 1, CV_THRESH_BINARY ); // and threshold it
	cvUpdateMotionHistory( silh, mhi, timestamp, mMhiDuration ); // update MHI

	// convert MHI to blue 8u image
	cvCvtScale( mhi, mask, 255. / mMhiDuration,
	            (mMhiDuration - timestamp)*255. / mMhiDuration );
	cvZero( dst );
	cvCvtPlaneToPix( mask, 0, 0, 0, dst );

	// calculate motion gradient orientation and valid orientation mask
	cvCalcMotionGradient( mhi, mask, orient, mMaxTimeDelta, mMinTimeDelta, 3 );

	if ( !storage )
		storage = cvCreateMemStorage(0);
	else
		cvClearMemStorage(storage);

	// segment motion: get sequence of motion components
	// segmask is marked motion components map. It is not used further
	seq = cvSegmentMotion( mhi, segmask, storage, timestamp, mSegmentThreshold );

	// iterate through the motion components,
	// One more iteration (i == -1) corresponds to the whole image (global motion)
	for ( i = -1; i < seq->total; i++ ) {
	  Particle tmpParticle; // Used to put the calculated value in memory
		if ( i < 0 ) { // case of the whole image
			comp_rect = cvRect( 0, 0, size.width, size.height );
			color = CV_RGB(255, 255, 255);
			magnitude = 100;
		}
		else { // i-th motion component
			comp_rect = ((CvConnectedComp*)cvGetSeqElem( seq, i ))->rect;
			if ( comp_rect.width + comp_rect.height < 100 ) // reject very small components
				continue;
			color = CV_RGB(255, 0, 0);
			magnitude = 30;
		}

		// select component ROI
		cvSetImageROI( silh, comp_rect );
		cvSetImageROI( mhi, comp_rect );
		cvSetImageROI( orient, comp_rect );
		cvSetImageROI( mask, comp_rect );

		// calculate orientation
		angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, mMhiDuration);
		angle = 360.0 - angle;  // adjust for images with top-left origin

		count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI

		cvResetImageROI( mhi );
		cvResetImageROI( orient );
		cvResetImageROI( mask );
		cvResetImageROI( silh );

		// check for the case of little motion
		if ( count < comp_rect.width*comp_rect.height * 0.05 )
			continue;

		// draw a clock with arrow indicating the direction
		center = cvPoint( (comp_rect.x + comp_rect.width / 2),
		                  (comp_rect.y + comp_rect.height / 2) );


		
		tmpParticle.mCenter.x = center.x;
		tmpParticle.mCenter.y = center.y;
		tmpParticle.mTimestamp = frameTimestamp;
		tmpParticle.mFrameNumber = mCore->mDataStructureInput.mFrameNumber;
		
		//int histSizes[] = {100,100,100};
		if (foregroundMask != NULL) {
		  cvZero(mForegroundMask);
		  cvRectangle(mForegroundMask, 
			      cvPoint(comp_rect.x, comp_rect.y),
			      cvPoint(comp_rect.x + comp_rect.width,
				      comp_rect.y + comp_rect.height),
			      cvScalar(255),
			      CV_FILLED);
		  
		  cvAnd(mForegroundMask, foregroundMask, mForegroundMask);
		  cvSplit(mHsv, mInputChannels[0], mInputChannels[1], mInputChannels[2], NULL);

		  int nBins = 16;
		  float h_ranges[] = { 0, 180 }; /* hue varies from 0 (~0°red) to 180 (~360°red again) */
		  float * ranges[] = {h_ranges};
		  //float s_ranges[] = { 0, 255 }; /* saturation varies from 0 (black-gray-white) to 255 (pure spectrum color) */
  
		  tmpParticle.mColorModel = cvCreateHist(1, &nBins, CV_HIST_ARRAY, ranges);
		  cvClearHist(tmpParticle.mColorModel);
		  cvCalcHist(mInputChannels, tmpParticle.mColorModel, 0, mForegroundMask);
		  IplImage * hist_image = Utility::DrawHistogram1D(tmpParticle.mColorModel);
		  cvSaveImage("particle.hist.jpg", hist_image);
		  cvSet(dst, cvScalar(255,255,255), mForegroundMask);
		  //tmpParticle.mArea = comp_rect.width * comp_rect.height;
		  tmpParticle.mArea = cvCountNonZero(mForegroundMask);
		} else {
		  // no histogram when there's no background model. 
		  tmpParticle.mArea = comp_rect.width * comp_rect.height;
		  tmpParticle.mColorModel = NULL;
		}
		
		mParticles.push_back(tmpParticle);


		cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
		cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI / 180)),
		                              cvRound( center.y - magnitude*sin(angle*CV_PI / 180))), color, 3, CV_AA, 0 );
		cvRectangle(dst, 
			    cvPoint(comp_rect.x, comp_rect.y),
			    cvPoint(comp_rect.x + comp_rect.width,
				    comp_rect.y + comp_rect.height),
			    CV_RGB(255,0,0));
	}
}
