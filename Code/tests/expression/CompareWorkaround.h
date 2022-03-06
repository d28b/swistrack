#ifndef HEADER_CompareWorkaround
#define HEADER_CompareWorkaround

#include <opencv2/core.hpp>

// 2022-03-06:
// cv::compare, as well as the operators "<" and ">" don't work with float32 matrices. They return an empty matrix.
// The following functions provide the same functionality. They require CV_32FC1 matrices (float32, 1 channel).

cv::Mat smallerThan(cv::Mat a, cv::Mat b);
cv::Mat smallerThan(cv::Mat a, double b);
cv::Mat smallerThan(double a, cv::Mat b);
double smallerThan(double a, double b);

cv::Mat biggerThan(cv::Mat a, cv::Mat b);
cv::Mat biggerThan(cv::Mat a, double b);
cv::Mat biggerThan(double a, cv::Mat b);
double biggerThan(double a, double b);

#endif
