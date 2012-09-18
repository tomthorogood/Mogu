/*
 * TokenGenerator.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: tom
 */

#include <Parsers/TokenGenerator.h>

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

	/* Determine if the current char is part of a wrapping pair.
	 *
	 * If the current character starts a pair, `reqd_end` will be equal
	 * to the second half of the pair. Otherwise, the match will
	 * be searching for the delimiter.
	 */
	char reqd_end = starts_pair(chcurr);
	if (reqd_end != (char)0)chmatch = reqd_end;

	/* Finds either the index of the next instance of the delimiter or the
	 * second part of the wrapping pair.
	 *
	 * Start with the next char, in case the pairs are the same char.
	 */
	size_t tend = __orig.find(chmatch, __cpos+1);
	if (tend == string::npos)
	{ // If no match can be found, goes to the end of the line.
		tend = (__orig.length());
	}

	/* If we've searched for and found an appropriate match for the second
	 * part of the wrapping, we'll set the wrapped value to true.
	 */
	if (chmatch != delim && tend != __orig.length()) wrapped = true;

	/* Finds the length of the substring that we'll actually be returning */
	int tlen = (tend - __cpos);
	string token = wrapped?
        __orig.substr(__cpos,tlen+1) : __orig.substr(__cpos, tlen);

	/* Now we need to prep for the next iteration. */
	size_t tmp = tend;
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

