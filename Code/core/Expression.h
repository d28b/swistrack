#ifndef HEADER_Expression
#define HEADER_Expression

#include <opencv2/core.hpp>
#include <map>

class Expression {
public:
	enum Operation {
		UNKNOWN,
		NUMBER_CONSTANT,
		MATRIX_CONSTANT,
		ADD,
		SUB,
		MUL,
		DIV,
		NEGATION,
		NOT,
		AND,
		OR,
		XOR,
		LOG,
		POW,
	};

	enum ResultType {
		NONE,
		NUMBER,
		MATRIX,
	};

	Expression();
	~Expression();

	class Node {
	public:
		enum Operation operation;
		Node * left;
		Node * right;

		enum ResultType resultType;
		double number;
		cv::Mat matrix;

		Node(): operation(NUMBER_CONSTANT), left(NULL), right(NULL), resultType(NONE), number(0), matrix() { }
		~Node() { }

		void Add();
		void Sub();
		void Mul();
		void Div();
		void Not();
		void Unexpected();

		void Set(cv::Mat value);
		void Set(double value);
		void Execute();
		double GetNumber();
		cv::Mat GetMatrix();
	};

private:
	std::vector<Node *> nodes;
	std::map<std::string, Node *> nodesByLabel;

	class Parser {
	public:
		Expression * expression;
		std::string error;
		char text[4096];
		int lineNumber;
		int lineStart;
		int pos;
		char c;

		Parser(Expression * expression, std::string text);
		~Parser() { }

		Node * SetError(std::string message);
		void Next();
		void SkipWhitespace();
		bool IsUppercaseLetter(char c);
		bool IsLowercaseLetter(char c);
		bool IsDigit(char c);
		void StatementList();
		std::string ReadLabel();
		Node * Assignment();
		Node * BitOperations();
		Node * Sum();
		Node * Product();
		Node * Term();
		Node * LabelOrFunction();
		int64_t ReadInteger();
		double ReadNumber();
		Node * NumberLiteral();
		Node * Parenthesis();
		Node * Not();
		Node * Negation();
	};

public:
	Node * CreateNode();
	Node * CreateNode(std::string label);
	std::string Parse(std::string expression);
	Node * GetNode(std::string label);
	Node * GetOrCreateNode(std::string label);
	void ResetExecution();
};

#endif
