#include <opencv2/core.hpp>
#include <iostream>

#include "Expression.h"
#include "ExpressionNode.h"
#include "ExpressionParser.h"

cv::Mat filled(int value) {
	cv::Mat m = cv::Mat::zeros(2, 2, CV_8UC1);
	return m + value;
}

int main(int argc, char * argv[]) {
	Expression::Context context;
	context.AddNumber("pi", M_PI);
	context.AddNumber("e", M_E);

	Expression::NodeWithMatrix * zeros = context.AddMatrix("zeros", cv::Mat());
	Expression::NodeWithMatrix * ones = context.AddMatrix("ones", cv::Mat());
	Expression::NodeWithMatrix * red = context.AddMatrix("R", cv::Mat());
	Expression::NodeWithMatrix * green = context.AddMatrix("G", cv::Mat());
	Expression::NodeWithMatrix * blue = context.AddMatrix("B", cv::Mat());

	std::string syntaxError = context.Parse("x = R + B    y = R + G     out1 = x + y - 3 * 2   out2 = ");
	if (! syntaxError.empty()) {
		std::cout << "Syntax error: " << syntaxError << std::endl;
		return 1;
	}

	Expression::Node * out1 = context.GetNode("out1");
	if (! out1) {
		std::cout << "Assign the resulting image using 'out1 = ...'." << std::endl;
		return 1;
	}

	Expression::Node * out2 = context.GetNode("out2");
	if (! out2) {
		out2 = blue;
		//std::cout << "Assign the resulting image using 'out2 = ...'." << std::endl;
		//return 1;
	}

	std::string typeError1 = out1->Prepare();
	if (! typeError1.empty()) {
		std::cout << "Type error: " << typeError1 << std::endl;
		return 1;
	} else if (out1->resultType != Expression::Node::MATRIX) {
		std::cout << "The result must be an image, not a scalar value." << std::endl;
		return 1;
	}

	std::string typeError2 = out2->Prepare();
	if (! typeError2.empty()) {
		std::cout << "Type error: " << typeError2 << std::endl;
		return 1;
	} else if (out2->resultType != Expression::Node::MATRIX) {
		std::cout << "The result must be an image, not a scalar value." << std::endl;
		return 1;
	}

	zeros->Set(filled(0));
	ones->Set(filled(1));
	red->Set(filled(24));
	green->Set(filled(50));
	blue->Set(filled(52));

	out1->Execute();
	out2->Execute();
	std::cout << *out1 << std::endl;
	std::cout << *out2 << std::endl;
	std::cout << " = " << out1->GetMatrix() << std::endl;

	red->Set(filled(2));
	out1->Execute();
	out2->Execute();
	std::cout << " = " << out2->GetMatrix() << std::endl;

	return 0;
}
