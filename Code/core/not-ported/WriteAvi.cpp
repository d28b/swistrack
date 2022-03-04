
#include "WriteAvi.h"
using namespace std;
#include <iostream>
#define THISCLASS WriteAvi

THISCLASS::WriteAvi(): mWriter(0), mFrameRate(15) {
}

void THISCLASS::Open(wxString filename) {
  Close();
  mFilename = filename;
}

THISCLASS::~WriteAvi() {
  Close();
}

void THISCLASS::WriteFrame(cv::Mat * image) {

  cout << "Image: " << image << endl;
  cv::Size size = cvGetSize(image);
  cout << "bounds: " << size.width << ", " << size.height << endl;
  if (size.width == 960) {
    if (! mWriter) {
      //CV_FOURCC('P','I','M','1')
      mWriter = cvCreateVideoWriter(mFilename.mb_str(wxConvFile), 0, 10, cvGetSize(image));
      //mWriter = cvCreateVideoWriter(mFilename.mb_str(wxConvFile), -1, 10, cvGetSize(image));
    }

    cv::Mat *rgbImage = cvCreateImage(cvGetSize(image), 8, 3);
    cvCvtColor(image, rgbImage, CV_BGR2RGB);
    cvWriteFrame(mWriter, rgbImage);
    rgbImage->release();
  }
}

void THISCLASS::Close() {
  if (mWriter) {
    mWriter->release();
  }
  mWriter = 0;
}
