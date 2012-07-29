#include "Mogu.h"

Wt::WApplication* createApplication(const Wt::WEnvironment& env)
{
    return new Mogu(env);
}

int main (int argc, char** argv)
{
    Wt::ApplicationCreator fn = &createApplication;
    return Wt::WRun(argc, argv, fn);
}

