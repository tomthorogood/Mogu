/*
 * MoldableFactory.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include "MoldableFactory.h"
#include <Moldable/Implementations.h>
#include <Types/syntax.h>
#include <Redis/ContextQuery.h>
#include <Mogu.h>
#include <WidgetServer.h>

MoldableFactory::MoldableFactory (Mogu* application) : 
    server(new WidgetServer(application))
{
}

MoldableFactory::~MoldableFactory()
{
    delete server;
}

Moldable* MoldableFactory::createMoldableWidget(const std::string& node) const
{
    WidgetAssembly* assembly = server->request(node);
    std::string s_type = (std::string)
        assembly->attrdict[MoguSyntax::type.integer];
    const SyntaxDef& widget_type = MoguSyntax::get(s_type);
    Moldable* product = nullptr;
    switch(widget_type)
    {
    case MoguSyntax::container:
        product =  new MoldableContainer(assembly);
    case MoguSyntax::stack:
        product =  new MoldableStack(assembly);
    case MoguSyntax::text:
        product =  new MoldableText(assembly);
    case MoguSyntax::anchor:
        product =  new MoldableLink(assembly);
    case MoguSyntax::image:
        product =  new MoldableImage(assembly);
    case MoguSyntax::image_link:
        product =  new MoldableImageLink(assembly);
    case MoguSyntax::input:
        product =  new MoldableInput(assembly);
    case MoguSyntax::password:
        product =  new MoldablePassword(assembly);
    default:
        product =  new MoldableContainer(assembly);
    }

    delete assembly;
    return product;
}
