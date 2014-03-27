// capture.h - by Robin Hewitt, 2007
// http://www.cognotics.com/opencv/downloads/camshift_wrapper
// This is free software. See License.txt, in the download
// package, for details.
//


// Public interface for video capture
int  initCapture();
void closeCapture();
IplImage * nextVideoFrame();

