/*
 * Dynamic.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: tom
 */

#include <Core/Dynamic.h>
#include <Sessions/Submission.h>
#include <Core/Sculptory.h>

namespace Goo{

Dynamic::Dynamic(
		std::string auth,
		std::string constructorNode,
		Wt::WContainerWidget* parent)
: Moldable(constructorNode,parent), __submitted(this)
{
	__auth_token 	= auth;

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

std::string Dynamic::requestAuthorization()
{
	return __auth_token;
}

std::string Dynamic::storageLocker()
{
	return __storage_locker;
}

void Dynamic::load()
{
	if (!loaded() || reload())
	{
		Moldable::load();
		Sessions::SubmissionHandler::emerge(this);
	}
}

}//namespace Goo

