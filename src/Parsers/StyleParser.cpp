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
#include <Core/Moldable.h>
#include <Redis/RedisCore.h>
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
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hget", nodeName, "type");
    string reply_str = Redis::toString();
    Parsers::NodeValueParser parser(reply_str, broadcaster,
            Parsers::enum_callback <Parsers::WidgetTypeParser>);
    __type = (WidgetType) parser.getValue()->getInt();
    return __type;
}

string getWidgetStyleClass(Moldable* broadcaster)
{
    string styleClass;
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hget", nodeName, "class");
    styleClass = Redis::toString();
    return styleClass;
}

bool widgetHasStyling(Moldable* broadcaster)
{
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hexists", nodeName, "class");
    return (bool) Redis::getInt();
}

string getWidgetText(Moldable* broadcaster)
{
    string text_content;

    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hget", nodeName, "content");
    string reply_str = Redis::toString();
    Parsers::NodeValueParser parser(reply_str, broadcaster);
    text_content = parser.getValue()->getString();
    return text_content;
}


bool widgetIsNamed(Moldable* broadcaster)
{
	bool named = false;
	string nodeName = broadcaster->getNodeList()->at(0);
	Redis::command("hexists", nodeName, "name");
	named = Redis::getInt();
	return named;
}

string getWidgetName(Moldable* broadcaster)
{
	string nodeName = broadcaster->getNodeList()->at(0);
	Redis::command("hget", nodeName, "name");
	return Redis::toString();
}

bool widgetHasStackIndex(Moldable* broadcaster)
{
	string nodeName = broadcaster->getNodeList()->at(0);
	Redis::command("hexists", nodeName, "index");
	return (bool) Redis::getInt();
}

int getWidgetStackIndex(Moldable* broadcaster)
{
	string nodeName = broadcaster->getNodeList()->at(0);
	Redis::command("hget", nodeName, "index");
	string reply_str = Redis::toString();
	Parsers::NodeValueParser parser(reply_str,broadcaster);
	return parser.getValue()->getInt();
}

string getWidgetInternalPath(Moldable* broadcaster)
{
    string internal_path;
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hget", nodeName, "internal_path");
    internal_path = Redis::toString();
    return internal_path;
}

string getWidgetImgSource(Moldable* broadcaster)
{
    string img_src;
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hget", nodeName, "source");
    img_src = Redis::toString();
    return img_src;
}

string getWidgetLinkLocation(Moldable* broadcaster)
{
    string location;
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hget", nodeName, "location");
    location = Redis::toString();
    return location;
}

void getWidgetChildren(Moldable* broadcaster, Redis::strvector& children)
{
    string nodeName = broadcaster->getNodeList()->at(0);
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
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hget", nodeName, "animation");
    string animation_str = Redis::toString();
    NodeValueParser value(animation_str, broadcaster,
            &Parsers::enum_callback<Parsers::WtAnimationParser>);
    return (Wt::WAnimation::AnimationEffect) value.getValue()->getInt();
}

bool widgetHasEvents(Moldable* broadcaster)
{
    bool has_events = false;
    string nodeName = broadcaster->getNodeList()->at(0);

    Redis::command("hexists", nodeName, "events");
    bool key_exists = (bool) Redis::getInt();
    if (key_exists)
    {
        Redis::command("hget", nodeName, "events");
        string reply = Redis::toString();
        has_events = reply == "on";
    }
    return has_events;
}

bool widgetHasChildren(Moldable* broadcaster)
{
    string nodeName = broadcaster->getNodeList()->at(0);
    nodeName.append(".children");
    Redis::command("exists", nodeName);
    return Redis::getInt() == 1;
}

bool widgetHasAnimation(Moldable* broadcaster)
{
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hexists", nodeName, "animation");
    return (bool) Redis::getInt();
}

bool widgetBlocksActions(Moldable* broadcaster)
{
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hexists", nodeName, "block");
    return (bool) Redis::getInt();
}

bool widgetIsDynamic(std::string nodeName)
{
	Redis::command("hget", nodeName, "type");
	std::string type_str = Redis::toString();

	Parsers::NodeValueParser parser(type_str, 0,
			&Parsers::enum_callback <Parsers::BitMaskParser>);
	Enums::WidgetTypes::WidgetTypes type_ = (Enums::WidgetTypes::WidgetTypes)
			parser.getValue()->getInt();
	return type_;

}

uint8_t getActionBlock(Moldable* broadcaster)
{
    uint8_t block   =0;
    string nodeName = broadcaster->getNodeList()->at(0);
    Redis::command("hget", nodeName, "block");
    string block_str = Redis::toString();
    string block_arr[Enums::SignalActions::NUM_ACTIONS];
    TurnLeft::Utils::Explosion explosion(block_str);
    explosion.explode(' ',block_arr);
    int num_blocks = explosion.getNumWords();

    for (int b = 0; b < num_blocks; b++)
    {
        uint8_t action =0;
        Parsers::NodeValueParser parser(block_arr[b], broadcaster,
                &Parsers::enum_callback <Parsers::SignalActionParser>);
        action = (uint8_t) parser.getValue()->getInt();
        block |= action;
    }
    return block;
}

} //namespace StyleParser
} //namespace Parsers
