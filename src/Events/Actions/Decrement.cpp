#include "../Actions.h"
#include "Includes.h"
#include <Redis/NodeEditor.h>
#include <Groups/GroupManager.h>
namespace Actions {

void decrement (Moldable& broadcaster, CommandValue& v)
{
    mApp;
    const SyntaxDef& object = MoguSyntax::get(v.get(CommandFlags::OBJECT));
    Prefix prefix = syntax_to_prefix.at(object);
    std::string identifier = (std::string) v.get(CommandFlags::IDENTIFIER);
    NodeValue arg;
    std::string str;

    if (v.test(CommandFlags::ARG))
        arg = v.get(CommandFlags::ARG);
    
    Redis::NodeEditor node(prefix,identifier,&arg);

    NodeValue final;
    int value = 
        (v.getFlags() & (uint8_t) CommandFlags::VALUE) ?
            (int) v.get(CommandFlags::VALUE) : 1;
    switch(object)
    {
        case MoguSyntax::own:{
            NodeValue current;
            broadcaster.getAttribute(MoguSyntax::get(
                        v.get(CommandFlags::ARG)), current);
            if (current.getType() == ReadType::string_value)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()-value);
            broadcaster.setAttribute(MoguSyntax::get(
                        v.get(CommandFlags::ARG)), current);
            break;}

        case MoguSyntax::user:{
            str = node.read();
            final.setInt(final.getInt() - atoi(str.c_str()));
            node.write(final);
            break;}
        case MoguSyntax::group:{
            GroupManager group(app->getGroup());
            if (!group.hasReadAccess(identifier)) return;
            str = node.read();
            final.setInt(
                final.getInt() - atoi(str.c_str()));
            node.write(final);
            break;}

        case MoguSyntax::data:{
            str = node.read();
            final.setInt(
                final.getInt() - atoi(str.c_str()));
            node.write(final);
            break;}

        case MoguSyntax::slot:{
            mApp;
            final.setString(app->slotManager().retrieveSlot(v.getIdentifier()));
            final.setInt(atoi(final.getString().c_str()));
            final.setInt(final.getInt()-value);
            app->slotManager().setSlot(
                    v.getIdentifier(), final);
            break;}

        case MoguSyntax::widget:{
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            NodeValue current;
            widget->getAttribute(MoguSyntax::get(
                        v.get(CommandFlags::ARG)), current);
            if (current.getType() == ReadType::string_value)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()-value);
            widget->setAttribute(MoguSyntax::get(
                        v.get(CommandFlags::ARG)), current );
            break;}

        default: return;
    }
} 


}//namespace Actions
