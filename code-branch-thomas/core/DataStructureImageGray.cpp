#include "DataStructureImageGray.h"
#define THISCLASS DataStructureImageGray

CvSize THISCLASS::GetInputDim() {
	return cvSize(mImage->width, mImage->height);
}
