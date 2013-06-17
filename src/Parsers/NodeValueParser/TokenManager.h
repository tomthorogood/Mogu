/*
 * TokenManager.h
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

#ifndef TOKENMANAGER_H_
#define TOKENMANAGER_H_

#include <declarations.h>
#include <Types/StringMap.h>

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

	const std::unordered_set <int> __stateTokens = {
	    (int) MoguSyntax::style,
	    (int) MoguSyntax::text,
	    (int) MoguSyntax::hidden,
	    (int) MoguSyntax::source,
	    (int) MoguSyntax::location,
	    (int) MoguSyntax::index
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

	inline bool isStateToken(MoguSyntax token)
	{
	    return __stateTokens.count((int) token) == 1;
	}

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
		enum class OutOfRange { Begin = 2147483640, End = 2147483639 };

		//initialization methods
		TokenManager();
		void reset();
		void addToken(int numToken);
		void addToken(std::string strToken);
		void end(); // Called for NodeValue input
		void begin(); // Called for CommandValue input

		//methods for navigating the token list.
		void next();
		void prev();

		//token access methods
        template <typename T> T currentToken();
        std::string fetchStringToken();
        bool isQuotedString(const std::string& str) const
        {
            return str[0] == '"';
        }

		//set of functions for replacing sets of tokens with a single
		//token (i.e. upon completion of StateParser and MathParser)
		//WARNING: NEED TO PREVENT REALLOCATION ON injectToken, ELSE
		//ALL ITERATORS ARE INVALIDATED AND THIS MANAGER BREAKS!!
		//though we shouldn't have a reallocation because we only
		//inject a token after we delete several tokens.
		void saveLocation();
		void deleteToSaved();
		void deleteFromSaved();
		void truncateHead();
		void injectToken(int numToken);
		void injectToken(std::string strToken);

		//in the case that our object-chain turns out to be
		//non-resolvable, return to our saved location without
		//deleting any tokens
		void returnToSaved();

		//debug methods
		void printTokens();
		void printNumTokens();
		void printStringTokens();

	private:
		//readability method
		bool isTokenDelim();

		//token vectors
		std::vector<int> __numTokens;
		StringMap __strTokens;


		//indexes our current token/stringToken
		fwd_iterator __it;
		fwd_iterator __savedit;
		size_t __index;

		/* After a more complicated operation, this makes sure the index
		 * is correctly updated. As this is an expensive method, it's always
		 * better to use ++ and -- when possible.
		 */

		inline void updateIndex() {
		    __index = __numTokens.size() - (__numTokens.end()-__it);
		}

		inline size_t getIndex(fwd_iterator& iter) {
		    return __numTokens.size() - (__numTokens.end()-iter);
		}
};


}	// namespace Parsers
#endif /* TOKENMANAGER_H_ */
