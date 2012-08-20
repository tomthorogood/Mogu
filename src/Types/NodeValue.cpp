#include <Types/NodeValue.h>
#include <string.h>
#include <iostream>
#include <assert.h>

namespace Nodes{

using std::string;
NodeValue::NodeValue()
{
    __value = new ValueStruct();
    __type = NO_VALUE;
}

NodeValue::NodeValue(NodeValue* proto)
{
	__value = new ValueStruct();
	__type = proto->getType();
#ifdef DEBUG
	switch( __type)
	{
	case int_value:
		setInt(proto->getInt());
		assert(__value->as_int != INT_MIN);
		break;
	case string_value:
		setString(proto->getString());
		assert(__value->as_string != "");
		break;
	case float_value:
		setFloat(proto->getFloat());
		assert(__value->as_float != FLT_MIN);
		break;
	default:
		break;
	}
#else

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
#endif
}

NodeValue::NodeValue(ValueStruct* value, ReadType _type)
{
    __value = value;
    __type = _type;
}

NodeValue::~NodeValue()
{
		delete __value;
}

void NodeValue::setString(string val)
{
	__value->as_string = val;
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
