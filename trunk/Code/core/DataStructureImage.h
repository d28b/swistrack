#ifndef HEADER_DataStructureImage
#define HEADER_DataStructureImage

#include "DataStructure.h"
#include <wx/string.h>
#include <cv.h>




#include "opencv2/opencv.hpp"

#ifdef GPU_ENABLED
#include "opencv2/gpu/gpu.hpp"
#endif

//! A DataStructure holding an image.
class DataStructureImage: public DataStructure {

public:
	IplImage* mImage;	  //! Image.
	cv::Mat mMat;       // Added by Felix Schill, Jan. 2013. Required by ComponentFFT
	
	#ifdef GPU_ENABLED  // Added by Felix Schill, Jan. 2013. Required by ComponentFFT when using GPU
	cv::gpu::GpuMat mMat_Gpu;
	#endif
	
	//! Constructor.
	DataStructureImage(const wxString &name, const wxString &displayname): DataStructure(name), mImage(0) {
		mDisplayName = displayname;
	}
	//! Destructor.
	~DataStructureImage() {}
};

#endif
