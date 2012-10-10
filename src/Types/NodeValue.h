/*
 * NodeValue.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef NODEVALUE_H_
#define NODEVALUE_H_

#include <declarations.h>
#include <string>


#include <float.h>
#include <limits.h>

namespace Nodes
{
enum ReadType
{
	NO_VALUE		=0x0,
    string_value	=0x1,
    int_value		=0x2,
    float_value		=0x3
};

/*!\brief Since a NodeValue can only be int, float, or string, the NumericUnion
 * consolidates the int/float possibilities into one type to save a hair of
 * memory. The string is held inside the NodeValue class itself.
 */
union NumericUnion
{
	int		as_int;
	float 	as_float;
};

/*!\brief The NodeValue class holds a parsed node value of any valid type,
 * currently limited to int/float/string. This allows for added polymorphic
 * capabilities of any classes that use node values in their logic.
 */
class NodeValue
{
	/*!\brief one of the possible types of values */
    ReadType __type;

    /*!\brief If the value is a string, it is stored here. */
    std::string as_string;

    /*!\brief Sometimes a value may need to be parsed again later. This is
     * a sloppy solution to that problem.
     */
    std::string orig_string;

    /*!\brief If the value is numeric, it is stord here. */
    NumericUnion* __numerics;

public:
    NodeValue();

    /*!\brief Copies the value of another node value into this one. */
    NodeValue(NodeValue*);

    /*!\brief deletes the pointer to the NumericUnion*/
    virtual ~NodeValue();

    /*!\brief Sets the string as well as __type */
    inline void  setString(std::string val)
    {
    	as_string = val;
    	__type = string_value;
    }

    /*!\brief Sets the original string for values that do not
     * parse to strings.
     * @param val
     */
    inline void setOriginal(std::string val)
    {
    	orig_string = val;
    }

    inline std::string getOriginal() { return orig_string;}

    /*!\brief Sets an int as well as __type */
    inline void setInt(int val)
    {
    	__numerics->as_int = val;
    	__type = int_value;
    }

    /*!\brief Sets a float as well as __type */
    inline void setFloat(float val)
    {
    	__numerics->as_float = val;
    	__type = float_value;
    }

    /*!\brief If the type is `string_value`, returns a string. */
    inline std::string& getString()
    {
    	return as_string;
    }

    /*!\brief If the type is `int_value`, returns the int.*/
    inline int& getInt()
    {
    	return __numerics->as_int;
    }

    /*!\brief If the type is `float_value`, returns the float. */
    inline float getFloat()
    {
    	return __numerics->as_float;
    }

    /*!\brief Returns the type so the correct data can be extracted. */
    inline ReadType getType() { return __type;}

    void copy(NodeValue*);
}; // end NodeValue

} // namespace nodes


#endif /* NODEVALUE_H_ */
