#ifndef HEADER_DisplayRenderer
#define HEADER_DisplayRenderer

class DisplayRenderer;

//! This class renders a display image.
class DisplayRenderer {

public:
	//! The attached Display.
	Display mDisplay;

	//! Constructor.
	DisplayRenderer(Display *display=0);
	//! Destructor.
	~DisplayRenderer();

	//! Creates (if necessary) and returns the image.
	IplImage *GetImage();
	//! Returns the size.
	CvSize GetSize();

private:
	double mCachedViewScalingFactor;	//!< The scaling factor of the last created image.
	IplImage *mCachedViewImage;			//!< The last created image.

};

#endif