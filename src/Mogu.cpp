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
#include <TurnLeftLib/Utils/explosion.h>
#include <TurnLeftLib/Utils/randomcharset.h>

#include <Mogu.h>
#include <Core/Moldable.h>
#include <Core/MoldableFactory.h>
#include <Core/MoldableTemplate.h>
#include <Parsers/StyleParser.h>
#include <Perspectives/PerspectiveHandler.h>
#include <hash.h>

Mogu::Mogu(const Wt::WEnvironment& env)
:   Wt::WApplication(env)
{
	TurnLeft::Utils::RandomCharSet rchar;

	__session = GLOBAL_SESSION;
	__auth_token = AUTH_TOKEN;
	__instanceid = rchar.generate(4);

	widgetRegister = new WidgetRegister();
	__reply = 0;
	__redis = redisConnect(REDIS_HOST,REDIS_PORT);
    std::string styleSheet("/resources/mogu/style.css");
    useStyleSheet(styleSheet);

    std::string outermost_container = "widgets.wrapper";
    Goo::MoldableTemplate* __wrapper_tmpl =
    		Goo::MoldableFactory::conceptualize(outermost_container);


    __wrapper = Goo::MoldableFactory::sculpt(__wrapper_tmpl);

    root()->addWidget(__wrapper);

    internalPathChanged().connect(this, &Mogu::handlePathChange);

    std::string entry_path = internalPath();
#ifdef DEBUG
    std::cout << "Entry Path: " << entry_path << std::endl;
#endif

    if (entry_path != "/" && entry_path.length() > 0)
    {
    	handlePathChange(entry_path);
    }

    if (Application::metaKeyConfigured("analytics"))
    {
    	loadAnalytics(Application::getMetaValue("analytics"));
    }
}

void Mogu::handlePathChange(std::string path)
{
	std::string perspective = path.substr(1,path.length()-1);
#ifdef DEBUG
	std::cout << "Perspective Chain: " << perspective << std::endl;
#endif
	TurnLeft::Utils::Explosion explosion(perspective);
	std::string molds[16];
	explosion.explode('/',molds);
	int num_molds = explosion.getNumWords();
	for (int m = 0; m < num_molds; m++)
	{
		std::string mold = molds[m];
		Perspective::Handler::mold(mold);
	}
}

Mogu::~Mogu()
{
	if (__reply != 0) freeReplyObject(__reply);
	redisFree(__redis);
}
