/*
 * EventNodeExtractor.cpp
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#include <Events/EventNodeExtractor.h>
#include <Events/NodeConfiguration.h>
#include <Redis/RedisCore.h>
#include <Parsers/Parsers.h>
namespace Events{

using std::string;
using Redis::strvector;
using std::map;

EventNodeExtractor::EventNodeExtractor(string nodeName)
{
    string command = "hkeys ";
    command.append(nodeName);
    Redis::command(command);
    strvector keys;
    Redis::toVector(keys);
#ifdef DEBUG
    std::cout << "List of keys at " << nodeName << ": ";
    for (unsigned int i = 0; i < keys.size(); i++)
    {
    	std::cout << keys.at(i) <<", ";
    }
    std::cout << std::endl;
#endif

    Parsers::NodeLabelParser keyParser;
    int num_keys = keys.size();
    for (int k = 0; k < num_keys; k++)
    {
        string key = keys.at(k);
        command = "hget ";
        command.append(nodeName).append(" ").append(key);
        Redis::command(command);
#ifdef DEBUG
        std::cout << "Adding entry " << keyParser.parse(key);
        std::cout << "(" << key << ") ";
        std::cout << " to value map for " << nodeName << std::endl;
#endif
        stringValues[keyParser.parse(key)] = Redis::toString();
    }
#ifdef DEBUG
    std::cout << "Extractor Map ( " << this << ") for " << nodeName;
    std::cout << " looks like this: " << std::endl;
    for (StringValueMap::iterator iter = stringValues.begin();
    		iter != stringValues.end();
    		iter++)
    {
    	std::cout << "\t" << iter->first <<" : " << iter->second << std::endl;
    }
#endif
}

string EventNodeExtractor::getValue(Node::Labels index)
{
    return stringValues[index];
}

inline bool EventNodeExtractor::exists(Node::Labels index)
{
    StringValueMap::iterator iter;
    iter = stringValues.find(index);
    return iter != stringValues.end();
}

}//namespace Events
