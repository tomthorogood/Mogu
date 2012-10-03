
#include <Events/EventNodeProcessor.h>
#include <Core/Moldable.h>
#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>

namespace Events
{
using std::string;
using Goo::Moldable;
using Parsers::NodeValueParser;


EventNodeProcessor::EventNodeProcessor()
{
    for (int i =0; i < MAX_FIELDS; i++)
    {
        values[i] = 0;
    }
#ifdef DEBUG
    std::cout << "Creating EventNodeProcessor ("<<this<<")" << std::endl;
#endif
}

void EventNodeProcessor::set (
        Node::Labels index,
        string& value,
        Moldable* broadcaster,
        int (*callback)(const string&)
        )
{
	values[index] = new Nodes::NodeValue();
    NodeValueParser parser(value, values[index], broadcaster, callback);
}

Nodes::NodeValue* EventNodeProcessor::getValue(Node::Labels index)
{
    return values[index];
}

EventNodeProcessor::~EventNodeProcessor()
{
	for (int i = 0; i < MAX_FIELDS; i++)
	{
		if (values[i] != 0)
		{
			delete values[i];
		}
	}
#ifdef DEBUG
	std::cout << "Deleting EventNodeProcessor ("<<this<<")"<<std::endl;
#endif
}

} // namespace Events
