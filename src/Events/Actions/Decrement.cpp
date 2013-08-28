#include "../Actions.h"
#include "Includes.h"
#include <Redis/NodeEditor.h>
namespace Actions {

void decr_db_val(Redis::Node_Editor& e, int decr_amt)
{
    std::string s {e.read()};
    int i {atoi(s.c_str())};
    s = std::to_string(i-decr_amt);
    e.write(s);
}

void decrement (Moldable& broadcaster, Command_Value& v)
{
    mApp;
    const Syntax_Def& o = Mogu_Syntax::get(v.get(Command_Flags::object));
    Prefix p {syntax_to_prefix(o)};
    std::string id = (std::string) v.get(Command_Flags::identifier);
    Node_Value arg {};
    Node_Value final {};
    std::string str {};
    int value {};

    if (v.test(Command_Flags::arg))
        arg = v.get(Command_Flags::arg);
    
    Redis::Node_Editor node {p, id, &arg};

    if (v.get_flags() & (uint8_t) Command_Flags::value)
    {
        value = (int) v.get(Command_Flags::value);
    }
    else
    {
        value = 1;
    }

    const Syntax_Def& s = Mogu_Syntax::get(arg);
    
    switch(o)
    {
        case Mogu_Syntax::own:{
            broadcaster.get_attribute(s,final);
            if (final.is_string())
                final.set_int(atoi(final.get_string().c_str()));
            final.set_int(final.get_int()-value);
            broadcaster.set_attribute(s, final);
            break;}
        
        case Mogu_Syntax::group:{
            int g {app->get_group()};
//            Group_Manager m {g};
//            if (!m.has_read_access(identifier)) return;}
            node.set_id(g);
            decr_db_val(node, value);
            break;}
        case Mogu_Syntax::user:
            node.set_id(app->get_user());
            decr_db_val(node,value);
            break;
        case Mogu_Syntax::data:
            decr_db_val(node,value);
            break;

        case Mogu_Syntax::slot:{
            Slot_Manager& m = app->get_slot_manager();
            str = m.get_slot(id).get_string();
            int i {atoi(str.c_str())};
            i -= value;
            str = std::to_string(i);
            final.set_string(str);
            m.set_slot(id,final);
            break;}

        case Mogu_Syntax::widget:{
            Moldable* w {app->get_widget(id)};
            w->get_attribute(s, final);
            if (final.is_string())
                final.set_int(atoi(final.get_string().c_str()));
            final.set_int(final.get_int()-value);
            w->set_attribute(s, final);
            break;}

        default: return;
    }
} 


}//namespace Actions
