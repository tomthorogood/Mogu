#include "Includes.h"
#include "../Actions.h"
#include <Redis/NodeEditor.h>
namespace Actions {

namespace {
    const uint8_t SET_USER_FROM_SLOT    =1;
    const uint8_t SET_USER_FROM_VALUE   =2;
    const uint8_t SET_FIELD             =3;

    inline const uint8_t logic_flags (const CommandValue& v)
    {
        uint8_t flags = v.getFlags();
        if ( (flags & (uint8_t) CommandFlags::VALUE) &&
                (flags & (uint8_t) CommandFlags::IDENTIFIER) )
            return SET_FIELD;
        if ( flags & (uint8_t) CommandFlags::VALUE ) 
            return SET_USER_FROM_VALUE;
        return SET_USER_FROM_SLOT;
    }

    inline bool login_user(std::string& username)
    {
        mApp;
        std::string password = app->slotManager().retrieveSlot("USERAUTH");
        return app->getUserManager().loginUser(username,password)
            == SecurityStatus::OK_LOGIN;
    }

    inline bool set_user_from_slot()
    {
        mApp;
        std::string username = app->slotManager().retrieveSlot("USERNAME");
        return login_user(username);
    }

    inline bool set_user_from_value(std::string& value)
    {
        return login_user(value);
    }
}


void set (Moldable& broadcaster, CommandValue& v)
{
    const SyntaxDef& object = MoguSyntax::get(v.get(CommandFlags::OBJECT));
    switch(object)
    {
        case MoguSyntax::own:{  // set attribute of calling widget
            const SyntaxDef& attr = MoguSyntax::get(v.get(CommandFlags::ARG));
            broadcaster.setAttribute(attr,v.get(CommandFlags::VALUE));
            break;}
        case MoguSyntax::path:{ // set application URL
            mApp;
            app->setPath(stripquotes((std::string) v.get(CommandFlags::VALUE)));
            break;}
        case MoguSyntax::user:
        {
            const uint8_t flags = logic_flags(v);
            if (flags == SET_USER_FROM_SLOT)
            {
                if (!set_user_from_slot())
                    broadcaster.fail().emit();
                else
                    broadcaster.succeed().emit();
                return;
            }
            if (flags == SET_USER_FROM_VALUE)
            {
                std::string val = (std::string) v.get(CommandFlags::VALUE);
                if (!set_user_from_value(val))
                    broadcaster.fail().emit();
                else
                    broadcaster.succeed().emit();
                return;
            }
         }
        // NOT BREAKING ON PURPOSE! If we're setting a user field, 
        // the same logic will apply as with data/meta/group.
        case MoguSyntax::data:
        case MoguSyntax::meta:
        case MoguSyntax::group:
             {
                 mApp;
                 bool is_list_value = v.test(CommandFlags::ARG);
                 NodeValue arg;
                 if (is_list_value) arg = v.get(CommandFlags::ARG);
                 Prefix prefix = syntax_to_prefix.at(
                     MoguSyntax::get(v.get(CommandFlags::OBJECT)));
                 Redis::NodeEditor editor(prefix, 
                         (std::string)v.get(CommandFlags::IDENTIFIER), &arg);
                 if (prefix==Prefix::user)
                 {
                     editor.setId(app->getUser());
                 }
                 else if (prefix==Prefix::group)
                 {
                     editor.setId(app->getGroup());
                 }
                 editor.write(v.get(CommandFlags::VALUE));
             }
             break;
        case MoguSyntax::slot:{ // set application slot
            mApp;
            std::string slot_name = v.getIdentifier();
            NodeValue value = v.get(CommandFlags::VALUE);
            if (slot_name.empty())
            {
                Application::log.log(LogLevel::CRITICAL,
                    "Attempting to set a slot that has no name. ",
                    "Something has gone wrong if this is the case.");
            }
            if (value.getString().empty())
            {
                Application::log.log(LogLevel::WARN,
                    "Setting slot ", slot_name, " to empty value (",
                    value.getString(), ")");
            }
            app->slotManager().setSlot(slot_name, value);
            break;}
        case MoguSyntax::widget:{   // set arbitrary widget attribute
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            if (widget == nullptr) break;
            const SyntaxDef& attr = MoguSyntax::get(v.get(CommandFlags::ARG));
            widget->setAttribute(attr, v.get(CommandFlags::VALUE));
            break;}
        default: break; // If bad input, stop immediately
    }
}

}//namespace Actions
