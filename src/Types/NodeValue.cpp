#include "NodeValue.h"
#include <string.h>
#include <iostream>
#include <assert.h>

using std::string;
NodeValue::NodeValue()
{
    __numerics = new NumericUnion();
    __type = NO_VALUE;
    as_string = EMPTY;
}

NodeValue::NodeValue(
    NodeValue* proto)
{
    __numerics = new NumericUnion();
    copy(proto);
}

void NodeValue::copy(
    const NodeValue* proto)
{
    __type = proto->getType();

    switch (__type) {
    case int_value:
        setInt(proto->getInt());
        break;
    case string_value:
        setString(proto->getString());
        break;
    case float_value:
        setFloat(proto->getFloat());
        break;
    default:
        break;
    }
}

NodeValue::~NodeValue()
{
	delete __numerics;
}
