/*
 * Validators.cpp
 *
 *  Created on: Aug 14, 2012
 *      Author: tom
 */


#include <Validators/Validators.h>

#include <Types/Enums.h>
#include <Parsers/NodeValueParser.h>
#include <Parsers/StyleParser.h>
#include <Parsers/Parsers.h>
#include <Wt/WValidator>
#include <Wt/WRegExpValidator>
#include <Redis/RedisCore.h>
#include <Types/NodeValue.h>

namespace Validators{

using namespace Parsers::StyleParser;
using namespace Enums::Validators;
using Parsers::NodeValueParser;
using Parsers::ValidatorTypeParser;
using std::string;
using Goo::Moldable;

Wt::WValidator* createValidator(Moldable* widget)
{
	Wt::WValidator* validator =0;
	string validatorName = getWidgetValidator(widget);

	string validator_node = "validators."+validatorName;
	Redis::command("hget", validator_node, "type");
	string vtype = Redis::toString();
	Nodes::NodeValue vval;
	Parsers::NodeValueParser nparser (
			vtype, &vval,widget,
			Parsers::enum_callback <Parsers::ValidatorTypeParser>);
	switch (vval.getInt())
	{
	case regex:{
		validator = (Wt::WValidator*) createRegexValidator(validator_node);
		break;}
	}
	return validator;
}

Wt::WRegExpValidator* createRegexValidator(const string& node)
{
	Wt::WRegExpValidator* validator =0;
	Redis::command("hget", node, "test");
	string pattern = Redis::toString();
	Wt::WString wpattern(pattern);
	validator = new Wt::WRegExpValidator(wpattern);


	return validator;
}


}//namespace Validators

