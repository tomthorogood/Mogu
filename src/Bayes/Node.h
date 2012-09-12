/*
 * Node.h
 *
 *  Created on: Sep 12, 2012
 *      Author: tom
 */

#ifndef NODE_H_
#define NODE_H_

namespace Bayes{

class Node{
	Nodes::NodeValue* __value;
	std::vector <Edge*> __connections;
	std::string __redis_key;

public:
	Node (std::string redis_key);
	void connect (Node* node, EdgeDirection direction)
	{
		Edge* edge;
		if (direction == outbound) edge = new Edge(node, this, direction);
		else edge = new Edge(this, node, direction);
		__connections.push_back(edge);
		node->connect(edge);
	}
	inline void connect (Edge* edge)
	{
		__connections.push_back(edge);
	}

	inline const Nodes::NodeValue* readValue()
	{
		return __value;
	}

	inline void writeValue(Nodes::NodeValue* value)
	{
		__value = value;
		int edges = __connections.size();
		for (int e = 0; e < edges; e++)
		{
			__connections[e]->traverse();
		}
	}
};

}//namespace Bayes


#endif /* NODE_H_ */
