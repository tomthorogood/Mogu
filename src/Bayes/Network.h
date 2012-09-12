/*
 * Network.h
 *
 *  Created on: Sep 12, 2012
 *      Author: tom
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <declarations.h>

namespace Bayes{

class Network
{
private:
	std::vector <Node*> __nodes;
	std::string __keyspace;

public:
	Network (std::string keyspace ="");
	inline void add (Node* node)
	{
		__nodes.push_back(node);
	}
	void remove (Node*);
};

}//namespace Bayes



#endif /* NETWORK_H_ */
