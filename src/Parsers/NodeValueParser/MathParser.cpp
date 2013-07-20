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

MathParser::MathParser(TokenManager& tm) : tm(tm)
{
}

int MathParser::processInput()
{
	reset();

	// temporary operator stack for doing conversion
	std::stack<int> opStack;

	tm.saveLocation();

	bool done = false;
	while(!done)
	{
        int currToken = (int) tm.currentToken();
		tm.next();	//move

		if(isOperator(MoguSyntax::get(currToken))) {
            const SyntaxDef& oper = MoguSyntax::get(currToken);
			switch(oper) {
				case MoguSyntax::OPER_OPPAREN:
                    opStack.push(currToken);
                    break;
				case MoguSyntax::OPER_CLPAREN:
                    while(opStack.top() != MoguSyntax::OPER_OPPAREN) {
                        postfixExpression.push_back(opStack.top());
                        opStack.pop();
                    }
                    done = true;
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
                        postfixExpression.push_back(opStack.top());
                        opStack.pop();

                        if(opStack.empty())
                            break;
                    }
                    opStack.push(currToken);
                    break;
                }
		}
		else {
			postfixExpression.push_back(currToken);
		}
	}

	while(!opStack.empty()) {
		postfixExpression.push_back(opStack.top());
		opStack.pop();
	}

	//std::cout << "converted to PostFix: ";
	//for(int i=0; i<postfixExpression.size(); i++)
		//std::cout << postfixExpression[i] << " ";
	//std::cout << std::endl;

	// *** EVALUATE POSTFIX EXPRESSION ***
	for(unsigned int i=0; i<postfixExpression.size(); i++) {
        int currToken = postfixExpression[i];
        const SyntaxDef& syn_token = MoguSyntax::get(currToken);

		if(!isOperator(syn_token)) {	//operand 
			operandStack.push(currToken);
		}
		else {							//operator
			int op2 = operandStack.top();
			operandStack.pop();
			int op1 = operandStack.top();
			operandStack.pop();

			int opResult;
			switch(syn_token) {
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
		operandStack.push(opResult);
		}
	}

	assert(operandStack.size() == 1);

	tm.prev();
	tm.deleteFromSaved();
	tm.injectToken(operandStack.top());
	return operandStack.top();
}

// read: has STRICTLY higher precedence
bool MathParser::hasHigherPrecedence(const SyntaxDef& op1, const SyntaxDef& op2)
{
    if ((op1 == MoguSyntax::OPER_MULT || op1 == MoguSyntax::OPER_DIV)
        && (op2 == MoguSyntax::OPER_PLUS || op2 == MoguSyntax::OPER_MINUS))
        return true;
        
    if (op2 == MoguSyntax::OPER_OPPAREN)
        return true;
	return false;
}

void MathParser::reset()
{
	postfixExpression.clear();
	while(!operandStack.empty())
		operandStack.pop();
}


}	// namespace Parsers
