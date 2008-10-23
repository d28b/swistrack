// facedet.h - by Robin Hewitt, 2007
// http://www.cognotics.com/opencv/downloads/camshift_wrapper
// This is free software. See License.txt, in the download
// package, for details.
//

// Public interface for face detection
int      initFaceDet(const char * haarCascadePath);
void     closeFaceDet();
CvRect * detectFace(IplImage * pImg);