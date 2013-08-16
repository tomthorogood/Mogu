#include "Includes.h"
#include "../Actions.h"
#include "../../Config/inline_utils.h"
#include <Redis/NodeEditor.h>

namespace Application {
    extern Mogu_Logger log;
}

namespace Actions {

namespace {
    const uint8_t set_user_from_slot    =1;
    const uint8_t set_user_from_value   =2;
    const uint8_t set_field             =3;

    inline const uint8_t logic_flags (const Command_Value& v)
    {
        if (v.test(Command_Flags::value))
            return v.test(Command_Flags::identifier) ? set_field : set_user_from_value;
        return set_user_from_slot;
    }

    inline bool login_user(std::string& username)
    {
        mApp;
        std::string pw {app->get_slot_manager().get_slot("USERAUTH").get_string()};
        Security_Status s {app->get_user_manager().login_user(username,pw)};
        return s == Security_Status::ok_login;
    }

    inline bool do_set_user_from_slot()
    {
        mApp;
        std::string u {app->get_slot_manager().get_slot("USERNAME").get_string()};
        return login_user(u);
    }

    inline bool do_set_user_from_value(std::string& value)
    {
        return login_user(value);
    }
}


void set (Moldable& broadcaster, Command_Value& v)
{
    mApp;
    const Syntax_Def& o {Mogu_Syntax::get(v.get(Command_Flags::object))};
    int i {-1};
    
    switch(o)
    {
        case Mogu_Syntax::own:{
            const Syntax_Def& a {Mogu_Syntax::get(v.get(Command_Flags::arg))};
            broadcaster.set_attribute(a, v.get(Command_Flags::value));
            break;}

        case Mogu_Syntax::path:{ // set application URL
            app->set_path(stripquotes(v.get(Command_Flags::value)));
            break;}
                               
        case Mogu_Syntax::user:
        {
            const uint8_t f = logic_flags(v);
            if (f == set_user_from_slot)
            {
                if (!do_set_user_from_slot())
                    broadcaster.fail().emit();
                else
                    broadcaster.succeed().emit();
                return;
            }
            else if (f == set_user_from_value)
            {
                std::string val {v.get(Command_Flags::value).get_string()};
                if (!do_set_user_from_value(val))
                    broadcaster.fail().emit();
                else
                    broadcaster.succeed().emit();
                return;
            }
            else // Cascade to node editor case
            {
                i = app->get_user();
            }
         }
        case Mogu_Syntax::group: // Don't do if i already set. 
            i = i>=0 ? i : app->get_group();
        case Mogu_Syntax::data:
        case Mogu_Syntax::meta:
         {
             Node_Value arg {};
             bool has_arg {v.test(Command_Flags::arg)};
             if (has_arg) arg = v.get(Command_Flags::arg);
             Prefix p {syntax_to_prefix.at(o)};
             std::string id {v.get_identifier()};
             Redis::Node_Editor e {p, id, has_arg ? &arg : nullptr};
             if (i>=0) e.set_id(i);
             e.write(v.get(Command_Flags::value));
             break;
         }
        case Mogu_Syntax::slot:{ // set application slot
            std::string s {v.get_identifier()};
            Node_Value n {v.get(Command_Flags::value)};
            if (s.empty())
            {
                Application::log.log(Log_Level::critical,
                    "Attempting to set a slot that no name. ",
                    "Something has gone terribly wrong!");
            }
            app->get_slot_manager().set_slot(s,n);
            break;}
        case Mogu_Syntax::widget:{   // set arbitrary widget attribute
            Moldable* w {app->get_widget(v.get_identifier())};
            if (!w) break;
            const Syntax_Def& a {Mogu_Syntax::get(v.get(Command_Flags::arg))};
            w->set_attribute(a, v.get(Command_Flags::value));
            break;}
        default: break; // If bad input, stop immediately
    }
}

}//namespace Actions
