#ifndef HEADER_ExpressionParser
#define HEADER_ExpressionParser

#include "Expression.h"
#include "ExpressionNode.h"

namespace Expression {
	class Parser {
	public:
		Context * context;
		std::string error;
		char text[4096];
		int lineNumber;
		int lineStart;
		int pos;
		char c;

		Parser(Context * context, std::string text);
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
		Node * Comparison();
		Node * Sum();
		Node * Product();
		Node * Term();
		Node * LabelOrFunction();
		Node * Function1(NodeFunction1 * node);
		Node * Reduce1(NodeReduce1 * node);
		Node * FloatOperation(NodeFloatOperation * node);
		Node * Pow();
		int64_t ReadInteger();
		double ReadNumber();
		Node * NumberLiteral();
		Node * Parenthesis();
		Node * Not();
		Node * Negation();
	};
}

#endif
