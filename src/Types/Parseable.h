/*
 * WrappedValue.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef WRAPPEDVALUE_H_
#define WRAPPEDVALUE_H_

#include <declarations.h>

class Parseable
{
private:
	std::string __value;

protected:
	inline std::string getValue() { return __value;}
public:
	Parseable(std::string value) { __value = value;}
};


template <typename T>
class Parseable_Tmpl : public Parseable
{
public:
	Parseable_Tmpl(std::string value) : Parseable(value){}

	/*!\brief The main logic of the value is to determine what the actual
	 * value should be based on its wrappings -- this method takes care of
	 * that, and must be implemented in all derived classes.
	 * @return The interpreted value of type T declared in the template.
	 */
	virtual T interpret(uint8_t nargs, ...) =0;
	virtual ~Parseable_Tmpl(){}
};

#endif /* WRAPPEDVALUE_H_ */
