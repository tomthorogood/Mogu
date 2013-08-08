#ifndef PERSPECTIVEHANDLER_H_
#define PERSPECTIVEHANDLER_H_

#include <declarations.h>
#include "Command_Processor.h"
#include <Redis/MoguQueryHandler.h>

class Perspective_Handler : public Command_Processor
{
public:
    Perspective_Handler(Moldable& broadcaster, const std::string& perspective);
    void mold_perspective();
private:
    Redis::Mogu_Query_Handler db {};
    std::vector <std::string> commands {}
    int num_commands {};
    std::string perspective {};
};

#endif //PERSPECTIVEHANDLER_H_
