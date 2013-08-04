 /*
 *  Created on: Dec 10, 2012
 *      Author: tom
 */


#include "MoldableStack.h"
#include <Wt/WAnimation>
#include <Types/WidgetAssembly.h>

MoldableStack::MoldableStack(WidgetAssembly* assembly)
: MoldableAbstractParent(assembly, MoguSyntax::stack)
{
    init();
}
