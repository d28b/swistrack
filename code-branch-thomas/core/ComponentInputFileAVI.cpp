#include "ComponentInputFileAVI.h"
#define THISCLASS ComponentInputFileAVI

THISCLASS::ComponentInputFileAVI(xmlpp::Element* cfgRoot):
		Component(cfgRoot), mCapture(0) {

	mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/INPUT/FileAVI","mode");

	CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='0']");
	CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE");

	Capture = cvCaptureFromFile(GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"));
	if (!Capture){
		FILE* f;
#ifdef VS2003
		f=fopen(GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"),"r");
#else
		fopen_s(&f,GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"),"r");
#endif
		if(f){
			fclose(f);
			throw "Input: Can not open AVI file (codec problem, VFW codec required, not DirectShow)";
		}
		else{
#ifdef VS2003
			f = fopen("swistrack.log","w");
#else
			fopen_s(&f,"swistrack.log","w");
#endif
			fprintf(f,"%s not found",GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"));
			fclose(f);
			throw "Input: Can not open AVI file (file not found)";
		}
		return;
	}
	input =  cvQueryFrame( Capture );	
	if(input){
		if(strcmp(input->colorModel,"GRAY")==0)
			isinputincolor=0;
		else
			isinputincolor=1;          
	}

	cvSetCaptureProperty( Capture , CV_CAP_PROP_POS_FRAMES,0); // Reset to first frame
}

void THISCLASS::SetParameters() {
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
double Input::GetProgress(int kind) {
	switch(kind){
	case 3:
		if(mode)
			return(1);
		else
			return(cvGetCaptureProperty( Capture, CV_CAP_PROP_POS_AVI_RATIO));
	case 1:
		if(mode)
			return(nFrame*1000/15);
		else
			return(cvGetCaptureProperty(Capture, CV_CAP_PROP_POS_MSEC));
	case 2:
		if(mode==1)
			return(nFrame);
		else
			return(cvGetCaptureProperty(Capture, CV_CAP_PROP_POS_FRAMES));
	default:
		throw "[Input::GetProgress] invalid type";
	}
	return 0;
}

IplImage* Input::GetFrame() {
	if (Capture) {
		input =  cvQueryFrame( Capture );
		if (input) {  // Get a pointer to the current frame (if query was successful)
			nFrame++;                         // Increment the frame counter.
			cvFlip(input,0);              // AVI files are flipped
			return(input);
		} // end if input
	} else {
		throw "[Input::GetFrame] Video feed broken (Video from file)";
	}
	return NULL;
}

CvSize Input::GetInputDim() {
	return cvSize(input->width, input->height);
}

int Input::GetInputDepth(){
	if(input) 
		return input->depth;
	else throw "[Input::GetInputDepth] not implemented for this input mode";
	/** \todo Implement for different modes */
		
}
int Input::GetInputNbChannels(){
	if(input) return input->nChannels;
	else throw "[Input::GetInputNbChannels] not implemented for this input mode";
	/** \todo Implement for different modes */
}

int Input::GetInputOrigin(){
	if(input) return input->origin;
	else throw "[Input::GetInputOrigin] not implemented for this input mode";
	/** \todo Implement for different modes */
}

IplImage* Input::GetInputIpl(){
	if(input) return input;
	else throw "[Input::GetInputIpl] not implemented for this input mode";
	/** \todo Implement for different modes */
}

Input::~Input()
{
	if (Capture) cvReleaseCapture( &Capture );	
}

double Input::GetFPS(){
	if(mode){
		switch(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/FRAMERATE")){
			case 0 : return(2); break;
			case 1 : return(5); break;
			case 2 : return(7); break;
			case 3 : return(15); break;
			case 4 : return(30); break;
			case 5 : return(60); break;
			default : throw "[Input::GetFPS] Unknown Framerate";
		}
	}
	else
		if(Capture)
			return(cvGetCaptureProperty( Capture, CV_CAP_PROP_FPS ));
		else
			return(0);
}
