#include <Parsers/NodeValueParser/TokenManager.h>
#include <Parsers/NodeValueParser/MathParser.h>
#include <Types/syntax.h>

#include <iostream>

using namespace std;
using namespace Parsers;

int main()
{
	TokenManager tm;
	MathParser mp;

	mp.setTokenManager(tm);

	/*
	tm.addToken((int) MoguSyntax::OPER_OPPAREN);
	tm.addToken(3);
	tm.addToken((int) MoguSyntax::OPER_PLUS);
	tm.addToken(4);
	tm.addToken((int) MoguSyntax::OPER_CLPAREN);
	tm.setIterator();
	tm.printTokens();

	tm.prev();
	tm.prev();
	tm.prev();
	tm.prev();

	mp.processInput();
	tm.printTokens();

	tm.reset();
	tm.addToken((int) MoguSyntax::OPER_OPPAREN);
	tm.addToken(7);
	tm.addToken((int) MoguSyntax::OPER_DIV);
	tm.addToken(7);
	tm.addToken((int) MoguSyntax::OPER_CLPAREN);

	tm.printTokens();

	mp.processInput();
	tm.printTokens();

	tm.reset();*/
	// (5 + 6*23/6 - 3*49/7)
	tm.addToken((int) MoguSyntax::OPER_OPPAREN);
	tm.addToken(5);
	tm.addToken((int) MoguSyntax::OPER_PLUS);
	tm.addToken(6);
	tm.addToken((int) MoguSyntax::OPER_MULT);
	tm.addToken(23);
	tm.addToken((int) MoguSyntax::OPER_DIV);
	tm.addToken(6);
	tm.addToken((int) MoguSyntax::OPER_MINUS);
	tm.addToken(3);
	tm.addToken((int) MoguSyntax::OPER_MULT);
	tm.addToken(49);
	tm.addToken((int) MoguSyntax::OPER_DIV);
	tm.addToken(7);
	tm.addToken((int) MoguSyntax::OPER_CLPAREN);
	tm.setIterator();
	for(int i=0; i<14; i++) tm.prev();
	mp.processInput();
	// should be 7

	tm.printTokens();

	return 0;
}
