/*
 * Edge.h
 *
 *  Created on: Sep 12, 2012
 *      Author: tom
 */

#ifndef EDGE_H_
#define EDGE_H_

namespace Bayes{

class Edge
{
private:
	std::pair <Node*,Node*> __nodes;
	EdgeDirection __direction;
	Manipulation* __traversal;

public:
	Edge(Node* source, Node* destination, EdgeDirection direction);
	void traverse ()
	{
		if (__traversal != 0)
		{
			__traversal->stage(__nodes);
			__traversal->exec();
		}
	}

	void onTraverse(Manipulation* traversal)
	{
		__traversal = traversal;
	}
};

}//namespace Bayes


#endif /* EDGE_H_ */
