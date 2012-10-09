/*
 * T_TokenGenerator.cpp
 *
 *  Created on: Sep 17, 2012
 *      Author: tom
 */

#include <Parsers/MoguScript_Tokenizer.h>
#include <TurnLeftLib/TestSuite/TestSuite.h>
using namespace Parsers;

TestValue __m_next (va_list* args)
{
	TokenGenerator* t = va_arg(args, TokenGenerator*);
	TestValue result(t->next());
	return result;
}

TestValue __m_getOriginal (va_list* args)
{
	TokenGenerator* t = va_arg(args, TokenGenerator*);
	TestValue result(t->getOriginal());
	return result;
}

TestValue __m_starts_pair (va_list* args)
{
	TokenGenerator* t = va_arg(args, TokenGenerator*);
	std::cout << __LINE__ << t->starts_pair('{') << std::endl;
	TestValue result(t->starts_pair('{'));
	return result;
}

class T_TokenGenerator : public Test
{
	TokenGenerator* __t;
public:
	T_TokenGenerator(
			TokenGenerator* t
			, std::string t_name
			, TestValue* expected
			, Evaluation* evaluation
			, ResultType::type r_typ =ResultType::t_str)
	:	Test(t_name, r_typ, expected, evaluation)
	{
		__t = t;
	}
	virtual bool run()
	{
		__result = *__evaluation->run(1,__t).getResult();
		return compareResults();
	}
};

namespace TestReports{

std::string T_MoguScript_Tokenizer()
{
	const int num_tokens = 7;
	const int num_tests = num_tokens+2;
	TestSuite __mg_suite("Parsers::MoguScript_Tokenizer");
	MoguScript_Tokenizer __mg;
	TokenGenerator __tg;
	T_TokenGenerator* __mg_tests[num_tests];

	Evaluation __e_m_next(ResultType::t_str,&__m_next);
	Evaluation __e_m_getOriginal(ResultType::t_str,&__m_getOriginal);
	Evaluation __e_m_starts_pair(ResultType::t_chr,&__m_starts_pair);

	TestValue tokens[] = {
			TestValue("{Mary}")
			, TestValue("^had a^")
			, TestValue("^little lamb^")
			, TestValue("whose")
			, TestValue("~fleece~")
			, TestValue("was")
			, TestValue("white")
	};

	std::string token = "";
	for (int i =0; i < num_tokens; i++)
	{
		token += tokens[i].as_string;
		if (i != num_tokens-1) token += " ";

		__mg_tests[i] = new T_TokenGenerator(
				&__mg, "MoguScript_Tokenizer::next()"
				, &tokens[i]
				, &__e_m_next
				);

	}

	__mg.newString(token);
	__tg.newString(token);
	TestValue __t_token(token);
	TestValue __t_end_pr('}');
	TestValue __t_nul((char)0);
	__mg_tests[num_tokens] = new T_TokenGenerator(
			&__mg, "MoguScript_Tokenizer::getOriginal()"
			, &__t_token
			, &__e_m_getOriginal);



	__mg_tests[num_tokens+1] = new T_TokenGenerator(
			&__mg, "MoguScript_Tokenizer::starts_pair('{')"
			, &__t_end_pr
			, &__e_m_starts_pair
			, ResultType::t_chr);


	for (int i =0; i < num_tests; i++)
	{
		__mg_suite.add_test(__mg_tests[i]);
	}
	__mg_suite.run();

	for (int i =0; i < num_tests; i++)
	{
		delete __mg_tests[i];
	}
	return __mg_suite.report();
}

}//namespace TestReports
