/*
 * Wrapped_StaticNode.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#include <Types/Wrapped_StaticNode.h>
#include <Redis/RedisCore.h>

Wrapped_StaticNode::Wrapped_StaticNode (std::string value)
: WrappedValue <Maps::Strings> ('{','}',value)
{
	interpreted = false;
}

Maps::Strings Wrapped_StaticNode::interpret(uint8_t nargs, ...)
{
	std::string node = unwrap();
	std::string node_type;
	Maps::Strings nodeMap;

	/* First, determine what type of node it is we're dealing with: */
	Redis::command("type", node);
	node_type = Redis::toString();

	/* Then, based on the type, determine how we should populate the map: */
	if (node_type == "hash")
	{
		/* If we have a Redis hash, we need to get the keys, then the values,
		 * then use those two vectors to populate the map.
		 */
		Redis::strvector keys;
		Redis::strvector values;
		Redis::command("hkeys", node);
		Redis::toVector(keys);
		Redis::command("hvals", node);
		Redis::toVector(values);

		int num_vals = keys.size();
		for (int i = 0; i < num_vals; i++)
		{
			nodeMap[ keys.at(i) ] = values.at(i);
		}
	}

	else if (node_type == "list")
	{
		/* If we have a list, we're going to populate the map with string
		 * representations of the integral indexes
		 */
		int num_vals;
		Redis::strvector values;
		Redis::command("llen", node);
		num_vals = Redis::getInt();

		Redis::command("lrange", "0", itoa(num_vals));
		Redis::toVector(values);

		for (int i = 0; i < num_vals; i++)
		{
			nodeMap[ itoa(i) ] = values.at(i);
		}
	}
	else if (node_type == "string")
	{
		/* If we have a string, we only need to store the value in the
		 * first location on the map. We'll name the value simply 'string'.
		 */
		std::string nval;
		Redis::command("get", node);
		nval = Redis::toString();
		nodeMap["string"] = nval;
	}

	return nodeMap;
}

