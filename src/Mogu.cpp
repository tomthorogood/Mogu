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
#include <TurnLeftLib/Utils/randomcharset.h>

#include "Mogu.h"
#include "Moldable/Moldable.h"
#include "Factories/MoldableFactory.h"
#include "hash.h"
#include "Redis/DatabaseConfigReader.h"
#include "Redis/ContextMap.h"
#include "Events/PerspectiveHandler.h"
#include "Redis/NodeEditor.h"

Mogu::Mogu(
    const Wt::WEnvironment& env)
    : Wt::WApplication(env), moldableFactory(this), widgetRegister()
{
    userManager = new UserManager();
    TurnLeft::Utils::RandomCharSet rchar;
    setLoadingIndicator(new Wt::WOverlayLoadingIndicator());

    group = 0;
    instanceid = rchar.generate(4);

    std::string styleSheet("/resources/mogu/style.css");
    useStyleSheet(styleSheet);

    Redis::NodeEditor node(Prefix::meta, "root");
    std::string root_widget = node.read();

    wrapper = moldableFactory.createMoldableWidget(root_widget);

    root()->addWidget(wrapper);

    internalPathChanged().connect(this, &Mogu::handlePathChange);

    std::string entry_path = internalPath();

    if (entry_path != "/" && entry_path.length() > 0) {
        handlePathChange(entry_path);
    }

    loadMoguStyles();
}

void Mogu::handlePathChange(const std::string& path)
{
    // Each perspective will be separated by a '/' character.
    std::vector <std::string> perspectives;
    split(path, '/', perspectives);
    for (auto perspective : perspectives)
    {
        PerspectiveHandler proc(*wrapper, perspective);
        proc.moldPerspective();
    }
}

Mogu::~Mogu()
{
    /* Stop the widgets from trying to communicate with the base application
     * during the application's destruction, as it leads to a segfault.
     */
    auto iterator = widgetRegister.begin();
    while (iterator != widgetRegister.end())
    {
        if (iterator->second != nullptr)
            iterator->second->shun();
        ++iterator;
    }
    delete userManager;
}
