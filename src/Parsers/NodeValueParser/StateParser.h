/*
 * StateParser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef STATEPARSER_H_
#define STATEPARSER_H_

#include <list>
#include <string>
#include <vector>
#include <unordered_set>

namespace Parsers {

class StateParser
{
	public:
		StateParser();
		void processInput(std::list<int>::reverse_iterator rit,
						  Moldable* broadcaster);

	private:
		const std::unordered_set<int> __objectTokens;
		std::list<int>::reverse_iterator objectArgsRit;
		Moldable* targetWidget;

		void resolveWidgetInfo(NodeValue &v);
		void resolveDatabaseInfo(MoguSyntax token, NodeValue &v);
		void resolveNamedWidgetInfo(NodeValue &v);
};

}	// namespace Parsers
#endif /* STATEPARSER_H_ */
