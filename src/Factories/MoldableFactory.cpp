/*
 * Moldable_Factory.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include "MoldableFactory.h"
#include <Moldable/Implementations.h>
#include <Types/syntax.h>
#include <Mogu.h>
#include <Types/MoguLogger.h>

namespace Application {
    extern Mogu_Logger log;
}

Moldable* Moldable_Factory::create_moldable_widget(const std::string& node)
{
    mApp;
    static int iters = {};
    ++iters;
    Application::log.log(Log_Level::notice,
            "Moldable_Factory::create_moldabe_widget:", __LINE__,
            " : Creating Widget ", node, " ", iters++, ")");

    Widget_Assembly&& assembly = server.request(
            node,app->get_user(), app->get_group());

    Node_Value& s = assembly.attrdict[Mogu_Syntax::type.integer];
    const Syntax_Def& widget_type = Mogu_Syntax::get(s);
    Moldable* product {};

    switch(widget_type)
    {
    case Mogu_Syntax::container:
        product = new Moldable_Container(assembly);
        break;
    case Mogu_Syntax::stack:
        product = new Moldable_Stack(assembly);
        break;
    case Mogu_Syntax::text:
        product = new Moldable_Text(assembly);
        break;
    case Mogu_Syntax::anchor:
        product = new Moldable_Link(assembly);
        break;
    case Mogu_Syntax::image:
        product = new Moldable_Image(assembly);
        break;
    case Mogu_Syntax::image_link:
        product = new Moldable_Image_Link(assembly);
        break;
    case Mogu_Syntax::input:
        product = new Moldable_Input(assembly);
        break;
    case Mogu_Syntax::password:
        product = new Moldable_Password(assembly);
        break;
    default:
        product = new Moldable_Container(assembly);
        break;
    }

    return product;
}
