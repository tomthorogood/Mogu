#include "Mogu.h"
#include <exception>
#include <Types/syntax.h>
#include <Redis/DatabaseConfigReader.h>
#include "Types/AssemblyCache.h"

namespace Application { 
    Timed_Assembly_Cache global_cache {300};
}


Wt::WApplication* createApplication(
    const Wt::WEnvironment& env)
{
    return new Mogu(env);
}

int main(
    int argc, char** argv)
{
    Application::load_database_contexts();
    Wt::ApplicationCreator fn = &createApplication;
    return Wt::WRun(argc, argv, fn);
}
