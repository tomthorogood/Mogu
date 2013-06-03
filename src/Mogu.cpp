/*
 * preview.cpp
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

Mogu::Mogu(
    const Wt::WEnvironment& env)
    : Wt::WApplication(env), widgetRegister()
{
    Application::loadDatabaseContexts();
    userManager = new UserManager();
    TurnLeft::Utils::RandomCharSet rchar;
    setLoadingIndicator(new Wt::WOverlayLoadingIndicator());

    __group = 0;
    __instanceid = rchar.generate(4);

    std::string styleSheet("/resources/mogu/style.css");
    useStyleSheet(styleSheet);

    Redis::ContextQuery db(Prefix::meta);
    CreateQuery(db, "get meta.root");

    std::string root_widget = db.yieldResponse <std::string>();

    __wrapper = moldableFactory.createMoldableWidget(root_widget);

    root()->addWidget(__wrapper);

    internalPathChanged().connect(this, &Mogu::handlePathChange);

    std::string entry_path = internalPath();

    if (entry_path != "/" && entry_path.length() > 0) {
        handlePathChange(entry_path);
    }

    loadMoguStyles();
}

void Mogu::handlePathChange(
    std::string path)
{
}

Mogu::~Mogu()
{
}
