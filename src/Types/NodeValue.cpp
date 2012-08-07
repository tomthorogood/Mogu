#include <Types/NodeValue.h>

namespace Nodes{
using std::string;

NodeValue::NodeValue()
{
    __value = new ValueUnion();
    __owned_value = true;
}

NodeValue::NodeValue(ValueUnion* value, ReadType _type)
{
    __value = value;
    __type = _type;
    __owned_value = false;
}

NodeValue::~NodeValue()
{
	if (__owned_value)
	{
		delete __value;
	}
}

void NodeValue::setString(string val)
{
    __value->as_string = val.c_str();
    __type = string_value;
}

void NodeValue::setInt(int val)
{
    __value->as_int = val;
    __type = int_value;
}

void NodeValue::setFloat(float val)
{
    __value->as_float = val;
    __type = float_value;
}

ReadType NodeValue::getType()
{
    return __type;
}

string NodeValue::getString()
{
    string as_string(__value->as_string);
    return as_string;
}

int NodeValue::getInt()
{
    return __value->as_int;
}

float NodeValue::getFloat()
{
    return __value->as_float;
}

}//namespace Nodes
