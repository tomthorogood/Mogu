/*
 * _StyleParser.cpp
 *
 *  Created on: Jul 15, 2012
 *      Author: tom
 */

#include <Parsers/StyleParser.h>
#include <Parsers/Parsers.h>
#include <Parsers/NodeValueParser.h>
#include <Types/NodeValue.h>
#include <sstream>
#include <Wt/WAnimation>
#include <TurnLeftLib/Utils/explosion.h>

namespace Parsers
{
namespace StyleParser
{
using std::string;
using Goo::Moldable;

WidgetType getWidgetType(Moldable* broadcaster)
{
   WidgetType __type;
   Nodes::NodeValue val;

   std::string reply_str = getWidgetProperty(
    		broadcaster->getNode(), "type");

   Parsers::NodeValueParser parser(reply_str, &val, broadcaster,
            Parsers::enum_callback <Parsers::WidgetTypeParser>);

   __type = (WidgetType) val.getInt();
    return __type;
}

string getWidgetText(Moldable* broadcaster)
{
    string text_content;
    string nodeName = broadcaster->getNode();

    string reply_str = getWidgetProperty(nodeName, "content");
    Nodes::NodeValue val;
    Parsers::NodeValueParser parser(reply_str, &val, broadcaster);
    text_content = val.getString();
    return text_content;
}

void getWidgetChildren(Moldable* broadcaster, Redis::strvector& children)
{
    string nodeName = broadcaster->getNode();
    nodeName.append(".children");
    int num_children =0;

    Redis::command("llen", nodeName);
    num_children = Redis::getInt();
    std::stringstream strm;
    strm << num_children;
    string _num_children(strm.str());
    Redis::command("lrange", nodeName, "0", _num_children);
    Redis::toVector(children);
}

Wt::WAnimation::AnimationEffect getWidgetAnimation(Moldable* broadcaster)
{
    string nodeName = broadcaster->getNode();
    Redis::command("hget", nodeName, "animation");
    string animation_str = Redis::toString();
    Nodes::NodeValue val;
    NodeValueParser value(animation_str, &val, broadcaster,
            &Parsers::enum_callback<Parsers::WtAnimationParser>);
    return (Wt::WAnimation::AnimationEffect) val.getInt();
}

bool widgetHasChildren(Moldable* broadcaster)
{
    string nodeName = broadcaster->getNode();
    nodeName.append(".children");
    Redis::command("exists", nodeName);
    return Redis::getInt() == 1;
}



bool widgetBlocksActions(Moldable* broadcaster)
{
    string nodeName = broadcaster->getNode();
    Redis::command("hexists", nodeName, "block");
    return (bool) Redis::getInt();
}

bool widgetIsDynamic(std::string nodeName)
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

uint8_t getActionBlock(Moldable* broadcaster)
{
    uint8_t block   =0;
    string nodeName = broadcaster->getNode();
    Redis::command("hget", nodeName, "block");
    string block_str = Redis::toString();
    string block_arr[Enums::SignalActions::NUM_ACTIONS];
    TurnLeft::Utils::Explosion explosion(block_str);
    explosion.explode(' ',block_arr);
    int num_blocks = explosion.getNumWords();

    for (int b = 0; b < num_blocks; b++)
    {
        uint8_t action =0;
        Nodes::NodeValue val;
        Parsers::NodeValueParser parser(block_arr[b], &val, broadcaster,
                &Parsers::enum_callback <Parsers::SignalActionParser>);
        action = (uint8_t) val.getInt();
        block |= action;
    }
    return block;
}

} //namespace StyleParser
} //namespace Parsers
