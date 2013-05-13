/*
 * MathParser.cpp
 *
 *  Created on: March 12th, 2013
 *      Author: cameron
 */

#include <Parsers/NodeValueParser/MathParser.h>
#include <Types/syntax.h>
#include <cassert>
#include <cstdlib>

#include <iostream>

namespace Parsers {

MathParser::MathParser()
{

}

void MathParser::setTokens(std::vector<int>& numTokens)
{
	__numTokens = &numTokens;
}

int MathParser::processInput(std::vector<int>::reverse_iterator& endRit)
{
	reset();

	// *** CONVERT INFIX -> POSTFIX ***
	__postfixExpression.reserve(__numTokens->size());

	// temporary operator stack for doing conversion
	std::stack<int> opStack;

	// do-while loop... conditional is until level = 0
	//
	for(auto it=endRit.base()--; it!=__numTokens->end(); it++) {
		int currToken = *it;

		if(isOperator(currToken)) {
			switch((MoguSyntax) currToken) {
				case MoguSyntax::OPER_OPPAREN:
				opStack.push(currToken);
				break;

				case MoguSyntax::OPER_CLPAREN:
				while((MoguSyntax) opStack.top() != MoguSyntax::OPER_OPPAREN) {
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

	//std::cout << "converted to PostFix: ";
	//for(int i=0; i<__postfixExpression.size(); i++)
		//std::cout << __postfixExpression[i] << " ";
	//std::cout << std::endl;

	// *** EVALUATE POSTFIX EXPRESSION ***
	for(unsigned int i=0; i<__postfixExpression.size(); i++) {

		int currToken = __postfixExpression[i];

		if(!isOperator(currToken)) {	//operand 
			__operandStack.push(currToken);
		}
		else {							//operator
			int op2 = __operandStack.top();
			__operandStack.pop();
			int op1 = __operandStack.top();
			__operandStack.pop();

			int opResult;
			switch((MoguSyntax) currToken) {
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
				default:
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
	if((MoguSyntax) op1 == MoguSyntax::OPER_MULT || (MoguSyntax) op1 == MoguSyntax::OPER_DIV)
		if((MoguSyntax) op2 == MoguSyntax::OPER_PLUS || (MoguSyntax) op2 == MoguSyntax::OPER_MINUS)
			return true;

	if((MoguSyntax) op2 == MoguSyntax::OPER_OPPAREN)
		return true;

	return false;
}

void MathParser::reset()
{
	__postfixExpression.clear();
	while(!__operandStack.empty())
		__operandStack.pop();
}


}	// namespace Parsers
