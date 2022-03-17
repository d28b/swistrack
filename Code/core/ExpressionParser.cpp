#include "ExpressionParser.h"
#include "CompareWorkaround.h"
#include <iostream>
#define THISCLASS Parser

namespace Expression {
class NodeNegation: public Node {
public:
	Node * value;

	std::string Prepare() {
		return PrepareCommon(value);
	}

	void ToString(std::stringstream & ss) const {
		ss << "-";
		value->ToString(ss);
	}

	void Execute() {
		if (! CheckDependencies(value)) return;

		if (value->resultType == MATRIX)
			resultMatrix = -value->GetFloatMatrix();
		else
			resultNumber = -value->resultNumber;
	}
};

class NodeAdd: public NodeFloatOperation {
public:
	NodeAdd(): NodeFloatOperation(" + ") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return left + right;
	}
	cv::Mat Calculate(cv::Mat left, double right) {
		return left + right;
	}
	cv::Mat Calculate(double left, cv::Mat right) {
		return left + right;
	}
	double Calculate(double left, double right) {
		return left + right;
	}
};

class NodeSub: public NodeFloatOperation {
public:
	NodeSub(): NodeFloatOperation(" - ") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return left - right;
	}
	cv::Mat Calculate(cv::Mat left, double right) {
		return left - right;
	}
	cv::Mat Calculate(double left, cv::Mat right) {
		return left - right;
	}
	double Calculate(double left, double right) {
		return left - right;
	}
};

class NodeMul: public NodeFloatOperation {
public:
	NodeMul(): NodeFloatOperation(" * ") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return left * right;
	}
	cv::Mat Calculate(cv::Mat left, double right) {
		return left * right;
	}
	cv::Mat Calculate(double left, cv::Mat right) {
		return left * right;
	}
	double Calculate(double left, double right) {
		return left * right;
	}
};

class NodeDiv: public NodeFloatOperation {
public:
	NodeDiv(): NodeFloatOperation(" / ") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return left / right;
	}
	cv::Mat Calculate(cv::Mat left, double right) {
		return left / right;
	}
	cv::Mat Calculate(double left, cv::Mat right) {
		return left / right;
	}
	double Calculate(double left, double right) {
		return left / right;
	}
};

class NodeSmaller: public NodeFloatOperation {
public:
	NodeSmaller(): NodeFloatOperation(" < ") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return smallerThan(left, right);
	}
	cv::Mat Calculate(cv::Mat left, double right) {
		return smallerThan(left, right);
	}
	cv::Mat Calculate(double left, cv::Mat right) {
		return smallerThan(left, right);
	}
	double Calculate(double left, double right) {
		return smallerThan(left, right);
	}
};

class NodeBigger: public NodeFloatOperation {
public:
	NodeBigger(): NodeFloatOperation(" > ") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return biggerThan(left, right);
	}
	cv::Mat Calculate(cv::Mat left, double right) {
		return biggerThan(left, right);
	}
	cv::Mat Calculate(double left, cv::Mat right) {
		return biggerThan(left, right);
	}
	double Calculate(double left, double right) {
		return biggerThan(left, right);
	}
};

class NodeMin: public NodeFloatOperation {
public:
	NodeMin(): NodeFloatOperation("min") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return cv::min(left, right);
	}
	cv::Mat Calculate(cv::Mat left, double right) {
		return cv::min(left, right);
	}
	cv::Mat Calculate(double left, cv::Mat right) {
		return cv::min(left, right);
	}
	double Calculate(double left, double right) {
		return left < right ? left : right;
	}
};

class NodeMax: public NodeFloatOperation {
public:
	NodeMax(): NodeFloatOperation("max") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return cv::max(left, right);
	}
	cv::Mat Calculate(cv::Mat left, double right) {
		return cv::max(left, right);
	}
	cv::Mat Calculate(double left, cv::Mat right) {
		return cv::max(left, right);
	}
	double Calculate(double left, double right) {
		return left > right ? left : right;
	}
};

class NodeNot: public Node {
public:
	Node * value;

	std::string Prepare() {
		return PrepareCommon(value);
	}

	void ToString(std::stringstream & ss) const {
		ss << "~";
		value->ToString(ss);
	}

	void Execute() {
		if (! CheckDependencies(value)) return;

		if (value->resultType == MATRIX)
			resultMatrix = ~value->GetMatrix();
		else
			resultNumber = (double) (~(unsigned char) value->resultNumber);
	}
};

class NodeAnd: public NodeBitOperation {
public:
	NodeAnd(): NodeBitOperation(" & ") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return left & right;
	}
	cv::Mat Calculate(cv::Mat left, unsigned char right) {
		return left & right;
	}
	cv::Mat Calculate(unsigned char left, cv::Mat right) {
		return left & right;
	}
	int Calculate(unsigned char left, unsigned char right) {
		return left & right;
	}
};

class NodeOr: public NodeBitOperation {
public:
	NodeOr(): NodeBitOperation(" | ") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return left | right;
	}
	cv::Mat Calculate(cv::Mat left, unsigned char right) {
		return left | right;
	}
	cv::Mat Calculate(unsigned char left, cv::Mat right) {
		return left | right;
	}
	int Calculate(unsigned char left, unsigned char right) {
		return left | right;
	}
};

class NodeXor: public NodeBitOperation {
public:
	NodeXor(): NodeBitOperation(" ^ ") { }

protected:
	cv::Mat Calculate(cv::Mat left, cv::Mat right) {
		return left ^ right;
	}
	cv::Mat Calculate(cv::Mat left, unsigned char right) {
		return left ^ right;
	}
	cv::Mat Calculate(unsigned char left, cv::Mat right) {
		return left ^ right;
	}
	int Calculate(unsigned char left, unsigned char right) {
		return left ^ right;
	}
};

class NodeSqrt: public NodeFunction1 {
public:
	NodeSqrt(): NodeFunction1("sqrt") { }

protected:
	void Calculate(cv::Mat value, cv::Mat & result) {
		cv::sqrt(value, result);
	}
	double Calculate(double value) {
		return sqrt(value);
	}
};

class NodeLog: public NodeFunction1 {
public:
	NodeLog(): NodeFunction1("log") { }

protected:
	void Calculate(cv::Mat value, cv::Mat & result) {
		cv::log(value, result);
	}
	double Calculate(double value) {
		return log(value);
	}
};

class NodeExp: public NodeFunction1 {
public:
	NodeExp(): NodeFunction1("exp") { }

protected:
	void Calculate(cv::Mat value, cv::Mat & result) {
		cv::exp(value, result);
	}
	double Calculate(double value) {
		return exp(value);
	}
};

class NodeSum: public NodeReduce1 {
public:
	NodeSum(): NodeReduce1("sum") { }

protected:
	double Calculate(cv::Mat value) {
		return cv::sum(value).val[0];
	}
	double Calculate(double value) {
		return value;
	}
};

class NodeMean: public NodeReduce1 {
public:
	NodeMean(): NodeReduce1("mean") { }

protected:
	double Calculate(cv::Mat value) {
		return cv::mean(value).val[0];
	}
	double Calculate(double value) {
		return value;
	}
};

class NodePow: public Node {
public:
	Node * base;
	Node * exponent;

	std::string Prepare() {
		std::string error1 = base->Prepare();
		if (! error1.empty()) return error1;
		std::string error2 = exponent->Prepare();
		if (! error2.empty()) return error2;
		if (exponent->resultType == MATRIX) return "The exponent in pow(...) must be a number.";
		resultType = base->resultType;
		return "";
	}

	void ToString(std::stringstream & ss) const {
		ss << "pow" << "(";
		base->ToString(ss);
		ss << ", ";
		exponent->ToString(ss);
		ss << ")";
	}

	void Execute() {
		if (! CheckDependencies(base, exponent)) return;

		if (base->resultType == MATRIX) {
			cv::Mat inputMatrix = base->GetFloatMatrix();
			resultMatrix.create(inputMatrix.size(), CV_32FC1);
			cv::pow(inputMatrix, exponent->resultNumber, resultMatrix);
		} else {
			resultNumber = pow(base->resultNumber, exponent->resultNumber);
		}
	}
};

THISCLASS::Parser(Context * context, std::string text): context(context), error(""), lineNumber(0), lineStart(0) {
	strncpy(this->text, text.c_str(), 4096);
	pos = 0;
	c = this->text[0];

	StatementList();
}

Node * THISCLASS::SetError(std::string message) {
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

void THISCLASS::Next() {
	if (c == 0) return;

	if (c == 10) {
		lineNumber += 1;
		lineStart = pos + 1;
	}

	pos += 1;
	c = text[pos];
}

void THISCLASS::SkipWhitespace() {
	while (c == 32 || c == 9 || c == 10 || c == 13) Next();
}

bool THISCLASS::IsUppercaseLetter(char c) {
	return c >= 'A' && c <= 'Z';
}
bool THISCLASS::IsLowercaseLetter(char c) {
	return c >= 'a' && c <= 'z';
}
bool THISCLASS::IsDigit(char c) {
	return c >= '0' && c <= '9';
}

void THISCLASS::StatementList() {
	while (c != 0) {
		SkipWhitespace();
		Node * node = Assignment();
		if (! node) break;
	}
}

std::string THISCLASS::ReadLabel() {
	if (! IsUppercaseLetter(c) && ! IsLowercaseLetter(c)) return "";

	int start = pos;
	while (IsUppercaseLetter(c) || IsLowercaseLetter(c) || c == '_' || IsDigit(c)) Next();
	return std::string(text + start, pos - start);
}

Node * THISCLASS::Assignment() {
	std::string label = ReadLabel();
	if (label.empty()) return SetError("Expected variable name.");

	SkipWhitespace();
	if (c != '=') return SetError("Expected '='.");
	Next();

	SkipWhitespace();
	Node * node = BitOperations();
	if (! node) return NULL; // SetError("Expected expression.");

	context->nodesByLabel[label] = node;
	return node;
}

Node * THISCLASS::BitOperations() {
	Node * node = Comparison();
	if (! node) return NULL;

	while (c != 0) {
		SkipWhitespace();
		NodeBitOperation * parent =
		    c == '&' ? (NodeBitOperation *) new NodeAnd() :
		    c == '|' ? (NodeBitOperation *) new NodeOr() :
		    c == '^' ? (NodeBitOperation *) new NodeXor() :
		    NULL;
		if (! parent) break;
		context->nodes.push_back(parent);
		parent->left = node;
		Next();

		SkipWhitespace();
		parent->right = Comparison();
		if (! parent->right) break;

		node = parent;
	}

	return node;
}

Node * THISCLASS::Comparison() {
	Node * node = Sum();
	if (! node) return NULL;

	while (c != 0) {
		SkipWhitespace();
		NodeFloatOperation * parent =
		    c == '>' ? (NodeFloatOperation *) new NodeBigger() :
		    c == '<' ? (NodeFloatOperation *) new NodeSmaller() :
		    NULL;
		if (! parent) break;
		context->nodes.push_back(parent);
		parent->left = node;
		Next();

		SkipWhitespace();
		parent->right = Sum();
		if (! parent->right) break;

		node = parent;
	}

	return node;
}

Node * THISCLASS::Sum() {
	Node * node = Product();
	if (! node) return NULL;

	while (c != 0) {
		SkipWhitespace();
		NodeFloatOperation * parent =
		    c == '+' ? (NodeFloatOperation *) new NodeAdd() :
		    c == '-' ? (NodeFloatOperation *) new NodeSub() :
		    NULL;
		if (! parent) break;
		context->nodes.push_back(parent);
		parent->left = node;
		Next();

		SkipWhitespace();
		parent->right = Product();
		if (! parent->right) break;

		node = parent;
	}

	return node;
}

Node * THISCLASS::Product() {
	Node * node = Term();
	if (! node) return NULL;

	while (c != 0) {
		SkipWhitespace();
		NodeFloatOperation * parent =
		    c == '*' ? (NodeFloatOperation *) new NodeMul() :
		    c == '/' ? (NodeFloatOperation *) new NodeDiv() :
		    NULL;
		if (! parent) break;
		context->nodes.push_back(parent);
		parent->left = node;
		Next();

		SkipWhitespace();
		parent->right = Term();
		if (! parent->right) break;

		node = parent;
	}

	return node;
}

Node * THISCLASS::Term() {
	Node * node = Parenthesis();
	if (node) return node;

	node = Not();
	if (node) return node;

	node = Negation();
	if (node) return node;

	node = LabelOrFunction();
	if (node) return node;

	return NumberLiteral();
}

Node * THISCLASS::LabelOrFunction() {
	std::string label = ReadLabel();
	if (label.empty()) return NULL;

	SkipWhitespace();
	if (c != '(') {
		Node * node = context->GetNode(label);
		if (! node) return SetError("Unknown variable name.");
		return node;
	}

	Next();
	SkipWhitespace();

	if (label == "sqrt") return Function1(new NodeSqrt());
	if (label == "pow") return Pow();
	if (label == "log") return Function1(new NodeLog());
	if (label == "exp") return Function1(new NodeExp());
	if (label == "min") return FloatOperation(new NodeMin());
	if (label == "max") return FloatOperation(new NodeMax());
	if (label == "sum") return Reduce1(new NodeSum());
	if (label == "mean") return Reduce1(new NodeMean());
	return SetError("Unknown function.");
}

Node * THISCLASS::Function1(NodeFunction1 * node) {
	context->nodes.push_back(node);

	node->value = BitOperations();
	if (! node->value) return SetError("Expected function argument.");

	SkipWhitespace();
	if (c != ')') return SetError("Expected ')'.");
	Next();
	return node;
}

Node * THISCLASS::Reduce1(NodeReduce1 * node) {
	context->nodes.push_back(node);

	node->value = BitOperations();
	if (! node->value) return SetError("Expected function argument.");

	SkipWhitespace();
	if (c != ')') return SetError("Expected ')'.");
	Next();
	return node;
}

Node * THISCLASS::FloatOperation(NodeFloatOperation * node) {
	context->nodes.push_back(node);

	node->left = BitOperations();
	if (! node->left) return SetError("Expected function argument.");

	SkipWhitespace();
	if (c != ',') return SetError("Expected ',' and second function argument.");
	Next();

	SkipWhitespace();
	node->right = BitOperations();
	if (! node->right) return SetError("Expected second function argument.");

	SkipWhitespace();
	if (c != ')') return SetError("Expected ')'.");
	Next();
	return node;
}

Node * THISCLASS::Pow() {
	NodePow * node = new NodePow();
	context->nodes.push_back(node);

	node->base = BitOperations();
	if (! node->base) return SetError("Expected function argument.");

	SkipWhitespace();
	if (c != ',') return SetError("Expected ',' and exponent.");
	Next();

	SkipWhitespace();
	node->exponent = BitOperations();
	if (! node->exponent) return SetError("Expected exponent.");

	SkipWhitespace();
	if (c != ')') return SetError("Expected ')'.");
	Next();
	return node;
}

int64_t THISCLASS::ReadInteger() {
	int64_t value = 0.0;
	while (IsDigit(c)) {
		value *= 10;
		value += c - 48;
		Next();
	}

	return value;
}

double THISCLASS::ReadNumber() {
	int64_t value = ReadInteger();
	if (c != '.') return (double) value;
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

Node * THISCLASS::NumberLiteral() {
	if (! IsDigit(c)) return NULL;

	double number = ReadNumber();
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
		number *= pow(10.0, exponent);
	}

	NodeWithNumber * node = new NodeWithNumber();
	context->nodes.push_back(node);
	node->context = context;
	node->Set(number);
	return node;
}

Node * THISCLASS::Parenthesis() {
	if (c != '(') return NULL;
	Next();

	Node * node = BitOperations();
	if (c != ')') return SetError("Expected ')'.");
	Next();
	return node;
}

Node * THISCLASS::Not() {
	if (c != '~') return NULL;
	Next();

	NodeNot * node = new NodeNot();
	context->nodes.push_back(node);
	node->value = Term();
	return node;
}

Node * THISCLASS::Negation() {
	if (c != '-') return NULL;
	Next();

	NodeNegation * node = new NodeNegation();
	context->nodes.push_back(node);
	node->value = Term();
	return node;
}
}
