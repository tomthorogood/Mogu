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
#include <sstream>


using namespace Parsers;
using namespace std;

int main()
{
	MathParser m;
	std::stringstream ss;

	vector<string> input;
	
	// TEST 1
	input.clear();
	input.push_back("2147483642");
	input.push_back("4");		
	input.push_back("2147483644");
	input.push_back("3");
	input.push_back("2147483643");
	input.push_back("2147483647");
	input.push_back("7");

	cout << "math expression: (4 + 3) / 7" << endl;
	cout << "input to mathparser: ";

	for(int i=0; i<input.size(); i++)
		cout << input[i] << " ";
	cout << endl;

	cout << "*** output value: ";
	cout << m.evaluate(input) << endl;

	// TEST 2
	input.clear();
	input.push_back("5");
	input.push_back("2147483645");
	input.push_back("1");

	cout << "math expression: 5 - 1" << endl;
	cout << "input to mathparser: ";

	for(int i=0; i<input.size(); i++)
		cout << input[i] << " ";
	cout << endl;

	cout << "*** output value: ";
	cout << m.evaluate(input) << endl;


	// TEST 3
	input.clear();
	input.push_back("4");
	input.push_back("2147483644");
	input.push_back("2147483642");
	input.push_back("6");		
	input.push_back("2147483646");
	input.push_back("3");
	input.push_back("2147483644");
	input.push_back("2");
	input.push_back("2147483643");
	input.push_back("2147483647");
	input.push_back("2");
	input.push_back("2147483645");
	input.push_back("2147483642");
	input.push_back("2147483642");
	input.push_back("5");
	input.push_back("2147483644");
	input.push_back("6");
	input.push_back("2147483643");
	input.push_back("2147483646");
	input.push_back("2");
	input.push_back("2147483643");

	cout << "math expression: 4 + (6 * 3 + 2) / 2 - ( (5+6) * 2 )" << endl;
	cout << "input to mathparser: ";

	for(int i=0; i<input.size(); i++)
		cout << input[i] << " ";
	cout << endl;

	cout << "*** output value: ";
	cout << m.evaluate(input) << endl;
	

	return 0;
}
