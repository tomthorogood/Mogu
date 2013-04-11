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
#include <vector>
#include <string>



using namespace Parsers;
using namespace std;

int main()
{
	std::vector<std::string> input;
	AttributeParser a;
	NodeValue v;

	input.push_back("1");	//should be int result
	input.push_back("\"string literal\"");  //should be string result
	input.push_back("data datahash cute");  //should be "monkey"
	input.push_back("data datalist 0");		//should be "monkey"
	input.push_back("data datavalue");		//should be "42"

	a.giveInput("haha these are \"some tokens.\"", v);

	return 0;
}
