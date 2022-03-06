#include "CompareWorkaround.h"

cv::Mat smallerThan(cv::Mat a, cv::Mat b) {
	cv::Mat output(a.size(), CV_32FC1);
	for (int y = 0; y < output.rows; y++) {
		float * aLine = a.ptr<float>(y);
		float * bLine = b.ptr<float>(y);
		float * outputLine = output.ptr<float>(y);
		for (int x = 0; x < output.cols; x++)
			outputLine[x] = aLine[x] < bLine[x] ? 255.0f : 0.0f;
	}

	return output;
}

cv::Mat smallerThan(cv::Mat a, double b) {
	cv::Mat output(a.size(), CV_32FC1);
	for (int y = 0; y < output.rows; y++) {
		float * aLine = a.ptr<float>(y);
		float * outputLine = output.ptr<float>(y);
		for (int x = 0; x < output.cols; x++)
			outputLine[x] = aLine[x] < b ? 255.0f : 0.0f;
	}

	return output;
}

cv::Mat smallerThan(double a, cv::Mat b) {
	cv::Mat output(b.size(), CV_32FC1);
	for (int y = 0; y < output.rows; y++) {
		float * bLine = b.ptr<float>(y);
		float * outputLine = output.ptr<float>(y);
		for (int x = 0; x < output.cols; x++)
			outputLine[x] = a < bLine[x] ? 255.0f : 0.0f;
	}

	return output;
}

double smallerThan(double a, double b) {
	return a < b ? 255.0 : 0.0;
}

cv::Mat biggerThan(cv::Mat a, cv::Mat b) {
	cv::Mat output(a.size(), CV_32FC1);
	for (int y = 0; y < output.rows; y++) {
		float * aLine = a.ptr<float>(y);
		float * bLine = b.ptr<float>(y);
		float * outputLine = output.ptr<float>(y);
		for (int x = 0; x < output.cols; x++)
			outputLine[x] = aLine[x] > bLine[x] ? 255.0f : 0.0f;
	}

	return output;
}

cv::Mat biggerThan(cv::Mat a, double b) {
	cv::Mat output(a.size(), CV_32FC1);
	for (int y = 0; y < output.rows; y++) {
		float * aLine = a.ptr<float>(y);
		float * outputLine = output.ptr<float>(y);
		for (int x = 0; x < output.cols; x++)
			outputLine[x] = aLine[x] > b ? 255.0f : 0.0f;
	}

	return output;
}

cv::Mat biggerThan(double a, cv::Mat b) {
	cv::Mat output(b.size(), CV_32FC1);
	for (int y = 0; y < output.rows; y++) {
		float * bLine = b.ptr<float>(y);
		float * outputLine = output.ptr<float>(y);
		for (int x = 0; x < output.cols; x++)
			outputLine[x] = a > bLine[x] ? 255.0f : 0.0f;
	}

	return output;
}

double biggerThan(double a, double b) {
	return a > b ? 255.0 : 0.0;
}
