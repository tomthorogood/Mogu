/*
 * MoldableFactory.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: tom
 */

#include "MoldableFactory.h"
#include <Moldable/Implementations.h>
#include <Types/syntax.h>
#include <Mogu.h>
#include <WidgetServer.h>
#include <Types/MoguLogger.h>

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
    static int iters = 0;
    ++iters;
    Application::log.log(LogLevel::NOTICE,
            "MoldableFactory::createMoldableWidget:", __LINE__,
            " : Creating Widget ", node, " ", iters++, ")");
    WidgetAssembly* assembly = server->request(node);
    std::string s_type = (std::string)
        assembly->attrdict[MoguSyntax::type.integer];
    const SyntaxDef& widget_type = MoguSyntax::get(s_type);
    Moldable* product = nullptr;
    switch(widget_type)
    {
    case MoguSyntax::container:
        product =  new MoldableContainer(assembly);
        break;
    case MoguSyntax::stack:
        product =  new MoldableStack(assembly);
        break;
    case MoguSyntax::text:
        product =  new MoldableText(assembly);
        break;
    case MoguSyntax::anchor:
        product =  new MoldableLink(assembly);
        break;
    case MoguSyntax::image:
        product =  new MoldableImage(assembly);
        break;
    case MoguSyntax::image_link:
        product =  new MoldableImageLink(assembly);
        break;
    case MoguSyntax::input:
        product =  new MoldableInput(assembly);
        break;
    case MoguSyntax::password:
        product =  new MoldablePassword(assembly);
        break;
    default:
        product =  new MoldableContainer(assembly);
        break;
    }

    delete assembly;
    return product;
}
