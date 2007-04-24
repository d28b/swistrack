#ifndef HEADER_DisplayRenderer
#define HEADER_DisplayRenderer

class DisplayRenderer;

//! This class renders a display image.
class DisplayRenderer {

public:
	//! Constructor.
	DisplayRenderer(Display *display=0);
	//! Destructor.
	~DisplayRenderer();

	//! Sets the display.
	void SetDisplay(Display *display) {
	//! Sets a new scaling factor. This will cause image repaining.
	void SetScalingFactor(double scalingfactor);
	//! Sets the horizontal flip flag.
	void SetFlipHorizontal(bool flip);
	//! Sets the vertical flip flag.
	void SetFlipVertical(bool flip);
	//! Sets the crop rectangle (the area of interest). This is currently not implemented.
	void SetCropRectangle(CvRect rect);

	//! Returns the display.
	double GetDisplay() {return mDisplay;}
	//! Returns the scaling factor.
	double GetScalingFactor() {return mScalingFactor;}
	//! Returns the horizontal flip flag.
	bool GetFlipHorizontal() {return mFlipHorizontal;}
	//! Returns the vertical flip flag.
	bool GetFlipVertical() {return mFlipVertical;}
	//! Returns the crop rectangle.
	CvRect GetCropRect() {return mCropRect;}

	//! Creates (if necessary) and returns the image.
	IplImage *GetImage();
	//! Returns the total size of the rendered image.
	CvSize GetSize();

private:
	Display mDisplay;		//!< The attached Display.
	IplImage *mImage;		//!< The cached image.

	CvFont mFontMain;		//!< The main font used to annotate the image.

	// View properties
	double mScalingFactor;		//!< The scaling factor.
	CvRect mCropRectangle;		//!< The rectangle selecting the area to draw (TODO not implemented).
	bool mViewFlipVertically;	//!< Whether the image should be flipped vertically.
	bool mViewFlipHorizontally;	//!< Whether the image should be flipped horizontally.

	// Drawing layer properties
	bool mDrawImage;		//!< Whether to draw the main image.
	bool mDrawMask;			//!< Whether to draw the mask.
	bool mDrawParticles;	//!< Whether to draw the particles.

	//! Deletes the cached image. This forces the image to be recreated on the next request.
	void DeleteCache();

	//! Draws the main image layer.
	bool DrawMainImage();
	//! Draws the mask image layer.
	bool DrawMaskImage();
	//! Draws the particle layer.
	bool DrawParticles();
	//! Draws the errors.
	bool DrawErrors();

};

#endif