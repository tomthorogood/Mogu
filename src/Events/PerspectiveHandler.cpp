#include "PerspectiveHandler.h"
#include <Parsers/NodeValueParser.h>
#include <Types/CommandValue.h>
#include <Types/MoguLogger.h>
#include <Mogu.h>


namespace Application {
    extern Mogu_Logger log;
}

Perspective_Handler::Perspective_Handler(Moldable& w, const std::string& p)
    : Command_Processor(w)
    , db(Prefix::perspectives)
    , perspective(p)
{
    const char* c_node = perspective.c_str();
    db.append_query("exists perspectives.%s", c_node);
    db.append_query("llen perspectives.%s", c_node);
    if (!db.yield_response<bool>()) return;
    num_commands = db.yield_response<int>();
}

void Perspective_Handler::mold_perspective()
{
    mApp;
    Parsers::Node_Value_Parser& nvp = app->get_interpreter();
    db.append_query("lrange perspectives.%s 0 %d", 
        perspective.c_str(),num_commands);    
    commands = db.yield_response <std::vector<std::string>>();
    for (std::string cmd : commands)
    {
        Command_Value v(broadcaster);
        Application::log.log(Log_Level::notice,
                "Processing Perspective ", perspective, " command: " ,cmd);
        nvp.give_input(cmd,v);
        process_command(v);
    }
}
