/*
 * EventNodeExtractor.h
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#ifndef EVENTNODEEXTRACTOR_H_
#define EVENTNODEEXTRACTOR_H_

#include <declarations.h>

namespace Events{

namespace Node = Enums::Labels;
typedef std::map <Node::Labels, std::string> StringValueMap;

class EventNodeExtractor
{
    std::map <Node::Labels, std::string> stringValues;
public:
    EventNodeExtractor(std::string nodeName);
    std::string getValue(Node::Labels index);
    bool exists(Node::Labels index);
};

}//namespace Events

#endif /* EVENTNODEEXTRACTOR_H_ */
