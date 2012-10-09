/*
 * TokenTestPackage.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: tom
 */

#include <Parsers/TokenTestPackage.h>
#include <Parsers/Parsers.h>
#include <Core/Moldable.h>
namespace Parsers
{
void TokenTestPackage::interpret(
			Enums::NodeValueTypes::InterpretationMode _mode,
			size_t arg)
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
				Nodes::NodeValue state_val;
				Enums::WidgetTypes::States st = parser.parse(__args[0]);
				__broadcaster->getState(st, state_val);
				Nodes::ReadType type = state_val.getType();
				if (type == Nodes::int_value)
					__nval_final->setInt(state_val.getInt());
				else if (type == Nodes::string_value)
					__nval_final->setString(state_val.getString());
				break;}

			// Both of these are technically just going to contain strings.
			case registry_value:
			case redis_command:{
				Parsers::NodeValueParser p(__val,&__nval_final);
				break;}
			default:
				__nval_final->setString(ival);
		}
	}
}
