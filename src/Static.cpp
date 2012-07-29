/*
 * Static.cpp
 *
 *  Created on: Jul 29, 2012
 *      Author: tom
 */

#include <Static.h>

namespace Application
{
	namespace {
		Mogu* app;
	}
	Mogu* mogu()
	{
		return app;
	}

	void defineMogu(Mogu* application)
	{
		app = application;
	}
}


