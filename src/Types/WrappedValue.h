/*
 * WrappedValue.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef WRAPPEDVALUE_H_
#define WRAPPEDVALUE_H_

#include <declarations.h>

template <typename T>
class WrappedValue
{
private:
	char __start;
	char __end;
	std::string __value;

protected:
	/*!\brief Provides an accessor for derived classes to
	 * retrieve the set value; */
	inline std::string getValue() { return __value; }

public:

	/*!\brief The main logic of the value is to determine what the actual
	 * value should be based on its wrappings -- this method takes care of
	 * that, and must be implemented in all derived classes.
	 * @return The interpreted value of type T declared in the template.
	 */
	virtual T interpret() =0;

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

	WrappedValue(char start, char end, std::string value) {
		set_wrapping(start,end);
		__value = value;
	}

	virtual ~WrappedValue(){};
};


#endif /* WRAPPEDVALUE_H_ */
