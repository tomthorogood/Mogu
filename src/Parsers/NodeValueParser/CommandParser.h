/*
 * CommandParser.h
 *
 *  Created on: March 1st, 2013
 *      Author: cameron
 */

#ifndef COMMANDPARSER_H_
#define COMMANDPARSER_H_

namespace Parsers {

enum CP_States
{

};

class CommandParser: public Automaton::FiniteAutomatonTmpl<std::string, void,
    NVP_States>
{
	public:
		CommandParser();
		void processInput(std::forward_list<int>& tokens, NodeValue& outputVal);

};

}	// namespace Parsers
#endif /* COMMANDPARSER_H_ */

