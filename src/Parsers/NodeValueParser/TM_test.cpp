#include <Parsers/NodeValueParser/TokenManager.h>
#include <iostream>

using namespace std;

int main()
{
	Parsers::TokenManager tm;
	tm.addToken(1);
	tm.addToken(R"(First)");
	tm.addToken(2);
	tm.addToken(3);
	tm.addToken(R"(Second)");
	tm.addToken(4);
	tm.printTokens();

	cout << "tm tokens in reverse: ";
	tm.setIterator();
	while(tm.currentToken() != (int) Parsers::TokenManager::OutOfRange::Begin)
	{
		if(tm.currentToken() != (int) MoguSyntax::TOKEN_DELIM)
			cout << tm.currentToken() << " ";
		else
			cout << tm.fetchStringToken() << " ";

		tm.prev();
	}
	cout << endl;

	cout << endl;

	tm.setIterator();
	tm.prev();
	tm.prev();
	tm.saveLocation();
	tm.next();
	tm.next();
	tm.deleteFromSaved();
	//tm.injectToken(R"(Third)");
	tm.injectToken(9);
	tm.printTokens();
	cout << "current token: " << tm.currentToken() << endl;

	tm.printStrIndex();

	cout << endl;

	tm.prev();
	tm.prev();
	tm.prev();
	tm.saveLocation();
	tm.next();
	tm.next();
	tm.next();
	cout << "pre-delete" << endl;
	tm.deleteFromSaved();
	cout << "pre-inject" << endl;
	tm.injectToken(R"(Final)");
	cout << "pre-print" << endl;
	tm.printTokens();



	return 0;
}
