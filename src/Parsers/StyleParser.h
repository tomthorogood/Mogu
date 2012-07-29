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

using std::string;
typedef Enums::WidgetTypes::WidgetTypes WidgetType;

namespace StyleParser
{


WidgetType getWidgetType(Goo::Moldable* broadcaster);
string getWidgetStyleClass(Goo::Moldable* broadcaster);
string getWidgetText(Goo::Moldable* broadcaster);
string getWidgetInternalPath(Goo::Moldable*  broadcaster);
string getWidgetImgSource(Goo::Moldable* broadcaster);
string getWidgetLinkLocation(Goo::Moldable* broadcaster);

Wt::WAnimation::AnimationEffect getWidgetAnimation(
        Goo::Moldable* broadcaster);

void getWidgetChildren(
        Goo::Moldable* broadcaster, Redis::strvector& empty_vector);

bool widgetHasEvents(Goo::Moldable* broadcaster);
bool widgetHasChildren(Goo::Moldable* broadcaster);
bool widgetHasStyling(Goo::Moldable* broadcaster);
bool widgetHasAnimation(Goo::Moldable* broadcaster);
bool widgetBlocksActions(Goo::Moldable* broadcaster);

uint8_t getActionBlock(Goo::Moldable* broadcaster);

} //namespace StyleParser

} //namespace Parser

#endif /* STYLEPARSER_H_ */
