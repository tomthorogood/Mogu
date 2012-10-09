/*
 * run_tests.cpp
 *
 *  Created on: Sep 17, 2012
 *      Author: tom
 */

#include <iostream>
#include "Tests.h"
using namespace std;
int main()
{
	cout << TestReports::T_MoguScript_Tokenizer() << endl;
	cout << TestReports::Suite_NodeValueParser() << endl;
}


