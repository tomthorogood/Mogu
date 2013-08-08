#include "Command_Processor.h"
#include "../Types/Command_Value.h"
#include "Actions.h"


void Command_Processor::process_command(Command_Value& v)
{
    switch(Mogu_Syntax::get(v.get(Command_Flags::action)))
    {
        case Mogu_Syntax::set:
            Actions::set(broadcaster,v);
            break;
        case Mogu_Syntax::increment:
            Actions::increment(broadcaster,v);
            break;
        case Mogu_Syntax::decrement:
            Actions::decrement(broadcaster,v);
            break;
        case Mogu_Syntax::test:
            Actions::test(broadcaster,v);
            break;
        case Mogu_Syntax::email:
            Actions::email(broadcaster,v);
            break;
        case Mogu_Syntax::reload:
            Actions::reload(broadcaster,v);
            break;
        case Mogu_Syntax::append:
            Actions::append(broadcaster,v);
            break;
        case Mogu_Syntax::reset:
            Actions::reset(broadcaster,v);
            break;
        case Mogu_Syntax::remove:
            Actions::remove(broadcaster,v);
            break;
        case Mogu_Syntax::script:
            Actions::javascript(broadcaster,v);
            break;
        case Mogu_Syntax::clear:
            Actions::clear(broadcaster,v);
            break;
        default:return;
    }
}
