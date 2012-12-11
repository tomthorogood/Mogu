/*
 * T_TokenGenerator.cpp
 *
 *  Created on: Sep 17, 2012
 *      Author: tom
 */

#include <Mogu.h>
#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>
#include <Core/MoldableTemplate.h>
#include <TurnLeftLib/TestSuite/TestSuite.h>
#include <Parsers/Parsers.h>
#include <tests/WtTestSetup.h>

using namespace Parsers;

TestValue __m_named_state (va_list* args)
{
	std::string s_val_in = va_arg(args, const char*);
	Moldable* w = va_arg(args, Moldable*);
	NodeValue v;
	Parsers::NodeValueParser(s_val_in,&v,w);
	TestValue result(v.getString());
	return result;
}

class T_NodeValueParser : public Test
{
	WtTestApp* app;
	Moldable* add;
	std::string &value_in;

public:
	T_NodeValueParser(
			MoldableTemplate* tpl
			,std::string& __value_in
			,std::string tname
			,TestValue* expected
			,Evaluation* evaluation
			,ResultType::type r_type = ResultType::t_str)
	:	Test(tname, r_type, expected, evaluation)
		,value_in(__value_in)
	{
		app = createWtTest();
		add = addTextWidget(*app, tpl);
	}
	~T_NodeValueParser() { delete app; }
	virtual bool run()
	{
		__result = *__evaluation->run(2,value_in.c_str(), add).getResult();
		return compareResults();
	}

};

namespace TestReports{

std::string Suite_NodeValueParser()
{
	TestSuite suite("NodeValueParser Suite");
	std::string __hello  		= "Hello, World!";
	std::string __hello_name 	= "HelloWidget";
	std::string __hello_r 	= "|HelloWidget| $value$";

	Evaluation __e_named_state(ResultType::t_str, &__m_named_state);
	TestValue __v_named_state(__hello);

	MoldableTemplate* __tpl_named_state = new MoldableTemplate();
	__tpl_named_state->content = __hello;
	__tpl_named_state->type = Enums::WidgetTypes::text;
	__tpl_named_state->name = __hello_name;
	__tpl_named_state->flags |= Enums::WidgetTypes::is_named;

	T_NodeValueParser t1(
			__tpl_named_state
			,__hello_r
			,__hello_r
			,&__v_named_state
			,&__e_named_state);

	suite.add_test(&t1);

	suite.run();
	return suite.report();
}

}//namespace TestReports
