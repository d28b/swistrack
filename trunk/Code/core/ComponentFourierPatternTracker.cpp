#include "ComponentFourierPatternTracker.h"
#define THISCLASS ComponentFourierPatternTracker
#include <highgui.h>
#include "DisplayEditor.h"

using namespace cv;
using namespace std;

	
THISCLASS::ComponentFourierPatternTracker(SwisTrackCore *stc):
		Component(stc, wxT("FourierPatternTracker")),
		mDisplayOutput(wxT("Output"), wxT("FilteredImage")),
		mPatternPreview(wxT("Output"), wxT("PatternPreview")), 
		mMatchPreview(wxT("Output"), wxT("MatchPreview")),
		mParticles() {
 
	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);
	AddDisplay(&mPatternPreview);
	AddDisplay(&mMatchPreview);
	

	mPatternPreviewImage=NULL;
	mMatchPreviewImage=NULL;
	mDisplayOutputImage=NULL;	
	particle_size=6.0;
	particle_lifetime=10;
	rejection_threshold=0.1;
	sensing_prediction_balance=0.3;
	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentFourierPatternTracker() {
}

void THISCLASS::OnStart() {
	// Load other parameters
	OnReloadConfiguration();

	// Load the image
	wxString filename_string = GetConfigurationString(wxT("Pattern1"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	if (! filename.IsOk()) {
		AddError(wxT("Invalid image file path/filename."));
		return;
	}
	mPattern1 = cvLoadImage(filename.GetFullPath().mb_str(wxConvFile), CV_LOAD_IMAGE_GRAYSCALE);

// Load  the other image
    wxString filename_string2 = GetConfigurationString(wxT("Pattern2"), wxT(""));
	wxFileName filename2 = mCore->GetProjectFileName(filename_string2);
	if (! filename2.IsOk()) {
		AddError(wxT("Invalid image file path/filename."));
		return;
	}
	mPattern2 = cvLoadImage(filename2.GetFullPath().mb_str(wxConvFile), CV_LOAD_IMAGE_GRAYSCALE);

	matchPattern=0;
	return;
}

void THISCLASS::OnReloadConfiguration() {
	rejection_threshold=GetConfigurationDouble(wxT("RejectionThreshold"), true);
	sensing_prediction_balance=GetConfigurationDouble(wxT("SensePredBalance"), true);
	particle_size      =GetConfigurationDouble(wxT("ParticleSize"), true);
	
	mConfigChanged=true;
}

void THISCLASS::OnStep() {
	
#ifdef GPU_ENABLED
	cv::gpu::GpuMat input=mCore->mDataStructureImageFFT.mMat_Gpu;
#else
	Mat input= mCore->mDataStructureImageFFT.mMat;
#endif
	
	if ( input.empty())
	{
	  AddError(wxT("Cannot access input image."));
	  return;
	}
	
	if (input.channels() != 2)
	{
	  AddError(wxT("Input must be a complex image (fourier domain) with 2 channels"));
	}
	

	// get configuration options
	mGenerateOutput = GetConfigurationBool(wxT("GenerateOutput"), true);

    // check if anything has changed (input size, filter parameters) and generate filter matrix
	if ((input.size()!=mFilterMatrix.size()) || mConfigChanged) {
		#ifdef GPU_ENABLED
			Mat input_cpu(input);
			prepareFilter(input_cpu);
		#else
			prepareFilter(input);
			
		#endif
		mConfigChanged=false;

		Mat mPatternsDisplay;
		normalize(mPatterns, mPatternsDisplay, 0, 255, CV_MINMAX);
		
		mPatternsDisplay.convertTo(mPatternsDisplay, CV_8UC1);
		
		if ((mPatternPreviewImage == NULL) || (mPatternPreviewImage->width!=mPatternsDisplay.cols) || (mPatternPreviewImage->height!=mPatternsDisplay.rows)){
			mPatternPreviewImage= cvCreateImage(cvSize(mPatternsDisplay.cols, 
								  mPatternsDisplay.rows), 8, 1);
		}
		*mPatternPreviewImage=mPatternsDisplay;
	}

#ifdef GPU_ENABLED
	

if (matchPattern==0) {
		cv::gpu::mulAndScaleSpectrums(input, mPatternFourierFiltered1_GPU, output_gpu, DFT_COMPLEX_OUTPUT,2.0/(input.cols*input.rows), true);
	} else {
		cv::gpu::mulAndScaleSpectrums(input, mPatternFourierFiltered2_GPU, output_gpu, DFT_COMPLEX_OUTPUT,2.0/(input.cols*input.rows), true);
	}

	cv::gpu::dft(output_gpu, output_gpu, input.size(), DFT_INVERSE); 
	
	cv::gpu::split(output_gpu, planes_gpu);    
	magnitude(planes_gpu[0], planes_gpu[1], matchResult_gpu);
    
	Mat download(matchResult_gpu);
	matchResult=download;
//	output=(2.0/(input.cols*input.rows))*download;
#else
	if (matchPattern==0) {
		cv::mulSpectrums(input, mPatternFourierFiltered1, output, DFT_COMPLEX_OUTPUT, true);
	} else {
		cv::mulSpectrums(input, mPatternFourierFiltered2, output, DFT_COMPLEX_OUTPUT, true);
	}
	dft(output, output, DFT_INVERSE+DFT_SCALE );
	split(output, planes);    
	magnitude(planes[0], planes[1], matchResult);
#endif


	matchPattern=(matchPattern+1)%2;
	
	
	


	//cv::normalize(matchResult, matchResult, 0, 1.0, CV_MINMAX);
	//cv::threshold(matchResult, temp, 0.5, 1.0, CV_THRESH_BINARY);
	
	temp=cv::Mat::zeros(matchResult.size(), CV_32F);
	temp=0.1;
	for (DataStructureParticles::tParticleVector::iterator pIt = 
		mParticles.begin(); pIt != mParticles.end(); pIt++) {
			const Particle & p = *pIt;
			const float gain=p.mArea*8;
			temp.at<float>(p.mCenter.y  , p.mCenter.x  )=1.0*gain;
			temp.at<float>(p.mCenter.y-1, p.mCenter.x-1)=0.7*gain;
			temp.at<float>(p.mCenter.y-1, p.mCenter.x  )=0.8*gain;
			temp.at<float>(p.mCenter.y-1, p.mCenter.x+1)=0.7*gain;
			temp.at<float>(p.mCenter.y  , p.mCenter.x-1)=0.8*gain;
			temp.at<float>(p.mCenter.y  , p.mCenter.x+1)=0.8*gain;
			temp.at<float>(p.mCenter.y+1, p.mCenter.x-1)=0.7*gain;
			temp.at<float>(p.mCenter.y+1, p.mCenter.x  )=0.8*gain;
			temp.at<float>(p.mCenter.y+1, p.mCenter.x+1)=0.7*gain;
			
			
	}
	cv::threshold(temp, temp, 0.99, 0.99, CV_THRESH_TRUNC);
	cv::GaussianBlur(temp, tracker, Size(15,15), 0,0);
	tracker=1.5*tracker;
	temp=(1.0-sensing_prediction_balance) * matchResult+sensing_prediction_balance;
	cv::multiply(temp, tracker, matchResult);

	//cv::threshold(tracker, tracker, 0.99, 0.99, CV_THRESH_TRUNC);
    //matchResult=tracker;

	// particle generation
	
	
	for (int i=0; i<50; i++) {
		Particle newParticle;
		newParticle.mCenter.x=rand() % (int)(matchResult.cols-2*particle_size) + particle_size;
		newParticle.mCenter.y=rand() % (int)(matchResult.rows-2*particle_size) + particle_size;
		newParticle.mFrameNumber=0;
		mParticles.push_back(newParticle);
		
	}




	// particle update
	for (DataStructureParticles::tParticleVector::iterator pIt = mParticles.begin(); pIt != mParticles.end(); pIt++) 
	{	
		Particle & p = *pIt;
		if (p.mCenter.x<particle_size) p.mCenter.x=particle_size;
		if (p.mCenter.x>matchResult.cols-particle_size) p.mCenter.x=matchResult.cols-particle_size;
		if (p.mCenter.y<particle_size) p.mCenter.y=particle_size;
		if (p.mCenter.y>matchResult.cols-particle_size) p.mCenter.y=matchResult.rows-particle_size;
		// find maximum within particle area
		cv::Mat p_area(matchResult, cv::Rect((int)(p.mCenter.x-particle_size/2.0), 
											 (int)(p.mCenter.y-particle_size/2.0), 
											 (int)(particle_size), 
											 (int)(particle_size)));
		Point LocMax;
		cv::minMaxLoc(p_area,NULL,NULL,NULL,&LocMax);
		int px=LocMax.x+ p.mCenter.x-particle_size/2.0;
		int py=LocMax.y+ p.mCenter.y-particle_size/2.0;
		p.mArea=(matchResult.at<float>(py, px)); 
		//matchResult.at<float>(py, px)=0;
		//cout << p.mArea << "\n";
		p.mCenter.x=px;
		p.mCenter.y=py;
			

		for (DataStructureParticles::tParticleVector::iterator pIt2 = mParticles.begin(); pIt2 != pIt; pIt2++) 
		{	
			Particle & p2 = *pIt2;
			if (sqr(p.mCenter.x-p2.mCenter.x)+sqr(p.mCenter.y-p2.mCenter.y) < sqr(particle_size)) {
				if (p.mArea>p2.mArea) {
					p2.mFrameNumber=-1;
				} else {
					p.mFrameNumber=-1;
				}
			}
		}
		if ((p.mFrameNumber>=0)&&(p.mArea>rejection_threshold)){
			p.mFrameNumber=min(p.mFrameNumber+1, particle_lifetime);
		}  else {
			p.mFrameNumber--;
		}
	}
	for (DataStructureParticles::tParticleVector::iterator pIt = 
	 mParticles.begin(); pIt != mParticles.end();) 
	{	
		Particle & p = *pIt;
		if (p.mFrameNumber<=0) {
			// delete particle
			//printf("deleting");
			pIt=mParticles.erase(pIt);
		} else {
			pIt++;
		}
	}

    matchResult=255*matchResult;	
	matchResult.convertTo(output, CV_8UC1);
	
	mParticlesOutput.clear();
	
	for (DataStructureParticles::tParticleVector::iterator pIt = mParticles.begin(); pIt != mParticles.end(); pIt++) 
	{	
	
		Particle newParticle;
		Particle & p = *pIt;
		newParticle.mCenter.x=p.mCenter.x-mPattern1.cols/2.0;
		newParticle.mCenter.y=p.mCenter.y-mPattern1.rows/2.0;
		newParticle.mFrameNumber=p.mFrameNumber;
		newParticle.mArea=p.mArea;
		mParticlesOutput.push_back(newParticle);
	}

	mCore->mDataStructureParticles.mParticles = &mParticlesOutput;
	
	
	if (mGenerateOutput) {
		DisplayEditor de(&mDisplayOutput);
			if (de.IsActive()) {
			complexI=output;
			//create display output image (log scale magnitude)
			if (mDisplayOutputImage == NULL) {
			  mDisplayOutputImage = cvCreateImage(cvSize(complexI.cols, 
								  complexI.rows), 8, 1);
			  
			}
			//generateFFTPreview(complexI, &output);
			//matchResult.convertTo(displayoutput, CV_8UC1);
			//*mDisplayOutputImage=displayoutput;
			mDisplayOutputImage=mCore->mDataStructureImageGray.mImage;
			// draw particles
			for (DataStructureParticles::tParticleVector::iterator pIt = mParticles.begin(); pIt != mParticles.end(); pIt++) {
				const Particle & p = *pIt;
				if (p.mFrameNumber>2){
					cvCircle(mDisplayOutputImage, Point(p.mCenter.x-mPattern1.cols/2.0, p.mCenter.y-mPattern1.rows/2.0), particle_size/2.0, CV_RGB(255, 255, 255), 1, CV_AA, 0 );
				} 
				
			}
			//cvGetImage(output, mDisplayOutputImage);
			//*mDisplayOutputImage=output;

			if (de.IsActive()) de.SetMainImage(mDisplayOutputImage);
		}
				
		DisplayEditor dep(&mPatternPreview);
		if (dep.IsActive()) {
			if (matchPattern==0) {
				*mPatternPreviewImage=mPattern1;
			} else {
				*mPatternPreviewImage=mPattern2;
			}
			dep.SetMainImage(mPatternPreviewImage);
		}
	
		
		DisplayEditor dem(&mMatchPreview);
		if (dem.IsActive()) {
			if (mMatchPreviewImage == NULL) {
			  mMatchPreviewImage = cvCreateImage(cvSize(output.cols, 
								  output.rows), 8, 1);
			  
			}
			*mMatchPreviewImage=output;
			dem.SetMainImage(mMatchPreviewImage);
		}
	
	}
	//*mCore->mDataStructureImageGray.mImage=output;
	
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	//cvReleaseImage(&mDisplayOutputImage);
}


void THISCLASS::prepareFilter(Mat input) {
			// prepare bandpass filter
		cv::Mat planes[] = {cv::Mat::ones(input.size(),CV_32F), cv::Mat::ones(input.size(), CV_32F)};
		int cx = input.cols;
		int cy = input.rows;
		float center_frequency=GetConfigurationDouble(wxT("CenterFrequency"), true)*cx/1.4;
		float filter_sigma=GetConfigurationDouble(wxT("Sigma"), true)*(cx/2.0);
		float filter_width=GetConfigurationDouble(wxT("Width"), true)*cx/2.0;
		float filter_gain=GetConfigurationDouble(wxT("Gain"), true);

		for (int j=0; j<cy; j++){
			for (int i=0; i<cx; i++){
				float dist=sqrt((float)((i+cx/2)%cx-cx/2) * (float)((i+cx/2)%cx-cx/2)  +  (float)((j+cy/2)%cy-cy/2) * (float)((j+cy/2)%cy-cy/2));
				float x=fabs(dist - center_frequency) -filter_width;
				if (x<0.0) {
					x=0.0;
				}
				//float f_value=(float)1.0/(filter_sigma*sqrt((2*3.1415))) * exp( -0.5 * pow((dist-(center_frequency))/ (filter_sigma), 2));
				float f_value= exp( -0.5 * pow( (x) / (filter_sigma), 2));
				
				if( GetConfigurationBool(wxT("InvertFilter"), true)) {
					//f_value=1.0-f_value;
				}
				
				f_value=filter_gain*f_value;
				if ((i==0) || (j==0)) f_value=0.0;
				planes[0].at<float>(i,j) = f_value;
				planes[1].at<float>(i,j) = f_value;
				
			}
		}
		
		cv::merge(planes, 2, mFilterMatrix);

		// prepare pattern image
		cv::Mat I;
		mPattern1.convertTo(I, CV_32F);
		I-=128.0;
		I/=128.0;
		Mat I_Blurred;
		GaussianBlur(I, I_Blurred, Size(15,15), 0,0);
		I-=I_Blurred;
		cv::Mat padded;                            //expand input image to optimal size
		int m = cx;
		int n = cy;
	
		int shift=0;//(int)GetConfigurationDouble(wxT("shift"), true);
		// pad pattern to same size as input
		cv::copyMakeBorder(I, padded,  n - I.rows-shift, shift, m- I.cols-shift,shift, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	
		mPatterns=padded;
		
		planes[0] = cv::Mat_<float>(padded);
		planes[1] = cv::Mat::zeros(padded.size(), CV_32F);
		cv::Mat mPatternFourier1;
		cv::merge(planes, 2, mPatternFourier1);         // Add to the expanded another plane with zeros

		dft(mPatternFourier1, mPatternFourier1, 0);            // this way the result may fit in the source matrix
		mPatternFourierFiltered1=mPatternFourier1;
		cv::mulSpectrums(mPatternFourier1, mFilterMatrix, mPatternFourierFiltered1, DFT_ROWS);

		// prepare second pattern image
		
		cv::Mat mPatternFourier2;
		mPattern2.convertTo(I, CV_32F);
		I-=128.0;
		I/=128.0;
		GaussianBlur(I, I_Blurred, Size(15,15), 0,0);
		I-=I_Blurred;
		m = cx;
		n = cy;
	
		shift=0;
		// pad pattern to same size as input
		cv::copyMakeBorder(I, padded,  n - I.rows-shift, shift, m- I.cols-shift,shift, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	
		mPatterns=padded;
		
		planes[0] = cv::Mat_<float>(padded);
		planes[1] = cv::Mat::zeros(padded.size(), CV_32F);
		cv::merge(planes, 2, mPatternFourier2);         // Add to the expanded another plane with zeros

		dft(mPatternFourier2, mPatternFourier2, 0);            // this way the result may fit in the source matrix
		mPatternFourierFiltered2=mPatternFourier2;
		cv::mulSpectrums(mPatternFourier2, mFilterMatrix, mPatternFourierFiltered2, DFT_ROWS);

#ifdef GPU_ENABLED
		mPatternFourierFiltered1_GPU.upload(mPatternFourierFiltered1);
		mPatternFourierFiltered2_GPU.upload(mPatternFourierFiltered2);
#endif


		//prepare tracking matrix
		tracker=cv::Mat::zeros(padded.size(), CV_32F);
		temp=cv::Mat::zeros(matchResult.size(), CV_32F);
		
		
}


void THISCLASS::generateFFTPreview(Mat complexI, Mat *output) {
			// compute the magnitude and switch to logarithmic scale
		// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
		cv::Mat planes[2];
		
		split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
		magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
		cv::Mat magI = planes[0];

		magI += cv::Scalar::all(1);                    // switch to logarithmic scale
		cv::log(magI, magI);

		// crop the spectrum, if it has an odd number of rows or columns
		//magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

		// rearrange the quadrants of Fourier image  so that the origin is at the image center
		int cx = magI.cols/2;
		int cy = magI.rows/2;

		cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
		cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));  // Top-Right
		cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));  // Bottom-Left
		cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

		cv::Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
		q0.copyTo(tmp);
		q3.copyTo(q0);
		tmp.copyTo(q3);

		q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
		q2.copyTo(q1);
		tmp.copyTo(q2);

		cv::normalize(magI, magI, 0, 255, CV_MINMAX); // Transform the matrix with float values into a
												// viewable image form (float between values 0 and 1).
		magI.convertTo(*output, CV_8UC1);
}

inline float THISCLASS::sqr(float in) {return in*in;}

