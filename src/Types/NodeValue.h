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

enum class ReadType
{
    NO_VALUE = 0x0, string_value = 0x1, int_value = 0x2, float_value = 0x3
};

/*!\brief Since a NodeValue can only be int, float, or string, the NumericUnion
 * consolidates the int/float possibilities into one type to save a hair of
 * memory. The string is held inside the NodeValue class itself.
 */
union NumericUnion
{
    int as_int;
    float as_float;
};

/*!\brief The NodeValue class holds a parsed node value of any valid type,
 * currently limited to int/float/string. This allows for added polymorphic
 * capabilities of any classes that use node values in their logic.
 */
class NodeValue
{
    /*!\brief one of the possible types of values */
    ReadType __type = ReadType::NO_VALUE;

    /*!\brief If the value is a string, it is stored here. */
    std::string as_string = EMPTY ;

    /*!\brief If the value is numeric, it is stord here. */
    NumericUnion* __numerics = nullptr;

    inline void resetStr()
    {
        as_string = EMPTY;
    }

public:
    NodeValue();

    /*!\brief Copies the value of another node value into this one. */
    NodeValue(NodeValue*);

    inline NodeValue& operator=(const NodeValue& v)
    {
        __type = v.getType();
        as_string = v.getString();
        if (__numerics != nullptr) delete __numerics;
        __numerics = new NumericUnion();
        
        switch (__type) {
        case int_value:
            setInt(v.getInt());
            break;
        case string_value:
            setString(v.getString());
            break;
        case float_value:
            setFloat(v.getFloat());
            break;
        default:
            break;
        }
        return *this;
    }

    inline bool operator==(const NodeValue& v)
    {
        if (__type != getType()) return false;
        switch(getType())
        {
            case integer_type:
                return __numerics.as_int == v.getInt();
            case string_type:
                return as_string == v.getString();
            case float_type:
                return __numerics.as_float == v.getFloat();
            default:return false;
        }
    }

    /*!\brief deletes the pointer to the NumericUnion*/
    virtual ~NodeValue();

    /*!\brief Sets the string as well as __type */
    inline void setString(std::string val)
    {
        as_string = val;
        __type = string_value;
    }

    /*!\brief Sets an int as well as __type */
    inline void setInt(int val)
    {
        resetStr();
        __numerics->as_int = val;
        __type = int_value;
    }

    /*!\brief Sets a float as well as __type */
    inline void setFloat(
        float val)
    {
        resetStr();
        __numerics->as_float = val;
        __type = float_value;
    }

    /*!\brief If the type is `string_value`, returns a string. */
    inline const std::string& getString() const
    {
        return as_string;
    }

    /*!\brief If the type is `int_value`, returns the int.*/
    inline int& getInt() const
    {
        return __numerics->as_int;
    }

    /*!\brief If the type is `float_value`, returns the float. */
    inline float getFloat() const
    {
        return __numerics->as_float;
    }

    /*!\brief Returns the type so the correct data can be extracted. */
    inline ReadType getType() const
    {
        return __type;
    }

};
// end NodeValue

#endif /* NODEVALUE_H_ */
