/*
 * CreateKey.cpp
 *
 *  Created on: Jul 22, 2012
 *      Author: tom
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <stdint.h>
using namespace std;

int main()
{
	int bytes[16];
	ofstream filestream;
	filestream.open("KeyDefinitions.h");
	srand(time(NULL));

	for (int i = 0; i < 16; i++)
	{
		bytes[i] = rand() % 255;
	}

	filestream << "#define BF_RANDOM_BYTES" << endl;
	for (int i = 0; i < 16; i++)
	{
		filestream << "define BF_RANDOM_BYTE_"<< i << " " << bytes[i] << endl;
	}

	filestream << "#endif" << endl;
	filestream.close();
	return 0;
}

