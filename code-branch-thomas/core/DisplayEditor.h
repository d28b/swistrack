#ifndef HEADER_DisplayEditor
#define HEADER_DisplayEditor

class DisplayEditor;

//! This class is used by the components to set the contents of a Display object.
class DisplayEditor {

public:
	//! The attached Display. If this is 0, the construction is invalid (or not necessary) and all setter methods will just return without doing anything.
	Display mDisplay;

	//! Constructor.
	DisplayEditor(Display *display=0);
	//! Destructor.
	~DisplayEditor();

	//! Sets the main image.
	void SetImage(IplImage *img);
	//! Sets the mask.
	void SetMask(IplImage *img);

	//! Sets the particles.
	void SetParticles(DataStructureParticles::ParticleVector *pv);

	//! Sets the size of the display.
	void SetSize(CvSize size);
	//! Tries to determine the size automatically.
	bool SetSizeAuto();

};

#endif