#include <sstream>
#include "calibration.h" 

/*************************************************************************************************/
/************ private types, variables, classes and function prototypes **************************/
/*************************************************************************************************/

int cmx, cmy, cmevent;

void UpdateMouse(int event, int x, int y, int flags,void* param)
	{
		cmx=x;
		cmy=y;
		cmevent=event;
	}  



Calibration::Calibration(char* _bgfname, char* _calfname)
{
    try{    
    cameraMatrix32f = cvCreateMat(2,6,CV_32F);
	transformationMatrix32f = cvCreateMat(2,2,CV_32F);
	cvSetIdentity(transformationMatrix32f,cvRealScalar(1));

	bgfname=_bgfname;
	calfname=_calfname;
	bgimg = cvLoadImage(bgfname,0);
	calibimg = cvLoadImage(calfname,0);
//	arenaboundary = new CvPoint2D32f[8];
	
	meani.x=0;	meani.y=0;
	meano.x=0;	meano.y=0;

	stdi.x=0; stdi.y=0;
	stdo.x=0; stdo.y=0;

	dxy.x=0; dxy.y=0;
        }
    catch (...){
        throw "[Core] Calibration::Calibration";
        }

	/*printf("Mean i: %f/%f\n",meani.x,meani.y);
	printf("Mean o: %f/%f\n",meano.x,meano.y);
	printf("Std i: %f/%f\n",stdi.x,stdi.y);
	printf("Std o: %f/%f\n",stdo.x,stdo.y);*/

}


Calibration::~Calibration()
{
try{
	cvReleaseImage(&bgimg);
	cvReleaseImage(&calibimg);
        }
    catch(...){
        throw "[Core] Calibration::~Calibration";
        }
    }


double Calibration::GetArenaRadius(){
    try{	
	cvNamedWindow(						
		"Estimate Arena Width",				
		CV_WINDOW_AUTOSIZE					
		);									
	
	IplImage* tmpimg=cvCloneImage(bgimg);
	CvPoint2D32f r;

	CvFont font;							
	cvInitFont(&font,CV_FONT_VECTOR0,		
		0.5,0.5,0.0,1);						
	char s[250];					
	sprintf_s(s,"Mark a point on the arena border, right-click when finished");
	cvPutText(tmpimg,s,			
		cvPoint(20,20),&font,CV_RGB(255,255,255));		
	cvShowImage("Estimate Arena Width",tmpimg);
	cvWaitKey(1);		
	
	cvSetMouseCallback("Estimate Arena Width", UpdateMouse);
	cmevent=0;
	int clicked=0;
	while(!clicked){
		if(cmevent==CV_EVENT_LBUTTONDOWN){
			r=ImageToWorld(cvPoint2D32f(cmx-bgimg->width/2,cmy-bgimg->height/2));
			cvCircle(tmpimg,cvPoint(cmx,cmy),1,CV_RGB(255,255,255),1);
			printf("Coordinates of this point(%f/%f)\n",r.x,r.y);
			printf("Distance from center %f m\n",sqrt(r.x*r.x+r.y*r.y));
			cvShowImage("Estimate Arena Width",tmpimg);
			cmevent=0;
			}
		if(cmevent==CV_EVENT_RBUTTONDOWN){
			clicked=1;
			}
		cvWaitKey(250);
		}
	
	cvReleaseImage(&tmpimg);
	return(0);
        }
    catch(...){
        throw "[Core] Calibration::GetArenaRadius";
        }
	}



// -----------------------------------------------------------------------------
//  FIND THE CENTER AND BOUNDARY OF THE ARENA
// -----------------------------------------------------------------------------

int centerthresholdvalue=160;				// unfortunately, we need some 
IplImage *centermask, *centerbg;// *tmask;	// global variables. (opencv stuff)
int patternsize;


void centerthreshold (int id)				// ---------------------------------
{
	centermask=cvCloneImage(centerbg);		// start with the original bg image

	cvThreshold(							// binarize the image
		centerbg,							// to black & grey,
		centermask,							// and store the result in
		centerthresholdvalue,				// centermask
		128,								// 
		CV_THRESH_BINARY					//
		);									//

	cvErode(centermask,centermask,0,2);		// smooth the blobs by first
	cvDilate(centermask,centermask,0,2);	// contracting, then expanding

	cvFloodFill(							// do a watershed transform,
		centermask,							// filling with white pixels,
		cvPoint(							// starting from the center of 
			centermask->width/2,			// the binarized image,
			centermask->height/2			// turning the centermost grey
			),								// blob white.
		CV_RGB(255,255,255)					//
		);									// ??? WHY RGB IN GRAY IMG ???

	cvThreshold(							// now we threshold again
		centermask,							// to keep our white blob,
		centermask,							// but flush out all the other
		254,								// grey blobs.
		255,								//
		CV_THRESH_BINARY					//
		);									//

	cvFloodFill(							// change the pixel value (color) of
		centermask,							// the center blob to 1, from 255.
		cvPoint(							//
			centermask->width/2,			// this creates a binary "mask"
			centermask->height/2			// which will later be multiplied
			),								// into the calibration image,
		CV_RGB(1,1,1));									// clearing everything but the arena

//	tmask=cvCloneImage(centermask);

	cvMul(centermask,centerbg,				// mask the calibration image
		centermask);						//

	cvShowImage("Find the Arena",			// and display it
		centermask);						//
		cvWaitKey(1);						//  (wait for display refresh)
}

bool Calibration::CalibrateCenter()		// ---------------------------------
{
try{
	cvNamedWindow(							// build a window to show the 
		"Find the Arena",					// feedback, so we can properly
		CV_WINDOW_AUTOSIZE					// adjust the thresholding
		);									//

	cvCreateTrackbar(						// add an adjustable slider to
		"Arena",							// our window that will allow
		"Find the Arena",					// us to properly segment out 
		&centerthresholdvalue,				// the arena
		255,								//
		centerthreshold						//
		);									//

	centerbg    = cvCloneImage(bgimg);		// allocate and initialize the
	centermask  = cvCloneImage(bgimg);		// result images.

	centerthreshold(0);						// jump start the slider bar.

	cvWaitKey(0);							// let the user play with the 
											// slider for a while, and then
											// press a key when done.

											// ---------------------------------

	spots.EmptyDetectedBlobsList();			// clear the blob list,
	spots.FindBlobs(centermask);			// and get the new ones,
	printf("Number of blobs: %d\n",spots.GetTotalNumBlobs());
	
	if(spots.GetTotalNumBlobs()>1) spots.SortByArea();						// biggest one first.
	center = spots.GetCenterReal(0);		// the first/biggest blob should
											// be the arena, so its center
											// will be our origin.

	//printf("CENTER: (%f/%f) %f\n",center.x,center.y,spots.GetEllipseSim(0));

	spots.DrawContours(						// outline the arena blob.
		centerbg,cvScalarAll(255),cvScalarAll(0));	// ??? WHY RGB IN GRAY IMG ???

	spots.DisplayCross(centerbg,cvScalarAll(0),1,10,0);	// show the origin as a cross on 
											// the image.
											//   (img,color,thickness,
											//    armlength,blobnumber)
	
	cvShowImage("Find the Arena",centerbg);	// display the image with origin
		cvWaitKey(0);						// so the user can check it.

											// ---------------------------------

	mask=cvCloneImage(centermask);			// store the final arena mask.
	//mask=cvCloneImage(tmask);			// store the final arena mask.

	cvDestroyWindow("Find the Arena");		// clean up all the memory we used. 
	cvReleaseImage(&centermask);			//
	cvReleaseImage(&centerbg);				//
	return true;							// ??? WHO USES THE RETURN VALUE ???
    }
catch (...){
    throw "[Core] Calibration::CalibrateCenter";
    }
}

// -----------------------------------------------------------------------------
//  FIND THE CALIBRATION TARGET
// -----------------------------------------------------------------------------

int outlinethresholdvalue=200,				// unfortunately, we need some 
	calibthresholdvalue=190;				// global variables. (opencv stuff)
IplImage *calibmask, *calibbg,				//
	*calibtarget;							// 

void outlinethreshold (int id)				// ---------------------------------
{
	calibmask=cvCloneImage(calibbg);		// start with the original bg image

	cvThreshold(							// binarize the image
		calibbg,							// to black & grey,
		calibmask,							// and store the result in
		outlinethresholdvalue,				// calibmask
		128,								// 
		CV_THRESH_BINARY					//
		);									//

	cvErode(calibmask,calibmask,0,2);		// smooth the blobs by first
	cvDilate(calibmask,calibmask,0,2);		// contracting, then expanding

	cvFloodFill(							// do a watershed transform,
		calibmask,							// filling with white pixels,
		cvPoint(							// starting from the center of 
			calibmask->width/2,				// the binarized image,
			calibmask->height/2				// turning the centermost grey
			),								// blob white.
		CV_RGB(255,255,255)					//
		);									// ??? WHY RGB IN GRAY IMG ???

	cvThreshold(							// now we threshold again
		calibmask,							// to keep our white blob,
		calibmask,							// but flush out all the other
		254,								// grey blobs.
		255,								//
		CV_THRESH_BINARY					//
		);									//

	cvFloodFill(							// change the pixel value (color) of
		calibmask,							// the center blob to 1, from 255.
		cvPoint(							//
			calibmask->width/2,				// this creates a binary "mask"
			calibmask->height/2				// which will later be multiplied
			),								// into the calibration image,
		CV_RGB(1,1,1));									// clearing everything but the arena
		

	cvMul(calibmask,calibtarget,			// mask the calibration image
		calibmask);							//

	cvShowImage("Find the Calibration Target",		// and display it
		calibmask);							//
		cvWaitKey(1);						//  (wait for display refresh)
}

void calibthreshold (int id)				// ---------------------------------
{
	outlinethreshold(0);

	VisBlobSet squares;
	IplImage* tmp;							// declare temporary image
	//int i=0;

	cvThreshold(							// try to segment out all the
		calibmask, calibmask,				// squares of the calibration 
		calibthresholdvalue, 128,			// target.
		CV_THRESH_BINARY);					//

	squares.EmptyDetectedBlobsList();		// clear the blob list,
	squares.FindBlobs(calibmask);			// and get the new ones,
		printf("Number of blobs: %d\n",squares.GetTotalNumBlobs());
	
	if(squares.GetTotalNumBlobs()>1) squares.SortByArea();					// biggest one first.
	squares.DrawContours(calibmask,cvScalarAll(255),cvScalarAll(255));// outline the located blobs

	CvFont font;							// write the number of spots found
	cvInitFont(&font,CV_FONT_VECTOR0,		// into the image, so that the 
		0.5,0.5,0.0,1);						// user will know when they have 
	ostringstream s;						// successfully adjusted the 
	s	<< "found "							// sliders to allow detection of
		<< squares.GetTotalNumBlobs()-1		// the calibration target.
		<< "/"
		<< patternsize
		<< " spots, press any key when the pattern is properly visible";						//
	tmp = cvCloneImage(calibmask);
	cvPutText(tmp,s.str().c_str(),			//
		cvPoint(20,20),&font,CV_RGB(255,255,255));			//

	cvShowImage("Find the Calibration Target",tmp);	// and display the image
		cvWaitKey(1);						//  (wait for display refresh)

	cvReleaseImage(&tmp);					//
}

bool Calibration::CalibrateTarget()		// ---------------------------------
{
try{
	cvNamedWindow(							// build a window to show the 
		"Find the Calibration Target",				// feedback, so we can properly
		CV_WINDOW_AUTOSIZE					// adjust the thresholding
		);									//

	cvCreateTrackbar(						// add an adjustable slider to
		"Mask",								// our window that will allow
		"Find the Calibration Target",				// us to properly segment out 
		&outlinethresholdvalue,				// the an appropriate border
		255,								// for the region in which we
		outlinethreshold					// expect to find the target.
		);									//
	
	cvCreateTrackbar(						// add another slider for 
		"Squares",							// segmenting the squares of 
		"Find the Calibration Target",				// the calibration chessboard.
		&calibthresholdvalue,				//
		255,								//
		calibthreshold						//
		);									//
	
	calibbg    = cvCloneImage(bgimg);		// allocate and initialize the
	calibmask  = cvCloneImage(bgimg);		// result images.
	calibtarget = cvCloneImage(calibimg);	//
	
	calibthreshold(0);						// jump start the slider bars.

	cvWaitKey(0);							// let the user play with the 
											// sliders for a while, and then
											// press a key when done.

											// ---------------------------------

	spots.EmptyDetectedBlobsList();			// clear the blob list,
	spots.FindBlobs(calibmask);				// and get the new ones,
	printf("Number of blobs: %d\n",spots.GetTotalNumBlobs());
	
	if(spots.GetTotalNumBlobs()>1)	spots.SortByArea();						// biggest one first.
	spots.DrawContours(calibmask,cvScalarAll(255),cvScalarAll(255));	// outline the located blobs

	cvDestroyWindow("Find the Calibration Target");	// clean up all the memory we used. 
	cvReleaseImage(&calibbg);				//
	cvReleaseImage(&calibtarget);			//
	return true;							// ??? WHO USES THE RETURN VALUE ???
    }
catch(...){
    throw "[Core] Calibration::CalibrateTarget";
    }
}

// -----------------------------------------------------------------------------
//  ANALYZE THE CALIBRATION TARGET
// -----------------------------------------------------------------------------


bool Calibration::CalibrateRoundPattern(int h, int* nrofpoints, double vd){

    try{
	int i=0;
	int ps;


	patternsize=1; // global variable used by sliders 
	for(i=0; i<h; i++){
		patternsize+=nrofpoints[i];
		}

	ps=patternsize;

	CalibrateTarget();
	
	cvNamedWindow("Calibration",CV_WINDOW_AUTOSIZE);
	

	spots.EmptyDetectedBlobsList();			// clear the blob list,
	spots.FindBlobs(calibmask);				// and get the new ones,
	spots.SortByArea();						// biggest one first.
	spots.DrawContours(calibmask,cvScalarAll(255),cvScalarAll(255));	// outline the located blobs

	CvPoint2D32f* imagePoints32f =			// declare and allocate arrays of
		new CvPoint2D32f[ps];				// points for calculating the 
	CvPoint3D32f* objectPoints32f =			// transformation operator.
		new CvPoint3D32f[ps];				//

	if (FindRoundPoints(imagePoints32f,objectPoints32f,spots,h,nrofpoints,vd)!=ps)
		{
			delete [] imagePoints32f;
			delete [] objectPoints32f;
			return false;
		}
		

	
 //NONLINEAR FIT

	CvMat* pseudoinverse32f   = cvCreateMat(12,ps*2,CV_32F);	// matrix to hold the pseudoinverse
	CvMat* imagematrix32f     = cvCreateMat(ps*2,12,CV_32F);	// matrix containing points in image space
	CvMat* objectmatrix32f    = cvCreateMat(ps*2,1,CV_32F);	// matrix containing points in world space
	CvMat* cameratransform32f = cvCreateMat(12,1,CV_32F);	// matrix containing camera matrix (nonlinear model)
	



	for(i=0; i<ps; i++){
		meani.x+=imagePoints32f[i].x;
		meani.y+=imagePoints32f[i].y;
		meano.x+=objectPoints32f[i].x;
		meano.y+=objectPoints32f[i].y;
		}
	meani.x/=ps;
	meani.y/=ps;
	meano.x/=ps;
	meano.y/=ps;
	

	
	for(i=0; i<ps; i++){
		stdi.x+=pow(imagePoints32f[i].x-meani.x,2);
		stdi.y+=pow(imagePoints32f[i].y-meani.y,2);
		stdo.x+=pow(objectPoints32f[i].x-meano.x,2);
		stdo.y+=pow(objectPoints32f[i].y-meano.y,2);
		}

	stdi.x=sqrt(stdi.x/(ps)); stdi.y=sqrt(stdi.y/(ps));
	stdo.x=sqrt(stdo.x/(ps)); stdo.y=sqrt(stdo.y/(ps));

   //stdi.x=1; stdi.y=1;
   //stdo.x=1; stdo.y=1;

	// build up matrices
	for(i=0; i<ps; i++)
	{

		CvPoint2D32f p;

		p.x=(imagePoints32f[i].x-meani.x)/stdi.x;
		p.y=(imagePoints32f[i].y-meani.y)/stdi.y;

		imagematrix32f->data.fl[i*12]=p.x;
		imagematrix32f->data.fl[i*12+1]=p.y;
		imagematrix32f->data.fl[i*12+2]=1;
		imagematrix32f->data.fl[i*12+3]=p.x*p.x;
		imagematrix32f->data.fl[i*12+4]=p.y*p.y;
		imagematrix32f->data.fl[i*12+5]=p.x*p.y;
		imagematrix32f->data.fl[i*12+6]=0;
		imagematrix32f->data.fl[i*12+7]=0;
		imagematrix32f->data.fl[i*12+8]=0;
		imagematrix32f->data.fl[i*12+9]=0;
		imagematrix32f->data.fl[i*12+10]=0;
		imagematrix32f->data.fl[i*12+11]=0;
		
		
		imagematrix32f->data.fl[ps*12+i*12]=0;
		imagematrix32f->data.fl[ps*12+i*12+1]=0;
		imagematrix32f->data.fl[ps*12+i*12+2]=0;
		imagematrix32f->data.fl[ps*12+i*12+3]=0;
		imagematrix32f->data.fl[ps*12+i*12+4]=0;
		imagematrix32f->data.fl[ps*12+i*12+5]=0;
		imagematrix32f->data.fl[ps*12+i*12+6]=p.x;
		imagematrix32f->data.fl[ps*12+i*12+7]=p.y;
		imagematrix32f->data.fl[ps*12+i*12+8]=1;
		imagematrix32f->data.fl[ps*12+i*12+9]=p.x*p.x;
		imagematrix32f->data.fl[ps*12+i*12+10]=p.y*p.y;
		imagematrix32f->data.fl[ps*12+i*12+11]=p.x*p.y;

		objectmatrix32f->data.fl[i]=(objectPoints32f[i].x-meano.x)/stdo.x;
		objectmatrix32f->data.fl[i+ps]=(objectPoints32f[i].y-meano.y)/stdo.y;
	}


	CvMat* multransposed    = cvCreateMat(12,12,CV_32F);
	CvMat* invmultransposed = cvCreateMat(12,12,CV_32F);
	CvMat* transposed       = cvCreateMat(12,ps*2,CV_32F);

	cvMulTransposed(imagematrix32f,multransposed,1);
	printf("Matrix inversion: %f\n",cvInv(multransposed,invmultransposed,CV_LU));
	cvTranspose(imagematrix32f,transposed);
	cvMatMul(invmultransposed,transposed,pseudoinverse32f);

	cvMatMul(pseudoinverse32f, objectmatrix32f, cameratransform32f);


/*	FILE* f1=fopen("imagematrix.dat","w");
	FILE* f2=fopen("pseudoinverse.dat","w");
	FILE* f3=fopen("objectmatrix.dat","w");
	
	for(int j=0; j<2*ps; j++){
		for(i=0; i<12; i++){
			fprintf(f1,"%f ",imagematrix32f->data.fl[j*12+i]);
			}
		fprintf(f1,"\n");
		fprintf(f3,"%f\n",objectmatrix32f->data.fl[j]);
		}

	for(j=0; j<12; j++){
		for(i=0; i<2*ps; i++){
			fprintf(f2,"%f ",pseudoinverse32f->data.fl[j*2*ps+i]);
			}
		fprintf(f2,"\n");
		}


	
	fclose(f1);
	fclose(f2);
	fclose(f3);
*/	

	// copy cameratransformation to appropriate format
	for(i=0;i<12;i++)
		cameraMatrix32f->data.fl[i]=cameratransform32f->data.fl[i];

	// finally, calculate displacement from target to image center and angle of rotation

	



	double absu=  sqrt(pow(target.x-xaxis.x,2)+pow(target.y-xaxis.y,2)); // length of xaxis
	CvPoint2D32f newu=cvPoint2D32f(target.x+absu,target.y); // extreme of new x-axis (image space)
	
	// At this point it is important, that dxy is still zero, otherwise the transformatio won't work
	CvPoint2D32f newx=ImageToWorld(newu); // extreme of new x-axis
	
	alpha = -atan2(newx.y,newx.x);	

	printf("Rotational offset: %f degree\n",alpha/3.14159265*180);
	
	transformationMatrix32f->data.fl[0]=(float)(cos(alpha));
	transformationMatrix32f->data.fl[1]=(float)(-sin(alpha));
	transformationMatrix32f->data.fl[2]=(float)(sin(alpha));
	transformationMatrix32f->data.fl[3]=(float)(cos(alpha));

	CvPoint2D32f wc,wt;
	center.x-=bgimg->width/2;  // move center into origin
	center.y-=bgimg->height/2;
	wc=ImageToWorld(center);
	wt=ImageToWorld(target);
	dxy.x=wc.x-wt.x; // remember dxy
	dxy.y=wc.y+wt.y;
	printf("Center offset: (%f/%f)\n",dxy.x,dxy.y);


	/*FILE* f4=fopen("calibrationpattern.dat","w");
	for(j=0; j<ps; j++){
		CvPoint2D32f calibpoint;
		calibpoint=ImageToWorld(imagePoints32f[j]);
		fprintf(f4,"%f %f\n",calibpoint.x,calibpoint.y);
		}
	fclose(f4);


	FILE* f5=fopen("cameramatrix.dat","w");
	for(i=0;i<12;i++){
		
		fprintf(f4,"%d \n",cameraMatrix32f->data.fl[i]);
		printf("CameraMatrix[%d]: %f\n",i,cameraMatrix32f->data.fl[i]);
		}
	fclose(f4);
*/

	SaveError(ps,imagePoints32f,objectPoints32f,"calibration_error.txt");


	cvDestroyWindow("Calibration");			// clean up all the memory we used. 
	cvReleaseMat(&pseudoinverse32f);		// 
	cvReleaseMat(&imagematrix32f);			//
	cvReleaseMat(&objectmatrix32f);			//
	cvReleaseMat(&cameratransform32f);		//
	cvReleaseImage(&calibmask);				//
	
	delete [] imagePoints32f;
	delete [] objectPoints32f;
	return true;							// ??? WHO USES THE RETURN VALUE ???
    }
    catch (...){
        throw "[Core] Calibration::CalibrateRoundPattern";
        }
}

bool Calibration::CalibratePattern(int cw, int ch,double wx, double wy)
{
try{
    int i=0;

	
	patternsize=cw*ch; // global variable used by sliders 
	CalibrateTarget();

	
	cvNamedWindow("Calibration",CV_WINDOW_AUTOSIZE);
	

	spots.EmptyDetectedBlobsList();			// clear the blob list,
	spots.FindBlobs(calibmask);				// and get the new ones,
	spots.SortByArea();						// biggest one first.
	spots.DrawContours(calibmask,cvScalarAll(255),cvScalarAll(255));	// outline the located blobs

	CvPoint2D32f* imagePoints32f =			// declare and allocate arrays of
		new CvPoint2D32f[cw*ch];				// points for calculating the 
	CvPoint3D32f* objectPoints32f =			// transformation operator.
		new CvPoint3D32f[cw*ch];				//

	if (FindPoints(imagePoints32f,objectPoints32f,spots,cw,ch,wx,wy)!=cw*ch)
		{
			delete [] imagePoints32f;
			delete [] objectPoints32f;
			return false;
		}

 //NONLINEAR FIT

	CvMat* pseudoinverse32f   = cvCreateMat(12,cw*ch*2,CV_32F);	// matrix to hold the pseudoinverse
	CvMat* imagematrix32f     = cvCreateMat(cw*ch*2,12,CV_32F);	// matrix containing points in image space
	CvMat* objectmatrix32f    = cvCreateMat(cw*ch*2,1,CV_32F);	// matrix containing points in world space
	CvMat* cameratransform32f = cvCreateMat(12,1,CV_32F);	// matrix containing camera matrix (nonlinear model)
	



	for(i=0; i<cw*ch; i++){
		meani.x+=imagePoints32f[i].x;
		meani.y+=imagePoints32f[i].y;
		meano.x+=objectPoints32f[i].x;
		meano.y+=objectPoints32f[i].y;
		}
	meani.x/=cw*ch;
	meani.y/=cw*ch;
	meano.x/=cw*ch;
	meano.y/=cw*ch;
	

	
	for(i=0; i<cw*ch; i++){
		stdi.x+=pow(imagePoints32f[i].x-meani.x,2);
		stdi.y+=pow(imagePoints32f[i].y-meani.y,2);
		stdo.x+=pow(objectPoints32f[i].x-meano.x,2);
		stdo.y+=pow(objectPoints32f[i].y-meano.y,2);
		}

	stdi.x=sqrt(stdi.x/(cw*ch)); stdi.y=sqrt(stdi.y/(cw*ch));
	stdo.x=sqrt(stdo.x/(cw*ch)); stdo.y=sqrt(stdo.y/(cw*ch));

   //stdi.x=1; stdi.y=1;
   //stdo.x=1; stdo.y=1;

	// build up matrices
	for(i=0; i<cw*ch; i++)
	{

		CvPoint2D32f p;

		p.x=(imagePoints32f[i].x-meani.x)/stdi.x;
		p.y=(imagePoints32f[i].y-meani.y)/stdi.y;

		imagematrix32f->data.fl[i*12]=p.x;
		imagematrix32f->data.fl[i*12+1]=p.y;
		imagematrix32f->data.fl[i*12+2]=1;
		imagematrix32f->data.fl[i*12+3]=p.x*p.x;
		imagematrix32f->data.fl[i*12+4]=p.y*p.y;
		imagematrix32f->data.fl[i*12+5]=p.x*p.y;
		imagematrix32f->data.fl[i*12+6]=0;
		imagematrix32f->data.fl[i*12+7]=0;
		imagematrix32f->data.fl[i*12+8]=0;
		imagematrix32f->data.fl[i*12+9]=0;
		imagematrix32f->data.fl[i*12+10]=0;
		imagematrix32f->data.fl[i*12+11]=0;
		
		
		imagematrix32f->data.fl[cw*ch*12+i*12]=0;
		imagematrix32f->data.fl[cw*ch*12+i*12+1]=0;
		imagematrix32f->data.fl[cw*ch*12+i*12+2]=0;
		imagematrix32f->data.fl[cw*ch*12+i*12+3]=0;
		imagematrix32f->data.fl[cw*ch*12+i*12+4]=0;
		imagematrix32f->data.fl[cw*ch*12+i*12+5]=0;
		imagematrix32f->data.fl[cw*ch*12+i*12+6]=p.x;
		imagematrix32f->data.fl[cw*ch*12+i*12+7]=p.y;
		imagematrix32f->data.fl[cw*ch*12+i*12+8]=1;
		imagematrix32f->data.fl[cw*ch*12+i*12+9]=p.x*p.x;
		imagematrix32f->data.fl[cw*ch*12+i*12+10]=p.y*p.y;
		imagematrix32f->data.fl[cw*ch*12+i*12+11]=p.x*p.y;

		objectmatrix32f->data.fl[i]=(objectPoints32f[i].x-meano.x)/stdo.x;
		objectmatrix32f->data.fl[i+cw*ch]=(objectPoints32f[i].y-meano.y)/stdo.y;
	}


	CvMat* multransposed    = cvCreateMat(12,12,CV_32F);
	CvMat* invmultransposed = cvCreateMat(12,12,CV_32F);
	CvMat* transposed       = cvCreateMat(12,cw*ch*2,CV_32F);

	cvMulTransposed(imagematrix32f,multransposed,1);
	printf("Matrix inversion: %f\n",cvInv(multransposed,invmultransposed,CV_LU));
	cvTranspose(imagematrix32f,transposed);
	cvMatMul(invmultransposed,transposed,pseudoinverse32f);

	cvMatMul(pseudoinverse32f, objectmatrix32f, cameratransform32f);


	/*FILE* f1=fopen("imagematrix.dat","w");
	FILE* f2=fopen("pseudoinverse.dat","w");
	FILE* f3=fopen("objectmatrix.dat","w");
	
	for(int j=0; j<2*cw*ch; j++){
		for(i=0; i<12; i++){
			fprintf(f1,"%f ",imagematrix32f->data.fl[j*12+i]);
			}
		fprintf(f1,"\n");
		fprintf(f3,"%f\n",objectmatrix32f->data.fl[j]);
		}

	for(j=0; j<12; j++){
		for(i=0; i<2*cw*ch; i++){
			fprintf(f2,"%f ",pseudoinverse32f->data.fl[j*2*cw*ch+i]);
			}
		fprintf(f2,"\n");
		}


	
	fclose(f1);
	fclose(f2);
	fclose(f3);
	
*/
	// copy cameratransformation to appropriate format
	for(i=0;i<12;i++)
		cameraMatrix32f->data.fl[i]=cameratransform32f->data.fl[i];

/*	CvPoint2D32f t;
	for(i=0; i<cw*ch; i++)
	{
		t=WorldToImage(cvPoint2D32f(objectPoints32f[i].x,objectPoints32f[i].y));
		cvCircle(calibimg,cvPoint(t.x,t.y),i/3,255,1);
	}
	cvCircle(calibimg,cvPoint(center.x,center.y),2,255,1);
	cvShowImage("Calibration",calibimg);
		cvWaitKey(0);
*/

	
	
	// finally, calculate displacement from target to image center and angle of rotation

	



	double absu=  sqrt(pow(target.x-xaxis.x,2)+pow(target.y-xaxis.y,2)); // length of xaxis
	CvPoint2D32f newu=cvPoint2D32f(target.x+absu,target.y); // extreme of new x-axis (image space)
	
	// At this point it is important, that dxy is still zero, otherwise the transformatio won't work
	CvPoint2D32f newx=ImageToWorld(newu); // extreme of new x-axis
	
	alpha = -atan2(newx.y,newx.x);	

	printf("Rotational offset: %f degree\n",alpha/3.14159265*180);
	
	transformationMatrix32f->data.fl[0]=(float)(cos(alpha));
	transformationMatrix32f->data.fl[1]=(float)(-sin(alpha));
	transformationMatrix32f->data.fl[2]=(float)(sin(alpha));
	transformationMatrix32f->data.fl[3]=(float)(cos(alpha));

	CvPoint2D32f wc,wt;
	center.x-=bgimg->width/2;  // move center into origin
	center.y-=bgimg->height/2;
	wc=ImageToWorld(center);
	wt=ImageToWorld(target);
	dxy.x=wc.x-wt.x; // remember dxy
	dxy.y=wc.y+wt.y;
	printf("Center offset: (%f/%f)\n",dxy.x,dxy.y);


	/*FILE* f4=fopen("calibrationpattern.dat","w");
	for(j=0; j<cw*ch; j++){
		CvPoint2D32f calibpoint;
		calibpoint=ImageToWorld(imagePoints32f[j]);
		fprintf(f4,"%f %f\n",calibpoint.x,calibpoint.y);
		}
	fclose(f4);


	FILE* f5=fopen("cameramatrix.dat","w");
	for(i=0;i<12;i++){
		
		fprintf(f4,"%d \n",cameraMatrix32f->data.fl[i]);
		printf("CameraMatrix[%d]: %f\n",i,cameraMatrix32f->data.fl[i]);
		}
	fclose(f4);
*/
	cvDestroyWindow("Calibration");			// clean up all the memory we used. 
	cvReleaseMat(&pseudoinverse32f);		// 
	cvReleaseMat(&imagematrix32f);			//
	cvReleaseMat(&objectmatrix32f);			//
	cvReleaseMat(&cameratransform32f);		//
	cvReleaseImage(&calibmask);				//
	delete [] imagePoints32f;
	delete [] objectPoints32f;
	return true;							// ??? WHO USES THE RETURN VALUE ???
}
catch(...){
    throw "[Core] Calibration::CalibratePattern";
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------























  /** 1-line description of the method
   * 
   * Detailed description of the method
   * and bla, bla, bla.
   *
   * \param vol: Volume a ajouter
   * \result Returns current level or the overflow as a negative value.
   * \sa Empty()
   */

CvPoint2D32f Calibration::ImageToWorld(CvPoint2D32f p, int transform)
{
try{
	CvPoint2D32f w;

	
	CvMat* ispace    = cvCreateMat(6,1,CV_32F);
	CvMat* result	 = cvCreateMat(2,1,CV_32F);

	
	
	p.x-=meani.x;
	p.y-=meani.y;

	p.x/=stdi.x;
	p.y/=stdi.y;

	ispace->data.fl[0]=p.x;
	ispace->data.fl[1]=p.y;
	ispace->data.fl[2]=1;
	ispace->data.fl[3]=p.x*p.x;
	ispace->data.fl[4]=p.y*p.y;
	ispace->data.fl[5]=p.x*p.y;

	
	cvMatMul(cameraMatrix32f,ispace,result);

	result->data.fl[0]=result->data.fl[0]*stdo.x+meano.x;
	result->data.fl[1]=result->data.fl[1]*stdo.y+meano.y;
	

	if(transform){
		cvMatMul(transformationMatrix32f,result,result);

		w.x=result->data.fl[0]-dxy.x;
		w.y=result->data.fl[1]-dxy.y;
	}
	else{
		w.x=result->data.fl[0];
		w.y=result->data.fl[1];
	}

	cvReleaseMat(&ispace);
	cvReleaseMat(&result);
	return(w);
            }
    catch(...){
        throw "[Core] Calibration::GetArenaRadius";
        }
}

CvPoint2D32f Calibration::WorldToImage(CvPoint2D32f p)
{
try{
	CvMat* invcam = cvCreateMat(3,3,CV_32F);
	CvMat* w      = cvCreateMat(3,1,CV_32F);
	CvMat* u      = cvCreateMat(3,1,CV_32F);
	CvPoint2D32f r;

	w->data.fl[0]=p.x;
	w->data.fl[1]=p.y;
	w->data.fl[2]=1;
	cvInv(cameraMatrix32f,invcam,CV_SVD);
	cvMatMul(invcam,w,u);
	r.x=u->data.fl[0];
	r.y=u->data.fl[1];

	return(r);
            }
    catch(...){
        throw "[Core] Calibration::WorldToImage";
        }
}

/*************************************************************************************************/
/************* private class methods and functions implementations *******************************/
/*************************************************************************************************/


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int Calibration::FindPoints(
	CvPoint2D32f* imagePoints32f, 
	CvPoint3D32f* objectPoints32f,
	VisBlobSet& blobs, 
	int cw, int ch, double wx,double wy, IplImage* img)
{
try{

	if (blobs.GetTotalNumBlobs() != (cw*ch+1))
	{
		cout << "Should have found only " << cw*ch+1 << "blobs. Is ROI defined?" << endl;
		return 0;
	} else {
		blobs.SortByArea();					// Sort in descending size
		// Now blob 0 is the bounding box, blob 1 is the biggest square (frame origin), 
		// blob 2 is the rectangle and the other blobs are the small squares

		CvPoint2D32f p1 = blobs.GetCenterReal(1);
		// blobs.DisplayCross(img,255,2,10,1);

		target.x=p1.x-bgimg->width/2;;
		target.y=p1.y-bgimg->height/2;

		//p1.x=p1.x-(center.x-p1.x);
		//p1.y=p1.y-(center.y-p1.y);

		//cout << p1.x << " " << p1.y << endl;

		CvPoint2D32f p2 = blobs.GetCenterReal(2);
		xaxis.x=p2.x-bgimg->width/2;
		xaxis.y=p2.y-bgimg->height/2;


		// blobs.DisplayCross(img,255,1,10,2);
		
		//cout << p2.x << " " << p2.y << endl;
		
		// The intersquare distance dx is 1/(cw-1) of the distance between p1 and p2.
		CvPoint2D32f dp = cvPoint2D32f((p2.x-p1.x)/(cw-1),(p2.y-p1.y)/(cw-1));
		//cout << dp.x << " " << dp.y << endl;
		
		// The extremity of the y axis is at a distance 4/5=0.8 times shorter in an orthogonal direction to dp
		CvPoint2D32f p3 = cvPoint2D32f(dp.y*(ch-1.0)+p1.x,-dp.x*(ch-1.0)+p1.y);
		//cout << "p3: " << p3.x << " " << p3.y << endl;
		
		// cvCircle( img, cvPoint(p3.x,p3.y), 10, 255, 1);
		
		blobs.SortByPosition(p3);
		CvPoint2D32f p4 = blobs.GetCenterReal(0); 
		
		yaxis.x = p4.x-bgimg->width/2; // save y-axis extreme
		yaxis.y = p4.y-bgimg->height/2;

		// Now, the big square is p1, the rectangle is p2 and the extremity of the y-axis is p4

		//cout << "p4: " << p4.x << " " << p4.y << endl;
		// cout << endl << "% Img_x Img_y World_x World_y " << endl;
		// cout << "m=[ " << endl;
		
		//for (int k=0;k<30;k++) objectPoints32f[k] = cvPoint3D32f(0.0, 0.0, 0.0);
		
		if(img)
		{
			cvNamedWindow("debug", CV_WINDOW_AUTOSIZE);
			blobs.DrawContours(img,cvScalarAll(255),cvScalarAll(0));
		}
		
		for (int x=0;x<cw;x++)
			for (int y=0;y<ch;y++)
			{
				// Points to square (x,y) on grid
				p3 = cvPoint2D32f(y*dp.y+x*dp.x+p1.x,-(y)*dp.x+x*dp.y+p1.y);
				blobs.SortByPosition(p3);
				p4 = blobs.GetCenterReal(0);
				// cvCircle( img, cvPoint(p4.x,p4.y), 10, 255, 1);   
				imagePoints32f[ch*x+y]  = p4;
				imagePoints32f[ch*x+y].x-=bgimg->width/2;
				imagePoints32f[ch*x+y].y-=bgimg->height/2;

				objectPoints32f[ch*x+y] = cvPoint3D32f(wx*x, wy*y, 0.0);  // in 40 mm interspot distance
 
			//	cout << imagePoints32f[5*x+y].x << " " << imagePoints32f[5*x+y].y << " ";
			//	cout << objectPoints32f[5*x+y].x << " " << objectPoints32f[5*x+y].y;
			//	cout << " " << objectPoints32f[5*x+y].z << ";" << endl;
			//	cvShowImage( "blobs", img );
			//		cvWaitKey(); 

				if(img)
				{
					cvCircle(img,cvPoint((int)(p4.x),(int)(p4.y)),10,CV_RGB(255,255,255),1);
					cvShowImage("debug",img);
					cvWaitKey(0);
				}
			};
		//cout << "];" << endl;
		return (cw*ch);
	}
    }
catch(...){
    throw "[Core] Calibration::FindPoints";
    }
}


int Calibration::FindRoundPoints(
	CvPoint2D32f* imagePoints32f, 
	CvPoint3D32f* objectPoints32f,
	VisBlobSet& blobs, 
	int h, int* nrofpoints, double vd)
{

	//IplImage* img = cvLoadImage("C:/Documents and Settings/Nikolaus/My Documents/work/Leurre/robotrack/calibrationtest/roundpattern.bmp",0);
try{
	if (blobs.GetTotalNumBlobs() != (patternsize+1))
	{
		cout << "Should have found only " << patternsize+1 << "blobs. Is ROI defined?" << endl;
		return 0;
	} else {
		blobs.SortByArea();					// Sort in descending size
		// Now blob 0 is the bounding box, blob 1 is the biggest disc, 
		// blob 2 is the second biggest disc and the other blobs are the small circles

		CvPoint2D32f p1 = blobs.GetCenterReal(1);

		target.x=p1.x-bgimg->width/2;;
		target.y=p1.y-bgimg->height/2;

		CvPoint2D32f p2 = blobs.GetCenterReal(2);
		xaxis.x=p2.x-bgimg->width/2;
		xaxis.y=p2.y-bgimg->height/2;

		// The intersquare distance dx is 1/(h) of the distance between p1 and p2.
		CvPoint2D32f dp = cvPoint2D32f((p2.x-p1.x)/(h),(p2.y-p1.y)/(h));
		//cout << dp.x << " " << dp.y << endl;
		
		// The extremity of the y axis is at a distance 4/5=0.8 times shorter in an orthogonal direction to dp
		CvPoint2D32f p3 = cvPoint2D32f(dp.y*(h)+p1.x,-dp.x*(h)+p1.y);
		//cout << "p3: " << p3.x << " " << p3.y << endl;
		
		// cvCircle( img, cvPoint(p3.x,p3.y), 10, 255, 1);
		
		blobs.SortByPosition(p3);
		CvPoint2D32f p4 = blobs.GetCenterReal(0);
		
		yaxis.x = p4.x-bgimg->width/2; // save y-axis extreme
		yaxis.y = p4.y-bgimg->height/2;

		// Now, the big square is p1, the rectangle is p2 and the extremity of the y-axis is p4

		double da=-atan2(p2.y-p1.y,p2.x-p1.x)+3.14159265/2;


/*		cvCircle( img, cvPoint(p1.x,p1.y), 5, 255, 1);   
		cvCircle( img, cvPoint(p2.x,p2.y), 5, 255, 1);   
		cvCircle( img, cvPoint(p4.x,p4.y), 5, 255, 1);   
*/
	/*	if(img)
		{
		cvNamedWindow("debug", CV_WINDOW_AUTOSIZE);
		//blobs.DrawContours(img,255,0);
			}
		// save cemter
	*/	
		imagePoints32f[0]  = p1;
		imagePoints32f[0].x-=bgimg->width/2;
		imagePoints32f[0].y-=bgimg->height/2;
		
		objectPoints32f[0] = cvPoint3D32f(0, 0, 0.0);
		
		int i=0;
		for (int x=0;x<h;x++){
			double alpha=2*3.14159265/nrofpoints[x];
			for (int y=0;y<nrofpoints[x];y++)
				{
				// Points to square (x,y) on grid
				//p3 = cvPoint2D32f((y*dp.y+x*dp.x+p1.x)*sin(alpha*y),(-(y)*dp.x+x*dp.y+p1.y)*cos(alpha*y));
				p3 = cvPoint2D32f(p1.x+sqrt(dp.x*dp.x+dp.y*dp.y)*(x+1)*sin(da+alpha*y),p1.y+sqrt(dp.x*dp.x+dp.y*dp.y)*(x+1)*cos(da+alpha*y));
				
				blobs.SortByPosition(p3);
				
				
				p4 = blobs.GetCenterReal(0);
	//			cvCircle( img, cvPoint(p4.x,p4.y), 2, 255, 1);   
				imagePoints32f[i+y+1]  = p4;
				imagePoints32f[i+y+1].x-=bgimg->width/2;
				imagePoints32f[i+y+1].y-=bgimg->height/2;
				
				objectPoints32f[i+y+1] = cvPoint3D32f((x+1)*vd*sin(alpha*y),(x+1)*vd*cos(alpha*y), 0.0);  // in 40 mm interspot distance
				
				//	cout << imagePoints32f[5*x+y].x << " " << imagePoints32f[5*x+y].y << " ";
			//	cout << objectPoints32f[i+y+1].x << " " << objectPoints32f[i+y+1].y << endl;
				//	cout << " " << objectPoints32f[5*x+y].z << ";" << endl;
				//	cvShowImage( "blobs", img );
				//		cvWaitKey(); 
				
	/*			if(img)
					{
					cvCircle(img,cvPoint(p3.x,p3.y),4,255,1);
					cvShowImage("debug",img);
					cvWaitKey(0);
					}*/
				}
			i+=nrofpoints[x];
			}
		//cout << "];" << endl;
		return (patternsize);
		}
    }    
catch(...){
    throw "[Core] Calibration::FindRoundPoints";
    }
}


void Calibration::TestRoundCalibration(char* testpattern,int h, int* nrofpoints, double vd){
    try{
    FILE* f;
	fopen_s(&f,"calibrationerror.dat","w");
	
	calibimg = cvLoadImage(testpattern,0);
	CalibrateTarget();
	cvNamedWindow("Validation",CV_WINDOW_AUTOSIZE);
	
	
	spots.EmptyDetectedBlobsList();			// clear the blob list,
	spots.FindBlobs(calibmask);				// and get the new ones,
	spots.SortByArea();						// biggest one first.
	spots.DrawContours(calibmask,cvScalarAll(255),cvScalarAll(255));	// outline the located blobs
	
	CvPoint2D32f* imagePoints32f =			// declare and allocate arrays of
		new CvPoint2D32f[patternsize];				// points for calculating the 
	CvPoint3D32f* objectPoints32f =			// transformation operator.
		new CvPoint3D32f[patternsize];				//
	FindRoundPoints(imagePoints32f,objectPoints32f,spots,h,nrofpoints,vd);
	
	

	
	double talpha=0; //-178.175863/180*3.14159265;
	
		
	double min=10000, max=0, mean=0; //, std=0;
	
	
	
//	CvPoint2D32f tdxy=cvPoint2D32f(0.0,0.0);
	talpha=alpha;
	int i=0;
	for (int x=0;x<h;x++){
		//double alpha=2*3.14159265/nrofpoints[x];
		for (int y=0;y<nrofpoints[x];y++)
			{
			CvPoint2D32f wp = ImageToWorld(imagePoints32f[i+y+1]);
			CvPoint2D32f op;
			op.x=(float)(objectPoints32f[i+y+1].x*cos(talpha)-sin(talpha)*objectPoints32f[i+y+1].y-dxy.x);
			op.y=(float)(objectPoints32f[i+y+1].x*sin(talpha)+cos(talpha)*objectPoints32f[i+y+1].y-dxy.y);
		
			
			
			double e = sqrt(pow(op.x-wp.x,2)+pow(op.y-wp.y,2));
			
			if(e<min) min=e;
			if(e>max) max=e;
			
			mean+=e;
			
			
			printf("%f %f %f %f %f\n",wp.x,wp.y,op.x,op.y,e);
			fprintf(f,"%f %f %f %f %f\n",wp.x,wp.y,op.x,op.y,e);
			}
		i+=nrofpoints[x];
		}
		mean/=(patternsize);
		
		printf("Min. Error: %f\n",min);
		printf("Max. Error: %f\n",max);
		printf("Mean Error: %f\n",mean);
		fclose(f);
		        }
    catch(...){
        throw "[Core] Calibration::TestRoundCalibration";
        }
        }
 
void Calibration::TestCalibration(char* testpattern,int cw, int ch,double wx, double wy){
    try{
	FILE* f;
	fopen_s(&f,"calibrationerror.dat","w");
	calibimg = cvLoadImage(testpattern,0);
	CalibrateTarget();
	cvNamedWindow("Validation",CV_WINDOW_AUTOSIZE);
	
	
	spots.EmptyDetectedBlobsList();			// clear the blob list,
	spots.FindBlobs(calibmask);				// and get the new ones,
	spots.SortByArea();						// biggest one first.
	spots.DrawContours(calibmask,cvScalarAll(255),cvScalarAll(255));	// outline the located blobs
	
	CvPoint2D32f* imagePoints32f =			// declare and allocate arrays of
		new CvPoint2D32f[cw*ch];				// points for calculating the 
	CvPoint3D32f* objectPoints32f =			// transformation operator.
		new CvPoint3D32f[cw*ch];				//
	FindPoints(imagePoints32f,objectPoints32f,spots,cw,ch,wx,wy);
	
	

	CvPoint2D32f tdxy=cvPoint2D32f(0.35,0.25);  // data for test pattern
	tdxy=cvPoint2D32f(-dxy.x,-dxy.y); // disable this line when working with small calibration pattern
	
	double talpha=-178.175863/180*3.14159265;
	
		printf("Center offset: (%f/%f)\n",tdxy.x,tdxy.y);



	double min=10000, max=0, mean=0; //, std=0;
	
	
	
//	CvPoint2D32f tdxy=cvPoint2D32f(0.0,0.0);
	talpha=alpha;

	for(int j=0;j<ch;j++)
		for(int i=0;i<cw;i++)
			{
			CvPoint2D32f wp = ImageToWorld(imagePoints32f[j*cw+i]);
			CvPoint2D32f op;
			op.x=(float)(objectPoints32f[j*cw+i].x*cos(talpha)-sin(talpha)*objectPoints32f[j*cw+i].y);
			op.y=(float)(objectPoints32f[j*cw+i].x*sin(talpha)+cos(talpha)*objectPoints32f[j*cw+i].y);
			
			op.x=op.x+tdxy.x;//+dxy.x;
			op.y=op.y+tdxy.y;//+dxy.y;
			


			double e = sqrt(pow(op.x-wp.x,2)+pow(op.y-wp.y,2));
			
			if(e<min) min=e;
			if(e>max) max=e;
			
			mean+=e;
			
			
			printf("%f %f %f %f %f\n",wp.x,wp.y,op.x,op.y,e);
			fprintf(f,"%f %f %f %f %f\n",wp.x,wp.y,op.x,op.y,e);
			}
		fclose(f);
		
		mean/=(ch*cw);

		printf("Min. Error: %f\n",min);
		printf("Max. Error: %f\n",max);
		printf("Mean Error: %f\n",mean);

		
	/*	FILE* f1=fopen("tpimagematrix.dat","w");
		FILE* f2=fopen("tpobjectmatrix.dat","w");
		
		for(j=0; j<ch*cw; j++){
			fprintf(f2,"%f %f\n",objectPoints32f[j].x-0.35,objectPoints32f[j].y-0.248);
			fprintf(f1,"%f %f\n",imagePoints32f[j].x,imagePoints32f[j].y);
			}
		
		fclose(f1);
		fclose(f2);*/
		        }
    catch(...){
        throw "[Core] Calibration::TestCalibration";
        }
	}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

			
/********************************************* end of File ********************/


void Calibration::SaveError(int n, CvPoint2D32f* u, CvPoint3D32f* x, char *fname)
{
try{
	FILE* f;
	fopen_s(&f,fname,"w");
	if(f){
	for(int i=0; i<n; i++){

	CvPoint2D32f p = ImageToWorld(u[i],0);

	///////   u  v  x  y x' y' e
	fprintf(f,"%f %f %f %f %f %f %f\n",u[i].x,u[i].y,
									 x[i].x,x[i].y,
									 p.x,p.y,
									 sqrt(pow(x[i].x-p.x,2)+pow(x[i].y-p.y,2)));
									
	}
	fclose(f);
	}
            }
    catch(...){
        throw "[Core] Calibration::SaveError";
        }
}
