#ifndef HEADER_Expression
#define HEADER_Expression

#include <opencv2/core.hpp>
#include <iostream>
#include <map>

#include "ExpressionNode.h"

namespace Expression {
	class Parser;

	class Context {
	public:
		Context();
		~Context();

	private:
		int revision = 0;
		std::vector<Node *> nodes;
		std::map<std::string, Node *> nodesByLabel;

		int NextRevision();

	public:
		NodeWithNumber * AddNumber(std::string label, double value);
		NodeWithMatrix * AddMatrix(std::string label, cv::Mat value);
		std::string Parse(std::string expression);
		Node * GetNode(std::string label);

		friend class Parser;
		friend class NodeWithNumber;
		friend class NodeWithMatrix;
	};
}

#endif
