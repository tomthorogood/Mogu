#ifndef PERSPECTIVEHANDLER_H_
#define PERSPECTIVEHANDLER_H_

#include "CommandProcessor.h"
#include <Redis/MoguQueryHandler.h>
#include <vector>
#include <string>

class Perspective_Handler : public Command_Processor
{
public:
    Perspective_Handler(Moldable& broadcaster, const std::string& perspective);
    void mold_perspective();
private:
    Redis::Node_Editor db {};
    std::vector <std::string> commands {};
    int num_commands {};
    std::string perspective {};
};

#endif //PERSPECTIVEHANDLER_H_
