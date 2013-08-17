/*
 * Token_Manager.h
 *
 *  Created on: May 7th, 2013
 *      Author: cameron
 */

#ifndef TOKENMANAGER_H_
#define TOKENMANAGER_H_

#include <Types/StringMap.h>
#include <Types/syntax.h>

#include <string>
#include <unordered_set>
#include <vector>
#include <cassert>

namespace Parsers {

	//centralized token groups for decision logic in the various
	//parsers
    const static std::unordered_set<int> widget_tokens = {
		 (int) Mogu_Syntax::own,
		 (int) Mogu_Syntax::widget
	};

    const static std::unordered_set<int> state_tokens = {
	     (int) Mogu_Syntax::style,
	     (int) Mogu_Syntax::text,
	     (int) Mogu_Syntax::hidden,
	     (int) Mogu_Syntax::source,
	     (int) Mogu_Syntax::location,
	     (int) Mogu_Syntax::index
	};

    const static std::unordered_set<int> db_tokens = {
		 (int) Mogu_Syntax::user,
		 (int) Mogu_Syntax::session,
		 (int) Mogu_Syntax::group,
		 (int) Mogu_Syntax::data
	};
	
    const static std::unordered_set<int> object_tokens = {
		 (int) Mogu_Syntax::own,
		 (int) Mogu_Syntax::user,
		 (int) Mogu_Syntax::session,
		 (int) Mogu_Syntax::group,
		 (int) Mogu_Syntax::data,
		 (int) Mogu_Syntax::slot,
		 (int) Mogu_Syntax::widget,
         (int) Mogu_Syntax::key
	};

	inline bool is_state_token(int token) 
	{
	    return state_tokens.count( token) == 1;
	}

	inline bool is_widget_token(int token) 
	{
		return widget_tokens.count( token) == 1;
	}

	inline bool is_db_token(int token) 
	{
		return db_tokens.count( token) == 1;
	}

	inline bool is_object_token(int token) 
	{
		return object_tokens.count( token) == 1;
	}

	inline bool is_preposition_token(int token) 
	{
        return token == Mogu_Syntax::preposition;
	}

//class for maintaining a consistent list of tokens between
//parsers, string token tracking, and funky bidirectional
//iteration we do while parsing mogu commands
class Token_Manager
{
	typedef std::vector<int>::iterator fwd_iterator;

	public:
		Token_Manager() {}

		//set of functions for replacing sets of tokens with a single
		//token (i.e. upon completion of StateParser and MathParser)
		//WARNING: NEED TO PREVENT REALLOCATION ON inject_token, ELSE
		//ALL ITERATORS ARE INVALIDATED AND THIS MANAGER BREAKS!!
		//though we shouldn't have a reallocation because we only
		//inject a token after we delete several tokens.
		void delete_to_saved();
		void delete_from_saved();
		void truncate_head();

        inline void reset()
        {
            i_tokens.clear();
            s_tokens.clear();
            index = 0 ;
            begin_ptr = i_tokens.begin();
        }
        inline void add_token(int num_token)
        {
            i_tokens.push_back(num_token);
            begin_ptr = i_tokens.begin();
        }
        inline void add_token(std::string str_token)
        {
            i_tokens.push_back((int) Mogu_Syntax::TOKEN_DELIM);
            s_tokens.set(i_tokens.size()-1, str_token);
            begin_ptr = i_tokens.begin();
        }
        //must call this function directly after all tokens are added
        //or else we will have undefined behavior!
        inline void end()
        {
            //place iterator at end of numerical token vector
            it = i_tokens.end() - 1;
            update_index();
        }
        /* Like 'setiterator', but returns to the beginning, for when
         * we will not be stepping backward through the input.
         */
        inline void begin() {
            it = i_tokens.begin();
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

        inline void save_location()
        {
            if (it >= i_tokens.end())
            {
                realign_iterators();
            }
            savedit = it;
        }

		//token access methods
        inline int current_token() const
        {
            if (it < i_tokens.begin())
                return Mogu_Syntax::OUT_OF_RANGE_BEGIN;
            else if (it >= i_tokens.end()) 
                return Mogu_Syntax::OUT_OF_RANGE_END;
            else
                return *it;
        }

        inline std::string fetch_string()
        {
            if(is_delim()) return s_tokens.get(index);
            return "";
        }

        inline bool is_quoted_string(const std::string& str) const
        {
            return str[0] == '"';
        }

        //call this directly after delete_to_saved()!
        inline void inject_token(int num_token)
        {
            it = i_tokens.insert(it, num_token);
            update_index();
        }
        //call this directly after delete_to_saved()!
        inline void inject_token(std::string str_token)
        {
            it = i_tokens.insert(it, (int) Mogu_Syntax::TOKEN_DELIM);
            s_tokens.set(index, str_token);
            update_index();
        }
		
        //in the case that our object-chain turns out to be
		//non-resolvable, return to our saved location without
		//deleting any tokens
        inline void return_to_saved()
        {
            it = savedit;
            update_index();
        }


		size_t size() { return i_tokens.size();}

	private:
        size_t index {};
        size_t sz {};
		std::vector<int> i_tokens {};
		String_Map s_tokens {};
		fwd_iterator it {};
		fwd_iterator savedit {};
		fwd_iterator begin_ptr {};

		void update_string_indexes();


		inline bool is_delim()
        {
            return current_token() == Mogu_Syntax::TOKEN_DELIM;
        }

		/* After a more complicated operation, this makes sure the index
		 * is correctly updated. As this is an expensive method, it's always
		 * better to use ++ and -- when possible.
		 */
		inline void update_index() {
		    sz = i_tokens.size();
		    index = i_tokens.size() - (i_tokens.end()-it);
		}

		inline size_t get_index(fwd_iterator& iter) {
		    return i_tokens.size() - (i_tokens.end()-iter);
		}

		/* If the vector was completely reallocated, its start pointer will
		 * have changed.
		 */
		inline bool was_reallocated() { return begin_ptr != i_tokens.begin();}

		/* In the case of a reallocation, we must update the __it
		 * , __savedit, and __begin variables.
		 */
		inline void realign_iterators() {
		    int saved_offset = savedit - begin_ptr;
		    begin_ptr = i_tokens.begin();
		    it = begin_ptr + index;
		    if (saved_offset > 0)
		        savedit = begin_ptr + saved_offset;
		    else
		        savedit = begin_ptr;
		}
};


}	// namespace Parsers
#endif /* TOKENMANAGER_H_ */
