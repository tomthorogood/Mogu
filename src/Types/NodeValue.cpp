#include "NodeValue.h"
#include <string.h>
#include <iostream>
#include <assert.h>

NodeValue::NodeValue()
{
    __numerics = new NumericUnion();
    __type = NO_VALUE;
    as_string = EMPTY;
}

NodeValue::NodeValue( const NodeValue& proto)
{
    if (&proto == this) return;
    __numerics = new NumericUnion();
    __type = NO_VALUE;
    switch(proto.getType())
    {
        case (ReadType::as_integer):
            setInt(proto.getInt());
            break;
        case (ReadType::as_string):
            setString(proto.getString());
            break;
        case (ReadType::as_float):
            setFloat(proto.getFloat());
            break;
        default:break;
    }
}

NodeValue::~NodeValue()
{ 
    delete __numerics;
}
