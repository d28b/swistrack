#ifndef HEADER_WriteAvi
#define HEADER_WriteAvi

#include <wx/string.h>
#include <cv.h>
#include <highgui.h>

class WriteAvi {
 public: 
  WriteAvi();
  ~WriteAvi();

  void Open(wxString filename);
  void WriteFrame(IplImage * image);
  void Close();

 private:

  wxString mFilename;
  CvVideoWriter* mWriter;  //!< Pointer to AVI sequence.
  double mFrameRate;
  
};

#endif

