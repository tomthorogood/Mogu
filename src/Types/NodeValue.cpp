#include <Types/NodeValue.h>
#include <string.h>
#include <iostream>
#include <assert.h>

namespace Nodes{

using std::string;
NodeValue::NodeValue()
{
    __numerics = new NumericUnion();
    __type = NO_VALUE;
}

NodeValue::NodeValue(NodeValue* proto)
{
	__numerics = new NumericUnion();
	__type = proto->getType();

	switch( __type)
	{
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

void NodeValue::setString(string val)
{
	as_string = val;
    __type = string_value;
}

void NodeValue::setInt(int val)
{
    __numerics->as_int = val;
    __type = int_value;
}

void NodeValue::setFloat(float val)
{
    __numerics->as_float = val;
    __type = float_value;
}

ReadType NodeValue::getType()
{
    return __type;
}

string NodeValue::getString()
{
    return as_string;
}

int NodeValue::getInt()
{
    return __numerics->as_int;
}

float NodeValue::getFloat()
{
    return __numerics->as_float;
}

}//namespace Nodes
