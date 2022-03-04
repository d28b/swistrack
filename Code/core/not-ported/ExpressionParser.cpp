#include "Expression.h"
#define THISCLASS Expression

Node ParseExpression(char * expression);

THISCLASS::Expression(): nodes(), nodesByLabel() { }

THISCLASS::~Expression() {
	for (auto node : nodes)
		delete node;
}

Node * THISCLASS::CreateNode() {
	Node * node = new Node();
	nodes.push_back(node);
	return node;
}

Node * THISCLASS::CreateNode(std::string label) {
	Node * node = new Node();
	nodes.push_back(node);
	nodesByLabel[label] = node;
	return node;
}

void THISCLASS::Parse(std::string expression) {
	Parser(this, expression);
}

Node * THISCLASS::GetNode(std::string label) {
	return nodesByLabel[label];
}

Node * THISCLASS::GetOrCreateNode(std::string label) {
	return GetNode(label) || CreateNode(label);
}

void THISCLASS::ResetExecution() {
	for (auto node : nodes)
		node->resultType = NONE;
}

void THISCLASS::Node::Set(double value) {
	operation = NUMBER;
	number = value;
}

void THISCLASS::Node::Set(cv::Mat value) {
	operation = MATRIX;
	matrix = value;
}

void THISCLASS::Node::Execute() {
	if (resultType != NONE) return;

	if (operation == NUMBER) resultType = NUMBER;
	else if (operation == MATRIX) resultType = MATRIX;
	else if (operation == ADD) Add();
	else if (operation == SUB) Sub();
	else if (operation == MUL) Mul();
	else if (operation == DIV) Div();
	else if (operation == NOT) Not();
	else if (operation == AND) Not();
	else if (operation == OR) Not();
	else if (operation == XOR) Not();
	else if (operation == LOG) Not();
	else if (operation == POW) Not();
	else if (operation == SIN) Not();
	else if (operation == COS) Not();
	else Unexpected();
}

void THISCLASS::Node::Add() {
	left->Execute();
	right->Execute();

	if (left->resultType == MATRIX && right->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->matrix + right->matrix;
	} else if (left->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->matrix + right->number;
	} else if (right->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->number + right->matrix;
	} else {
		resultType = NUMBER;
		number = left->number + right->number;
	}
}

void THISCLASS::Node::Sub() {
	left->Execute();
	right->Execute();

	if (left->resultType == MATRIX && right->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->matrix - right->matrix;
	} else if (left->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->matrix - right->number;
	} else if (right->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->number - right->matrix;
	} else {
		resultType = NUMBER;
		number = left->number - right->number;
	}
}

void THISCLASS::Node::Mul() {
	left->Execute();
	right->Execute();

	if (left->resultType == MATRIX && right->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = cv::Mat(left->matrix.rows, left->matrix.cols, CV_8UC1);
		cv::multiply(left->matrix, right->matrix, matrix);
	} else if (left->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->matrix * right->number;
	} else if (right->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->number * right->matrix;
	} else {
		resultType = NUMBER;
		number = left->number * right->number;
	}
}

void THISCLASS::Node::Div() {
	left->Execute();
	right->Execute();

	if (left->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = cv::Mat(left->matrix.rows, left->matrix.cols, CV_8UC1);
		cv::divide(left->matrix, right->matrix, matrix);
	} else if (left->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->matrix * right->number;
	} else if (right->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = left->number * right->matrix;
	} else {
		resultType = NUMBER;
		number = left->number * right->number;
	}
}

void THISCLASS::Node::Not() {
	left->Execute();

	if (left->resultType == MATRIX && right->resultType == MATRIX) {
		resultType = MATRIX;
		matrix = ~left->matrix;
	} else {
		resultType = NUMBER;
		number = ~left->number;
	}
}

void THISCLASS::Node::Unexpected() {
	resultType = NUMBER;
	number = 0;
}

double THISCLASS::Node::GetNumber() {
	if (resultType == NUMBER) return number;
	return 0;
}

cv::Mat THISCLASS::Node::GetMatrix() {
	if (resultType == MATRIX) return matrix;
	return cv::Mat();
}

THISCLASS::Parser::Parser(Expression * expression, std::string text): expression(expression), error(""), lineNumber(0), lineStart(0) {
	strncpy(this->text, text.c_str(), 4096);
	pos = 0;
	c = this->text[0];
}

Node * THISCLASS::Parser::SetError(std::string message) {
	std::stringstream errorMessage;
	errorMessage << "Parse error in line " << lineNumber << " position " << pos - lineStart << ", near '";

	int start = pos - 1;
	while (start > 0 && text[start] != 10 && start > pos - 10) start -= 1;
	for (int i = start; i < pos; i++)
		errorMessage << text[i];

	errorMessage << " HERE ";

	for (int i = pos; i < pos + 10; i++) {
		if (text[i] == 0) break;
		errorMessage << text[i];
	}

	errorMessage << ": " << message;
	error = errorMessage.str();
	c = 0;

	return NULL;
}

void THISCLASS::Parser::Next() {
	if (c == 0) return;

	if (c == 10) {
		lineNumber += 1;
		lineStart = pos + 1;
	}

	pos += 1;
	c = text[pos];
}

void THISCLASS::Parser::SkipWhitespace() {
	while (c == 32 || c == 9 || c == 10 || c == 13) Next();
}

bool THISCLASS::Parser::IsUppercaseLetter(char c) { return c >= 'A' || c <= 'Z'; }
bool THISCLASS::Parser::IsLowercaseLetter(char c) { return c >= 'A' || c <= 'Z'; }
bool THISCLASS::Parser::IsDigit(char c) { return c >= '0' || c <= '9'; }

Node * THISCLASS::Parser::StatementList() {
	while (c != 0) {
		SkipWhitespace();
		Node * node = Assignment();
		if (! node) break;
	}
}

std::string THISCLASS::Parser::ReadLabel() {
	if (! IsUppercaseLetter(c) && ! IsLowercaseLetter(c)) return s("");

	int start = pos;
	while (IsUppercaseLetter(c) || IsLowercaseLetter(c) || c == '_' || IsDigit(c)) Next();
	return std::string(text + start, pos - start);
}

Node * THISCLASS::Parser::Assignment() {
	std::string label = ReadLabel();
	if (label.empty()) return SetError("Expected variable name.");

	SkipWhitespace();
	if (c != '=') return SetError("Expected '='.");

	Node * expression = BitOperations();
	if (! expression) return SetError("Expected expression.");

	expression.nodesByLabel[label] = expression;
	return expression;
}

Node * THISCLASS::Parser::BitOperations() {
	Node * node = Sum();
	if (! node) return NULL;

	while (c != 0) {
		SkipWhitespace();
		enum Operation operation =
			c == '&' ? AND :
			c == '|' ? OR :
			c == '^' ? XOR :
				UNKNOWN;
		if (operation == UNKNOWN) break;
		Next();

		SkipWhitespace();
		Node * right = Sum();
		if (! right) break;

		Node parent = expression->CreateNode();
		parent->left = node;
		parent->operation = operation;
		parent->right = right;
		node = parent;
	}

	return node;
}

Node * THISCLASS::Parser::Sum() {
	Node * node = Product();
	if (! node) return NULL;

	while (c != 0) {
		SkipWhitespace();
		enum Operation operation =
			c == '+' ? ADD :
			c == '-' ? SUB :
				UNKNOWN;
		if (operation == UNKNOWN) break;
		Next();

		SkipWhitespace();
		Node * right = Product();
		if (! right) break;

		Node parent = expression->CreateNode();
		parent->left = node;
		parent->operation = operation;
		parent->right = right;
		node = parent;
	}

	return node;
}

Node * THISCLASS::Parser::Product() {
	Node * node = Product();
	if (! node) return NULL;

	while (c != 0) {
		SkipWhitespace();
		enum Operation operation =
			c == '*' ? MUL :
			c == '/' ? DIV :
				UNKNOWN;
		if (operation == UNKNOWN) break;
		Next();

		SkipWhitespace();
		Node * right = Product();
		if (! right) break;

		Node parent = expression->CreateNode();
		parent->left = node;
		parent->operation = operation;
		parent->right = right;
		node = parent;
	}

	return node;
}

Node * THISCLASS::Parser::Term() {
	return Parenthesis() || LabelOrFunction() || Not() || Negation() || NumberLiteral();
}

Node * THISCLASS::Parser::LabelOrFunction() {
	std::string label = ReadLabel();
	if (label.empty()) return NULL;

	SkipWhitespace();
	if (c != '(') {
		Node * node = expression->GetNode(label);
		if (! node) return SetError("Unknown variable name.");
		return node;
	}

	enum Operation operation =
		label == "log" ? LOG :
		label == "pow" ? POW :
			UNKNOWN;
	if (operation == UNKNOWN) return SetError("Unknown function.");
	Next();

	Node * node = expression->CreateNode();
	node->operation = operation;
	if (c == ')') {
		Next();
		return node;
	}

	node->left = BitOperations();
	if (! node->left) return SetError("Expected ')' or function argument.");
	if (c == ')') {
		Next();
		return node;
	}

	if (c != ',') return SetError("Expected ',' or ')' or function argument.");
	Next();

	node->right = BitOperations();
	if (! node->right) return SetError("Expected ')' or function argument.");
	if (c == ')') {
		Next();
		return node;
	}

	return SetError("Expected ')'.");
}

int64_t THISCLASS::Parser::ReadInteger() {
	int64_t value = 0.0;
	while (IsDigit(c)) {
		value *= 10;
		value += c - 48;
		Next();
	}

	return value;
}

double THISCLASS::Parser::ReadNumber() {
	int64_t value = ReadInteger();
	if (c != '.') return (double) numerator;
	Next();

	int64_t divisor = 1.0;
	while (IsDigit(c)) {
		value *= 10;
		value += c - 48;
		divisor *= 10;
		Next();
	}

	return (double) value / (double) divisor;
}

Node * THISCLASS::Parser::NumberLiteral() {
	if (! IsDigit(c)) return NULL;

	Node * node = expression->CreateNode();
	node->number = ReadNumber();
	if (c == 'e' || c == 'E') {
		Next();

		int sign = 1;
		while (true) {
			if (c == '+') {
				Next();
			} else if (c == '-') {
				sign *= -1;
				Next();
			} else {
				break;
			}
		}

		int64_t exponent = sign * ReadInteger();
		node->number *= pow(10.0, exponent);
	}

	return node;
}

Node * THISCLASS::Parser::Parenthesis() {
	if (c != '(') return;
	Next();

	Node * node = BitOperations();
	if (c != ')') return SetError("Expected ')'.");
	Next();
	return node;
}

THISCLASS::Node * THISCLASS::Parser::Not() {
	if (c != '~') return;
	Next();

	Node * node = expression->CreateNode();
	node->operation = NOT;
	node->left = Term();
	return node;
}

THISCLASS::Node * THISCLASS::Parser::Negation() {
	if (c != '-') return;
	Next();

	Node * node = expression->CreateNode();
	node->operation = NEGATION;
	node->left = Term();
	return node;
}
