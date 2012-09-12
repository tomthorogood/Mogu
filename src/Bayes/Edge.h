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

public:
	Edge(Node* source, Node* destination, EdgeDirection direction);
	void traverse (Manipulation* manipulation =0)
	{
		if (manipulation != 0)
		{
			manipulation->stage(__nodes);
			manipulation->exec();
		}
	}
};

}//namespace Bayes


#endif /* EDGE_H_ */
