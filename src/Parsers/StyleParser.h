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

#include <Wt/WAnimation> //For AnimationEffect enum.

namespace Parsers
{

typedef Enums::WidgetTypes::WidgetTypes WidgetType;

class WidgetPropertyAddress
{
	bool base_field_exists;
	bool base_has_template;
	bool tpl_field_exists;

	std::string tpl_name;

	enum Result {
		DOES_NOT_EXIST	=0x0
		,EXISTS_AT_NODE	=0x1
		,EXISTS_AT_TPL	=0x2
	};
	Mogu* __app;


public:
	WidgetPropertyAddress(std::string base, const char* field)
	{
		//If the "widgts." prefix was passed in, remove it!
		if (base.find("widgets.") == 0) base = base.substr(8);
		mApp;__app = app;
		address = EMPTY;
		tpl_name = EMPTY;
		result = DOES_NOT_EXIST;
		if (exists_at_node(base,field))
		{
			address = "widgets."+base;
			result = EXISTS_AT_NODE;
		}
		else if (exists_at_template(base, field))
		{
			address = "templates."+tpl_name;
			result = EXISTS_AT_TPL;
		}
		else
			result = DOES_NOT_EXIST;
	}

	inline bool exists_at(std::string prefix, std::string base, const char* field)
	{
		std::string saddress = prefix+"."+base;
		const char* address = saddress.c_str();
		__app->redisCommand("hexists %s %s", address, field);
		return (bool) Redis::getInt(__app->reply());
	}

	inline bool exists_at_node(std::string base, const char* field)
	{
		return exists_at("widgets", base, field);
	}

	inline bool exists_at_template(std::string base, const char* field)
	{

		/* If this node has no template */
		std::string saddress = "widgets."+base;
		const char* caddress = saddress.c_str();
		if (!exists_at("widgets", base, "template")) return false;
		__app->redisCommand("hget %s %s", caddress, "template");
		tpl_name = Redis::toString(__app->reply());;
		if (!exists_at("templates", tpl_name, field)) return false;
		return true;
	}
	std::string address;
	Result result;

};

namespace StyleParser
{

inline bool nodeExists(const std::string& nodeName)
{
	mApp;
	const char* cnodename = nodeName.c_str();
	app->redisCommand("exists %s", cnodename);
	return (bool) Redis::getInt(app->reply());
}

inline bool nodeHasField(const std::string& nodeName, const char* property)
{
	mApp;
	const char* cnodename = nodeName.c_str();
	app->redisCommand("hexists %s %s", cnodename, property);
	return (bool) Redis::getInt(app->reply());
}

inline bool setHasMember(const std::string& set, const char* member)
{
	mApp;
	const char* cset = set.c_str();
	app->redisCommand("sismember %s %s", cset, member);
	return (bool) Redis::getInt(app->reply());
}

inline bool widgetHasProperty(const std::string& node, const char* prop)
{
	WidgetPropertyAddress lookup(node, prop);
	return (int) lookup.result > 0;
}

inline bool widgetHasBooleanProperty(const std::string& node, const char* prop)
{
	mApp;
	std::string property_set = node+".properties";
	const char* cpropset = property_set.c_str();
	if (!nodeExists(property_set)) return false;
	app->redisCommand("sismember %s %s", cpropset, prop);
	return (bool) Redis::getInt(app->reply());
}

inline bool widgetHasEvents(
		const std::string& nodeName)
{
	mApp;
	std::string snodename = nodeName+".events.1";
	const char* cnodename = snodename.c_str();
	app->redisCommand("exists %s", cnodename);
	return (bool) Redis::getInt(app->reply());
}

inline std::string getHashEntry(
		std::string& hash_node
		,const char* hash_field
		,std::string alt=EMPTY)
{
	mApp;
	const char* chashnode = hash_node.c_str();
	app->redisCommand("hexists %s %s", chashnode, hash_field);
	if (!Redis::getBool(app->reply())) return alt;
	app->redisCommand("hget %s %s", chashnode, hash_field);
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
	WidgetPropertyAddress lookup(nodeName, property);
	if (lookup.address == EMPTY) return EMPTY;
	mApp;
	const char* caddress = lookup.address.c_str();
	app->redisCommand("hget %s %s", caddress, property);
	return Redis::toString(app->reply());
}



/*!\brief Retrieves the widget type as a string, converting it into an
 * enumerated type.
 * @param nodeName The widget node in question.
 * @return The enumerated type of the widget.
 */
inline WidgetType getWidgetType(std::string nodeName)
{
	mApp;
	WidgetType __type;
	NodeValue val;
	std::string reply_str = getWidgetProperty(
			nodeName, "type");
	app->interpreter().giveInput(reply_str, val, NONE,
			Parsers::enum_callback <Parsers::WidgetTypeParser>);
	__type = (WidgetType) val.getInt();
	return __type;
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
    const char* cnodename = nodeName.c_str();
    app->redisCommand("llen %s", cnodename);
    num_children = Redis::getInt(app->reply());
    app->redisCommand("lrange %s 0 %d", cnodename, num_children);
    Redis::toVector(app->reply(), children);
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
