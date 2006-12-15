#include "DataStructureImage.h"
#define THISCLASS DataStructureImage

CvSize THISCLASS::GetInputDim(){
	return cvSize(mImage->width, mImage->height);
}
