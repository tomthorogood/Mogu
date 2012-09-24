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

#include <Wt/WAnimation> //For AnimationEffect enum.

namespace Parsers
{

typedef Enums::WidgetTypes::WidgetTypes WidgetType;

namespace StyleParser
{

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
	Redis::command("hget", tpl_node, tpl_field);
	return Redis::toString();
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

/*!\brief Retrieves the "content" field of the widget.
 *
 * @param nodeName The widget node in question
 * @return the value stored at the 'content' field.
 */
inline std::string getWidgetText(std::string nodeName)
{
    std::string reply_str = getWidgetProperty(nodeName, "content");
    Nodes::NodeValue val;
    Parsers::NodeValueParser parser(reply_str, &val, NONE);
    return val.getString();
}

/*!\brief Retrieves an animation effect for a Wt::WStackedWidget, converting
 * it from a string into its native enumerative type.
 * @param nodeName The widget node in question
 * @return The enumerated Wt::WAnimation value
 */
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
    Redis::command("llen", nodeName);
    num_children = Redis::getInt();
    std::string _num_children = itoa(num_children);
    Redis::command("lrange", nodeName, "0", _num_children);
    Redis::toVector(children);
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
		std::string nodeName, const char* property)
{
	/* If the widget node does not have the property in question,
	 * but does have the 'template' property, we'll build the template
	 * node name and then get that property from the template.
	 * An error will occur if the property does not exist at the
	 * template node, meaning it is up to the CLI importer to assert
	 * that things are clean from the user's mogu syntax.
	 */
	if (!widgetHasProperty(nodeName, property)
		&& widgetHasProperty(nodeName, "template"))
	{
		std::string tpl = getWidgetProperty(nodeName, "template");
		tpl = "templates."+tpl;
		std::string prop = getFromTemplate(tpl,property);
		return prop;
	}
	Redis::command("hget", nodeName, property);
	return Redis::toString();
}

/*!\brief Returns whether or not a widget has a given property.
 *
 * @param nodeName The widget node in question
 * @param property The property sought
 * @return
 */
inline bool widgetHasProperty(std::string nodeName, std::string property)
{
	Redis::command("hexists", nodeName, property);
	return (bool) Redis::getInt();
}

/*!\brief Returns whether not the widget has children.
 *
 * @param nodeName The widget being questioned
 * @return
 */
inline bool widgetHasChildren(std::string nodeName)
{
    nodeName.append(".children");
    Redis::command("exists", nodeName);
    return Redis::getInt() == 1;
}

/*!\brief Returns whether or not the widget blocks certain actions.
 *
 * @param nodeName The widget name in question.
 * @return
 */
inline bool widgetBlocksActions(std::string nodeName)
{
    Redis::command("hexists", nodeName, "block");
    return (bool) Redis::getInt();
}

/*!\brief Returns whether or not the widget is considered dynamic for
 * reading purposes.
 * @param nodeName The widget in question
 * @return
 */
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

/*!\brief Returns a masked byte that contains encoded information about
 * which actions the widget blocks.
 * @param nodeName The widget in question
 * @return The bit-mapped value
 */
mapped getActionBlock(std::string nodeName);

} //namespace StyleParser

} //namespace Parser

#endif /* STYLEPARSER_H_ */
