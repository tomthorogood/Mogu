/*
 * _StyleParser.h
 *
 *  Created on: Jul 15, 2012
 *      Author: tom
 */

#ifndef STYLEPARSER_H_
#define STYLEPARSER_H_

#include <declarations.h>
#include <Core/Moldable.h>
#include <Redis/RedisCore.h>

#include <Wt/WAnimation> //For AnimationEffect enum.

namespace Parsers
{

typedef Enums::WidgetTypes::WidgetTypes WidgetType;

namespace StyleParser
{


inline WidgetType getWidgetType(std::string nodeName)
{
   WidgetType __type;
   Nodes::NodeValue val;
   std::string reply_str = getWidgetProperty(
    		nodeName, "type");
   Parsers::NodeValueParser parser(reply_str, &val, NONE,
            Parsers::enum_callback <Parsers::WidgetTypeParser>);
   __type = (WidgetType) val.getInt();
    return __type;
}


inline std::string getWidgetText(std::string nodeName)
{
    std::string reply_str = getWidgetProperty(nodeName, "content");
    Nodes::NodeValue val;
    Parsers::NodeValueParser parser(reply_str, &val, NONE);
    return val.getString();
}

inline std::string getWidgetImgSource(std::string nodeName);

inline Wt::WAnimation::AnimationEffect getWidgetAnimation(
        std::string nodeName)
{
    Redis::command("hget", nodeName, "animation");
    std::string animation_str = Redis::toString();
    Nodes::NodeValue val;
    NodeValueParser value(animation_str, &val, NONE,
            &Parsers::enum_callback<Parsers::WtAnimationParser>);
    return (Wt::WAnimation::AnimationEffect) val.getInt();
}

inline void getWidgetChildren(
        std::string nodeName, Redis::strvector& children)
{
    nodeName.append(".children");
    int num_children =0;

    Redis::command("llen", nodeName);
    num_children = Redis::getInt();
    std::string _num_children = itoa(num_children);
    Redis::command("lrange", nodeName, "0", _num_children);
    Redis::toVector(children);
}


inline std::string getWidgetProperty(
		std::string nodeName, const char* property)
{
	Redis::command("hget", nodeName, property);
	return Redis::toString();
}

inline bool widgetHasProperty(std::string nodeName, std::string property)
{
	Redis::command("hexists", nodeName, property);
	return (bool) Redis::getInt();
}

inline bool widgetHasChildren(std::string nodeName)
{
    nodeName.append(".children");
    Redis::command("exists", nodeName);
    return Redis::getInt() == 1;
}

inline bool widgetBlocksActions(std::string nodeName)
{
    Redis::command("hexists", nodeName, "block");
    return (bool) Redis::getInt();
}

inline bool widgetIsDynamic(std::string nodeName)
{
	Redis::command("hget", nodeName, "type");
	std::string type_str = Redis::toString();
	size_t index = type_str.find("dynamic");
#ifdef DEBUG
	if (index != std::string::npos)
	{
		std::cout << nodeName << " IS DYNAMIC" << std::endl;
	}
#endif
	return index != std::string::npos;
}

uint8_t getActionBlock(std::string nodeName);

} //namespace StyleParser

} //namespace Parser

#endif /* STYLEPARSER_H_ */
