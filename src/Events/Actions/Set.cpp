#include "Includes.h"
#include "../Actions.h"
#include "../../utilities.h"
#include "../../Types/GroupManager.h"
#include <Redis/NodeEditor.h>

namespace Application {
    extern Mogu_Logger log;
}

namespace Actions {

namespace {

constexpr uint8_t set_user_from_slot    =1;
constexpr uint8_t set_user_from_value   =2;
constexpr uint8_t set_field             =3;
constexpr uint8_t set_group_from_slot   =4;
constexpr uint8_t set_group_from_value  =5;

inline const uint8_t logic_flags (Command_Value& v)
{
    Node_Value& o = v.get(Command_Flags::object);
    if (Mogu_Syntax::user == o)
    {
        if (v.test(Command_Flags::identifier))
            return set_field;
        else if (v.test(Command_Flags::value))
            return set_user_from_value;
        else return set_user_from_slot;
    }
    else if (Mogu_Syntax::group == o)
    {
        if (v.test(Command_Flags::identifier))
            return set_field;
        else if (v.test(Command_Flags::value))
            return set_group_from_value;
        else return set_group_from_slot;
    }
    return set_field;
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

inline bool do_set_group_from_value(std::string& value)
{
    mApp;
    Group_Manager* g {};
    g = new Group_Manager {value};
    if (!g->is_valid())
    {
        delete g;
        g = new Group_Manager{value, app->get_user()};
    }
    if (g->is_valid())
    {
        app->set_group(g->get_id());
        Node_Value k(g->get_key());
        app->get_slot_manager().set_slot("GROUPKEY", k);
        delete g;
        return true;
    }
    else
    {
        delete g;
        return false;
    }
}

inline bool do_set_group_from_slot()
{
    mApp;
    std::string key {app->get_slot_manager().get_slot("GROUPKEY").get_string()};
    Group_Manager g {key};
    if (g.is_valid())
    {
        app->set_group(g.get_id());
        return true;
    }
    return false;
}

inline bool group_is_set(const uint8_t& f, Command_Value& v)
{
    if (f==set_group_from_value)
    {
        std::string val {v.get(Command_Flags::value).get_string()};
        do_set_group_from_value(val);
        return true;
    }
    else if (f==set_group_from_slot)
    {
        do_set_group_from_slot();
        return true;
    }
    return false;
}

inline bool user_is_set(const uint8_t& f, Command_Value& v, Moldable& broadcaster)
{

    if (f == set_user_from_slot)
    {
        if (!do_set_user_from_slot())
            broadcaster.fail().emit();
        else
            broadcaster.succeed().emit();
        return true;
    }
    else if (f == set_user_from_value)
    {
        std::string val {v.get(Command_Flags::value).get_string()};
        if (!do_set_user_from_value(val))
            broadcaster.fail().emit();
        else
            broadcaster.succeed().emit();
        return true;
    }
    return false;
}
}


void set (Moldable& broadcaster, Command_Value& v)
{
    mApp;
    const Syntax_Def& o {Mogu_Syntax::get(v.get(Command_Flags::object))};
    int i {-1};
    const uint8_t f {logic_flags(v)};
    
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
            if (user_is_set(f,v, broadcaster)) break;
            else // Cascade to node editor case
            {
                i = app->get_user();
            }
         } //NO BREAK
        case Mogu_Syntax::group:
        {
            if (o==Mogu_Syntax::group)
            {
                if (group_is_set(f,v))
                {
                    if (app->get_group()<=-1 && !broadcaster.isHidden())
                        broadcaster.errorReported().emit();
                    break;
                }
                else
                    i = app->get_group();
            }
        } // NO BREAK
        case Mogu_Syntax::data:
        case Mogu_Syntax::meta:
         {
             Node_Value arg {};
             bool has_arg {v.test(Command_Flags::arg)};
             if (has_arg) arg = v.get(Command_Flags::arg);
             Prefix p {syntax_to_prefix(o)};
             std::string id {v.get_identifier()};
             Redis::Node_Editor e {p, id, has_arg ? &arg : nullptr};
             if (e.id_required()) e.set_id(i);
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
        case Mogu_Syntax::widget:{ 
            Moldable* w {app->get_widget(v.get_identifier())};
            if (!w) break;
            const Syntax_Def& a {Mogu_Syntax::get(v.get(Command_Flags::arg))};
            w->set_attribute(a, v.get(Command_Flags::value));
            break;}
        default: break; 
    }
}

}//namespace Actions
