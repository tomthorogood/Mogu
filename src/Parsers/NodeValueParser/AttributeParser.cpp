/*
 * AttributeParser.cpp
 *
 *  Created on: March 4th, 2013
 *      Author: cameron
 */

#include <Parsers/NodeValueParser/AttributeParser.h>
#include <cassert>

namespace Parsers {

AttributeParser::AttributeParser()
{
}

void AttributeParser::giveInput(std::string input, NodeValue& v)
{
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
			// assumes only other single-token input 
			// possibility is an enumerated value!
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

		std::string db_keyspace, queryResult;
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
			app->redisExec(Mogu::Keep, "get %s", cnode);
			queryResult = redisReply_STRING;
		}


		else {
			// assuming __tokens.size() == 3, is a hash or list?
			std::string nodeType;
			app->redisExec(Mogu::Keep, "type %s", cnode);
			nodeType = redisReply_STRING;

			const char *carg = __tokens[2].c_str();
			if(nodeType == "hash")
				app->redisExec(Mogu::Keep, "hget %s %s", cnode, carg);

			else if(nodeType == "list")
				app->redisExec(Mogu::Keep,"lindex %s %s", cnode, carg);

			else {
				// unsupported node type or key doesn't exist!
				v.setString("ERROR: unsupported key of type '" 
							+ nodeType + "'");
				return;
			}

			queryResult = redisReply_STRING;
			v.setString(queryResult);
		}

	}
}


}	// namespace Parsers
