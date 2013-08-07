/*
ed * preview.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: tom
 */

#include <algorithm>

#include <Wt/WApplication>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WScrollArea>
#include <Wt/WString>
#include <Wt/WStackedWidget>

#include "Mogu.h"
#include "Moldable/Moldable.h"
#include "Factories/MoldableFactory.h"
#include "Events/PerspectiveHandler.h"
#include "Redis/NodeEditor.h"

Mogu::Mogu(
    const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , moldable_factory(this)
{

    setLoadingIndicator(new Wt::WOverlayLoadingIndicator());

    std::string style_sheet("/resources/mogu/style.css");
    useStyleSheet(style_sheet);

    Redis::Node_Editor node(Prefix::meta, "root");
    std::string root_widget = node.read();

    wrapper = moldable_factory.create_moldable_widget(root_widget);
    root()->addWidget(wrapper);

    internalPathChanged().connect(this, &Mogu::handle_path_change);

    std::string entry_path = internalPath();
    if (entry_path != "/" && entry_path.length() > 0) {
        handle_path_change(entry_path);
    }
}

void Mogu::handle_path_change(const std::string& path)
{
    std::vector <std::string> perspectives;
    split(path, '/', perspectives);
    for (std::string perspective : perspectives)
    {
        Perspective_Handler p(*wrapper, perspective);
        p.mold_perspective();
    }
}

Mogu::~Mogu()
{
    /* Stop the widgets from trying to communicate with the base application
     * during the application's destruction, as it leads to a segfault.
     */
    for (auto i = widget_register.begin() ; i != widget_register.end(); ++i)
    {
        if (i->second) i->second->shun();
    }
}
