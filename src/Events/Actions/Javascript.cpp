#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void javascript(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    app->doJavaScript(v.get(CommandFlags::VALUE).getString());
}

}//namespace Actions
