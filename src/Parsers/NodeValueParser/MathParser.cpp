/*
 * Math_Parser.cpp
 *
 *  Created on: March 12th, 2013
 *      Author: cameron
 */

#include <Parsers/NodeValueParser/Math_Parser.h>
#include <Types/syntax.h>
#include <cassert>
#include <cstdlib>

#include <iostream>

namespace Parsers {

int Math_Parser::process_input()
{
	reset();

	// temporary operator stack for doing conversion
	std::stack<int> op_stack {};

	tm.save_location();

	bool done {false};
	while(!done)
	{
        int curr_token {tm.current_token()};
		tm.next();	//move

		if(is_operator(Mogu_Syntax::get(curr_token)))
        {
            const Syntax_Def& oper {Mogu_Syntax::get(curr_token)};
			switch(oper)
            {
				case Mogu_Syntax::OPER_OPPAREN:
                    op_stack.push(curr_token);
                    break;
				case Mogu_Syntax::OPER_CLPAREN:
                    while(op_stack.top() != Mogu_Syntax::OPER_OPPAREN) {
                        postfix_expression.push_back(op_stack.top());
                        op_stack.pop();
                    }
                    done = true;
                    op_stack.pop();	//remove open parens
                    break;

				default:
                    //we have a +,-,* or /
                    if(op_stack.empty())
                    {
                        op_stack.push(curr_token);
                        break;
                    }
						
                    while(!has_higher_precedence(curr_token, op_stack.top()))
                    {
                        postfix_expression.push_back(op_stack.top());
                        op_stack.pop();

                        if(op_stack.empty())
                            break;
                    }
                    op_stack.push(curr_token);
                    break;
            }
		} else {
			postfix_expression.push_back(curr_token);
		}
	}

	while(!op_stack.empty()) {
		postfix_expression.push_back(op_stack.top());
		op_stack.pop();
	}


	// *** EVALUATE POSTFIX EXPRESSION ***
	for(unsigned int i=0; i<postfix_expression.size(); ++i) 
    {
        int curr_token = postfix_expression[i];
        const Syntax_Def& syn_token {Mogu_Syntax::get(curr_token)};

		if(!is_operator(syn_token))
        {	//operand 
			operand_stack.push(curr_token);
		}
        else
        {							//operator
			int op2 = operand_stack.top();
			operand_stack.pop();
			int op1 = operand_stack.top();
			operand_stack.pop();

			int op_result;
			switch(syn_token)
            {
				case Mogu_Syntax::OPER_PLUS:
                    op_result = op1 + op2;
                    break;
				case Mogu_Syntax::OPER_MINUS:
                    op_result = op1 - op2;
                    break;
				case Mogu_Syntax::OPER_MULT:
                    op_result = op1 * op2;
                    break;
				case Mogu_Syntax::OPER_DIV:
                    op_result = op1 / op2;
                    break;
				default:
                    break;
			};
            operand_stack.push(op_result);
		}
	}

	assert(operand_stack.size() == 1);

	tm.prev();
	tm.delete_from_saved();
	tm.inject_token(operand_stack.top());
	return operand_stack.top();
}

// read: has STRICTLY higher precedence
bool Math_Parser::has_higher_precedence(const Syntax_Def& op1, const Syntax_Def& op2)
{
    if ((op1 == Mogu_Syntax::OPER_MULT || op1 == Mogu_Syntax::OPER_DIV)
        && (op2 == Mogu_Syntax::OPER_PLUS || op2 == Mogu_Syntax::OPER_MINUS))
        return true;
        
    if (op2 == Mogu_Syntax::OPER_OPPAREN)
        return true;
	return false;
}

void Math_Parser::reset()
{
	postfix_expression.clear();
	while(!operand_stack.empty())
		operand_stack.pop();
}


}	// namespace Parsers
