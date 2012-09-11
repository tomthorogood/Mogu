/*
 * TokenGenerator.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: tom
 */

#include <TokenGenerator.h>

namespace Parsers{
using std::string;

TokenGenerator::TokenGenerator(string orig)
{
	__orig = orig;
    __cpos = 0;
}

string TokenGenerator::next(char delim)
{
	/* Don't bother with anything else if we've already reached the end
	 * of the string or somehow surpassed the string's length.
	 */
	if (__cpos >= __orig.length()-1)
	{
		return EMPTY;
	}

	char chcurr = __orig.at(__cpos);
	char chmatch = delim;
    bool wrapped = false;

	/* Determine how many chars are remaining in the string */
	unsigned int crem = (__orig.length() - __cpos);

	/* Determine if the current char is part of a wrapping pair */
	int pairs = __chars.size();
	for (int p = 0; p < pairs; p++)
	{
		if (chcurr == __chars[p].first)
		{
			chmatch = __chars[p].second;
            wrapped = true;
            break;
		}
	}

	/* Finds either the index of the next instance of the delimiter or the
	 * second part of the wrapping pair.
	 */
	int tend = __orig.find(chmatch, __cpos+1);
	if (tend == string::npos)
	{ // If no match can be found, goes to the end of the line.
		tend = (__orig.length());
	}

	/* Finds the length of the substring that we'll actually be returning */
	int tlen = (tend - __cpos);
	string token = wrapped?
        __orig.substr(__cpos,tlen+1) : __orig.substr(__cpos, tlen);

	/* Now we need to prep for the next iteration. */
	int tmp = tend;
	tmp = __orig.find_first_of(delim,tmp);
	if (tmp == string::npos)
	{
		__cpos = __orig.length();
	}
	else
	{
		//Consume the delimiter.
		__cpos = tmp+1;
	}
	return token;
}


}//namespace Parsers

