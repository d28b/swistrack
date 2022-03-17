#include "ExpressionNode.h"
#include "Expression.h"
#include <iostream>

namespace Expression {
Node::Node(): resultType(NUMBER), resultNumber(0), resultMatrix(), revision(0) { }

double Node::GetNumber() {
	if (resultType == NUMBER) return resultNumber;
	return 0;
}

cv::Mat Node::GetMatrix() {
	if (resultType != MATRIX) return cv::Mat();
	if (resultMatrix.depth() == CV_8U) return resultMatrix;

	cv::Mat converted;
	resultMatrix.convertTo(converted, CV_8UC1);
	return converted;
}

cv::Mat Node::GetFloatMatrix() {
	if (resultType != MATRIX) return cv::Mat();
	if (resultMatrix.depth() == CV_32F) return resultMatrix;

	cv::Mat converted;
	resultMatrix.convertTo(converted, CV_32FC1);
	return converted;
}

std::string Node::PrepareCommon(Node * node) {
	std::string error = node->Prepare();
	if (! error.empty()) return error;
	resultType = node->resultType;
	return "";
}

std::string Node::PrepareCommon(Node * a, Node * b) {
	std::string errorA = a->Prepare();
	if (! errorA.empty()) return errorA;
	std::string errorB = b->Prepare();
	if (! errorB.empty()) return errorB;
	resultType = a->resultType == MATRIX || b->resultType == MATRIX ? MATRIX : NUMBER;
	return "";
}

bool Node::CheckDependencies(Node * node) {
	node->Execute();
	if (node->revision <= revision) return false;
	revision = node->revision;
	return true;
}

bool Node::CheckDependencies(Node * a, Node * b) {
	a->Execute();
	b->Execute();
	int newRevision = std::max(a->revision, b->revision);
	if (newRevision <= revision) return false;
	revision = newRevision;
	return true;
}

void NodeWithNumber::Set(double value) {
	resultNumber = value;
	revision = context->NextRevision();
}

std::string NodeWithNumber::Prepare() {
	resultType = NUMBER;
	return "";
}

void NodeWithNumber::ToString(std::stringstream & ss) const {
	ss << resultNumber;
}

void NodeWithNumber::Execute() {
}

void NodeWithMatrix::Set(cv::Mat value) {
	resultMatrix = value;
	revision = context->NextRevision();
}

std::string NodeWithMatrix::Prepare() {
	resultType = MATRIX;
	return "";
}

void NodeWithMatrix::ToString(std::stringstream & ss) const {
	ss << "MATRIX";
}

void NodeWithMatrix::Execute() {
}

NodeFloatOperation::NodeFloatOperation(std::string symbol): Node(), symbol(symbol) { }

std::string NodeFloatOperation::Prepare() {
	return PrepareCommon(left, right);
}

void NodeFloatOperation::ToString(std::stringstream & ss) const {
	ss << "(";
	left->ToString(ss);
	ss << symbol;
	right->ToString(ss);
	ss << ")";
}

void NodeFloatOperation::Execute() {
	if (! CheckDependencies(left, right)) return;

	if (left->resultType == MATRIX && right->resultType == MATRIX)
		resultMatrix = Calculate(left->GetFloatMatrix(), right->GetFloatMatrix());
	else if (left->resultType == MATRIX)
		resultMatrix = Calculate(left->GetFloatMatrix(), right->resultNumber);
	else if (right->resultType == MATRIX)
		resultMatrix = Calculate(left->resultNumber, right->GetFloatMatrix());
	else
		resultNumber = Calculate(left->resultNumber, right->resultNumber);
}

NodeBitOperation::NodeBitOperation(std::string symbol): Node(), symbol(symbol) { }

std::string NodeBitOperation::Prepare() {
	return PrepareCommon(left, right);
}

void NodeBitOperation::ToString(std::stringstream & ss) const {
	ss << "(";
	left->ToString(ss);
	ss << symbol;
	right->ToString(ss);
	ss << ")";
}

void NodeBitOperation::Execute() {
	if (! CheckDependencies(left, right)) return;

	if (left->resultType == MATRIX && right->resultType == MATRIX)
		resultMatrix = Calculate(left->GetMatrix(), right->GetMatrix());
	else if (left->resultType == MATRIX)
		resultMatrix = Calculate(left->GetMatrix(), (unsigned char) right->resultNumber);
	else if (right->resultType == MATRIX)
		resultMatrix = Calculate((int) left->resultNumber, right->GetMatrix());
	else
		resultNumber = (double) Calculate((unsigned char) left->resultNumber, (unsigned char) right->resultNumber);
}

NodeFunction1::NodeFunction1(std::string name): Node(), name(name) { }

std::string NodeFunction1::Prepare() {
	return PrepareCommon(value);
}

void NodeFunction1::ToString(std::stringstream & ss) const {
	ss << name << "(";
	value->ToString(ss);
	ss << ")";
}

void NodeFunction1::Execute() {
	if (! CheckDependencies(value)) return;

	if (value->resultType == MATRIX) {
		cv::Mat inputMatrix = value->GetFloatMatrix();
		resultMatrix.create(inputMatrix.size(), CV_32FC1);
		Calculate(inputMatrix, resultMatrix);
	} else {
		resultNumber = Calculate(value->resultNumber);
	}
}

NodeReduce1::NodeReduce1(std::string name): Node(), name(name) { }

std::string NodeReduce1::Prepare() {
	std::string error = value->Prepare();
	if (! error.empty()) return error;
	resultType = NUMBER;
	return "";
}

void NodeReduce1::ToString(std::stringstream & ss) const {
	ss << name << "(";
	value->ToString(ss);
	ss << ") <" << resultNumber << ">";
}


void NodeReduce1::Execute() {
	if (! CheckDependencies(value)) return;

	if (value->resultType == MATRIX)
		resultNumber = Calculate(value->GetFloatMatrix());
	else
		resultNumber = Calculate(value->resultNumber);
}

std::ostream & operator<<(std::ostream & os, const Node & node) {
	std::stringstream stream;
	node.ToString(stream);
	os << stream.str();
	return os;
}
}
