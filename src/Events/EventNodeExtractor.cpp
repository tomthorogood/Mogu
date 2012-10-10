/*
 * EventNodeExtractor.cpp
 *
 *  Created on: Jul 11, 2012
 *      Author: tom
 */

#include <Events/EventNodeExtractor.h>
#include <Redis/RedisCore.h>
#include <Parsers/Parsers.h>
#include <Wt/WApplication>
#include <Mogu.h>

namespace Events{

using std::string;
using Redis::strvector;
using std::map;

EventNodeExtractor::EventNodeExtractor(string nodeName)
{
	mApp;
    string command = "hkeys ";
    command.append(nodeName);
    app->redisCommand(command);
    strvector keys;
    Redis::toVector(app->reply(),keys);
    Parsers::NodeLabelParser keyParser;
    int num_keys = keys.size();
    for (int k = 0; k < num_keys; k++)
    {
        string key = keys.at(k);
        command = "hget ";
        command.append(nodeName).append(" ").append(key);
        app->redisCommand(command);
        stringValues[keyParser.parse(key)] = Redis::toString(app->reply());
#ifdef DEBUG
        std::cout << command << " " << stringValues[keyParser.parse(key)];
        std::cout << std::endl;
#endif
    }
#ifdef DEBUG
    std::cout << "Creating event node extractor (" << this <<") for "
    		<< nodeName << std::endl;
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
