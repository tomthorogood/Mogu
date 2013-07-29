#include "../Actions.h"
#include "Includes.h"

#include <Groups/GroupManager.h>
#include <Redis/NodeEditor.h>

namespace Actions {

namespace {
const uint8_t INVALID_CONSTRUCT     =0;

// "append [object [identifier]] to [object [identifier]]"
const uint8_t OBJECT_TO_OBJECT      =1;

// "append [value] to [object identifier attribute]"
const uint8_t VALUE_TO_ATTRIBUTE    =2;

// "append [value] to [data|user|group identifier [argument]]"
const uint8_t VALUE_TO_FIELD        =3;

// "append user"
const uint8_t USER_TO_APPLICATION   =4;

// Based on the flags set in the CommandValue, determine what kind
// of 'append' construct we have (of the 
const uint8_t getConstruct(CommandValue& v)
{

    // OBJECT_TO_OBJECT will always have an R_IDENTIFIER, 
    // but the main object may be "self" OR another widget.
    if (v.test(CommandFlags::R_IDENTIFIER) &&
            (v.test(CommandFlags::IDENTIFIER) 
            || MoguSyntax::own == v.get(CommandFlags::OBJECT))
       )
        return OBJECT_TO_OBJECT;

    if (v.test(CommandFlags::VALUE))
    {
        if (MoguSyntax::widget == v.get(CommandFlags::OBJECT))
            return VALUE_TO_ATTRIBUTE;
        else return VALUE_TO_FIELD;
    }

    if (MoguSyntax::user == v.get(CommandFlags::R_OBJECT))
        return USER_TO_APPLICATION;

    return INVALID_CONSTRUCT;
}

/* The only valid use case for this function is when a single widget
 * shall be appended to another single widget. This will return without
 * effect if there's a problem resolving any of the requisite objects.
 */
void handleObjectToObject(Moldable& broadcaster, CommandValue& v)
{   
    mApp;

    std::string new_obj_id = (std::string) v.get(CommandFlags::R_IDENTIFIER);
    Moldable* new_widget = app->getFactory().createMoldableWidget(new_obj_id); 

    Moldable* curr_widget = app->registeredWidget( 
            (std::string) v.get(CommandFlags::IDENTIFIER));
    if (curr_widget != nullptr) curr_widget->addWidget(new_widget);
}

/* Appends a resolved value to an existing attribute. Sums two numbers. */
void handleValueToAttribute(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    NodeValue tmp;
    Moldable* curr_widget = app->registeredWidget(
            (std::string) v.get(CommandFlags::IDENTIFIER));
    
    if (curr_widget == nullptr) return;

    const SyntaxDef& attribute = 
        MoguSyntax::get(v.get(CommandFlags::ARG));

    curr_widget->getAttribute(attribute, tmp);
    tmp += v.get(CommandFlags::VALUE);
    curr_widget->setAttribute(attribute, tmp);
}

/* Adds data to the database by first retrieving the value, 
 * ensuring the the current user has write access to the value, 
 * and then writing the appended value. 
 */
void handleValueToField(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    NodeValue arg;
    if (v.test(CommandFlags::ARG))
        arg = v.get(CommandFlags::ARG);

    bool writeable = true;
    Prefix obj = 
        syntax_to_prefix.at((MoguSyntax::get(v.get(CommandFlags::OBJECT))));
    std::string node = (std::string) v.get(CommandFlags::IDENTIFIER);

    if (Prefix::group == obj)
    {
        GroupManager gm(app->getGroup());
        writeable = gm.hasWriteAccess(node);
    }

    if (!writeable) return;

    NodeValue current_value;
    Redis::NodeEditor editor(obj, node, &arg);
    if (editor.getType() == MoguSyntax::string)
    {
        std::string cv = editor.read();
        cv += (std::string) v.get(CommandFlags::VALUE);
        current_value.setString(cv);
    }
    else
    {
        current_value.setString(v.get(CommandFlags::VALUE).getString());
    }
    editor.write(current_value);
}

void handleUserToApplication(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    app->getUserManager().registerUser(
        app->slotManager().retrieveSlot("USERNAME"),
        app->slotManager().retrieveSlot("PASSWORD"));
}

}//anonymous local namespace

void append(Moldable& broadcaster, CommandValue& v)
{
   const uint8_t construct = getConstruct(v); 
   switch(construct)
   {
       case OBJECT_TO_OBJECT: 
            handleObjectToObject(broadcaster,v);
            break;
       case VALUE_TO_ATTRIBUTE:
            handleValueToAttribute(broadcaster,v);
            break;
       case VALUE_TO_FIELD:
            handleValueToField(broadcaster,v);
            break;
       case USER_TO_APPLICATION:
            handleUserToApplication(broadcaster,v);
       default: 
            return;
   }
}

}//namespace Actions
