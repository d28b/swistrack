#ifndef HEADER_WriteAvi
#define HEADER_WriteAvi

#include <wx/string.h>
#include <opencv2/core.hpp>

class WriteAvi {
 public:
  WriteAvi();
  ~WriteAvi();

  void Open(wxString filename);
  void WriteFrame(cv::Mat * image);
  void Close();

 private:

  wxString mFilename;
  CvVideoWriter * mWriter;  //!< Pointer to AVI sequence.
  double mFrameRate;

};

#endif

