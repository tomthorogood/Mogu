/*
 * MathParser.h
 *
 *  Created on: March 12th, 2013
 *      Author: cameron
 */

#ifndef MATHPARSER_H_
#define MATHPARSER_H_

#include <vector>
#include <stack>
#include <string>
#include <climits>

#include <Parsers/NodeValueParser/TokenManager.h>

//TODO: replace these macros with generated syntax values
#define LOWEST_OP_ENUM (INT_MAX - 5)
#define isOperator(a) ( (a) >= LOWEST_OP_ENUM )


namespace Parsers {

class MathParser
{
	public:
		// empty constructor
		MathParser();

		// convert the input tokens into postfix, and then evaluate
		// the postfix expression.
		int processInput();

		// initialize tokenlists
		void setTokenManager(TokenManager& tm);
	
	private:
		// vector to build/store an equivalent postfix expression
		std::vector<int> __postfixExpression;

		// stack used for postfix evaluation
		std::stack<int> __operandStack;

		// token manager from NVP 
		TokenManager* __tm = 0;

		// compare precedence to do proper order-of-operations
		// returns true if op1 has higher precedence than op2
		bool hasHigherPrecedence(int op1, int op2);

		// reset the parser so we're ready for the next input
		void reset();
};

}	// namespace Parsers
#endif /* MATHPARSER_H_ */
