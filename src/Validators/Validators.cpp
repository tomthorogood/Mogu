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
#include <Wt/WApplication>
namespace Validators{

using namespace Parsers::StyleParser;
using namespace Enums::Validators;
using Parsers::NodeValueParser;
using Parsers::ValidatorTypeParser;
using std::string;
using Goo::Moldable;

Wt::WValidator* createValidator(std::string validatorName)
{
	Wt::WValidator* validator =0;
	string validator_node = "validators."+validatorName;
	string vtype =
	Parsers::StyleParser::getWidgetField(validator_node, "type");
	Nodes::NodeValue vval;
	Parsers::NodeValueParser nparser (
			vtype, vval,NULL,
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
	string pattern =
			Parsers::StyleParser::getWidgetField(node, "test");
	Wt::WString wpattern(pattern);
	validator = new Wt::WRegExpValidator(wpattern);
	return validator;
}


}//namespace Validators

