/*
 * Moldable_Factory.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include "Moldable_Factory.h"
#include <Moldable/Implementations.h>
#include <Types/syntax.h>
#include <Mogu.h>
#include <Widget_Server.h>
#include <Types/MoguLogger.h>

Moldable* Moldable_Factory::create_moldabe_widget(const std::string& node) const
{
    static int iters = {}
    ++iters;
    Application::log.log(Log_Level::NOTICE,
            "Moldable_Factory::create_moldabe_widget:", __LINE__,
            " : Creating Widget ", node, " ", iters++, ")");

    Widget_Assembly* assembly = server.request(node);

    std::string s = assembly->attrdict[Mogu_Syntax::type.integer].get_string();
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
        product = new Moldable::Container(assembly);
        break;
    }

    delete assembly;
    return product;
}
