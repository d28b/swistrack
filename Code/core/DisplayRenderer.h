#ifndef HEADER_DisplayRenderer
#define HEADER_DisplayRenderer

class DisplayRenderer;

#include "Display.h"
#include "ErrorList.h"

//! This class renders a display image.
class DisplayRenderer {

public:
	//! Constructor.
	DisplayRenderer(Display * display = 0);
	//! Destructor.
	~DisplayRenderer();

	//! Sets the display.
	void SetDisplay(Display *display);
	//! Sets a new scaling factor.
	void SetScalingFactor(double scalingfactor);
	//! Sets the scaling factor to the maximal possible value to fit the image within maxwidth/maxheight.
	void SetScalingFactorMax(cv::Size maxsize);
	//! Sets the horizontal flip flag.
	void SetFlipHorizontal(bool flip);
	//! Sets the vertical flip flag.
	void SetFlipVertical(bool flip);
	//! Sets the crop rectangle (the area of interest). This is currently not implemented.
	void SetCropRectangle(cv::Rect rect);

	//! Returns the display.
	Display *GetDisplay() {
		return mDisplay;
	}
	//! Returns the scaling factor.
	double GetScalingFactor() {
		return mScalingFactor;
	}
	//! Returns the horizontal flip flag.
	bool GetFlipHorizontal() {
		return mFlipHorizontal;
	}
	//! Returns the vertical flip flag.
	bool GetFlipVertical() {
		return mFlipVertical;
	}
	//! Returns the crop rectangle.
	cv::Rect GetCropRectangle() {
		return mCropRectangle;
	}

	//! Deletes the cached image. This forces the image to be recreated on the next request.
	void DeleteCache();
	//! Creates (if necessary) and returns the image.
	cv::Mat GetImage();
	//! Returns the total size of the rendered image.
	cv::Size GetSize();

private:
	Display * mDisplay;			//!< The attached Display.
	cv::Mat mImage;				//!< The cached image.

	int mFontFace;				//!< The main font used to annotate the image.
	double mFontSize;			//!< The base font size.
	double mFontSizeBig;		//!< The big font size.

	// View properties
	double mScalingFactor;		//!< The scaling factor.
	cv::Rect mCropRectangle;	//!< The rectangle selecting the area to draw (TODO not implemented).
	bool mFlipVertical;			//!< Whether the image should be flipped vertically.
	bool mFlipHorizontal;		//!< Whether the image should be flipped horizontally.

	// Drawing layer properties
	bool mDrawImage;			//!< Whether to draw the main image.
	bool mDrawParticles;		//!< Whether to draw the particles.
	bool mDrawTrajectories; 	//!< Whether to draw the trajectories.
	bool mDrawErrors;			//!< Whether to draw the error messages.
	bool mUseMask;				//!< Whether to use the mask.

	//! Draws the main image layer.
	bool DrawMainImage(ErrorList *errors);
	//! Draws the particle layer.
	bool DrawParticles(ErrorList *errors);
	//! Draws the trajectory layer.
	bool DrawTrajectories(ErrorList *errors);
	//! Draws the errors.
	bool DrawErrors(ErrorList *errors);
	//! Draws an image with a message in the center.
	bool DrawMessagePanel(wxString errstr = wxT(""));

};

#endif
