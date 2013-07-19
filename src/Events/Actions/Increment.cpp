#include "../Actions.h"
#include "Includes.h"
#include <Redis/NodeEditor.h>
#include <Groups/GroupManager.h>
namespace Actions {

void increment (Moldable& broadcaster, CommandValue& v)
{
    mApp;
    const SyntaxDef& object = MoguSyntax::get(v.get(CommandFlags::OBJECT));
    Prefix prefix = syntax_to_prefix.at(object);
    std::string identifier = (std::string) v.get(CommandFlags::IDENTIFIER);
    NodeValue arg;
    if (v.test(CommandFlags::ARG))
        arg = v.get(CommandFlags::ARG);
    Redis::NodeEditor node(prefix, identifier, &arg);

    int value = 
        (v.getFlags() & (uint8_t) CommandFlags::VALUE) ?
        v.get(CommandFlags::VALUE).getInt() : 1;
    NodeValue final;

    switch(object)
    {
        case MoguSyntax::own:{
            NodeValue current;
            broadcaster.getAttribute(MoguSyntax::get(
                v.get(CommandFlags::ARG)), current);
            if (current.getType() == ReadType::string_value)
                current.setInt(atoi(current.getString().c_str()));
            current.setInt(current.getInt()+value);
            broadcaster.setAttribute(MoguSyntax::get(
                    v.get(CommandFlags::ARG)), current);
            break;}

        case MoguSyntax::group:{
            GroupManager groupMgr(app->getGroup());
            if (!groupMgr.hasWriteAccess(identifier)) return;}
        case MoguSyntax::data:
        case MoguSyntax::user:{
            std::string val = node.read();
            final.setInt(
                final.getInt() - atoi(val.c_str()));
            node.write(final);
            break;}

        case MoguSyntax::slot:{
            mApp;
            final.setInt(
                app->slotManager().retrieveSlot(v.getIdentifier()).getInt());
            final.setInt(final.getInt()+value);
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
            current.setInt(current.getInt()+value);
            widget->setAttribute(MoguSyntax::get(
                v.get(CommandFlags::ARG)), current );
            break;}
        default: return;
    }
} 

}//namespace Actions
