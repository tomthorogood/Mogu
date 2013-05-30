/*
 * TokenManager.h
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

#ifndef TOKENMANAGER_H_
#define TOKENMANAGER_H_

#include <declarations.h>

#include <string>
#include <unordered_set>
#include <vector>

namespace Parsers {

	//centralized token groups for decision logic in the various
	//parsers
	const std::unordered_set<int> __widgetTokens = {
		(int) MoguSyntax::own,
		(int) MoguSyntax::parent,
		(int) MoguSyntax::child,
		(int) MoguSyntax::widget
	};

	const std::unordered_set<int> __dbTokens = {
		(int) MoguSyntax::user,
		(int) MoguSyntax::session,
		(int) MoguSyntax::group,
		(int) MoguSyntax::data
	};
	
	const std::unordered_set<int> __flippedActionTokens = {
		(int) MoguSyntax::remove,
		(int) MoguSyntax::append
	};

	const std::unordered_set<int> __objectTokens = {
		(int) MoguSyntax::own,
		(int) MoguSyntax::parent,
		(int) MoguSyntax::user,
		(int) MoguSyntax::session,
		(int) MoguSyntax::group,
		(int) MoguSyntax::child,
		(int) MoguSyntax::data,
		(int) MoguSyntax::slot,
		(int) MoguSyntax::widget
	};

	const std::unordered_set<int> __prepositionTokens = {
		(int) MoguSyntax::to,
		(int) MoguSyntax::as,
		(int) MoguSyntax::at
	};

	inline bool isWidgetToken(MoguSyntax token)
	{
		return __widgetTokens.count((int) token) == 1;
	}

	inline bool isDBToken(MoguSyntax token)
	{
		return __dbTokens.count((int) token) == 1;
	}

	inline bool isFlippedActionToken(MoguSyntax token)
	{
		return __flippedActionTokens.count((int) token) == 1;
	}

	inline bool isObjectToken(MoguSyntax token)
	{
		return __objectTokens.count((int) token) == 1;
	}

	inline bool isPrepositionToken(MoguSyntax token)
	{
		return __prepositionTokens.count((int) token) == 1;
	}

//class for maintaining a consistent list of tokens between
//parsers, string token tracking, and funky bidirectional
//iteration we do while parsing mogu commands
class TokenManager
{
	typedef std::vector<int>::iterator fwd_iterator;

	//these arbitrarily-picked values need to be greater than the
	//'largest' syntax enumeration and smaller than the 'smallest'
	//operator enumeration


	//TODO: inline where appropriate
	public:
		//TODO: refactor using generated syntax
		enum class OutOfRange { Begin = 2147483640, End = 2147483641 };

		TokenManager();
		void reset();
		void addToken(int numToken);
		void addToken(std::string strToken);
		//MUST set the iterator after input command has been tokenized
		void setIterator();

		//methods for navigating the token list.
        template <typename T> T currentToken();

        std::string fetchStringToken();
		void next();
		void prev();

		//set of functions for replacing sets of tokens with a single
		//token (i.e. upon completion of StateParser and MathParser)
		//WARNING: NEED TO PREVENT REALLOCATION ON injectToken, ELSE
		//ALL ITERATORS ARE INVALIDATED AND THIS MANAGER BREAKS!!
		//though we shouldn't have a reallocation because we only
		//inject a token after we delete several tokens.
		void saveLocation();
		void deleteToSaved();
		void deleteFromSaved();
		void injectToken(int numToken);
		void injectToken(std::string strToken);

		//debug methods
		void printTokens();
		void printNumTokens();
		void printStringTokens();

	private:
		std::vector<int> __numTokens;
		std::vector<std::string> __strTokens;

		fwd_iterator __it;
		fwd_iterator __savedit;

		//when we save a location for deletion, this variable keeps
		//track of how many strings are to be deleted so we can
		//updated strIndex accordingly.
		int __delStringCount = -1;


		//keeps track of which string corresponds to which tokendelim
		int __strIndex;
};


}	// namespace Parsers
#endif /* TOKENMANAGER_H_ */
