/*
 * NodeValue.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef NODEVALUE_H_
#define NODEVALUE_H_

#include <string>

namespace Nodes
{
enum ReadType
{
    string_value,
    int_value,
    float_value
};

struct ValueUnion
{
    const char* as_string;
    int as_int;
    float as_float;
};

class NodeValue
{
    ValueUnion* __value;
    ReadType __type;
    bool __owned_value;

public:
    NodeValue();
    NodeValue(ValueUnion* value, ReadType _type);
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
