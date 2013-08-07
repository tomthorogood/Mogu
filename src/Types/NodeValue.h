/*
 * NodeValue.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef NODEVALUE_H_
#define NODEVALUE_H_

#include <string>

enum class Read_Type
{
    NO_VALUE = 0x0, string_value = 0x1, int_value = 0x2, float_value = 0x3
};

/*!\brief Since a NodeValue can only be int, float, or string, the NumericUnion
 * consolidates the int/float possibilities into one type to save a hair of
 * memory. The string is held inside the NodeValue class itself.
 */
union Numeric_Union
{
    int as_int {}
    double as_float;
    
    Numeric_Union(const int& i)
        : as_int(i) {}
    Numeric_Union(const double& d)
        : as_float(d) {}
};

/*!\brief The NodeValue class holds a parsed node value of any valid type,
 * currently limited to int/float/string. This allows for added polymorphic
 * capabilities of any classes that use node values in their logic.
 */
class Node_Value
{
    /*!\brief one of the possible types of values */
    Read_Type type {Read_Type::NO_VALUE};

    /*!\brief If the value is a string, it is stored here. */
    std::string as_string {}

    /*!\brief If the value is numeric, it is stord here. */
    NumericUnion* numerics {}

    inline void resetStr()
        { as_string = ""; }

public:
    Node_Value() {}
    Node_Value(const int& i)
        : numerics(new Numeric_Union(i))
        , type(Read_Type::int_type)
    {}
    
    Node_Value(const std::string& s)
        : as_string(s)
        , type (Read_Type::string_type)
    {}

    Node_Value(const double& d)
        : numerics(new Numeric_Union(d))
        , type (Read_Type::float_type)
    {}
    /*!\brief Copies the value of another node value into this one. */

    Node_Value(const Node_Value& n)
        : type(n.type)
    {
        switch(type)
        {
            case Read_Type::string_value:
                as_string = n.as_string;
                break;
            case Read_Type::int_value:
                numerics = new Numeric_Union(n.numerics->as_int);
                break;
            case Read_Type::float_value:
                numerics = new Numeric_Union(n.numerics->as_float);
                break;
            default:
                break;
        }
    }

    Node_Value(Node_Value&& n)
        : type(n.type)
        , as_string(n.as_string)
        , numerics(n.numerics)
    {
        n.as_string = "";
        n.numerics = nullptr;
        n.type = Read_Type::NO_VALUE;
    }

    Node_Value& operator= (const Node_Value& n)
    {
        if (&v == this) return *this;
        type = v.getType();
        as_string = v.getString();
        if (!numerics) numerics = new Numeric_Union();
        switch(type)
        {
            case Read_Type::int_value:
                numerics->as_int = n.get_int();
                break;
            case Read_Type::string_value:
                as_string = n.get_string();
                break;
            case Read_Type::float_value:
                numerics->as_float = n.get_float();
                break;
            default:
                break;
        }

        return *this;
    }

    inline bool operator== (const NodeValue& v)
    {
        if (type != v.get_type()) return false;
        switch(type)
        {
            case Read_Type::int_value:
                return numerics->as_int == v.get_int();
            case Read_Type::float_value:
                return numerics->as_float == v.get_float();
            case Read_Type::string_value:
                return as_string == v.get_string();
            default:
                return false;
        }
    }

    virtual ~NodeValue()
    {
        if(numerics) delete numerics;
    }

    operator float() const {
        if (numerics) return numerics->as_float;
        return 0;
    }

    operator int() const {
        if (numerics) return numerics->as_int;
        return 0;
    }

    operator std::string() const {
        return as_string;
    }

    inline void set_string(const std::string& val)
    {
        as_string = val;
        type = ReadType::string_value;
    }

    inline void set_int(const int& i)
    {
        if (!numerics) numerics = new Numeric_Union();
        numerics->as_int = i;
        type = ReadType::int_value;
    }

    inline void setFloat(const double& d)
    {
        if (!numerics) numerics = new Numeric_Union();
        numerics->as_float = d;
        type = ReadType::float_value;
    }

    inline const std::string& get_string() const
        { return as_string; }

    inline const int& get_int() const
    {
        if (numerics) return numerics->as_int; 
        return 0;
    }

    inline const double& get_float() const
    {   
        if (numerics) return numerics->as_float;
        return 0;
    }

    inline Read_Type get_type() const
        { return type; }

    inline void clear() 
    {
        as_string="";
        type = ReadType::NO_VALUE;
        if (numerics) 
        {
            delete numerics;
            numerics = nullptr;
        }
    }

    inline bool is_string() const
        { return type == Read_Type::string_value; }
    inline bool is_int() const 
        { return type == Read_Type::int_value; }
    inline bool is_float() const
        { return type == Read_Type::float_value;}

};
// end NodeValue

#endif /* NODEVALUE_H_ */
