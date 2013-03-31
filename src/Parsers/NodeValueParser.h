/*
 * NodeValueParser.h
 *
 *  Created on: March 31, 2013
 *      Author: cameron
 */

#ifndef NODEVALUEPARSER_H_
#define NODEVALUEPARSER_H_

namespace Parsers {

/* take an input (a string from the database), tokenize it, convert
 * the values that are meant to be read as integers into integers,
 * store it all as a std::vector<NodeValue>'s (basically a vector of
 * tokens), and then run that vector through the appropriate pipeline
 * of parsers
 *
 */
class NodeValueParser 
{


};

}	// namespace Parsers

#endif /* NODEVALUEPARSER_H_ */
