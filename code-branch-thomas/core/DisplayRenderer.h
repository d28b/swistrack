#ifndef HEADER_DisplayRenderer
#define HEADER_DisplayRenderer

class DisplayRenderer;

//! This class renders a display image.
class DisplayRenderer {

public:
	//! Constructor.
	DisplayRenderer(Display *display=0, double scalingfactor=1);
	//! Destructor.
	~DisplayRenderer();

	//! Sets a new scaling factor. This will cause image repaining.
	bool IsValid() {return (mDisplay!=null);}

	//! Sets a new scaling factor. This will cause image repaining.
	void SetScalingFactor(double scalingfactor);
	//! Returns the scaling factor.
	double GetScalingFactor() {return mScalingFactor;}

	//! Creates (if necessary) and returns the image.
	IplImage *GetImage();
	//! Returns the total size of the rendered image.
	CvSize GetSize();

private:
	Display mDisplay;		//! The attached Display.
	double mScalingFactor;	//!< The scaling factor.
	IplImage *mImage;		//!< The cached image.

};

#endif