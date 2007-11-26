#ifndef HEADER_ImageConversion
#define HEADER_ImageConversion

class ImageConversion;

#include <cv.h>

//! A list of error messages. This is used by the XML functions.
class ImageConversion {

public:
	//! Converts an image to RGB. If the image is in the right format already, src is returned. Otherwise, a new image is created and returned.
	static IplImage *ToRGB(IplImage *src);
	//! Converts an image to BGR. If the image is in the right format already, src is returned. Otherwise, a new image is created and returned.
	static IplImage *ToBGR(IplImage *src);
	//! Returns a new image with a different channel order.
	static IplImage *SwitchChannels(IplImage *src, char *order);

	//! Converts from packed YUV422 to BGR. (inputimage and outputimage must not be the same.)
	static void CvtYUV422ToBGR(IplImage *inputimage, IplImage *outputimage);
	//! Converts from packed YUV422 to grayscale. (inputimage and outputimage must not be the same.)
	static void CvtYUV422ToGray(IplImage *inputimage, IplImage *outputimage);
};

#endif
