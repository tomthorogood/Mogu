#include "../Actions.h"
#include "Includes.h"
#include <Redis/Node_Editor.h>
namespace Actions {

void email(Moldable& broadcaster, Command_Value& v)
{
    mApp;
    const Syntax_Def& o {Mogu_Syntax::get(v.get(Command_Flags::object))}
    Prefix p {syntax_to_prefix.at(object)};
    std::string id {(std::string) v.get(Command_Flags::identifier)};
    Node_Value arg {};
    Node_Value message {};
    Email_Manager email {};

    if (v.test(Command_Flags::arg))
        arg = v.get(Command_Flags::arg);
    Redis::Node_Editor node {p, id, &arg};

    email.set_recipient(v.get(Command_Flags::value).get_string());
    email.set_subject(app->slot_manager().retrieve_slot("EMAIL_SUBJECT"));

    const Syntax_Def& attr {Mogu_Syntax::get(arg)};

    switch(o)
    {
        case Mogu_Syntax::own:{
            broadcaster.get_attribute(attr, message);
            email.set_message(message.get_string());
            break;}

        case Mogu_Syntax::widget:{
            Moldable* w {app->get_widget(v.get_identifier())};
            w->get_attribute(attr,message);
            email.set_message(message.get_string());
            break;}

        case Mogu_Syntax::group:
            node.set_id(app->get_group());
            message.set_string(node.read());
            break;

        case Mogu_Syntax::user:
            node.set_id(app->get_user());
            // No break on purpose
        case Mogu_Syntax::data: 
            message.set_string(node.read());
            break;
        case Mogu_Syntax::slot:{
            Slot_Manager& m {app->slot_manager()};
            message.set_string(m.retrieve_slot(id));
            break;}
        default: return;
    }
    email.set_message(message.get_string());
    email.send();
}
}//namespace Actions
