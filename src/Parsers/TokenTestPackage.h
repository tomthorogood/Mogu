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
	{
		using namespace Enums::NodeValueTypes;
		__val = "";
		__last_result = ERR;
		__val_type = string_value;
		__callback =NULL;
		__nval_final = nval;
		__r_node_type ="";
	}

	inline void interpret(
			Enums::NodeValueTypes::InterpretationMode _mode,
			size_t arg=0)
	{
		using namespace Enums::NodeValueTypes;
		std::string ival = _mode == VAL ? __val : __args[arg];
		using namespace Enums::NodeValueTypes;
		switch(__val_type)
		{
			case integer_value:
				__nval_final->setInt(atoi(ival.c_str()));
				break;
			case enum_value:
				__nval_final->setInt( (int) __callback(ival));
				break;
			case float_value:
				__nval_final->setFloat(atof( ival.c_str()));
				break;
			case widget_state:{
				WidgetStateParser parser;
				Enums::WidgetTypes::States st = parser.parse(__args[0]);
				Nodes::NodeValue* state_val = __broadcaster->getState(st);
				Nodes::ReadType type = state_val->getType();
				if (type == integer_value)
					__nval_final->setInt(state_val->getInt());
				else if (type == string_value)
					__nval_final->setString(state_val->getString());
			default:
				__nval_final->setString(ival);
		}
	}
};

}//namespace Parsers


#endif /* TOKENTESTPACKAGE_H_ */
