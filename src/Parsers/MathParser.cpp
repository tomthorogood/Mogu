/*
 * MathParser.cpp
 *
 *  Created on: March 12th, 2013
 *      Author: cameron
 */

#include <Parsers/MathParser.h>
#include <cassert>


namespace Parsers {

MathParser::MathParser()
{

}

int MathParser::evaluate(std::vector<std::string>& infixExpression)
{
	__reset__();

	// *** CONVERT INFIX -> POSTFIX ***
	__postfixExpression.reserve(infixExpression.size());

	// temporary operator stack for doing conversion
	std::stack<int> opStack;

	for(int i=0; i<infixExpression.size(); i++) {
		int currToken = stoi(infixExpression[i]);

		if(isOperator(currToken)) {
			switch(currToken) {
			case MATHSYNTAX_OPENPAREN:
				opStack.push(currToken);
				break;

			case MATHSYNTAX_CLOSEPAREN:
				while(opStack.top() != MATHSYNTAX_OPENPAREN) {
					__postfixExpression.push_back(opStack.top());
					opStack.pop();
				}
				opStack.pop();	//remove open parens
				break;

			default:
			//we have a +,-,* or /
				while(!hasHigherPrecedence(currToken, opStack.top())) {
					__postfixExpression.push_back(opStack.top());
					opStack.pop();
				}
				opStack.push(currToken);
				break;
			}
		}
		else {
			__postfixExpression.push_back(currToken);
		}
	}

	while(!opStack.empty()) {
		__postfixExpression.push_back(opStack.top());
		opStack.pop();
	}

	// *** EVALUATE POSTFIX EXPRESSION ***
	for(int i=0; i<__postfixExpression.size(); i++) {

		int currToken = __postfixExpression[i];

		if(!isOperator(currToken)) {	//operand 
			__operandStack.push(currToken);
		}
		else {							//operator
			int op1 = __operandStack.top();
			__operandStack.pop();
			int op2 = __operandStack.top();
			__operandStack.pop();

			int opResult;
			switch(currToken) {
			case MATHSYNTAX_PLUS:
				opResult = op1 + op2;
				break;
			case MATHSYNTAX_MINUS:
				opResult = op1 - op2;
				break;
			case MATHSYNTAX_MULTIPLY:
				opResult = op1 * op2;
				break;
			case MATHSYNTAX_DIVIDE:
				opResult = op1 / op2;
				break;
			}
		__operandStack.push(opResult);
		}
	}

	assert(__operandStack.size() == 1);
	return __operandStack.top();
}

// read: has STRICTLY higher precedence
bool MathParser::hasHigherPrecedence(int op1, int op2)
{
	if(op1 == MATHSYNTAX_MULTIPLY || op1 == MATHSYNTAX_DIVIDE)
		if(op2 == MATHSYNTAX_PLUS || op2 == MATHSYNTAX_MINUS)
			return true;

	return false;
}

void MathParser::__reset__()
{
	__postfixExpression.clear();
	while(!__operandStack.empty())
		__operandStack.pop();
}


}	// namespace Parsers
