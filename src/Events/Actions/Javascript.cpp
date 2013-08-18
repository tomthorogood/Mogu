#include "../Actions.h"
#include "Includes.h"

namespace Actions {

void javascript(Moldable& broadcaster, Command_Value& v)
{
    mApp;
    app->doJavaScript(v.get(Command_Flags::value).get_string());
}

}//namespace Actions
