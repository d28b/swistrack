#ifndef HEADER_DisplayRenderer
#define HEADER_DisplayRenderer

class DisplayRenderer;

//! This class is used by the components to set the contents of a Display object.
class DisplayRenderer {

public:
	//! The attached Display. If this is 0, the construction is invalid (or not necessary) and all setter methods will just return without doing anything.
	Display mDisplay;

	//! Constructor.
	DisplayRenderer(Display *display=0);
	//! Destructor.
	~DisplayRenderer();

	//! Sets the main image.
	void SetImage(IplImage *img);
	//! Sets the mask.
	void SetMask(IplImage *img);

	//! Sets the particles.
	void SetParticles(DataStructureParticles::ParticleVector *pv);

};

#endif