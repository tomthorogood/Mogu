/*
 * AttributeParserTest.cpp
 *
 *  Created on: March 24th, 2013
 *      Author: cameron
 *
 *      this class tests the functionality
 *      of the AttributeParser class.
 */
#include <Parsers/AttributeParser.h>
#include <Types/NodeValue.h>
#include <iostream>



using namespace Parsers;
using namespace std;

int main()
{
	AttributeParser a;
	NodeValue v;
	a.giveInput("haha these are \"some tokens.\"", v);

	return 0;
}
