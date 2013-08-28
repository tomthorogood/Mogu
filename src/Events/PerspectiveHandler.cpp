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
    , db(Prefix::perspective, p)
    , perspective(p)
{
}

void Perspective_Handler::mold_perspective()
{
    mApp;
    Parsers::Node_Value_Parser& nvp = app->get_interpreter();
    db.read(commands);

    for (std::string cmd : commands)
    {
        Command_Value v(broadcaster);
        Application::log.log(Log_Level::notice,
                "Processing Perspective ", perspective, " command: " ,cmd);
        nvp.give_input(cmd,v);
        process_command(v);
    }
}
