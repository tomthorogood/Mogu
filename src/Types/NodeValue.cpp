#include "NodeValue.h"
#include <string.h>
#include <iostream>
#include <assert.h>

NodeValue::NodeValue()
{
    __numerics = new NumericUnion();
    __type = ReadType::NO_VALUE;
    as_string = "";
}

NodeValue::NodeValue( const NodeValue& proto)
{
    if (&proto == this) return;
    __numerics = new NumericUnion();
    __type = ReadType::NO_VALUE;
    switch(proto.getType())
    {
        case (ReadType::int_value):
            setInt(proto.getInt());
            break;
        case (ReadType::string_value):
            setString(proto.getString());
            break;
        case (ReadType::float_value):
            setFloat(proto.getFloat());
            break;
        default:break;
    }
}

NodeValue::~NodeValue()
{ 
    delete __numerics;
}
