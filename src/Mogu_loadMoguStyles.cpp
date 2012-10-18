/*
 * Mogu_loadMoguStyles.cpp
 *
 *  Created on: Oct 18, 2012
 *      Author: tom
 */

#include <Mogu.h>
#include <declarations.h>
#include <Wt/WCssStyleSheet>

/*!\brief This method loads styles used by Mogu in order to properly
 * handle certain fringe cases that Wt doesn't natively handle, and will
 * give more expected beavhiour in some cases.
 */

void Mogu::loadMoguStyles()
{
	Wt::WCssStyleSheet& css = styleSheet();
	css.addRule(".mogu_SCALED_IMAGE","width:100%;", "scaledImages");
}


