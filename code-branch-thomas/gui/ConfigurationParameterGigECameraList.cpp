#include "ConfigurationParameterGigECameraList.h"
#define THISCLASS ConfigurationParameterGigECameraList

#ifdef USE_CAMERA_PYLON_GIGE
#include <pylon/TlFactory.h>
#include <pylon/gige/BaslerGigECamera.h>

void THISCLASS::FillList(ConfigurationXML *config, ErrorList *errorlist) {
	try {
		// Get the list of all cameras
		Pylon::CTlFactory& tlfactory=Pylon::CTlFactory::GetInstance();
		Pylon::DeviceInfoList_t devices;
		if (tlfactory.EnumerateDevices(devices)==0) {
			AddItem("error", "No GigE cameras found!");
			return;
		}

		// Fill in the list
		Pylon::DeviceInfoList_t::iterator it=devices.begin();
		while (it!=devices.end()) {
			AddItem(it->GetFullName().c_str(), it->GetFriendlyName().c_str());
			it++;
		}
	} catch (GenICam::GenericException &e) {
		AddItem("error", e.GetDescription());
		return;
	}

	// Add the "Any" entry
	AddItem("", "Any");
}

#else

void THISCLASS::FillList(ConfigurationXML *config, ErrorList *errorlist) {
	AddItem("error", "No GigE support.");
}

#endif // USE_CAMERA_PYLON_GIGE
