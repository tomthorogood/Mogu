/*
 * NodeValue.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef NODEVALUE_H_
#define NODEVALUE_H_

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

    /*!\brief If the value is numeric, it is stord here. */
    NumericUnion* __numerics;

public:
    NodeValue();

    /*!\brief Copies the value of another node value into this one. */
    NodeValue(NodeValue*);

    /*!\brief deletes the pointer to the NumericUnion*/
    virtual ~NodeValue();

    /*!\brief Sets the string as well as __type */
    void setString(std::string val);

    /*!\brief Sets an int as well as __type */
    void setInt(int val);

    /*!\brief Sets a float as well as __type */
    void setFloat(float val);

    /*!\brief If the type is `string_value`, returns a string. */
    std::string getString();

    /*!\brief If the type is `int_value`, returns the int.*/
    int getInt();

    /*!\brief If the type is `float_value`, returns the float. */
    float getFloat();

    /*!\brief Returns the type so the correct data can be extracted. */
    ReadType getType();
}; // end NodeValue

} // namespace nodes


#endif /* NODEVALUE_H_ */
