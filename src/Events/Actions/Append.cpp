#include "../Actions.h"
#include "Includes.h"

namespace Actions {

namespace {
    // "append [object [identifier]] to [object [identifier]]"
    const uint8_t OBJECT_TO_OBJECT      =1;

    // "append [value] to [object identifier attribute]"
    const uint8_t VALUE_TO_ATTRIBUTE    =2;

    // "append [value] to [data|user|group identifier [argument]]"
    const uint8_t VALUE_TO_FIELD        =3;

    // "append 
    const uint8_t getConstruct(CommandValue& v)
    {
        uint8_t flags = v.getFlags();
        return flags;
    }

}//anonymous namespace

void append(Moldable& broadcaster, CommandValue& v)
{
    
}

}//namespace Actions
