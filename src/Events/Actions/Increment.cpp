#include "../Actions.h"
#include "Includes.h"
#include <Redis/NodeEditor.h>
namespace Actions {
namespace {
void incr_db_val(Redis::Node_Editor& e, int incr_amt)
{
    std::string s {e.read()};
    int i {atoi(s.c_str())};
    s = std::to_string(i+incr_amt);
    e.write(s);
} 
}//anonymous namespace

void increment (Moldable& broadcaster, Command_Value& v)
{
    mApp;
    const Syntax_Def& o {Mogu_Syntax::get(v.get(Command_Flags::object))};
    Prefix p {syntax_to_prefix(o)};
    std::string id {v.get(Command_Flags::identifier).get_string()};
    Node_Value arg {};
    Node_Value final {};
    Node_Value current {};
    
    if (v.test(Command_Flags::arg))
        arg = v.get(Command_Flags::arg);

    Redis::Node_Editor node(p, id, &arg);

    int value {};
    if (v.test(Command_Flags::value))
    {
        value = v.get(Command_Flags::value).get_int();
    }
    else
    {
        value = 1;
    }

    const Syntax_Def& s {Mogu_Syntax::get(arg)};
    int i {};
    std::string str {};

    switch(o)
    {
        case Mogu_Syntax::own:{
            const Syntax_Def& attr {Mogu_Syntax::get(arg)};
            broadcaster.get_attribute(s, current);
            if (current.is_string())
                current.set_int(atoi(current.get_string().c_str()));
            current.set_int(current.get_int()+value);
            broadcaster.set_attribute(attr,current);
            break;}

        case Mogu_Syntax::group:{
            i = app->get_group();
//          Group_Manager m {i};
//          if (!m.has_write_access(id)) return;}
            node.set_id(i);
            incr_db_val(node,value);
            break;}
        case Mogu_Syntax::user:
            i = app->get_user();
            node.set_id(i);
            incr_db_val(node,value);
            break;
        case Mogu_Syntax::data:
            incr_db_val(node,value);
            break;

        case Mogu_Syntax::slot:{
            Slot_Manager& m = app->get_slot_manager();
            final = m.get_slot(id);
            if (final.is_string())
                final.set_int(atoi(final.get_string().c_str()));
            i = final.get_int() + value;
            final.set_int(
                app->get_slot_manager().get_slot(v.get_identifier()).get_int());
            final.set_int(final.get_int()+value);
            m.set_slot(id,final);
            app->get_slot_manager().set_slot(
                    v.get_identifier(), final);
            break;}

        case Mogu_Syntax::widget:{
            Moldable* w {app->get_widget(id)};
            w->get_attribute(s,current);
            if (current.is_string())
                current.set_int(atoi(current.get_string().c_str()));
            current.set_int(current.get_int()+value);
            w->set_attribute(s,current);
            break;}
        default: return;
    }
} 

}//namespace Actions
