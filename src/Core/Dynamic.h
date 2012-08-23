/*
 * Dynamic.h
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#ifndef DYNAMIC_H_
#define DYNAMIC_H_

#include <declarations.h>
#include <Sessions/Submission.h>
#include <Wt/WSignal>
#include <Core/Moldable.h>

namespace Goo
{
class Dynamic : public Moldable
{
private:
	std::string __auth_token;
	std::string __storage_locker;
	void store ( Nodes::NodeValue* data );
	void submitValue();

	Wt::Signal <> __submitted;

public:
	Dynamic (
			std::string auth_token,
			std::string constructorNode,
			Wt::WContainerWidget* parent =0
			);

	Wt::Signal <>& submitted();

	Nodes::NodeValue* requestValue ();
	std::string requestAuthorization();
	std::string storageLocker();

	virtual void load();
};

}//namespace Goo
#endif /* DYNAMIC_H_ */
