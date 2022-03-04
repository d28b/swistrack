#ifndef HEADER_ComponentChannelArithmetic
#define HEADER_ComponentChannelArithmetic

#include <opencv2/core.hpp>
#include "Component.h"
#include "Expression.h"

//! A component subtracting a fixed background from a color image.
class ComponentChannelArithmetic: public Component {

public:
	//! Constructor.
	ComponentChannelArithmetic(SwisTrackCore * stc);
	//! Destructor.
	~ComponentChannelArithmetic();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentChannelArithmetic(mCore);
	}

private:
	Expression * mExpression;			//!< (configuration) Expression.
	Expression::Node * mRed;
	Expression::Node * mGreen;
	Expression::Node * mBlue;
	Expression::Node * mResult;
	wxString mError;
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

