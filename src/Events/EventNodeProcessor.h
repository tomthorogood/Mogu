/*
 * EventNodeProcessor.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef EVENTNODEPROCESSOR_H_
#define EVENTNODEPROCESSOR_H_

#include <declarations.h>


namespace Events{
const size_t MAX_FIELDS = 11;

namespace Node = Enums::Labels;

class EventNodeProcessor
{
private:
    Nodes::NodeValue* values[MAX_FIELDS];
public:
    EventNodeProcessor();
    virtual ~EventNodeProcessor();
    void set(
            Node::Labels index,
            std::string& value,
            Goo::Moldable* broadcaster           =0,
            int (*callback)(const std::string&) =0
            );

    Nodes::NodeValue* getValue(Node::Labels index);

    inline bool valueExists(Node::Labels index)
    {
    	return values[index] != 0;
    }
};

}//namespace Events

#endif /* EVENTNODEPROCESSOR_H_ */
