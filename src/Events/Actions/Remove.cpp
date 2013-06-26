#include "../Actions.h"
#include "Includes.h"
#include <Groups/GroupManager.h>
#include <Redis/NodeEditor.h>
namespace Actions {

namespace {

const uint8_t INVALID_CONSTRUCT         =0;
const uint8_t VALUE_FROM_ATTRIBUTE      =1;
const uint8_t ATTRIBUTE_FROM_OBJECT     =2;
const uint8_t VALUE_FROM_FIELD          =3;
const uint8_t OBJECT_FROM_APPLICATION   =4;

const uint8_t getConstruct(CommandValue& v)
{
    MoguSyntax obj = (MoguSyntax) v.get(CommandFlags::OBJECT);
    bool has_arg = v.test(CommandFlags::ARG);
    bool has_r_object = v.test(CommandFlags::R_OBJECT);
    bool has_value = v.test(CommandFlags::VALUE);

    if (! (has_arg || has_r_object)) 
        return OBJECT_FROM_APPLICATION;

    if (obj == MoguSyntax::widget)
    {
        if (has_arg && has_value) 
            return VALUE_FROM_ATTRIBUTE;
        else if (has_arg) 
            return ATTRIBUTE_FROM_OBJECT;
    }

    return VALUE_FROM_FIELD;
}

inline Moldable* setMoldablePointer(Moldable& broadcaster, CommandValue& v)
{
    mApp;
    return 
        (MoguSyntax::own == (MoguSyntax) v.get(CommandFlags::OBJECT))?
            &broadcaster : 
            app->registeredWidget(
                    (std::string) v.get(CommandFlags::IDENTIFIER));
}

void handle_value_from_attribute(Moldable& broadcaster, CommandValue& v)
{
    NodeValue currentValue;
    Moldable* widget = setMoldablePointer(broadcaster,v);
    MoguSyntax attribute = (MoguSyntax) v.get(CommandFlags::ARG);
    widget->getAttribute(attribute, currentValue);

    if (currentValue.isString())
    {
        std::string newValue = sreplace(
            (std::string) currentValue,
            (std::string) v.get(CommandFlags::VALUE));
        currentValue.setString(newValue);
    }
    else currentValue.setInt(0);

    widget->setAttribute(attribute, currentValue);
}

void handle_attribute_from_object(Moldable& broadcaster, CommandValue& v)
{
    NodeValue emptyValue(EMPTY);
    Moldable* widget = setMoldablePointer(broadcaster,v);
    widget->setAttribute((MoguSyntax) v.get(CommandFlags::ARG), emptyValue);
}

/* This removes values from collection nodes within Redis. It can be used
 * to remove specific indexes from list nodes, or specific keys from 
 * hash nodes:
 *
 * remove user somefield somekey
 * remove user somelist 2
 * remove "somevalue" from data somelist
 */
void handle_value_from_field(Moldable& broadcaster, CommandValue& v)
{
    MoguSyntax object = (MoguSyntax) v.get(CommandFlags::OBJECT);
    NodeValue arg = v.get(CommandFlags::ARG);
    std::string identifier = (std::string) v.get(CommandFlags::IDENTIFIER);
    std::string value = EMPTY;
    

    if (object == MoguSyntax::group)
    {
        mApp;
        int group = app->getGroup();
        GroupManager groupManager(group);
        if (!groupManager.hasWriteAccess(identifier)) return;
    }

    
    Redis::NodeEditor editor(object, identifier, &arg);
    if (editor.getType() == MoguSyntax::list) {
        editor.setIsListValue(true);
        if (v.test(CommandFlags::VALUE))
            value = (std::string) v.get(CommandFlags::VALUE);
    }

    else if (editor.getType() == MoguSyntax::hash)
        editor.setIsHashValue(true);

    editor.remove(value);
}

/* This removes widgets from the widget tree, users and groups, or nodes from
 * the database.
 */
void handle_object_from_application(Moldable& broadcaster, CommandValue& v)
{
    mApp;

    bool has_identifier = v.test(CommandFlags::IDENTIFIER);

    MoguSyntax object = (MoguSyntax) v.get(CommandFlags::OBJECT);
    if (object == MoguSyntax::widget)
    {
        Moldable* dying_widget = app->registeredWidget(
            (std::string) v.get(CommandFlags::IDENTIFIER));
        Moldable* parent_widget = (Moldable*) dying_widget->parent();
        parent_widget->removeChild(dying_widget);
    }
    else if (object == MoguSyntax::user && !has_identifier)
    {
        //TODO : Securely remove all user data.
    }
    else if (object == MoguSyntax::group && !has_identifier)
    {
        //TODO: Securiely remove all group data.
    }
    else // We're dealing with a database node.
    {
        MoguSyntax object = (MoguSyntax) v.get(CommandFlags::OBJECT);
        std::string identifier = (std::string) v.get(CommandFlags::IDENTIFIER);
        // There will not be an arg in this case.
        if (object == MoguSyntax::group)
        {
            mApp;
            int group = app->getGroup();
            GroupManager groupManager(group);
            if (!groupManager.hasWriteAccess(identifier)) return;
        }
        Redis::NodeEditor editor(object,identifier);
        editor.remove();
    }
}

}//anonymous local namespace


void remove (Moldable& broadcaster, CommandValue& v)
{
    const uint8_t construct = getConstruct(v);
    switch(construct)
    {
        case VALUE_FROM_ATTRIBUTE:
            handle_value_from_attribute(broadcaster,v);
            break;
        case ATTRIBUTE_FROM_OBJECT:
            handle_attribute_from_object(broadcaster,v);
            break;
        case VALUE_FROM_FIELD:
            handle_value_from_field(broadcaster,v);
            break;
        case OBJECT_FROM_APPLICATION:
            handle_object_from_application(broadcaster,v);
            break;
        default:
            return;
    };
}

}//namespace Actions
