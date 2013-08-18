/*
 * Math_Parser.h
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

namespace Parsers {

class Math_Parser
{
	public:
		// empty constructor
		Math_Parser(Token_Manager& tm_) : tm(tm_) {}

		// convert the input tokens into postfix, and then evaluate
		// the postfix expression.
		int process_input();

	private:
		// vector to build/store an equivalent postfix expression
		std::vector<int> postfix_expression;

		// stack used for postfix evaluation
		std::stack<int> operand_stack;

		// token manager from NVP 
		Token_Manager& tm;

		// compare precedence to do proper order-of-operations
		// returns true if op1 has higher precedence than op2
		bool has_higher_precedence(const Syntax_Def& op1, const Syntax_Def& op2);

        inline bool has_higher_precedence(const int& op1, const int& op2)
        {
            return has_higher_precedence(
                    Mogu_Syntax::get(op1), Mogu_Syntax::get(op2));
        }

		// reset the parser so we're ready for the next input
		void reset();
};

}	// namespace Parsers
#endif /* MATHPARSER_H_ */
