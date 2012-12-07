/*
 * @param widget
 * @return
 * PropertyCallbacks.h
 *
 *  Created on: Oct 8, 2012
 *      Author: tom
 */

#ifndef PROPERTYCALLBACKS_H_
#define PROPERTYCALLBACKS_H_

#include <declarations.h>

namespace Goo {
namespace MoldableFactory {
/*!\brief These callbacks are bound to the widgets upon creation and make it
 * easier to do simple tasks like get/set values without having to rely on
 * remembering what is a child of what and where.
 */
namespace Callbacks {

/*!\brief Gets the value of a WText widget. Bound to moldable->valueCallback()*/
std::string __value_wtext(
    Moldable& widget);

/*!\brief Gets the value of a WLineEdit widget. Bound to moldable->valueCallback()*/
std::string __value_wlineedit(
    Moldable& widget);
std::string __value_abstract(
    Moldable& widget);

/*!\brief Since containers do not have a text value, their style will be used. */
std::string __value_container(
    Moldable& widget);

/*!\brief Sets the valu of a WText widget. Bound to moldable->setValueCallback(str)*/
void __setvalue_wtext(
    Moldable& widget, std::string& newtext);

/*!\brief Sets the value of a WLineEdit widget. Bound to moldable->setValueCallback(str)*/
void __setvalue_wlineedit(
    Moldable& widget, std::string& newtext);

}    //namespace Callbacks
}    //namespace ModlableFactory
}    //namespace Goo

#endif /* PROPERTYCALLBACKS_H_ */
