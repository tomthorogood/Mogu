#include "../Actions.h"
#include "Includes.h"
#include <Redis/NodeEditor.h>
#include "../../Config/inline_utils.h"
namespace Actions {

namespace {

const uint8_t invalid_construct         =0;
const uint8_t value_from_attribute      =1;
const uint8_t attribute_from_object     =2;
const uint8_t value_from_field          =3;
const uint8_t object_from_application   =4;

const uint8_t get_construct(Command_Value& v)
{
    const Syntax_Def& o = Mogu_Syntax::get(v.get(Command_Flags::object));
    bool has_arg = v.test(Command_Flags::arg);
    bool has_r_object = v.test(Command_Flags::r_object);
    bool has_value = v.test(Command_Flags::value);

    if (! (has_arg || has_r_object)) 
        return object_from_application;

    if (o == Mogu_Syntax::widget)
    {
        if (has_arg && has_value) 
            return value_from_attribute;
        else if (has_arg) 
            return attribute_from_object;
    }

    return value_from_field;
}

inline Moldable* set_moldable_pointer(Moldable& broadcaster, Command_Value& v)
{
    mApp;
    const Syntax_Def& o {Mogu_Syntax::get(v.get(Command_Flags::object))};
    return 
        (Mogu_Syntax::own == o) ?  
            &broadcaster : app->get_widget(v.get_identifier());
}

void handle_value_from_attribute(Moldable& broadcaster, Command_Value& v)
{
    Node_Value n {};
    Moldable* w {set_moldable_pointer(broadcaster,v)};
    const Syntax_Def& attribute = Mogu_Syntax::get(v.get(Command_Flags::arg));

    w->get_attribute(attribute, n);

    if (n.is_string())
    {
        std::string new_value {n.get_string()};
        std::string rm {v.get(Command_Flags::value).get_string()};
        sreplace(new_value,rm);
        n.set_string(new_value);
    }
    else n.set_int(0);

    w->set_attribute(attribute,n);
}

void handle_attribute_from_object(Moldable& broadcaster, Command_Value& v)
{
    Node_Value n {};
    const Syntax_Def& s {Mogu_Syntax::get(v.get(Command_Flags::arg))};
    Moldable* w {set_moldable_pointer(broadcaster,v)};
    w->set_attribute(s,n);
}

/* This removes values from collection nodes within Redis. It can be used
 * to remove specific indexes from list nodes, or specific keys from 
 * hash nodes:
 *
 * remove user somefield somekey
 * remove user somelist 2
 * remove "somevalue" from data somelist
 */
void handle_value_from_field(Moldable& broadcaster, Command_Value& v)
{
    mApp;
    const Syntax_Def& o {Mogu_Syntax::get(v.get(Command_Flags::object))};
    Prefix p {syntax_to_prefix(o)};
    std::string id {v.get_identifier()};
    
    Node_Value arg {v.get(Command_Flags::arg)};
    Redis::Node_Editor e {p, id, &arg};
    

    if (o == Mogu_Syntax::user)
        e.set_id(app->get_user());
    else if (o == Mogu_Syntax::group)
    {
        int g {app->get_group()};
//        Group_Manager m(group);
//        if (!group_manager.has_write_access(id)) return;
        e.set_id(g);
    }

    std::string val {v.get(Command_Flags::value).get_string()};
    e.remove(val);
}

/* This removes widgets from the widget tree, users and groups, or nodes from
 * the database.
 */
void handle_object_from_application(Moldable& broadcaster, Command_Value& v)
{
    mApp;

    bool has_identifier = v.test(Command_Flags::identifier);
    
    const Syntax_Def& o {Mogu_Syntax::get(v.get(Command_Flags::object))};
    Prefix p {syntax_to_prefix(o)};
   
    if (o == Mogu_Syntax::widget)
    {
        Moldable* x {app->get_widget(v.get_identifier())};
        if (x)
        {
            Moldable* r {(Moldable*) x->parent()};
            r->removeChild(x);
        }
    } 
    else if (o == Mogu_Syntax::user && !has_identifier)
    {
        //TODO : Securely remove all user data.
    }
    else if (o == Mogu_Syntax::group && !has_identifier)
    {
        //TODO: Securiely remove all group data.
    }
    else // We're dealing with a database node.
    {
        std::string id {v.get_identifier()};
        Redis::Node_Editor e {p,id};
        if (o == Mogu_Syntax::group)
        {
            int g {app->get_group()};
//          Group_Manager m {g};
//          if (!m.has_write_access(id)) return;
            e.set_id(g);
        }
        else if (o==Mogu_Syntax::user)
        {
            e.set_id(app->get_user());
        }

        e.remove();
    }
}

}//anonymous local namespace


void remove (Moldable& broadcaster, Command_Value& v)
{
    const uint8_t construct = get_construct(v);
    switch(construct)
    {
        case value_from_attribute:
            handle_value_from_attribute(broadcaster,v);
            break;
        case attribute_from_object:
            handle_attribute_from_object(broadcaster,v);
            break;
        case value_from_field:
            handle_value_from_field(broadcaster,v);
            break;
        case object_from_application:
            handle_object_from_application(broadcaster,v);
            break;
        default:
            return;
    };
}

}//namespace Actions
