/*
 * _StyleParser.h
 *
 *  Created on: Jul 15, 2012
 *      Author: tom
 */

#ifndef STYLEPARSER_H_
#define STYLEPARSER_H_

#include <declarations.h>
#include <Redis/RedisCore.h>
#include <Types/NodeValue.h>
#include <Parsers/Parsers.h>
#include <Mogu.h>
#include <Parsers/NodeValueParser.h>

#include <Wt/WAnimation> //For AnimationEffect enum.

namespace Parsers
{

typedef Enums::WidgetTypes::WidgetTypes WidgetType;

namespace StyleParser
{

/*!\brief Returns whether or not a widget has a given property.
 *
 * @param nodeName The widget node in question
 * @param property The property sought
 * @return
 */

inline std::string getWidgetProperty(const std::string&,const char*);

inline bool nodeHasProperty(const std::string& nodeName, const char* property)
{
	mApp;
	app->redisCommand("hexists", nodeName, property);
	return (bool) Redis::getInt(app->reply());
}

inline bool widgetHasProperty(
		const std::string& nodeName, const char* property)
{
	if (!nodeHasProperty(nodeName, property)
		&& nodeHasProperty(nodeName, "template"))
	{
		std::string tpl = "templates."+
				getWidgetProperty(nodeName, "template");

		return nodeHasProperty(tpl, property);
	}
	mApp;
	app->redisCommand("hexists", nodeName, property);
	return (bool) Redis::getInt(app->reply());
}

/*!\brief Retrieves a property from a widget template. Essentially
 * the same as the getWidgetProperty field, but named differently
 * for readability and maintainability.
 * @param tpl_node The template node
 * @param tpl_field The property being retrieved
 * @return the value stored at the template node
 */
inline std::string getFromTemplate(
		std::string& tpl_node,
		const char* tpl_field)
{
	mApp;
	app->redisCommand("hget", tpl_node, tpl_field);
	return Redis::toString(app->reply());
}

/*!\brief Retrieves an arbirary property from a widget node. If the
 * node does not contain the property, a template property will be
 * sought, and if if found will be searched for the property in question.
 *
 * @param nodeName The node name in question
 * @param property The property being sought.
 * @return
 */
inline std::string getWidgetProperty(
		const std::string& nodeName, const char* property)
{
	/* If the widget node does not have the property in question,
	 * but does have the 'template' property, we'll build the template
	 * node name and then get that property from the template.
	 * An error will occur if the property does not exist at the
	 * template node, meaning it is up to the CLI importer to assert
	 * that things are clean from the user's mogu syntax.
	 */
	if (!nodeHasProperty(nodeName, property)
		&& nodeHasProperty(nodeName, "template"))
	{
		std::string tpl = getWidgetProperty(nodeName, "template");
		tpl = "templates."+tpl;
		std::string prop = getFromTemplate(tpl,property);
		return prop;
	}
	mApp;
	app->redisCommand("hget", nodeName, property);
	return Redis::toString(app->reply());
}



/*!\brief Retrieves the widget type as a string, converting it into an
 * enumerated type.
 * @param nodeName The widget node in question.
 * @return The enumerated type of the widget.
 */
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

/*!\brief Retrieves an animation effect for a Wt::WStackedWidget, converting
 * it from a string into its native enumerative type.
 * @param nodeName The widget node in question
 * @return The enumerated Wt::WAnimation value
 */
inline Wt::WAnimation::AnimationEffect getWidgetAnimation(
        std::string nodeName)
{
	std::string animation_str = getWidgetProperty(nodeName,"animation");
    Nodes::NodeValue val;
    NodeValueParser value(animation_str, &val, NONE,
            &Parsers::enum_callback<Parsers::WtAnimationParser>);
    return (Wt::WAnimation::AnimationEffect) val.getInt();
}

/*!\brief Retrieves a list of children from the widget's .children node,
 *
 * @param nodeName (in) The widget in question
 * @param children (in|out) An empty vector which will be populated.
 */
inline void getWidgetChildren(
        std::string nodeName, Redis::strvector& children)
{
    nodeName.append(".children");
    int num_children =0;
    mApp;
    app->redisCommand("llen", nodeName);
    num_children = Redis::getInt(app->reply());
    std::string _num_children = itoa(num_children);
    app->redisCommand("lrange", nodeName, "0", _num_children);
    Redis::toVector(app->reply(), children);
}

/*!\brief Returns whether or not the widget is considered dynamic for
 * reading purposes.
 * @param nodeName The widget in question
 * @return
 */
inline bool widgetIsDynamic(std::string nodeName)
{
	std::string type_str = getWidgetProperty(nodeName,"type");
	size_t index = type_str.find("dynamic");
#ifdef DEBUG
	if (index != std::string::npos)
	{
		std::cout << nodeName << " IS DYNAMIC" << std::endl;
	}
#endif
	return index != std::string::npos;
}

/*!\brief Returns a masked byte that contains encoded information about
 * which actions the widget blocks.
 * @param nodeName The widget in question
 * @return The bit-mapped value
 */
mapped getActionBlock(std::string nodeName);

} //namespace StyleParser

} //namespace Parser

#endif /* STYLEPARSER_H_ */
