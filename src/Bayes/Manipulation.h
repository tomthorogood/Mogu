/*
 * Manipulation.h
 *
 *  Created on: Sep 12, 2012
 *      Author: tom
 */

#ifndef MANIPULATION_H_
#define MANIPULATION_H_

namespace Bayes{

class Manipulation
{
private:
	std::pair <Node*,Node*> __nodes;
	void (*__call)(Node*,Node*);

public:
	void stage (const std::pair<Node*,Node*>&);
	inline void setExec (NodeManipulator manipulator)
	{
		__call = manipulator;
	}

	inline void exec()
	{
		__call(__nodes.first, __nodes.second);
	}
};

}//namespace Bayes


#endif /* MANIPULATION_H_ */
