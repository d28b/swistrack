#include "ImageTools.h"
#define THISCLASS ImageTools

cv::Mat THISCLASS::ToRGB(cv::Mat src, char * channelSequence) {
	if (src.channels() == 1) {
		cv::Mat dest(src.size(), CV_8UC3);
		cv::cvtColor(src, dest, cv::COLOR_GRAY2RGB);
		return dest;
	} else if (strcmp(channelSequence, "RGB") == 0) {
		return src;
	} else if (strcmp(channelSequence, "BGR") == 0) {
		cv::Mat dest(src.size(), CV_8UC3);
		cv::cvtColor(src, dest, cv::COLOR_BGR2RGB);
		return dest;
	} else if (strcmp(channelSequence, "HSV") == 0) {
		cv::Mat dest(src.size(), CV_8UC3);
		cv::cvtColor(src, dest, cv::COLOR_HSV2RGB);
		return dest;
	} else if (strcmp(channelSequence, "HLS") == 0) {
		cv::Mat dest(src.size(), CV_8UC3);
		cv::cvtColor(src, dest, cv::COLOR_HLS2RGB);
		return dest;
	} else if (strcmp(channelSequence, "YCC") == 0) {
		cv::Mat dest(src.size(), CV_8UC3);
		cv::cvtColor(src, dest, cv::COLOR_YCrCb2RGB);
		return dest;
	} else if (strcmp(channelSequence, "XYZ") == 0) {
		cv::Mat dest(src.size(), CV_8UC3);
		cv::cvtColor(src, dest, cv::COLOR_XYZ2RGB);
		return dest;
	} else if (strcmp(channelSequence, "Lab") == 0) {
		cv::Mat dest(src.size(), CV_8UC3);
		cv::cvtColor(src, dest, cv::COLOR_Lab2RGB);
		return dest;
	} else {
		return src;
	}
}

cv::Mat THISCLASS::BGRToRGB(cv::Mat src) {
	cv::Mat dest(src.size(), CV_8UC3);
	cv::cvtColor(src, dest, cv::COLOR_BGR2RGB);
	return dest;
}

cv::Mat THISCLASS::DistanceToColor(cv::Mat image, cv::Scalar color, double scaling) {
	cv::Mat distanceImage(image.rows, image.cols, CV_8UC1);
	for (int y = 0; y < image.rows; y++) {
		unsigned char * line = image.ptr(y);
		unsigned char * distanceLine = distanceImage.ptr(y);
		for (int x = 0; x < image.cols; x++) {
			int r = line[x * 3 + 0];
			int g = line[x * 3 + 1];
			int b = line[x * 3 + 2];
			int dr = abs(r - color.val[0]);
			int dg = abs(g - color.val[1]);
			int db = abs(b - color.val[2]);
			double distance = sqrt(dr * dr + dg * dg + db * db) * scaling;
			int distanceInt = (int) (distance + 0.5);
			if (distanceInt > 255) distanceInt = 255;
			distanceLine[x] = distanceInt;
		}
	}

	return distanceImage;
}

cv::Mat THISCLASS::AbsDiff3(cv::Mat image, cv::Scalar color) {
	cv::Mat absDiffImage(image.rows, image.cols, CV_8UC3);
	for (int y = 0; y < image.rows; y++) {
		unsigned char * line = image.ptr(y);
		unsigned char * absDiffLine = absDiffImage.ptr(y);
		for (int x = 0; x < image.cols; x++) {
			int b = line[x * 3 + 0];
			int g = line[x * 3 + 1];
			int r = line[x * 3 + 2];
			absDiffLine[x * 3 + 0] = abs(b - color.val[0]);
			absDiffLine[x * 3 + 1] = abs(g - color.val[1]);
			absDiffLine[x * 3 + 2] = abs(r - color.val[2]);
		}
	}

	return absDiffImage;
}

bool THISCLASS::EqualSize(cv::Mat a, cv::Mat b) {
	if (a.rows != b.rows) return false;
	if (a.cols != b.cols) return false;
	return true;
}

cv::Mat THISCLASS::Flip(cv::Mat image, bool flipHorizontally, bool flipVertically) {
	if (flipHorizontally && flipVertically) {
		cv::Mat flippedImage(image.size(), image.channels() == 1 ? CV_8UC1 : CV_8UC3);
		cv::flip(image, flippedImage, -1);
		return flippedImage;
	} else if (flipHorizontally) {
		cv::Mat flippedImage(image.size(), image.channels() == 1 ? CV_8UC1 : CV_8UC3);
		cv::flip(image, flippedImage, 1);
		return flippedImage;
	} else if (flipVertically) {
		cv::Mat flippedImage(image.size(), image.channels() == 1 ? CV_8UC1 : CV_8UC3);
		cv::flip(image, flippedImage, 0);
		return flippedImage;
	} else {
		return image;
	}
}
/*
void THISCLASS::CvtYUV422ToBGR(cv::Mat inputImage, cv::Mat outputImage) {
	unsigned char * crl = (unsigned char *) inputImage->imageData;
	unsigned char * cwl = (unsigned char *) outputImage->imageData;
	for (int y = 0; y < inputImage->height; y++) {
		unsigned char * cr = crl;
		unsigned char * cw = cwl;
		for (int x = 0; x < inputImage->width; x += 2) {
			int u = (int) * cr;
			cr++;
			int y1 = (int) * cr;
			cr++;
			int v = (int) * cr;
			cr++;
			int y2 = (int) * cr;
			cr++;

			// Pixel 1
			int c = y1 - 16;
			int d = u - 128;
			int e = v - 128;

			int r = ( 298 * c + 409 * e + 128) >> 8;
			int g = ( 298 * c - 100 * d - 208 * e + 128) >> 8;
			int b = ( 298 * c + 516 * d + 128) >> 8;

			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			if (r < 0) r = 0;
			if (g < 0) g = 0;
			if (b < 0) b = 0;

			*cw = (unsigned char)b;
			cw++;
			*cw = (unsigned char)g;
			cw++;
			*cw = (unsigned char)r;
			cw++;

			// Pixel 2
			c = y2 - 16;

			r = ( 298 * c + 409 * e + 128) >> 8;
			g = ( 298 * c - 100 * d - 208 * e + 128) >> 8;
			b = ( 298 * c + 516 * d + 128) >> 8;

			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			if (r < 0) r = 0;
			if (g < 0) g = 0;
			if (b < 0) b = 0;

			*cw = (unsigned char)b;
			cw++;
			*cw = (unsigned char)g;
			cw++;
			*cw = (unsigned char)r;
			cw++;
		}
		crl += inputImage->widthStep;
		cwl += outputImage->widthStep;
	}
	outputImage->dataOrder = 0;
}

void THISCLASS::CvtYUV422ToGray(cv::Mat inputImage, cv::Mat outputImage) {
	unsigned char * crl = (unsigned char *) inputImage->imageData;
	unsigned char * cwl = (unsigned char *) outputImage->imageData;
	for (int y = 0; y < inputImage->height; y++) {
		unsigned char * cr = crl;
		unsigned char * cw = cwl;
		for (int x = 0; x < inputImage->width; x += 2) {
			cr++;
			int y1 = (int) *cr;
			cr++;
			cr++;
			int y2 = (int) *cr;
			cr++;

			*cw = y1;
			cw++;
			*cw = y2;
			cw++;
		}
		crl += inputImage->widthStep;
		cwl += outputImage->widthStep;
	}
}
*/
