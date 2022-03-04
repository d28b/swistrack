#include <opencv2/core.hpp>

int main(int argc, char * argv[]) {
	cv::Mat image;
	printf("image %d %d %d %d\n", image.channels(), image.dims, image.cols, image.rows);

	image.create(100, 100, CV_8UC4);
	printf("image %d %d %d %d %lu %s\n", image.channels(), image.dims, image.cols, image.rows, image.elemSize(), image.empty() ? "empty" : "");

	cv::Mat output = image + image;
	//cv::add(image, image, output);

	cv::Scalar mean = cv::mean(image);
	printf("image mean %f\n", mean.val[0]);

	image.release();

	printf("image %d %d %d %d %lu %s\n", image.channels(), image.dims, image.cols, image.rows, image.elemSize(), image.empty() ? "empty" : "");
	return 0;
}
