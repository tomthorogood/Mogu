
#include <Events/EventNodeProcessor.h>
#include <Core/Moldable.h>
#include <Events/NodeConfiguration.h>
#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>

namespace Events
{
using std::string;
using Goo::Moldable;
using Parsers::NodeValueParser;

EventNodeProcessor::EventNodeProcessor()
{
    for (int i =0; i < EventNodeConfiguration::NUM_FIELDS; i++)
    {
        values[i] = 0;
    }
}

void EventNodeProcessor::set (
        Node::Labels index,
        string& value,
        Moldable* broadcaster,
        int (*callback)(const string&)
        )
//@TODO: Check to see if there's a memory leak here.
{
    NodeValueParser parser(value, broadcaster, callback);
    values[index] = parser.getValue();
}

Nodes::NodeValue* EventNodeProcessor::getValue(Node::Labels index)
{
    return values[index];
}

} // namespace Events
