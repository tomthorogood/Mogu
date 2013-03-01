/*
 * C_TokenGeneratorTest.cpp
 *
 * 	Created on:	Sep 28, 2013
 * 	Author: cameron
 */

#include <Parsers/TokenGenerator.h>
#include <iostream>

using namespace std;
using namespace Parsers;

int main()
{

	TokenGenerator tokenizer;

	//tokenizer.add_pair('"', '"');	// For escaped double quotes in event lists
	tokenizer.newString("22 1 27 56 \"clicked_style hahahah\"");
	
	for(int i=0; i<5; i++)
	{
		cout << tokenizer.next() << endl;
	}

	return 0;
}

