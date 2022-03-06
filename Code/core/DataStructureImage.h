#ifndef HEADER_DataStructureImage
#define HEADER_DataStructureImage

#include "DataStructure.h"
#include <wx/string.h>

#include "opencv2/opencv.hpp"

#ifdef GPU_ENABLED
#include "opencv2/gpu/gpu.hpp"
#endif

//! A DataStructure holding an image.
class DataStructureImage: public DataStructure {

public:
	cv::Mat mImage;	  //! Image.

	//! Constructor.
	DataStructureImage(const wxString & name, const wxString & displayname): DataStructure(name), mImage() {
		mDisplayName = displayname;
	}

	//! Destructor.
	~DataStructureImage() {}
};

#endif
