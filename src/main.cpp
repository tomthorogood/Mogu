#include "Mogu.h"
#include <exception>
#include <Types/syntax.h>
#include <Redis/DatabaseConfigReader.h>


Wt::WApplication* createApplication(
    const Wt::WEnvironment& env)
{
    return new Mogu(env);
}

int main(
    int argc, char** argv)
{
    Wt::ApplicationCreator fn = &createApplication;
    return Wt::WRun(argc, argv, fn);
}
