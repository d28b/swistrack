#include "DataStructureImageBinary.h"
#define THISCLASS DataStructureImageBinary

CvSize THISCLASS::GetInputDim() {
	return cvSize(mImage->width, mImage->height);
}
