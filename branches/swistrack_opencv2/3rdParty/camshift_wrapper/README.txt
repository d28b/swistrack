This Simple Camshift Wrapper is intended to make Camshift more accessible
to the casual, or beginning, user. It provides a minimal C-language interface
with four main methods:

 createTracker(): pre-allocates internal data structures

 releaseTracker(): releases Camshift resources

 startTracking(): initiates tracking from an image plus a rectangular region

 track(): tracks the object in this region from frame to frame using Camshift

There are also two methods for setting the parameters smin and vmin:

 setVmin()

 setSmin()

These two parameters control tracking quality. They're set to default to the
values that worked best for my ancient Logitech webcam. You may need to tweak
these values, using the above interfaces, to get good tracking results with
your setup.


The download includes an example program, called Track Faces. It detects a face
in live video and automatically begins tracking it. You may need to modify
OPENCV_ROOT (TrackFaces.c, line 20) to reflect your directory structure.

To use the Simple Camshift Wrapper, or run the example program, you'll need to
have OpenCV installed. See http://sourceforge.net/projects/opencvlibrary for
the latest download and installation instructions, if needed.

For documentation and a current download of the Simple Camshift Wrapper, please
visit the Cognotics website:
http://www.cognotics.com/opencv/downloads/camshift_wrapper

Robin Hewitt
Feb 2007
