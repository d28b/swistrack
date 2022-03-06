#ifndef HEADER_ComponentChannelCalculationColorToGray
#define HEADER_ComponentChannelCalculationColorToGray

#include <opencv2/core.hpp>
#include "Component.h"
#include "Expression.h"

//! A component subtracting a fixed background from a color image.
class ComponentChannelCalculationColorToGray: public Component {

public:
	//! Constructor.
	ComponentChannelCalculationColorToGray(SwisTrackCore * stc);
	//! Destructor.
	~ComponentChannelCalculationColorToGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentChannelCalculationColorToGray(mCore);
	}

private:
	Expression::Context * mContext;		//!< Expression context.
	Expression::NodeWithMatrix * mZeros;
	Expression::NodeWithMatrix * mOnes;
	Expression::NodeWithMatrix * mBlack;
	Expression::NodeWithMatrix * mWhite;
	Expression::NodeWithMatrix * mRed;
	Expression::NodeWithMatrix * mGreen;
	Expression::NodeWithMatrix * mBlue;
	Expression::Node * mOutput;
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

