#ifndef HEADER_ImageTools
#define HEADER_ImageTools

class ImageTools;

#include <opencv2/opencv.hpp>

class ImageTools {

public:
	//! Converts an image to RGB. If the image is in the right format already, src is returned. Otherwise, a new image is created and returned.
	static cv::Mat ToRGB(cv::Mat src, char * channelSequence);
	//! Converts an image from BGR to RGB.
	static cv::Mat BGRToRGB(cv::Mat src);
	//! Calculates norm(image - color) for every pixel of a 3-channel image.
	static cv::Mat DistanceToColor(cv::Mat image, cv::Scalar color, double scaling);
	//! Calculates abs(image - color) for every channel of a 3-channel image.
	static cv::Mat AbsDiff3(cv::Mat image, cv::Scalar color);

	//! Checks if two images have the same size.
	static bool EqualSize(cv::Mat a, cv::Mat b);

	//! Converts from packed YUV422 to BGR. (inputImage and outputimage must not be the same.)
	static void CvtYUV422ToBGR(cv::Mat inputImage, cv::Mat outputImage);
	//! Converts from packed YUV422 to grayscale. (inputImage and outputimage must not be the same.)
	static void CvtYUV422ToGray(cv::Mat inputImage, cv::Mat outputImage);
};

#endif
