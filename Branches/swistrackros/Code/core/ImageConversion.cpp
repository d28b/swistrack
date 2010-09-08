#include "ImageConversion.h"
#define THISCLASS ImageConversion

IplImage *THISCLASS::ToRGB(IplImage *src) {
	if (src->nChannels == 1) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_GRAY2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "RGB", 3) == 0) {
		return src;
	} else if (memcmp(src->channelSeq, "BGR", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_BGR2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "HSV", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_HSV2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "HLS", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_HLS2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "YCC", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_YCrCb2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "XYZ", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_XYZ2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "Lab", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_Lab2RGB);
		return dest;
	} else {
		return SwitchChannels(src, "RGB");
	}
}

IplImage *THISCLASS::ToBGR(IplImage *src) {
	if (src->nChannels == 1) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_GRAY2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "BGR", 3) == 0) {
		return src;
	} else if (memcmp(src->channelSeq, "RGB", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_RGB2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "HSV", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_HSV2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "HLS", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_HLS2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "YCC", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_YCrCb2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "XYZ", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_XYZ2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "Lab", 3) == 0) {
		IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_Lab2BGR);
		return dest;
	} else {
		return SwitchChannels(src, "BGR");
	}
}

IplImage *THISCLASS::SwitchChannels(IplImage *src, char *order) {
	// Create a new image of the same size
	IplImage* dest = cvCreateImage(cvGetSize(src), src->depth, 3);

	// Copy the channels to their desired position
	for (int srci = 0; srci < 3; srci++) {
		for (int desti = 0; desti < 3; desti++) {
			if (order[desti] == src->channelSeq[srci]) {
				dest->channelSeq[desti] = order[desti];

				cvSetImageCOI(src, srci + 1);
				cvSetImageCOI(dest, desti + 1);
				cvCopy(dest, src);
				break;
			}
		}
	}

	// Reset the COI
	cvSetImageCOI(src, 0);
	cvSetImageCOI(dest, 0);
	return dest;
}

void THISCLASS::CvtYUV422ToBGR(IplImage *inputimage, IplImage *outputimage) {
	unsigned char *crl = (unsigned char *)inputimage->imageData;
	unsigned char *cwl = (unsigned char *)outputimage->imageData;
	for (int y = 0; y < inputimage->height; y++) {
		unsigned char *cr = crl;
		unsigned char *cw = cwl;
		for (int x = 0; x < inputimage->width; x += 2) {
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
		crl += inputimage->widthStep;
		cwl += outputimage->widthStep;
	}
	outputimage->dataOrder = 0;
}

void THISCLASS::CvtYUV422ToGray(IplImage *inputimage, IplImage *outputimage) {
	unsigned char *crl = (unsigned char *)inputimage->imageData;
	unsigned char *cwl = (unsigned char *)outputimage->imageData;
	for (int y = 0; y < inputimage->height; y++) {
		unsigned char *cr = crl;
		unsigned char *cw = cwl;
		for (int x = 0; x < inputimage->width; x += 2) {
			cr++;
			int y1 = (int) * cr;
			cr++;
			cr++;
			int y2 = (int) * cr;
			cr++;

			*cw = y1;
			cw++;
			*cw = y2;
			cw++;
		}
		crl += inputimage->widthStep;
		cwl += outputimage->widthStep;
	}
}
