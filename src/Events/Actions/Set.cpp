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

        /* Either sets a user field, or logs in a user:
         *
         * "set user" will attempt to retrieve a USERNAME and PASSWORD from
         * the respective application slots.
         *
         * "set user to [value]" will retrieve the PASSWORD from the application
         * slot, but use the username resolved in [value].
         *
         * "set user [field [arg] ] to [value]" will set a field in the user's
         * database.
         */
        case MoguSyntax::user:{
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
            Redis::ContextQuery db(Prefix::user);
            policyQuery(
                    MoguSyntax::set
                    , db
                    , Prefix::user
                    , v);
            db.execute();
            break;}
        case MoguSyntax::group:{// set group field
            mApp;
            if (v.getIdentifier() == EMPTY)
            {
                app->getUserManager().setActiveGroup(
                        (std::string) v.get(CommandFlags::VALUE));
                return;
            }
            Redis::ContextQuery db(Prefix::group);
            policyQuery(
                MoguSyntax::set
                , db
                , Prefix::group
                , v);
            db.execute();
            break;}
        case MoguSyntax::data:{ // set data field
            Redis::ContextQuery db(Prefix::data);
            db.appendQuery("type data.%s", v.getIdentifier().c_str());
            MoguSyntax node_type = db.yieldResponse <MoguSyntax> ();
            Redis::addQuery(
                db
                , MoguSyntax::set 
                , Prefix::data
                , node_type
                , v);
            db.execute();                    
            break;}
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
