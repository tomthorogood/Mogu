#include "Includes.h"
#include "../Actions.h"

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

    inline void login_user(const std::string& username)
    {
        mApp;
        std::string password = app->slotManager().retrieveSlot("PASSWORD");
        app->getUserManager().loginUser(username,password);
    }

    inline void set_user_from_slot()
    {
        mApp;
        std::string username = app->slotManager().retrieveSlot("USERNAME");
        login_user(username);
    }

    inline void set_user_from_value(const std::string& value)
    {
        login_user(value);
    }
}


void set (Moldable& broadcaster, CommandValue& v)
{
    switch(v.getObject())
    {
        case MoguSyntax::own:{  // set attribute of calling widget
            MoguSyntax attr = (MoguSyntax) v.get(CommandFlags::ARG);
            broadcaster.setAttribute(attr,v.get(CommandFlags::VALUE));
            break;}
        case MoguSyntax::path:{ // set application URL
            mApp;
            app->setPath(stripquotes((std::string) v.get(CommandFlags::VALUE)));
            break;}
        case MoguSyntax::user:
                const uint8_t flags = logic_flags(v);
                if (flags == SET_USER_FROM_SLOT) 
                {
                    set_user_from_slot();
                    return;
                }
                if (flags == SET_USER_FROM_VALUE)
                {
                    set_user_from_value( (std::string) v.get(CommandFlags::VALUE) );
                    return;
                }
                // NOT BREAKING ON PURPOSE! If we're setting a user field, 
                // the same logic will apply as with data/meta/group.
        case MoguSyntax::data:
        case MoguSyntax::meta:
        case MoguSyntax::group:{
             bool is_list_value = v.test(CommandFlags::ARG);
             NodeValue* arg = is_list_value ? &(v.get(CommandFlags::arg)) : nullptr;
             Redis::NodeEditor editor(
                 v.get(CommandFlags::OBJECT),(std::string)v.get(CommandFlags::IDENTIFIER), arg);
             editor.setIsListValue(is_list_value);
             editor.write(v.get(Commandflags::VALUE));
              }break;
        case MoguSyntax::slot:{ // set application slot
            mApp;
            app->slotManager().setSlot(v.getIdentifier(), v.get(CommandFlags::VALUE));
            break;}
        case MoguSyntax::widget:{   // set arbitrary widget attribute
            mApp;
            Moldable* widget = app->registeredWidget(v.getIdentifier());
            if (widget == nullptr) break;
            widget->setAttribute((MoguSyntax) v.get(CommandFlags::ARG), v.get(CommandFlags::VALUE));
            break;}
        default: return; // If bad input, stop immediately
    }
}

}//namespace Actions
