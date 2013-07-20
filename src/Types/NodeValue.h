/*
 * NodeValue.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef NODEVALUE_H_
#define NODEVALUE_H_

#include <string>

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
    int as_int      =-1;
    float as_float;
};

/*!\brief The NodeValue class holds a parsed node value of any valid type,
 * currently limited to int/float/string. This allows for added polymorphic
 * capabilities of any classes that use node values in their logic.
 */
class NodeValue
{
    /*!\brief one of the possible types of values */
    ReadType type = ReadType::NO_VALUE;

    /*!\brief If the value is a string, it is stored here. */
    std::string as_string = "" ;

    /*!\brief If the value is numeric, it is stord here. */
    NumericUnion* numerics = nullptr;

    inline void resetStr()
    {
        as_string = "";
    }

public:
    NodeValue();
    NodeValue(const int i) : numerics(new NumericUnion()){

        setInt(i);
    }
    NodeValue(const std::string& s) : numerics(new NumericUnion()){
        setString(s);
    }
    NodeValue(const float f) : numerics(new NumericUnion()){
        setFloat(f);
    }
    /*!\brief Copies the value of another node value into this one. */
    NodeValue(const NodeValue&);

    inline NodeValue& operator=(const NodeValue& v)
    {
        if (&v == this) return *this;
        type = v.getType();
        as_string = v.getString();
        if (numerics != nullptr) delete numerics;
        numerics = new NumericUnion();
        
        switch (type) {
        case ReadType::int_value:
            setInt(v.getInt());
            break;
        case ReadType::string_value:
            setString(v.getString());
            break;
        case ReadType::float_value:
            setFloat(v.getFloat());
            break;
        default:
            break;
        }
        return *this;
    }

    inline bool operator==(const NodeValue& v)
    {
        if (type != v.getType()) return false;
        switch(getType())
        {
            case ReadType::int_value:
                return numerics->as_int == v.getInt();
            case ReadType::string_value:
                return as_string == v.getString();
            case ReadType::float_value:
                return numerics->as_float == v.getFloat();
            default:return false;
        }
    }

    /*!\brief deletes the pointer to the NumericUnion*/
    virtual ~NodeValue();

    operator float() const {
        return numerics->as_float;
    }

    operator int() const {
        return numerics->as_int;
    }

    operator std::string() const {
        return as_string;
    }

    /* Appends values to strings, sums numeric values. */
    void operator+= (const NodeValue& other)
    {
        if (isString())
        {
            if (other.isString())
                as_string += (std::string) other;
            else if (other.isInt())
                as_string += std::to_string( (int) other);
            else if (other.isFloat())
                as_string += std::to_string( (float) other);
        }
        else if (other.isInt())
        {
            if (isInt())
                numerics->as_int += (int) other;
            else if (isFloat())
                numerics->as_float += (int) other;
        }
        else if (other.isFloat())
        {
            if (isInt())
                numerics->as_int += (float) other;
            else if (other.isFloat())
                numerics->as_float += (float) other;
        }
    }

    /*!\brief Sets the string as well as type */
    inline void setString(std::string val)
    {
        as_string = val;
        type = ReadType::string_value;
    }

    /*!\brief Sets an int as well as type */
    inline void setInt(int val)
    {
        resetStr();
        numerics->as_int = val;
        type = ReadType::int_value;
    }

    /*!\brief Sets a float as well as type */
    inline void setFloat(
        float val)
    {
        resetStr();
        numerics->as_float = val;
        type = ReadType::float_value;
    }

    /*!\brief If the type is `string_value`, returns a string. */
    inline const std::string& getString() const
    {
        return as_string;
    }

    /*!\brief If the type is `int_value`, returns the int.*/
    inline int& getInt() const
    {
        return numerics->as_int;
    }

    /*!\brief If the type is `float_value`, returns the float. */
    inline float getFloat() const
    {
        return numerics->as_float;
    }

    /*!\brief Returns the type so the correct data can be extracted. */
    inline ReadType getType() const
    {
        return type;
    }

    inline bool isString() const { return type == ReadType::string_value;}
    inline bool isInt() const { return type == ReadType::int_value;}
    inline bool isFloat() const { return type == ReadType::float_value;}

};
// end NodeValue

#endif /* NODEVALUE_H_ */
