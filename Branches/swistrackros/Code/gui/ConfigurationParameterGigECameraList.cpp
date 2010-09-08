#include "ConfigurationParameterGigECameraList.h"
#define THISCLASS ConfigurationParameterGigECameraList

#ifdef USE_CAMERA_PYLON_GIGE
#include <pylon/TlFactory.h>
#include <pylon/gige/BaslerGigECamera.h>

void THISCLASS::FillList(ConfigurationXML *config, ErrorList *errorlist) {
	try {
		// Get the list of all cameras
		Pylon::CTlFactory& tlfactory = Pylon::CTlFactory::GetInstance();
		Pylon::DeviceInfoList_t devices;
		if (tlfactory.EnumerateDevices(devices) == 0) {
			AddItem(wxT(""), wxT("No GigE cameras found!"));
			return;
		}

		// Fill in the list
		Pylon::DeviceInfoList_t::iterator it = devices.begin();
		while (it != devices.end()) {
			wxString name(it->GetFullName().c_str(), wxConvLocal);
			wxString value(it->GetFriendlyName().c_str(), wxConvLocal);
			AddItem(name, value);
			it++;
		}
	} catch (GenICam::GenericException &e) {
		AddItem(wxT(""), wxString::Format(wxT("%s"), e.GetDescription()));
		return;
	}

	// Add the "Any" entry
	AddItem(wxT(""), wxT("Any"));
}

#else

void THISCLASS::FillList(ConfigurationXML *config, ErrorList *errorlist) {
	AddItem(wxT(""), wxT("No GigE support."));
}

#endif // USE_CAMERA_PYLON_GIGE
