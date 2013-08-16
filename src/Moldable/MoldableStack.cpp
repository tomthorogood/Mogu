 /*
 *  Created on: Dec 10, 2012
 *      Author: tom
 */


#include "MoldableStack.h"
#include <Wt/WAnimation>
#include <Types/WidgetAssembly.h>

Moldable_Stack::Moldable_Stack(Widget_Assembly* assembly)
: Moldable_Abstract_Parent(assembly, Mogu_Syntax::stack)
{
    init();
}
