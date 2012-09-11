/*
 * NodeValueParser.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

namespace Parsers{

using Nodes::NodeValue;
using Goo::Moldable;

NodeValueParser::NodeValueParser(
		std::string full_value,
		NodeValue* nval,
		Moldable* broadcaster)
{
	parsedValue = nval;

}


}//namespace Parsers


