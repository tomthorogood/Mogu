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

union NumericUnion
{
	int		as_int;
	float 	as_float;
};


struct ValueStruct
{
    std::string as_string;
    int as_int;
    float as_float;
    ValueStruct() {
    	as_string = "";
    	as_int = INT_MIN;
    	as_float = FLT_MIN;
    }
};

class NodeValue
{
    ReadType __type;
    std::string as_string;
    NumericUnion* __numerics;

public:
    NodeValue();

    /* Copies the value of another node value into this one. */
    NodeValue(NodeValue*);
    NodeValue(ValueStruct* value, ReadType _type);
    virtual ~NodeValue();

    void setString(std::string val);
    void setInt(int val);
    void setFloat(float val);
    std::string getString();
    int getInt();
    float getFloat();
    ReadType getType();
};

} // namespace nodes


#endif /* NODEVALUE_H_ */
