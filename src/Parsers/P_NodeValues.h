/*
 * P_NodeValues.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef P_NODEVALUES_H_
#define P_NODEVALUES_H_

#include <declarations.h>

namespace Parsers{

class NodeValueTypeParser :
    public TurnLeft::Utils::EnumParser <Enums::NodeValueTypes::NodeValueTypes>
{
	public: NodeValueTypeParser()
	{
		namespace NodeValueTypes = Enums::NodeValueTypes;
		enumMap["string_value"]     = NodeValueTypes::string_value;
		enumMap["field_value"]      = NodeValueTypes::field_value;
		enumMap["enum_value"]       = NodeValueTypes::enum_value;
		enumMap["dynamic_node_value"] = NodeValueTypes::dynamic_node_value;
		enumMap["static_node_value"] = NodeValueTypes::static_node_value;
		enumMap["float_value"]      = NodeValueTypes::float_value;
	}
};

}//namespace Parsers



#endif /* P_NODEVALUES_H_ */
