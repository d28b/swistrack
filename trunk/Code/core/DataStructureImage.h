#ifndef HEADER_DataStructureImage
#define HEADER_DataStructureImage

#include "DataStructure.h"
#include <wx/string.h>
#include <cv.h>

//! A DataStructure holding an image.
class DataStructureImage: public DataStructure {

public:
	IplImage* mImage;	//! Image.

	//! Constructor.
	DataStructureImage(const wxString &name, const wxString &displayname): DataStructure(name), mImage(0) {
		mDisplayName = displayname;
	}
	//! Destructor.
	~DataStructureImage() {}
};

#endif
