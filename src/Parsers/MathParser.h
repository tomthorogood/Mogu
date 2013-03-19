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

#define LOWEST_OP_ENUM (INT_MAX - 5)
#define isOperator(a) ( (a) >= LOWEST_OP_ENUM )


namespace Parsers {

/* since everything coming through the database is going to look like
 * an integer (or at least the mathematical symbols will), we need a
 * way to distinguish the mathematical symbols (+,-,*,/) from their operands.
 *
 * this implementation assumes that we designate our largest enum
 * values in MoguSyntax as the mathematical symbols.  this way, we
 * don't need to mark the symbols in any special way when we put them
 * in the database, and all we lose is the use of a few numbers that
 * the average content creator would never use anyway.
 *
 * after doing quite a bit of research, it seems like the fastest
 * general method for parsing a mathematical infix expression is to
 * first convert that expression to postfix notation and then evaluate the
 * postfix expression.  this saves you from scanning the entire string
 * for the highest precedence operator after each operator evaluation. 
 *
 * i'm concerned though, because although this may be the fastest general
 * method for infix parsing, our average use-case might just be in the
 * simple form 'database_value / 2' or something similar, in which
 * case all this extra processing overhead for converting to postfix
 * might be silly.  in order to have the power to do serious math,
 * though, we need this extra processing.  might need to optimize
 * later if this is slowing us down.
 *
 * this parser assumes that its input is well-formed (matching parens,
 * valid infix expression) and that the all the strings in the input
 * vector can be properly converted to integers using stoi()
 *
 */
class MathParser
{
	public:
		// empty constructor
		MathParser();

		// convert the input tokens into postfix, and then evaluate
		// the postfix expression.
		int evaluate(std::vector<std::string>& infixExpression);
	
	private:
		// vector to build/store our equivalent postfix expression
		std::vector<int> __postfixExpression;

		// stack used for postfix evaluation
		std::stack<int> __operandStack;

		// compare precedence to do proper order-of-operations
		// returns true if op1 has higher precedence than op2
		bool hasHigherPrecedence(int op1, int op2);

		// reset the parser so we're ready for the next input
		void __reset__();
};

}	// namespace Parsers

#endif /* MATHPARSER_H_ */
