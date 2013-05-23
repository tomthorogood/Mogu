/*
 * TokenManager.h
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

#ifndef TOKENMANAGER_H_
#define TOKENMANAGER_H_

#include <string>

namespace Parsers {

class TokenManager
{
	//these arbitrarily-picked values need to be greater than the
	//'largest' syntax enumeration and smaller than the 'smallest'
	//operator enumeration

	//TODO: refactor using generated syntax
	enum class OutOfRange { Begin = 2147483640, End = 2147483641 };

	public:
		TokenManager();
		void reset();
		void addToken(int numToken);
		void addToken(std::string strToken);
		//MUST set the iterator after input command has been tokenized
		void setIterator();

		int currentToken();
		std::string fetchStringToken();
		void next();
		void prev();

	private:
		std::vector<int> __numTokens;
		std::vector<std::string> __strTokens;

		std::vector<int>::reverse_iterator __revit;
		//keeps track of which string corresponds to which tokendelim
		size_type strIndex;
};

}	// namespace Parsers
#endif /* TOKENMANAGER_H_ */
