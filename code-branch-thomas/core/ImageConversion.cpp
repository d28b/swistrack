#include "ImageConversion.h"
#define THISCLASS ImageConversion

IplImage *THISCLASS::ToRGB(IplImage *src) {
	if (src->nChannels==1) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_GRAY2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "RGB", 3)) {
		return src;
	} else if (memcmp(src->channelSeq, "BGR", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_BGR2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "HSV", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_HSV2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "HLS", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_HLS2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "YCC", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_YCrCb2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "XYZ", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_XYZ2RGB);
		return dest;
	} else if (memcmp(src->channelSeq, "Lab", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_Lab2RGB);
		return dest;
	} else {
		return SwitchChannels(src, "RGB");
	}
}

IplImage *THISCLASS::ToBGR(IplImage *src) {
	if (src->nChannels==1) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_GRAY2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "BGR", 3)) {
		return src;
	} else if (memcmp(src->channelSeq, "RGB", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_RGB2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "HSV", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_HSV2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "HLS", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_HLS2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "YCC", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_YCrCb2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "XYZ", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_XYZ2BGR);
		return dest;
	} else if (memcmp(src->channelSeq, "Lab", 3)) {
		IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);
		cvCvtColor(src, dest, CV_Lab2BGR);
		return dest;
	} else {
		return SwitchChannels(src, "BGR");
	}
}

IplImage *THISCLASS::SwitchChannels(IplImage *src, char *order) {
	// Create a new image of the same size
	IplImage* dest=cvCreateImage(cvGetSize(src), src->depth, 3);

	// Copy the channels to their desired position
	for (int srci=0; srci<3; srci++) {
		for (int desti=0; desti<3; desti++) {
			if (order[desti]==src->channelSeq[srci]) {
				dest->channelSeq[desti]=order[desti];

				cvSetImageCOI(src, srci+1);
				cvSetImageCOI(dest, desti+1);
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
