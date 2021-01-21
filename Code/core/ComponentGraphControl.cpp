#include "ComponentGraphControl.h"
#define THISCLASS ComponentGraphControl

#include "DisplayEditor.h"

static inline int sqr(int x) {return x*x;};
using namespace cv;


THISCLASS::ComponentGraphControl(SwisTrackCore *stc):
		Component(stc, wxT("GraphControl")),
		mCommands(),
		mMaxNumber(10),
		mDisplayOutput(wxT("Output"), wxT("Graph Control"))
{
	// Data structure relations
	mCategory = &(mCore->mCategoryTracking);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureCommands));
	AddDisplay(&mDisplayOutput);

	Initialize();						// Read the XML configuration file
}


THISCLASS::~ComponentGraphControl()
{
	
}

void THISCLASS::OnStart()
{
	mMaxNumber = GetConfigurationInt(wxT("MaxNumber"), 10);
	maxParticles = 10;

	// Check for stupid configurations
	if (mMaxNumber < 1)
		AddError(wxT("Max number of tracks must be greater or equal to 1"));


	distanceArray = new double*[mMaxNumber];
	for (int i = 0;i < mMaxNumber;i++)
		distanceArray[i] = new double[maxParticles];
	THISCLASS::OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration()
{
	ClusterSize = GetConfigurationInt(wxT("ClusterSize"), 5);
	ParticleSize = GetConfigurationInt(wxT("ParticleSize"), 30);
	ArenaSize = GetConfigurationInt(wxT("ArenaSize"), 150);
	
}

void THISCLASS::OnStep()
{	
	int maxconnected;
	int mode=0;
	//distance array is too small, release and recreate
	if ((int) mCore->mDataStructureParticles.mParticles->size() > maxParticles)
	{
		maxParticles = mCore->mDataStructureParticles.mParticles->size();
		for (int i = 0;i < mMaxNumber;i++)
		{
			delete[] distanceArray[i];
			distanceArray[i] = new double[maxParticles];
		}
	}
	// get the particles as input to component
	//	(pointer modifies data in place!)
	mParticles = mCore->mDataStructureParticles.mParticles;
	imgCenterX=mCore->mDataStructureImageGray.mImage->width/2;
	imgCenterY=mCore->mDataStructureImageGray.mImage->height/2;
	// Graph associated with the current configuration of particles
	
	Graph cGraph=Graph();
	
	// Definition of the adjacency matrix for the current configuration of the particles
	DataStructureParticles::tParticleVector::iterator pIt1 = mParticles->begin();
	int i;
	int particlesInArena=0;
	for (i=0; pIt1 != mParticles->end() && (i<VcountMax); i++, pIt1++) {
		const Particle & p1 = *pIt1;
		if (sqr(p1.mCenter.x-imgCenterX)+sqr(p1.mCenter.y-imgCenterY)<sqr(ArenaSize)) {
			particlesInArena++;
			DataStructureParticles::tParticleVector::iterator pIt2 = mParticles->begin();

			for (int j=0; pIt2 != pIt1 &&(j<VcountMax); j++,pIt2++) 
				{	
				Particle & p2 = *pIt2;
				if ((sqr(p1.mCenter.x-p2.mCenter.x)+sqr(p1.mCenter.y-p2.mCenter.y) < sqr(ParticleSize*1.5))) {
					cGraph.AdjM[i][j] = 1;    // edge existing between the vertices (connected particles)
					cGraph.AdjM[j][i] = 1; 
				}
				else {
					cGraph.AdjM[i][j] = 0;	// edge not existing (unconnected particles)
					cGraph.AdjM[j][i] = 0;	// edge not existing (unconnected particles)
				}
			}
		}
	}
	cGraph.Vcount=i;
	
	maxconnected = cGraph.GetMaxConnectedComponents();
	
	
	mCommands.clear();
	
	//printf("connected: %i\n", maxconnected );
	if (particlesInArena< ClusterSize/3+1) { 	// random mode
		mode=-1;
	} else 
	if (maxconnected == ClusterSize) { 	// switch piezo off
		mode=0;
	} else 
	if (maxconnected < ClusterSize) { 	// switch to assembly mode
	    mode=1;
	} else 
	if (maxconnected > ClusterSize) {// switch to disassembly mode
		mode=2;
	}
	mCommands.push_back(Command(0, wxT("piezo_mode"), mode));
	
	mCore->mDataStructureCommands.mCommands=&mCommands;
	
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		mDisplayOutputImage=mCore->mDataStructureImageGray.mImage;
		// draw particles
		CvFont font;
		double hScale=0.5;
		double vScale=0.5;
		int    lineWidth=1;
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
		switch(mode) {
			case 0:
				cvPutText (mDisplayOutputImage,"off",cvPoint(20,20), &font, cvScalar(255,255,0));
				break;
			case 1:
				cvPutText (mDisplayOutputImage,"assembly",cvPoint(20,20), &font, cvScalar(255,255,0));
				break;
			case 2:
				cvPutText (mDisplayOutputImage,"disassembly",cvPoint(20,20), &font, cvScalar(255,255,0));
				break;

		}
		
		cvCircle(mDisplayOutputImage, Point(imgCenterX, imgCenterY), ArenaSize, CV_RGB(140, 140, 140), 1, CV_AA, 0 );
		pIt1 = mParticles->begin();
		for (int i=0; pIt1 != mParticles->end(); i++, pIt1++) {
			const Particle & p1 = *pIt1;
			if (p1.mFrameNumber>2){
				cvCircle(mDisplayOutputImage, Point(p1.mCenter.x, p1.mCenter.y), ParticleSize/2.0, CV_RGB(255, 255, 255), 1, CV_AA, 0 );
			}
			DataStructureParticles::tParticleVector::iterator pIt2 = mParticles->begin();
			for (int j=0; pIt2 != pIt1; j++,pIt2++) 
			{	
				const Particle & p2 = *pIt2;
				if (cGraph.AdjM[i][j]==1) {
					cvLine(mDisplayOutputImage, Point(p1.mCenter.x, p1.mCenter.y), 
												Point(p2.mCenter.x, p2.mCenter.y),
												CV_RGB(255, 255, 255), 
												1, CV_AA, 0);
					
				}
			}
		}			

		de.SetMainImage(mDisplayOutputImage);
	}
	
	
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles = 0;
}

void THISCLASS::OnStop() {
}
