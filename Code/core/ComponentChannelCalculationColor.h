#ifndef HEADER_ComponentChannelCalculationColor
#define HEADER_ComponentChannelCalculationColor

#include <opencv2/core.hpp>
#include "Component.h"
#include "Expression.h"

//! A component subtracting a fixed background from a color image.
class ComponentChannelCalculationColor: public Component {

public:
	//! Constructor.
	ComponentChannelCalculationColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentChannelCalculationColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentChannelCalculationColor(mCore);
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
	Expression::Node * mOutputRed;
	Expression::Node * mOutputGreen;
	Expression::Node * mOutputBlue;
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

