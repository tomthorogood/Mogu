/*
 * Dynamic.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#include <Core/Dynamic.h>
#include <Sessions/Submission.h>

namespace Goo{

Dynamic::Dynamic(
		std::string auth,
		std::string constructorNode,
		Wt::WContainerWidget* parent)
: Moldable(constructorNode,parent), __submitted(this)
{
	__auth_token = auth;

	/* Removes the ubiquitous "widgets." string from the first part of the
	 * node name.
	 */
	__storage_locker = constructorNode.substr(8,constructorNode.length()-1);
	submitted().connect(this, &Dynamic::submitValue);
}

Wt::Signal<>& Dynamic::submitted()
{
	return __submitted;
}

void Dynamic::submitValue()
{

}

std::string Dynamic::requestAuthorization(bool authorized)
{
	if (authorized)
	{
		return __auth_token;
	}
	else
	{
		return "ERR_UNMATCHED_WT_SESSION";
	}
}

std::string Dynamic::storageLocker()
{
	return __storage_locker;
}


}//namespace Goo

