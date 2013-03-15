/*
 * MathParserTest.cpp
 *
 *  Created on: March 12th, 2013
 *      Author: cameron
 *
 *      this class tests the functionality
 *      of the MathParser class.
 */
#include <Parsers/MathParser.h>
#include <iostream>
#include <string>

using namespace Parsers;
using namespace std;

int main()
{
	MathParser m;
	
	vector<string> input;
	input.push_back(to_string(MATHSYNTAX_OPENPAREN));
	input.push_back("4");
	input.push_back(to_string(MATHSYNTAX_PLUS));
	input.push_back("3");
	input.push_back(to_string(MATHSYNTAX_CLOSEPAREN));
	input.push_back(to_string(MATHSYNTAX_DIVIDE));
	input.push_back("7");

	cout << "input vector:" << endl;

	for(int i=0; i<input.size(); i++)
		cout << input[i] << " ";
	cout << endl << endl;

	cout << "output value:" << endl;
	cout << m.evaluate(input) << endl;

	return 0;
}
