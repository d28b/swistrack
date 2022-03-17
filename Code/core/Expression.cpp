#include "Expression.h"
#include "ExpressionNode.h"
#include "ExpressionParser.h"
#include <cstddef>
#include <cmath>
#define THISCLASS Context

namespace Expression {
THISCLASS::Context(): nodes(), nodesByLabel() { }

THISCLASS::~Context() {
	for (auto node : nodes)
		delete node;
}

int THISCLASS::NextRevision() {
	revision += 1;
	return revision;
}

NodeWithNumber * THISCLASS::AddNumber(std::string label, double value) {
	NodeWithNumber * node = new NodeWithNumber();
	node->context = this;
	node->Set(value);
	nodes.push_back(node);
	nodesByLabel[label] = node;
	return node;
}

NodeWithMatrix * THISCLASS::AddMatrix(std::string label, cv::Mat value) {
	NodeWithMatrix * node = new NodeWithMatrix();
	node->context = this;
	node->Set(value);
	nodes.push_back(node);
	nodesByLabel[label] = node;
	return node;
}

std::string THISCLASS::Parse(std::string expression) {
	Parser parser(this, expression);
	return parser.error;
}

Node * THISCLASS::GetNode(std::string label) {
	return nodesByLabel[label];
}
}
