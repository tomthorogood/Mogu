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
    const static std::unordered_set<int> __widgetTokens = {
		 (int) MoguSyntax::own,
		 (int) MoguSyntax::widget
	};

    const static std::unordered_set<int> __stateTokens = {
	     (int) MoguSyntax::style,
	     (int) MoguSyntax::text,
	     (int) MoguSyntax::hidden,
	     (int) MoguSyntax::source,
	     (int) MoguSyntax::location,
	     (int) MoguSyntax::index
	};

    const static std::unordered_set<int> __dbTokens = {
		 (int) MoguSyntax::user,
		 (int) MoguSyntax::session,
		 (int) MoguSyntax::group,
		 (int) MoguSyntax::data
	};
	
    const static std::unordered_set<int> __objectTokens = {
		 (int) MoguSyntax::own,
		 (int) MoguSyntax::user,
		 (int) MoguSyntax::session,
		 (int) MoguSyntax::group,
		 (int) MoguSyntax::data,
		 (int) MoguSyntax::slot,
		 (int) MoguSyntax::widget,
         (int) MoguSyntax::key
	};

	inline bool isStateToken(int token) 
	{
	    return __stateTokens.count( token) == 1;
	}

	inline bool isWidgetToken(int token) 
	{
		return __widgetTokens.count( token) == 1;
	}

	inline bool isDBToken(int token) 
	{
		return __dbTokens.count( token) == 1;
	}

	inline bool isObjectToken(int token) 
	{
		return __objectTokens.count( token) == 1;
	}

	inline bool isPrepositionToken(int token) 
	{
        return token == MoguSyntax::preposition;
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

		//initialization methods
		TokenManager();
        inline void reset()
        {
            __numTokens.clear();
            __strTokens.clear();
            __index = 0;
        }
        inline void addToken(int numToken)
        {
            __numTokens.push_back(numToken);
            __begin = __numTokens.begin();
        }
        inline void addToken(std::string strToken)
        {
            __numTokens.push_back((int) MoguSyntax::TOKEN_DELIM);
            __strTokens.set(__numTokens.size()-1, strToken);
            __begin = __numTokens.begin();
        }
        //must call this function directly after all tokens are added
        //or else we will have undefined behavior!
        inline void end()
        {
            //place iterator at end of numerical token vector
            __it = __numTokens.end() - 1;
            updateIndex();
        }
        /* Like 'setiterator', but returns to the beginning, for when
         * we will not be stepping backward through the input.
         */
        inline void begin() {
            __it = __numTokens.begin();
            __index = 0;
        }

		//methods for navigating the token list.
        inline void next()
        {
            ++__it;
            ++__index;
        }

        inline void prev()
        {
            --__it;
            --__index;
        }

        inline void saveLocation()
        {
            __savedit = __it;
        }

		//token access methods
        inline const SyntaxDef& currentToken() const
        {
            return
                (__it < __numTokens.begin())? MoguSyntax::OUT_OF_RANGE_BEGIN :
                (__it > __numTokens.end())  ? MoguSyntax::OUT_OF_RANGE_END   :
                MoguSyntax::get(*__it);
        }

        inline std::string fetchStringToken()
        {
            if(isTokenDelim())
                return __strTokens.get(__index);
            else
                return R"(ERR: DEREFERENCING NON-TOKENDELIM)";
        }
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
		void deleteToSaved();
		void deleteFromSaved();
		void truncateHead();
        //call this directly after deleteToSaved()!
        inline void injectToken(int numToken)
        {
            __it = __numTokens.insert(__it, numToken);
            updateIndex();
        }
        //call this directly after deleteToSaved()!
        inline void injectToken(std::string strToken)
        {
            __it = __numTokens.insert(__it, (int) MoguSyntax::TOKEN_DELIM);
            __strTokens.set(__index, strToken);
            updateIndex();
        }
		
        //in the case that our object-chain turns out to be
		//non-resolvable, return to our saved location without
		//deleting any tokens
        inline void returnToSaved()
        {
            __it = __savedit;
            updateIndex();
        }


		size_t size() { return __numTokens.size();}

		//debug methods
		void printTokens();
		void printNumTokens();
		void printStringTokens();

	private:
		//readability method
		inline bool isTokenDelim()
        {
            return currentToken() == MoguSyntax::TOKEN_DELIM;
        }

		void updateStringIndexes();

		//token vectors
		std::vector<int> __numTokens;
		StringMap __strTokens;


		//indexes our current token/stringToken
		fwd_iterator __it;
		fwd_iterator __savedit;
		fwd_iterator __begin;
		size_t __index;

		/* After a more complicated operation, this makes sure the index
		 * is correctly updated. As this is an expensive method, it's always
		 * better to use ++ and -- when possible.
		 */

		size_t sz =0;

		inline void updateIndex() {
		    sz = __numTokens.size();
		    __index = __numTokens.size() - (__numTokens.end()-__it);
		}

		inline size_t getIndex(fwd_iterator& iter) {
		    return __numTokens.size() - (__numTokens.end()-iter);
		}

		/* If the vector was completely reallocated, its start pointer will
		 * have changed.
		 */
		inline bool wasReallocated() { return __begin != __numTokens.begin();}

		/* In the case of a reallocation, we must update the __it
		 * , __savedit, and __begin variables.
		 */
		inline void realignIterators() {
		    int saved_offset = __savedit - __begin;
		    __begin = __numTokens.begin();
		    __it = __begin + __index;
		    if (saved_offset > 0)
		        __savedit = __begin + saved_offset;
		    else
		        __savedit = __begin;
		}
};


}	// namespace Parsers
#endif /* TOKENMANAGER_H_ */
