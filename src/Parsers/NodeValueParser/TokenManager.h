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
#include <cassert>

namespace Parsers {

	//centralized token groups for decision logic in the various
	//parsers
    const static std::unordered_set<int> widgetTokens = {
		 (int) MoguSyntax::own,
		 (int) MoguSyntax::widget
	};

    const static std::unordered_set<int> stateTokens = {
	     (int) MoguSyntax::style,
	     (int) MoguSyntax::text,
	     (int) MoguSyntax::hidden,
	     (int) MoguSyntax::source,
	     (int) MoguSyntax::location,
	     (int) MoguSyntax::index
	};

    const static std::unordered_set<int> dbTokens = {
		 (int) MoguSyntax::user,
		 (int) MoguSyntax::session,
		 (int) MoguSyntax::group,
		 (int) MoguSyntax::data
	};
	
    const static std::unordered_set<int> objectTokens = {
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
	    return stateTokens.count( token) == 1;
	}

	inline bool isWidgetToken(int token) 
	{
		return widgetTokens.count( token) == 1;
	}

	inline bool isDBToken(int token) 
	{
		return dbTokens.count( token) == 1;
	}

	inline bool isObjectToken(int token) 
	{
		return objectTokens.count( token) == 1;
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
            numTokens.clear();
            strTokens.clear();
            index = 0;
            begin_ptr = numTokens.begin();
        }
        inline void addToken(int numToken)
        {
            numTokens.push_back(numToken);
            begin_ptr = numTokens.begin();
        }
        inline void addToken(std::string strToken)
        {
            numTokens.push_back((int) MoguSyntax::TOKEN_DELIM);
            strTokens.set(numTokens.size()-1, strToken);
            begin_ptr = numTokens.begin();
        }
        //must call this function directly after all tokens are added
        //or else we will have undefined behavior!
        inline void end()
        {
            //place iterator at end of numerical token vector
            it = numTokens.end() - 1;
            updateIndex();
        }
        /* Like 'setiterator', but returns to the beginning, for when
         * we will not be stepping backward through the input.
         */
        inline void begin() {
            it = numTokens.begin();
            index = 0;
        }

		//methods for navigating the token list.
        inline void next()
        {
            ++it;
            ++index;
        }

        inline void prev()
        {
            --it;
            --index;
        }

        inline void saveLocation()
        {
            if (it >= numTokens.end())
            {
                realignIterators();
            }
            savedit = it;
        }

		//token access methods
        inline int currentToken() const
        {
            if (it > numTokens.end()) std::cout << "HOW IS THIS POSSIBLE?!";
            return
                (it < numTokens.begin())? MoguSyntax::OUT_OF_RANGE_BEGIN :
                (it >= numTokens.end())  ? MoguSyntax::OUT_OF_RANGE_END   :
                *it;
        }

        inline std::string fetchStringToken()
        {
            if(isTokenDelim())
                return strTokens.get(index);
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
            it = numTokens.insert(it, numToken);
            updateIndex();
        }
        //call this directly after deleteToSaved()!
        inline void injectToken(std::string strToken)
        {
            it = numTokens.insert(it, (int) MoguSyntax::TOKEN_DELIM);
            strTokens.set(index, strToken);
            updateIndex();
        }
		
        //in the case that our object-chain turns out to be
		//non-resolvable, return to our saved location without
		//deleting any tokens
        inline void returnToSaved()
        {
            it = savedit;
            updateIndex();
        }


		size_t size() { return numTokens.size();}

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
		std::vector<int> numTokens;
		StringMap strTokens;


		//indexes our current token/stringToken
		fwd_iterator it;
		fwd_iterator savedit;
		fwd_iterator begin_ptr;
		size_t index;

		/* After a more complicated operation, this makes sure the index
		 * is correctly updated. As this is an expensive method, it's always
		 * better to use ++ and -- when possible.
		 */

		size_t sz =0;

		inline void updateIndex() {
		    sz = numTokens.size();
		    index = numTokens.size() - (numTokens.end()-it);
		}

		inline size_t getIndex(fwd_iterator& iter) {
		    return numTokens.size() - (numTokens.end()-iter);
		}

		/* If the vector was completely reallocated, its start pointer will
		 * have changed.
		 */
		inline bool wasReallocated() { return begin_ptr != numTokens.begin();}

		/* In the case of a reallocation, we must update the __it
		 * , __savedit, and __begin variables.
		 */
		inline void realignIterators() {
		    int saved_offset = savedit - begin_ptr;
		    begin_ptr = numTokens.begin();
		    it = begin_ptr + index;
		    if (saved_offset > 0)
		        savedit = begin_ptr + saved_offset;
		    else
		        savedit = begin_ptr;
		}
};


}	// namespace Parsers
#endif /* TOKENMANAGER_H_ */
