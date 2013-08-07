#ifndef PERSPECTIVEHANDLER_H_
#define PERSPECTIVEHANDLER_H_

#include <declarations.h>
#include "CommandProcessor.h"
#include <Redis/MoguQueryHandler.h>

class Perspective_Handler : public CommandProcessor
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
