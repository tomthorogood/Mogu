/*
 * preview.cpp
 *
 *  Created on: Jul 17, 2012
 *      Author: tom
 */

#include <Static.h>
#include <algorithm>

#include <Wt/WApplication>
#include <Wt/WScrollArea>
#include <Wt/WString>
#include <Wt/WStackedWidget>
#include <TurnLeftLib/Utils/randomcharset.h>

#include <Mogu.h>
#include <Moldable/Moldable.h>
#include <Factories/MoldableFactory.h>
#include <Perspectives/PerspectiveHandler.h>
#include <hash.h>
#include <Wt/WOverlayLoadingIndicator>
#include <Types/ApplicationManager.h>

Mogu::Mogu(
    const Wt::WEnvironment& env)
    : Wt::WApplication(env), widgetRegister(), manager(*this), userManager(
        *this)
{
    TurnLeft::Utils::RandomCharSet rchar;
    setLoadingIndicator(new Wt::WOverlayLoadingIndicator());

    __group = DEFAULT_GROUP;
    __instanceid = rchar.generate(4);

    __reply = 0;
    __redis = redisConnect(REDIS_HOST, REDIS_PORT);
    std::string styleSheet("/resources/mogu/style.css");
    useStyleSheet(styleSheet);

    __wrapper = moldableFactory.createMoldableWidget("widgets.wrapper");

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
    std::string perspective = path.substr(1, path.length() - 1);
    Parsers::TokenGenerator t(perspective);
    std::string mold = t.next('/');
    while (mold != EMPTY) {
        Perspective::Handler::mold(mold);
        mold = t.next('/');
    }

}

Mogu::~Mogu()
{
    redisFree(__redis);
}
