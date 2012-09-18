/*
 * WrappedValue.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef WRAPPEDVALUE_H_
#define WRAPPEDVALUE_H_

#include <declarations.h>

class WValue
{
private:
	std::string __value;
	char __start;
	char __end;
protected:
	inline std::string getValue() { return __value; }
};

template <typename T>
class WrappedValue : public WValue
{
private:
	char __start;
	char __end;
	std::string __value;

protected:
	/*!\brief Provides an accessor for derived classes to
	 * retrieve the set value; */
	inline std::string getValue() { return __value; }

	inline std::string unwrap()
	{
		return __value.substr(1, __value.length()-2);
	}

public:

	/*!\brief The main logic of the value is to determine what the actual
	 * value should be based on its wrappings -- this method takes care of
	 * that, and must be implemented in all derived classes.
	 * @return The interpreted value of type T declared in the template.
	 */
	virtual T interpret(uint8_t nargs, ...) =0;

	/*!\brief All WrappedValues require some arbitrary wrappings -- this
	 * is required in all derived classes, and the method needs to set
	 * __start and __end respectively.
	 * @param start What the value at position '0' should be
	 * @param end What the value at the last position should be.
	 */
	inline void set_wrapping(char start, char end)
	{
		__start = start;
		__end = end;
	}

	WrappedValue(char start, char end, std::string value)
	{
		set_wrapping(start,end);
		__value = value;
	}

	virtual ~WrappedValue(){};
};


#endif /* WRAPPEDVALUE_H_ */
