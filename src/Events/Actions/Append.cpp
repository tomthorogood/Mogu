#include "../Actions.h"
#include "Includes.h"

#include <Groups/Group_Manager.h>
#include <Redis/Node_Editor.h>

namespace Actions {

namespace {
const uint8_t invalid_construct     =0;

// "append [object [identifier]] to [object [identifier]]"
const uint8_t object_to_object      =1;

// "append [value] to [object identifier attribute]"
const uint8_t value_to_attribute    =2;

// "append [value] to [data|user|group identifier [argument]]"
const uint8_t value_to_field        =3;

// "append user"
const uint8_t user_to_application   =4;

// Based on the flags set in the Command_Value, determine what kind
// of 'append' construct we have (of the 
const uint8_t get_construct(Command_Value& v)
{

    // object_to_object will always have an R_IDENTIFIER, 
    // but the main object may be "self" OR another widget.
    if (v.test(Command_Flags::r_identifier) &&
            (v.test(Command_Flags::identifier) 
            || Mogu_Syntax::own == v.get(Command_Flags::object))
       )
        return object_to_object;

    if (v.test(Command_Flags::VALUE))
    {
        if (Mogu_Syntax::widget == v.get(Command_Flags::OBJECT))
            return value_to_attribute;
        else return value_to_field;
    }

    if (Mogu_Syntax::user == v.get(Command_Flags::R_OBJECT))
        return user_to_application;

    return INVALID_CONSTRUCT;
}

/* The only valid use case for this function is when a single widget
 * shall be appended to another single widget. This will return without
 * effect if there's a problem resolving any of the requisite objects.
 */
void handle_object_to_object(Moldable& broadcaster, Command_Value& v)
{   
    mApp;

    std::string new_obj_id = (std::string) v.get(Command_Flags::R_IDENTIFIER);
    Moldable* new_widget = app->get_factory().create_moldable_widget(new_obj_id); 

    Moldable* curr_widget = app->get_widget( 
            (std::string) v.get(Command_Flags::IDENTIFIER));
    if (curr_widget != nullptr) curr_widget->addWidget(new_widget);
}

/* Appends a resolved value to an existing attribute. Sums two numbers. */
void handle_value_to_attribute(Moldable& broadcaster, Command_Value& v)
{
    mApp;
    Node_Value tmp;
    Moldable* curr_widget = app->get_widget(
            (std::string) v.get(Command_Flags::IDENTIFIER));
    
    if (curr_widget == nullptr) return;

    const Syntax_Def& attribute = 
        Mogu_Syntax::get(v.get(Command_Flags::ARG));

    curr_widget->get_attribute(attribute, tmp);
    tmp += v.get(Command_Flags::VALUE);
    curr_widget->set_attribute(attribute, tmp);
}

/* Adds data to the database by first retrieving the value, 
 * ensuring the the current user has write access to the value, 
 * and then writing the appended value. 
 */
void handle_value_to_field(Moldable& broadcaster, Command_Value& v)
{
    mApp;
    Node_Value arg;
    if (v.test(Command_Flags::ARG))
        arg = v.get(Command_Flags::ARG);

    bool writeable = true;
    Prefix obj = 
        syntax_to_prefix.at((Mogu_Syntax::get(v.get(Command_Flags::OBJECT))));
    std::string node = (std::string) v.get(Command_Flags::IDENTIFIER);

    if (Prefix::group == obj)
    {
        Group_Manager gm(app->get_group());
        writeable = gm.has_write_access(node);
    }

    if (!writeable) return;

    Node_Value current_value;
    Redis::Node_Editor editor(obj, node, &arg);
    if (editor.get_type() == Mogu_Syntax::string)
    {
        std::string cv = editor.read();
        cv += (std::string) v.get(Command_Flags::VALUE);
        current_value.set_string(cv);
    }
    else
    {
        current_value.set_string(v.get(Command_Flags::VALUE).get_string());
    }
    editor.write(current_value);
}

void handle_user_to_application(Moldable& broadcaster, Command_Value& v)
{
    mApp;
    if (&broadcaster==nullptr)
    {
        Application::log.log(Log_Level::WARN,__FILE__, " ", __LINE__,
            ": Broadcaster is null.");
    }

    std::string p_username = app->slot_manager().retrieve_slot("USERNAME");
    std::string p_userauth = app->slot_manager().retrieve_slot("USERAUTH");
    if (p_username.empty())
        Application::log.log(Log_Level::ERROR,__FILE__," ", __LINE__,
            "Username is Empty \"", p_username, "\"");
    if (p_userauth.empty())
        Application::log.log(Log_Level::ERROR, __FILE__," ", __LINE__,
            "Userauth is Empty \"", p_userauth, "\"");
    Security_Status status = app->get_user_manager().register_user(
        p_username,p_userauth);

    if (status != Security_Status::OK_REGISTER
        broadcaster.error_reported().emit();
}

}//anonymous local namespace

void append(Moldable& broadcaster, Command_Value& v)
{
   const uint8_t construct = get_construct(v); 
   switch(construct)
   {
       case object_to_object: 
            handle_object_to_object(broadcaster,v);
            break;
       case value_to_attribute:
            handle_value_to_attribute(broadcaster,v);
            break;
       case value_to_field:
            handle_value_to_field(broadcaster,v);
            break;
       case user_to_application:
            handle_user_to_application(broadcaster,v);
            break;
       default: 
            return;
   }
}

}//namespace Actions
