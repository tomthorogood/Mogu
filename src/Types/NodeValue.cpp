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
    __numerics = new NumericUnion();
    __type = NO_VALUE;
    copy(proto);
}

NodeValue::~NodeValue()
{ 
    delete __numerics;
}
