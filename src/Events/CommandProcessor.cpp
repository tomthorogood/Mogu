#include "CommandProcessor.h"
#include <Moldable/Moldable.h>
#include <Types/CommandValue.h>
#include "Actions.h"

CommandProcessor::CommandProcessor(Moldable& broadcaster_)
    :   broadcaster(broadcaster_)
{}

void CommandProcessor::processCommand(CommandValue& v)
{
    switch((MoguSyntax) v.get(CommandFlags::ACTION))
    {
        case MoguSyntax::set:
            Actions::set(broadcaster,v);
            break;
        case MoguSyntax::increment:
            Actions::increment(broadcaster,v);
            break;
        case MoguSyntax::decrement:
            Actions::decrement(broadcaster,v);
            break;
        case MoguSyntax::test:
            Actions::test(broadcaster,v);
            break;
        case MoguSyntax::email:
            Actions::email(broadcaster,v);
            break;
        case MoguSyntax::reload:
            Actions::reload(broadcaster,v);
            break;
        case MoguSyntax::append:
            Actions::append(broadcaster,v);
            break;
        case MoguSyntax::reset:
            Actions::reset(broadcaster,v);
            break;
        case MoguSyntax::remove:
            Actions::remove(broadcaster,v);
            break;
        case MoguSyntax::script:
            Actions::javascript(broadcaster,v);
            break;
        default:return;
    }
}
