/*
 * TokenTestPackage.h
 *
 *  Created on: Sep 17, 2012
 *      Author: tom
 */

#ifndef TOKENTESTPACKAGE_H_
#define TOKENTESTPACKAGE_H_

#include <declarations.h>
#include <Types/NodeValue.h>


namespace Parsers
{

struct TokenTestPackage
{
	std::string __val;
	Enums::NodeValueTypes::TokenTestResult __last_result;
	Enums::NodeValueTypes::NodeValueTypes __val_type;
	std::vector <std::string> __args;
	Nodes::NodeValue* __nval_final;
	int (*__callback)(const std::string&);
	Goo::Moldable* __broadcaster;
	std::string __r_node_type;

	TokenTestPackage(Nodes::NodeValue* nval) :
		__args()
		,__broadcaster(0)
	{
		using namespace Enums::NodeValueTypes;
		__val = "";
		__last_result = ERR;
		__val_type = string_value;
		__callback =NULL;
		__nval_final = nval;
		__r_node_type ="";
	}

	void interpret(
			Enums::NodeValueTypes::InterpretationMode _mode,
			size_t arg=0);
};

}//namespace Parsers


#endif /* TOKENTESTPACKAGE_H_ */
