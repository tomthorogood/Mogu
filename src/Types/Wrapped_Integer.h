/*
 * Wrapped_Integer.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef WRAPPED_INTEGER_H_
#define WRAPPED_INTEGER_H_

#include <declarations.h>
#include <WrappedValue.h>

class Wrapped_Integer : public WrappedValue<int>
{
public:
	Wrapped_Integer(std::string husk) : WrappedValue<int>('^','^',husk){}
	inline int interpret(uint8_t nargs=0,...)
	{
		std::string str = unwrap();
		return atoi(str.c_str());
	}
};


#endif /* WRAPPED_INTEGER_H_ */
