#ifndef PERSPECTIVEHANDLER_H_
#define PERSPECTIVEHANDLER_H_

#include <declarations.h>
#include "CommandProcessor.h"
#include <Redis/ContextQuery.h>

class PerspectiveHandler : public CommandProcessor
{
public:
    PerspectiveHandler(Moldable& broadcaster, const std::string& perspective);
    void moldPerspective();
private:
    Redis::ContextQuery db;
    std::vector <std::string> commands;
    int num_commands    =0;
    std::string perspective =EMPTY;
};

#endif //PERSPECTIVEHANDLER_H_
