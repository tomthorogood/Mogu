/*
 * AttributeParser.cpp
 *
 *  Created on: March 4th, 2013
 *      Author: cameron
 */

#include <Parsers/AttributeParser.h>
#include <cassert>

namespace Parsers {

AttributeParser::AttributeParser()
{
	__tokenizer.add_pair('"', '"');
}

void AttributeParser::giveInput(std::string input, NodeValue& v)
{
	/*
	 * three possibilities:
	 * 	input is a string literal -> remove the double quotes, return
	 * 	input is a single enumerated value -> return the integer
	 * 	input is a parseable command -> parse and return string
	 */	

	mApp;

	__tokenizer.reset().newString(input);

	while(__tokenizer.hasNext()) {
		std::string next_token = __tokenizer.next();
		__tokens.push_back(next_token);
	}

	if(__tokens.size() == 0) {
		// why did you invoke me with no input?  -_-
		v.setString(EMPTY);
		return;
	}

	else if(__tokens.size() == 1) {
		if(__tokenizer.isWrapped(__tokens[0]))
			v.setString(__tokens[0].substr(1, __tokens[0].length()-2));
		else
			// assumes only other single-token input possibility is an enumerated
			// value!
			v.setInt(std::stoi(__tokens[0]));
		return;
	}

	else {
		// we have a parseable command, we need to hit the database.
		// assumes that commands have more than one token in all
		// circumstances
		//
		// user keyspace (session): s.key
		// group keyspace: g.key
		// data keyspace: data.key

		std::string db_keyspace;
		switch(std::stoi(__tokens[0])) {
		case MoguSyntax::user:
			db_keyspace = __PREFIX_SESSION;
			break;
		case MoguSyntax::group:
			db_keyspace = __PREFIX_GROUP;
			break;
		case MoguSyntax::data:
			db_keyspace = "data."; 
			break;
		default:
			db_keyspace = __NODE_GLOBAL_;
			break;
		}

		std::string db_node = db_keyspace + __tokens[1];

		const char *cnode = db_node.c_str();

		if(__tokens.size() == 2) {
			app->redisCommand("get %s", cnode);
		}


		else {
			const char *carg = __tokens[2].c_str();
			app->redisCommand("hget %s %s", cnode, carg);
		}

		// TODO: process the redis reply here, convert it to an int
		// if appropriate, and store the result in NodeValue v.

	}



	// DEBUG
	std::cout << "tokens are as follows:" << std::endl << "************" << std::endl;
	for(int i=0; i<__tokens.size(); i++)
		std::cout << __tokens[i] << std::endl;
	std::cout << std::endl;

	std::cout << "original string" << std::endl << "************" << std::endl;
	std::cout << __tokenizer.getOriginal() << std::endl;
}



}	// namespace Parsers
