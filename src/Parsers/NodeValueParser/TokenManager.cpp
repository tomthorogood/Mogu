/*
 * TokenManager.cpp
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

namespace Parsers {

TokenManager::TokenManager()
{
}

void TokenManager::reset()
{
	__numTokens.clear();
	__strTokens.clear();
}

void TokenManager::addToken(int numToken)
{
	__numTokens.push_back(numToken);
}

void TokenManager::addToken(std::string strToken)
{
	__numTokens.push_back((int) MoguSyntax::TOKEN_DELIM);
	__strTokens.push_back(strToken);
}

//must call this function directly after all tokens are added
//or else we will have undefined behavior!
void TokenManager::setIterator()
{
	__revit = __numTokens.rbegin();
	__strIndex = __strTokens.size();	//initially points out-of-bounds

	if(*__revit == (int) MoguSyntax::TOKEN_DELIM)
		__strIndex--;		//in case we don't have a prev() call to properly 
						//line us up
}

int TokenManager::currentToken()
{
	//internally we use a reverse iterator, but externally we are to
	//think of the tokens being arranged in the traditional forwards
	//direction.  hence the out-of-bounds signals are reversed
	if(__revit >= __numTokens.rend())
		return OutOfRange::Begin;

	else if(__revit < __numTokens.rbegin())
		return OutOfRange::End;

	else
		return *__revit;
}



std::string TokenManager::fetchStringToken()
{
	if(*__revit == (int) MoguSyntax::TOKEN_DELIM)
		return __strTokens[__strIndex];

	else
		return R"(ERR: DEREFERENCING NON-TOKENDELIM");
}

//TODO: we are dereferencing our iterator twice; once on iterator
//incr/decr, and again when/if we fetchStringToken.  refactor

void TokenManager::next()
{
	__revit--;
	if(*__revit == (int) MoguSyntax::TOKEN_DELIM)
	   __strIndex++;	
}

void TokenManager::prev()
{
	__revit++;
	if(*__revit == (int) MoguSyntax::TOKEN_DELIM)
		__strIndex--;
}

void TokenManager::saveLocation()
{
	__savedrit = __revit;
}

void TokenManager::deleteToSaved()
{
	//up to and including saved loc.  also, update our iterator.
	__revit =  __numTokens.erase(__revit, __savedrit+1);	

	//TODO: updated strIndex as needed (deleted strings)
}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(int numToken)
{
	//ends pointing to our newly inserted element
	__revit = __numTokens.insert(__revit, numToken);

}

//call this directly after deleteToSaved()!
void TokenManager::injectToken(std::string strToken)
{
	__revit = __numTokens.insert(__revit, (int) MoguSyntax::TOKEN_DELIM);
	__strTokens.insert(__numTokens.begin()+strIndex, strToken);



}	// namespace Parsers
