#ifndef HEADER_ExpressionNode
#define HEADER_ExpressionNode

#include <opencv2/core.hpp>

namespace Expression {
class Context;

class Node {
public:
	enum Type {
		NUMBER,
		MATRIX,
	};

	enum Type resultType;
	double resultNumber;
	cv::Mat resultMatrix;
	int revision;

	Node();
	virtual ~Node() { }

	void ToString();
	double GetNumber();
	cv::Mat GetMatrix();
	cv::Mat GetFloatMatrix();

	virtual std::string Prepare() = 0;
	virtual void ToString(std::stringstream & ss) const = 0;
	virtual void Execute() = 0;

protected:
	std::string PrepareCommon(Node * node);
	std::string PrepareCommon(Node * a, Node * b);
	bool CheckDependencies(Node * node);
	bool CheckDependencies(Node * a, Node * b);
};

class NodeWithNumber: public Node {
public:
	Context * context;
	void Set(double value);

	std::string Prepare();
	void ToString(std::stringstream & ss) const;
	void Execute();
};

class NodeWithMatrix: public Node {
public:
	Context * context;
	void Set(cv::Mat value);

	std::string Prepare();
	void ToString(std::stringstream & ss) const;
	void Execute();
};

class NodeFloatOperation: public Node {
public:
	std::string symbol;
	Node * left;
	Node * right;

	NodeFloatOperation(std::string symbol);

	std::string Prepare();
	void ToString(std::stringstream & ss) const;
	void Execute();

protected:
	virtual cv::Mat Calculate(cv::Mat left, cv::Mat right) = 0;
	virtual cv::Mat Calculate(cv::Mat left, double right) = 0;
	virtual cv::Mat Calculate(double left, cv::Mat right) = 0;
	virtual double Calculate(double left, double right) = 0;
};

class NodeBitOperation: public Node {
public:
	std::string symbol;
	Node * left;
	Node * right;

	NodeBitOperation(std::string symbol);

	std::string Prepare();
	void ToString(std::stringstream & ss) const;
	void Execute();

protected:
	virtual cv::Mat Calculate(cv::Mat left, cv::Mat right) = 0;
	virtual cv::Mat Calculate(cv::Mat left, unsigned char right) = 0;
	virtual cv::Mat Calculate(unsigned char left, cv::Mat right) = 0;
	virtual int Calculate(unsigned char left, unsigned char right) = 0;
};

class NodeFunction1: public Node {
public:
	std::string name;
	Node * value;

	NodeFunction1(std::string name);

	std::string Prepare();
	void ToString(std::stringstream & ss) const;
	void Execute();

protected:
	virtual void Calculate(cv::Mat value, cv::Mat & result) = 0;
	virtual double Calculate(double value) = 0;
};

class NodeReduce1: public Node {
public:
	std::string name;
	Node * value;

	NodeReduce1(std::string name);

	std::string Prepare();
	void ToString(std::stringstream & ss) const;
	void Execute();

protected:
	virtual double Calculate(cv::Mat input) = 0;
	virtual double Calculate(double input) = 0;
};

std::ostream & operator<<(std::ostream & os, const Node & node);
}

#endif
