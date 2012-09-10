/*
 * Wrapped_StaticNode.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tom
 */

#ifndef WRAPPED_STATICNODE_H_
#define WRAPPED_STATICNODE_H_

#include <declarations.h>
#include <Types/WrappedValue.h>


/*!\brief Provides a way to obtain generic strings from the database that are
 * located at any node.
 *
 * This wrapping class adds much needed dynamic handling to Mogu, by
 */
class Wrapped_StaticNode :
	public WrappedValue <Maps::Strings>
{
private:
	Maps::Strings __node_map;
	bool interpreted;
public:

	Wrapped_StaticNode (std::string value);
	Maps::Strings interpret(uint8_t nargs=0, ...);

	inline std::string operator[](std::string key)
	{
		if (!interpreted)
		{
			__node_map = interpret();
		}
		return __node_map[key];
	}
};

#endif /* WRAPPED_STATICNODE_H_ */
