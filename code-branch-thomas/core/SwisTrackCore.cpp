#include "SwisTrackCore.h"
#define THISCLASS SwisTrackCore

THISCLASS::SwisTrackCore(xmlpp::Element* cfgroot):
		mConfigurationRoot(cfgroot),
		mDataStructureInput(),
		mDataStructureImageBGR("ImageBGR", "Color image (BGR)"),
		mDataStructureBackgroundBGR("BackgroundBGR", "Background color image (BGR)"),
		mDataStructureImageGray("ImageGray", "Grayscale image"),
		mDataStructureBackgroundGray("BackgroundGray", "Grayscale background image"),
		mDataStructureImageBinary("ImageBinary", "Binary image"),
		mDataStructureMaskBinary("MaskBinary", "Binary mask"),
		mDataStructureParticles()
		{

	// Initialize the list of component factories
	mComponents->push_back(new ComponentFactory<ComponentInputCamera1394>);
	mComponents->push_back(new ComponentFactory<ComponentInputCameraUSB>);
	mComponents->push_back(new ComponentFactory<ComponentInputCameraGBit>);
	mComponents->push_back(new ComponentFactory<ComponentInputFileAVI>);
	mComponents->push_back(new ComponentFactory<ComponentInputConvertToGray>);
	mComponents->push_back(new ComponentFactory<ComponentInputConvertToBGR>);
	mComponents->push_back(new ComponentFactory<ComponentInputBackgroundSubtractionGray>);
	mComponents->push_back(new ComponentFactory<ComponentInputThresholdGray>);
	mComponents->push_back(new ComponentFactory<ComponentInputParticleFilter>);
}

void THISCLASS::AddComponent(ComponentFactory *cf) {
	mComponentList->push_back(cf->Create());
}

void THISCLASS::RemoveComponent() {
}

bool THISCLASS::Start() {
	tComponentList::iterator it=mComponentList.begin();
	while (it!=mComponentList.end()) {
		it->mOK=it->Start();
		it++;
	}
}

void THISCLASS::Stop() {
	tComponentList::iterator it=mComponentList.end();
	while (it!=mComponentList.begin()) {
		it--;
		if (! it->Stop()) {it->mOK=false;}
	}
}

void THISCLASS::Step() {
	tComponentList::iterator it=mComponentList.begin();
	while (it!=mComponentList.end()) {
		if (! it->mOK) {break;}
		it->mOK=it->Step();
		if (! it->mOK) {break;}
		it++;
	}

	while (it!=mComponentList.begin()) {
		it--;
		if (! it->StepCleanup()) {it->mOK=false;}
	}
}
