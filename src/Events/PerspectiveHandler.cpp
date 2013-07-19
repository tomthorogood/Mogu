#include "PerspectiveHandler.h"
#include <Parsers/NodeValueParser.h>
#include <Types/CommandValue.h>
#include <Mogu.h>

PerspectiveHandler::PerspectiveHandler(Moldable& broadcaster, const std::string& perspective_)
    : CommandProcessor(broadcaster), db(Prefix::perspectives), perspective(perspective_)
{
    const char* c_node = perspective.c_str();
    db.appendQuery("exists perspectives.%s", c_node);
    db.appendQuery("llen perspectives.%s", c_node);
    if (!db.yieldResponse<bool>()) return;
    num_commands = db.yieldResponse<int>();
}

void PerspectiveHandler::moldPerspective()
{
    mApp;
    Parsers::NodeValueParser& nvp = app->interpreter();
    db.appendQuery("lrange perspectives.%s 0 %d", 
        perspective.c_str(),num_commands);    
    commands = db.yieldResponse <std::vector<std::string>>();
    for (auto cmd : commands)
    {
        CommandValue v(broadcaster);
#ifdef DEBUG
        std::cout << "Processing command: " << cmd << std::endl;
#endif
        nvp.giveInput(cmd,v);
        processCommand(v);
    }
}