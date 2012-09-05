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


WidgetType getWidgetType(Goo::Moldable* broadcaster);
std::string getWidgetText(Goo::Moldable* broadcaster);
std::string getWidgetImgSource(Goo::Moldable* broadcaster);

Wt::WAnimation::AnimationEffect getWidgetAnimation(
        Goo::Moldable* broadcaster);

void getWidgetChildren(
        Goo::Moldable* broadcaster, Redis::strvector& empty_vector);


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

bool widgetHasChildren(Goo::Moldable* broadcaster);
bool widgetBlocksActions(Goo::Moldable* broadcaster);
bool widgetIsDynamic(std::string);

uint8_t getActionBlock(Goo::Moldable* broadcaster);

} //namespace StyleParser

} //namespace Parser

#endif /* STYLEPARSER_H_ */
