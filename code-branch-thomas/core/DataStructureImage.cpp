#include "DataStructureImage.h"
#define THISCLASS DataStructureImage

CvSize THISCLASS::GetSize() {
	return cvSize(mImage->width, mImage->height);
}
