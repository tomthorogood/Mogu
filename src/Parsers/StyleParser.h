/*
 * _StyleParser.h
 *
 *  Created on: Jul 15, 2012
 *      Author: tom
 */

#ifndef STYLEPARSER_H_
#define STYLEPARSER_H_

#include <declarations.h>
#include <Wt/WAnimation> //For AnimationEffect enum.

namespace Parsers
{

typedef Enums::WidgetTypes::WidgetTypes WidgetType;

namespace StyleParser
{


WidgetType getWidgetType(Goo::Moldable* broadcaster);
std::string getWidgetStyleClass(Goo::Moldable* broadcaster);
std::string getWidgetText(Goo::Moldable* broadcaster);
std::string getWidgetInternalPath(Goo::Moldable*  broadcaster);
std::string getWidgetImgSource(Goo::Moldable* broadcaster);
std::string getWidgetLinkLocation(Goo::Moldable* broadcaster);
std::string getWidgetName(Goo::Moldable* broadcaster);
int getWidgetStackIndex(Goo::Moldable* broadcaster);

Wt::WAnimation::AnimationEffect getWidgetAnimation(
        Goo::Moldable* broadcaster);

void getWidgetChildren(
        Goo::Moldable* broadcaster, Redis::strvector& empty_vector);

bool widgetHasEvents(Goo::Moldable* broadcaster);
bool widgetHasChildren(Goo::Moldable* broadcaster);
bool widgetHasStyling(Goo::Moldable* broadcaster);
bool widgetHasAnimation(Goo::Moldable* broadcaster);
bool widgetBlocksActions(Goo::Moldable* broadcaster);
bool widgetIsNamed(Goo::Moldable* broadcaster);
bool widgetHasStackIndex(Goo::Moldable* broadcaster);
bool widgetIsDynamic(std::string);

uint8_t getActionBlock(Goo::Moldable* broadcaster);

} //namespace StyleParser

} //namespace Parser

#endif /* STYLEPARSER_H_ */
