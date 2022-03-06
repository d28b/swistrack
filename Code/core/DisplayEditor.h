#ifndef HEADER_DisplayEditor
#define HEADER_DisplayEditor

class DisplayEditor;

#include "Display.h"
#include "DataStructureParticles.h"

//! This class is used by the components to set the contents of a Display object.
class DisplayEditor {

public:
	//! The attached Display. If this is 0, the construction is invalid (or not necessary) and all setter methods will just return without doing anything.
	Display * mDisplay;

	//! Constructor.
	DisplayEditor(Display * display = 0);
	//! Destructor.
	~DisplayEditor();

	//! Returns if the display is active.
	bool IsActive() {
		return mDisplay != 0;
	}

	//! Sets the main image.
	void SetMainImage(cv::Mat img);
	//! Sets the mask.
	void SetMaskImage(cv::Mat img);

	//! Sets the particles.
	void SetParticles(DataStructureParticles::tParticleVector * pv);
	//! Sets whether to draw the trajectories.
	void SetTrajectories(bool trajectories);

	//! Sets the size of the display.
	void SetSize(cv::Size size);
	//! Tries to determine the size automatically.
	bool SetSizeAuto();
	//! Adds an error message.
	void AddError(const wxString & message);

	void SetArea(cv::Point2f topleft, cv::Point2f bottomright) {}
	void SetArea(double x1, double y1, double x2, double y2) {}

};

#endif
