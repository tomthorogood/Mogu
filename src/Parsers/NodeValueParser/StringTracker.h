/*
 * StringTracker.h
 *
 *  Created on: March 8, 2013
 *      Author: cameron
 */

#ifndef STRINGTRACKER_H_
#define STRINGTRACKER_H_

#include <unordered_map>
#include <declarations.h>

//temp enums until macros are added
#define SYNTAX_MAX 66
#define OPERATOR_MIN 2147483642

namespace Parsers {

class StringTracker
{
	public:
		StringTracker();
		inline int add(const std::string& newString)
		{
			__map[++current] = newString;
			return current;
		}

		inline const std::string& retrieve(const int& deref) const
		{
			if(current <= deref)
				return __map.at(deref);
			else
				return EMPTY;
		}


	private:
		std::unordered_map<int,std::string> __map;
		static const int START = SYNTAX_MAX + 1;
		static const int MAX = OPERATOR_MIN - 1;

		int current = START;
};

}	// namespace Parsers

#endif /* STRINGTRACKER_H_ */
