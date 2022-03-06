#ifndef HEADER_ComponentChannelCalculationGray
#define HEADER_ComponentChannelCalculationGray

#include <opencv2/core.hpp>
#include "Component.h"
#include "Expression.h"

//! A component subtracting a fixed background from a color image.
class ComponentChannelCalculationGray: public Component {

public:
	//! Constructor.
	ComponentChannelCalculationGray(SwisTrackCore * stc);
	//! Destructor.
	~ComponentChannelCalculationGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentChannelCalculationGray(mCore);
	}

private:
	Expression::Context * mContext;		//!< Expression context.
	Expression::NodeWithMatrix * mZeros;
	Expression::NodeWithMatrix * mOnes;
	Expression::NodeWithMatrix * mBlack;
	Expression::NodeWithMatrix * mWhite;
	Expression::NodeWithMatrix * mInput;
	Expression::Node * mOutput;
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

