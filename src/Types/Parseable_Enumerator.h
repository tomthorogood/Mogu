/*
 * Wrapped_Enumerator.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef WRAPPED_ENUMERATOR_H_
#define WRAPPED_ENUMERATOR_H_

#include <declarations.h>
#include <Types/Parseable.h>


/*!\brief Interprets a string such as `{enum_string}` as an enumerated type,
 * where a parser is passed into the
 */
template <typename E>
class Wrapped_Enumerator : public Parseable_Tmpl <E>
{
public:
	Wrapped_Enumerator (std::string husk) : Parseable_Tmpl<E> (husk)
	{

	}

	/*!\brief Based on the enum parser passed into the method, will try
	 * to interpret the wrapped value.
	 * @param nargs The number of arguments (1 -- mostly symbolic), changing
	 * this does not affect anything at all, really.
	 * @return An enumerated value based on the kernel of the wrapped value
	 * passed into the class.
	 */
	inline E interpret(uint8_t nargs=1, ...)
	{
		E val;
		va_list args;
		va_start (args, nargs);
		for (int arg = 0; arg < 1; arg++)
		{
			val = va_arg (args, callback)(getValue());
		}
		va_end(args);
		return val;
	}
};


#endif /* WRAPPED_ENUMERATOR_H_ */
