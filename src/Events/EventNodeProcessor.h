/*
 * EventNodeProcessor.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef EVENTNODEPROCESSOR_H_
#define EVENTNODEPROCESSOR_H_

#include <declarations.h>
#include <Events/NodeConfiguration.h>

namespace Goo
{
    class Moldable;
}

namespace Events{

namespace Node = Enums::Labels;

class EventNodeProcessor
{
private:
    Nodes::NodeValue* values[EventNodeConfiguration::NUM_FIELDS];
    bool __recycled;
public:
    EventNodeProcessor(bool recycled =true);
    ~EventNodeProcessor();
    void set(
            Node::Labels index,
            std::string& value,
            Goo::Moldable* broadcaster           =0,
            int (*callback)(const std::string&) =0
            );

    Nodes::NodeValue* getValue(Node::Labels index);
    inline bool isRecycled()
    {
        return __recycled;
    }
};

}//namespace Events

#endif /* EVENTNODEPROCESSOR_H_ */
