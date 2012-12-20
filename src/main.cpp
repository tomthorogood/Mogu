#include "Mogu.h"

#ifdef DEBUG
#include <exception>
#endif
#include <Types/syntax.h>

#include "../syntax/syntax.txt"

Wt::WApplication* createApplication(
    const Wt::WEnvironment& env)
{
    return new Mogu(env);
}

int main(
    int argc, char** argv)
{
#ifdef DEBUG
    assert(Tokens::syntaxMap.test());
#endif
    Wt::ApplicationCreator fn = &createApplication;
    return Wt::WRun(argc, argv, fn);
}
