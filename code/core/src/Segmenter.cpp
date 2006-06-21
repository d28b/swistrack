// Segmenter.cpp: implementation of the Segmenter class.
//
//////////////////////////////////////////////////////////////////////

#include "Segmenter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** Constructor
*
* Initializes segmentation parameters with defaults, opens video and background
* file, allocates necessary IplImages, and finally flips the background image in order
* to fit the format of the video. After successful construction, segmenter is in status
* READY_TO_START.
*
*/
Segmenter::Segmenter(xmlpp::Element* cfgRoot, TrackingImage* trackingimg)
    {
    this->cfgRoot=cfgRoot;
    this->trackingimg=trackingimg;
    
    Capture=0;
    binary=0;
    background=0;
    current=0;
    m_pBitmap=0;

    nFrame = 0; 
    isinputincolor=0;
    
    if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/INPUT","mode"))
        throw "[Segmenter::Segmenter] Input mode undefined (/CFG/COMPONENTS/INPUT)";
    if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/PARTICLEFILTER","mode"))
        throw "[Segmenter::Segmenter] Particlefilter mode undefined (/CFG/COMPONENTS/INPUT)";
    if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/SEGMENTER","mode"))
        throw "[Segmenter::Segmenter] Segmenter mode undefined (/CFG/COMPONENTS/INPUT)";
    
        /**
        * \todo We ignore particlefilter, imagefilter, and input modes, all this should be 
        * transported in their own classes. "mode" will instead solely been used for the 
        * segmenter mode
    */
    
    input_mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/INPUT","mode");
    particlefilter_mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/PARTICLEFILTER","mode");
    
    switch(input_mode){
        case 0 : /////////////// video file //////////////////////////
            if(!IsDefined(cfgRoot,"/CFG/INPUT[@mode='0']"))
                throw "[Segmenter::Segmenter] no parameters for input mode 0 found";
            
            if(!IsDefined(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"))
                throw "[Segmenter::Segmenter] input video not specified (/CFG/INPUT[@mode='0']/AVIFILE)";

            Capture = cvCaptureFromFile(GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"));

            if (!Capture){
                //printf("Can not open %s\n",avi_fname);
                status = CAN_NOT_OPEN_AVI_FILE;
                FILE* f;
                f=fopen(GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"),"r");
                if(f){
                    fclose(f);
                    throw "Segmenter: Can not open AVI file (codec problem)";
                    }
                else{
                    f=fopen("swistrack.log","w");
                    fprintf(f,"%s not found",GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"));
                    fclose(f);
                    throw "Segmenter: Can not open AVI file (file not found)";
                    }
                return;
                }
            input =  cvQueryFrame( Capture );	
            if(input){
                if(strcmp(input->colorModel,"GRAY")==0)
                    isinputincolor=0;
                else
                    isinputincolor=1;
                
                
                current = cvCreateImage(cvSize(input->width,input->height),input->depth,1);
                cvCvtPixToPlane(input,NULL,NULL,current,NULL);
                
                binary = cvCreateImage(cvSize(current->width,current->height),current->depth,1);
                binary->origin = current->origin;
                }
            cvSetCaptureProperty( Capture , CV_CAP_PROP_POS_FRAMES,0); // Reset to first frame
            trackingimg->Init(input,binary);
            
            
            break;
        case 1 : /////////////// 1394 camera /////////////////////////
#ifdef _1394
            if (theCamera.CheckLink() != CAM_SUCCESS){
                status = CAN_NOT_OPEN_AVI_FILE;
                throw "Segmenter: Can not access 1394 Camera";
                return;
                }
            
            theCamera.InitCamera();
            
            /**
            * \todo Fetch parameters from CFG instead setting default values 
            */
            theCamera.SetVideoFrameRate(4); // 15fps
            theCamera.SetVideoFormat(0);    // 640x480, yuv411
            theCamera.SetVideoMode(2);
            
            m_pBitmap = new unsigned char [theCamera.m_width * theCamera.m_height * 3];
            
            theCamera.StartImageAcquisition();
            input=cvCreateImage(cvSize(theCamera.m_width,theCamera.m_height),8,3);
            QueryFrame1394(input);
            if(input){
                if(strcmp(input->colorModel,"GRAY")==0)
                    isinputincolor=0;
                else
                    isinputincolor=1;
                
                
                current = cvCreateImage(cvSize(input->width,input->height),input->depth,1);
                cvCvtPixToPlane(input,NULL,NULL,current,NULL);
                
                binary = cvCreateImage(cvSize(current->width,current->height),current->depth,1);
                binary->origin = current->origin;
                trackingimg->Init(input,binary);
                
                }
            
#endif
            break;
        default:
            throw "[Segmenter::Segmenter] segmenter mode not implemented";
        }
    
    /** \todo query from cfg */
    bin_threshold = 15; // segmenter
    min_area = 10; // particle filter
    alpha = 0.05; // segmenter estimate
    
    
    
    status = READY_TO_START;
    
    
    mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/SEGMENTER","mode");
    
    switch(mode){
        case 0 :{ ///////// Background Subtraction ///////////////
            if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='0']"))
                throw "[Segmenter::Segmenter] no parameters for segmenter mode 0 (background subtraction) found";

            if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='0']/BGIMAGE"))
                throw "[Segmenter::Segmenter] background image not specified (/CFG/SEGMENTER[@mode='0']/BGIMAGE)";

            
            IplImage* bg = cvLoadImage(GetValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='0']/BGIMAGE"), -1); 
            background = cvCreateImage(cvSize(input->width,input->height),input->depth,1);
            
            if(bg->nChannels>1)
                cvCvtPixToPlane(bg,NULL,NULL,background,NULL);
            else
                cvCopy(bg,background);
            
            /** \todo Color space conversion should be chosen by user */
            // Flip image because AVI provides bottom left images and BMP top left.
            cvFlip(background,background,0);
            cvWaitKey(0);
            if (!background){
                status = CAN_NOT_OPEN_BACKGROUND_FILE;
                throw "Can not open background file";
                }
            }
            break;
        case 1 : /////////// Estimate Subtraction //////////////////
             if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='1']"))
                throw "[Segmenter::Segmenter] no parameters for segmenter mode 1 (estimate subtraction) found";

            background = cvCreateImage(cvSize(input->width,input->height),input->depth,1);
            break;	
        default :
            throw "[Segmenter::Segmenter] Segmenter mode not implemented";
        }
    }
	
	/** Destructor
	*/
	Segmenter::~Segmenter()
		{
		if (Capture) cvReleaseCapture( &Capture );
		if (binary) cvReleaseImage( &binary );
		if (background) cvReleaseImage( &background );
		if (current) cvReleaseImage( &current);
		if (m_pBitmap) delete(m_pBitmap);
		}

		/** Returns Status
		*
		* \return
		*  - FINISHED        = end of video
		*  - RUNNING         = successful segmentation
		*  - NO_AVI_SEQUENCE = video file not ready
	*/
	int Segmenter::GetStatus()
		{
		return(status);
		}
	
		/** \brief Gives AVI progress
		* 
		* @param kind : format of the progress
		*               - 1 = progress in milliseconds
		*               - 2 = progress in frames
		*               - 3 = progress in percent
		* \result Returns progress in the format specified by 'kind'
		* \todo Get the framerate from the camera in order to calculate the right time
	*/
	double Segmenter::GetProgress(int kind){
		switch(kind){
			case 3:
				if(input_mode)
					return(1);
				else
					return(cvGetCaptureProperty( Capture, CV_CAP_PROP_POS_AVI_RATIO));
			case 1:
				if(input_mode)
					return(nFrame*1000/15);
				else
					return(cvGetCaptureProperty(Capture, CV_CAP_PROP_POS_MSEC));
			case 2:
				if(input_mode)
					return(nFrame);
				else
					return(cvGetCaptureProperty(Capture, CV_CAP_PROP_POS_FRAMES));
            default:
                throw "INTERNAL ERROR: Segmenter::GetProgress invalid type";
			}
		return 0;
		}
	
		/**  \brief Does one segmentation step
		* stores resulting contours in "contours", that are retrieved
		* by GetContours()
		*
		* \return
		*  - FINISHED        = end of video
		*  - RUNNING         = successful segmentation
		*  - NO_AVI_SEQUENCE = video file not ready
	*/
    int Segmenter::Step()
        {
        
        
        switch(input_mode){
            case 0 :{ ////////////// Video from file //////////////////
                if(Capture){
                    input =  cvQueryFrame( Capture );
                    if (input)  // Get a pointer to the current frame (if query was successful)
                        {
                        nFrame++;                         // Increment the frame counter. 
                        
                        cvCvtPixToPlane(input,NULL,NULL,current,NULL);
                        
                        BWSegmentation();                 
                        
                        trackingimg->Refresh(input,binary);
                        
                        int numContours =                 // Fetch the contour list
                            GetContour(binary,&contours);    //
                        
                        status = RUNNING;
                        } // end if input
                    else                                // no more input, so stop cleanly.
                        {                                   //
                        status = FINISHED;                //
                        } 
                    }
                else
                    {
                    throw "[Segmenter::Step] Video feed broken (Video from file)";
                    status = NO_AVI_SEQUENCE;
                    }
                }
                break;
            case 1 :{ ///////////// IEEE 1394 Camera ///////////////
                QueryFrame1394(input);
                if (input)  // Get a pointer to the current frame (if query was successful)
                    {
                    nFrame++;                         // Increment the frame counter. 
                    
                    cvCvtPixToPlane(input,NULL,NULL,current,NULL);
                    
                    BWSegmentation();                 
                    
                    trackingimg->Refresh(input,binary);
                    
                    int numContours =                 // Fetch the contour list
                        GetContour(binary,&contours);    //
                    
                    status = RUNNING;
                    } // end if input
                else
                    {
                    throw "[Segmenter::Step] Video feed broken (IEEE 1394 camera)";
                    status = NO_AVI_SEQUENCE;
                    }
                }
                break;
            default : throw "[Segmenter::Step] Illegal input mode";
            }
    return status;
    }
	
	
		/**  \brief Segments image by subtracting against background 
        *
        *
		* 
    */
    void Segmenter::BWSegmentation()
        {
        switch(mode){
            case 0 : { /////////// BACKGROUND SUBTRACTION ////////////////
                // no special treatment needed in this case
                try{
                    cvAbsDiff(current, background, binary);
                    cvThreshold(binary,binary, bin_threshold, 255,  CV_THRESH_BINARY);
                    }
                catch(...){
                    throw "Segmenter: Segmentation with fixed background failed";
                    }
                }
                break;
            case 1 : { /////////// ESTIMATE SUBTRACTION //////////////////
                try{
                    cvConvertScale(current,current,alpha);
                    cvConvertScale(background,background,1.0-alpha);
                    cvAdd(current,background,background);
                    cvCvtPixToPlane(input,NULL,NULL,current,NULL);
                    cvAbsDiff(current, background, binary);
                    cvThreshold(binary,binary, bin_threshold, 255,  CV_THRESH_BINARY);
                    }
                catch(...){
                    throw "Segmenter: Segmentation with background estimate failed";
                    }
                }
                break;
            default : {
                throw "Invalid segmentation mode";
                }
                break;
            }
        
        // Perform a morphological opening to reduce noise. 
        /** \todo This should go in a separate class IMAGEFILTER */
        cvErode(binary, binary, NULL, 2);
        cvDilate(binary, binary, NULL, 2);
  
        }
	
		/**  \brief Find Contours in the image (called by GetContour)
		*
		* \param src: Segmented image
		* \param contour_obj: Vector to store resulting contours
		* \param min_area: Minimum area for the contours not to be rejected
	*/
    int Segmenter::FindContours(IplImage* src, vector<resultContour>* contour_obj, double min_area)
        {
        CvMemStorage* storage = cvCreateMemStorage(0);		// memory space that will be used for contour storage			
        CvSeq* contour = 0;
        //CvSeq* contour_max = 0;
        std::vector<CvSeq*> tmp;
        std::vector<CvSeq*>::iterator j;
        int i;
        int n_contours = 0; /* Number of contours found */
        
        //    CvPoint2D32f center = cvPoint2D32f(src->width/2,src->height/2);
        
        // Make a copy of source, because contents are destroyed by cvStartFindContours
        IplImage* src_tmp = cvCloneImage(src);
        //cvErode(src_tmp,src_tmp,NULL,2);
        CvContourScanner blobs = cvStartFindContours(src_tmp,storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
        
        // Traverse the sequence of contours and keep those that 
        // are within geometric bounds
        
        
        int counter=0;
        switch(particlefilter_mode){
            case 0 : { //////////////////// MIN-MAX FILTER ///////////////////////////////
                while((contour=cvFindNextContour(blobs))!=NULL)
                    {		
                    //printf("Contour: %d Size(tmp) %d\n",counter++,tmp.size());
                    // Traverse contours until a bigger contour is found
                    
                    for(j=tmp.begin(); 
                    (j != tmp.end()) && (fabs(cvContourArea(contour)) < fabs(cvContourArea(*j)));
                    j++);
                    // check size of the contour
                    if((fabs(cvContourArea(contour)) > min_area))
                        {
                        tmp.insert(j,contour);
                        n_contours++;
                        }
                    /* Releases contour scanner and returns pointer to the first outer contour */
                    contour=cvEndFindContours(&blobs);
                    
                    
                    
                    CvPoint2D32f pt_centre;
                    CvRect rect;
                    
                    // This is used to correct the position in case of ROI
                    if(src->roi != NULL)
                        rect = cvGetImageROI(src);
                    else
                        {
                        rect.x = 0;
                        rect.y = 0;
                        }
                    
                    
                    CvMoments moments;
                    
                    // Compute and store the moments and other data of each contour
                    for(i=0;i<n_contours;i++)
                        {
                        
                        resultContour rc;
                        
                        cvMoments(tmp[i],&moments,0);
                        pt_centre.x=rect.x + (moments.m10/moments.m00+0.5);  // moments using Green theorema
                        pt_centre.y=rect.y + (moments.m01/moments.m00+0.5);  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem
                        rc.center = pt_centre;
                        rc.area = moments.m00;
                        rc.compactness =  cvContourCompactness( tmp[i]);
                        rc.boundingrect = cvContourBoundingRect(tmp[i], 0);
                        
                        contour_obj->push_back(rc);
                        
                        /*for(j=0; // sort by distance from center
                        (j < contour_obj->size()) && (GetSqrDist(center,rc.center) < GetSqrDist(center,contour_obj->at(j).center));
                        j++);		
                        contour_obj->insert(&(contour_obj->at(j)),rc);
                        */
                        }
                    
                    
                    }
                }
                break;
            case 1 : { //////////////////// MARKER BASED ///////////////////////////////
                /** \todo Implement something that copies the contours according to the markers */
                /** \todo Of course, the whole thing has to go into PARTICLEFILTER class */
                }
                break;
            default : throw "[Segmenter::FindContounrs] Unknown particlefilter mode";
            }
        
        
        
        
        cvReleaseMemStorage(&storage);
        cvReleaseImage(&src_tmp);
        return n_contours;
        
        }
        
        /**  \brief Select contours in a picture
        *
        * This function computes all exterior contours in the picture and find the center of 
        * gravity of the ones whose areas are within the defined bounds and whose compactness is
        * higher than target one. It writes the retained contours and their features in a 
        * user-defined array 'contour_obj'.
        *
        * \param src: pointer on the binary source picture
        * \param contour_obj: pointer to a vector of resultContour where results are returned
        * \result Returns the number of detected contours.
        */
        int Segmenter::GetContour(IplImage* src, vector<resultContour>* contour_obj)
            {
            int n_contours;
            //printf("GetContour\n");
            // Erase all contours in resultContour vector
            contour_obj->clear();	
            n_contours=FindContours(src,contour_obj,min_area);
            GetContourColor(input,contour_obj);
            return n_contours;
            }
        
            /** \brief Determines contours average color 
            *
            * This function computes the average color value over all pixels in a contour. 
            * This might be a useful parameter in determining the fitness of the data
            * association algorithm.
            *
            * \param input: The color input image
            * \param contour_obj: pointer to a vector of resultContour 
            *
            * \todo Implementation
        */
        void Segmenter::GetContourColor(IplImage* input, vector<resultContour>* contour_obj)
            {
            }
        
            /**  \brief Initializes segmenter, by discarding the first few frames.
            *
            * \param overhead: Number of inital frames to discard
            * \result Returns the current status
            * \todo The segmenter has to be divided into input and segmenter part
            *     
        */
        int Segmenter::init(int overhead)
            {
            switch(input_mode){
                case 0 :{ ///////////// Video from file //////////
                    if(!Capture)
                        throw "[Segmenter::init] No video feed (video from file)";
                    status = READY_TO_START;						// change status
                    //  (status is read from the gui)
                    
                    //int numContours=0;
                    for(int i=1; i<=overhead; i++){
                        if(mode == 1){ // ESTIMATE
                            cvCvtPixToPlane(input,NULL,NULL,current,NULL);
                            cvConvertScale(current,current,0.3);
                            cvConvertScale(background,background,0.7);
                            cvAdd(current,background,background);
                            cvCvtPixToPlane(input,NULL,NULL,current,NULL);					
                            }
                        
                        if(input_mode)
                            QueryFrame1394(input);
                        else
                            input =  cvQueryFrame( Capture );	 // Discard first 'overhead' frames
                        
                        nFrame++;	
                        }
                    trackingimg->RefreshCoverage(background);
                    }
                    break;
                case 1 : {/////////// IEEE1394 camera //////
                    status = READY_TO_START;						// change status
                    //  (status is read from the gui)
                    
                    //int numContours=0;
                    for(int i=1; i<=overhead; i++){
                        if(mode == 1){ // estimate
                            cvCvtPixToPlane(input,NULL,NULL,current,NULL);
                            cvConvertScale(current,current,0.3);
                            cvConvertScale(background,background,0.7);
                            cvAdd(current,background,background);
                            cvCvtPixToPlane(input,NULL,NULL,current,NULL);					
                            }
                        
                        if(input_mode)
                            QueryFrame1394(input);
                        else
                            input =  cvQueryFrame( Capture );	 // Discard first 'overhead' frames
                        
                        nFrame++;	
                        }
                    trackingimg->RefreshCoverage(background);
                    }
                    break;
                default : throw "[Segmenter::Init] Illegal input mode";
                }
            
            
            return(OK);
            
            }
		
		vector<resultContour>* Segmenter::GetContours()
			{
			return(&contours);
			}
		
		
		
		double Segmenter::GetSqrDist(CvPoint2D32f p1, CvPoint2D32f p2)
			{
			return(
				(p1.x-p2.x)*
				(p1.x-p2.x)
				+
				(p1.y-p2.y)*
				(p1.y-p2.y)
				);
			}
		
void Segmenter::SetParameters()
			{
			// todo: update parameters from conf
			}
		
		double Segmenter::GetFPS()
			{
			if(input_mode){
				return(15);
				/**
				* \todo Extract the real frame rate from the camera 
				*/
				}
			else
				if(Capture)
					return(cvGetCaptureProperty( Capture, CV_CAP_PROP_FPS ));
				else
					return(0);
			}
		
		void Segmenter::QueryFrame1394(IplImage* input)
			{
#ifdef _1394
			theCamera.AcquireImage();
			theCamera.getRGB(m_pBitmap);
			
			input->imageData=(char*) m_pBitmap;
			input->width=640;
			input->height=480;
			input->nChannels=3;
#endif
			}

void Segmenter::RefreshCoverage()
{
	trackingimg->RefreshCoverage(background);
}
