/*
 * MathParser.cpp
 *
 *  Created on: March 12th, 2013
 *      Author: cameron
 */

#include <Parsers/MathParser.h>
#include <Types/syntax.h>
#include <cassert>
#include <cstdlib>

#include <iostream>



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
		int currToken = atoi(infixExpression[i].c_str());

		if(isOperator(currToken)) {
			switch(currToken) {
				case MoguSyntax::OPER_OPPAREN:
				opStack.push(currToken);
				break;

				case MoguSyntax::OPER_CLPAREN:
				while(opStack.top() != MoguSyntax::OPER_OPPAREN) {
					__postfixExpression.push_back(opStack.top());
					opStack.pop();
				}
				opStack.pop();	//remove open parens
				break;

				default:
				//we have a +,-,* or /
				if(opStack.empty())
				{
					opStack.push(currToken);
					break;
				}
						
				while(!hasHigherPrecedence(currToken, opStack.top())) {
					__postfixExpression.push_back(opStack.top());
					opStack.pop();

					if(opStack.empty())
						break;
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

	std::cout << "converted to PostFix: ";
	for(int i=0; i<__postfixExpression.size(); i++)
		std::cout << __postfixExpression[i] << " ";
	std::cout << std::endl;

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
				case MoguSyntax::OPER_PLUS:
				opResult = op1 + op2;
				break;
				case MoguSyntax::OPER_MINUS:
				opResult = op1 - op2;
				break;
				case MoguSyntax::OPER_MULT:
				opResult = op1 * op2;
				break;
				case MoguSyntax::OPER_DIV:
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
	if(op1 == MoguSyntax::OPER_MULT || op1 == MoguSyntax::OPER_DIV)
		if(op2 == MoguSyntax::OPER_PLUS || op2 == MoguSyntax::OPER_MINUS)
			return true;

	if(op2 == MoguSyntax::OPER_OPPAREN)
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
