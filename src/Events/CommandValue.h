/*
 * CommandValue.h
 *
 *  Created on: Apr 19, 2013
 *      Author: tom
 */

#ifndef COMMANDVALUE_H_
#define COMMANDVALUE_H_

#include <declarations.h>
#include <Types/NodeValue.h>

/*!\brief Holds parsed values from the CommandParser which can then be used
 * to handle events.
 */

class CommandValue
{
public:
    CommandValue(Moldable& widget);

    const static uint8_t ACTION      = 1;  //!<\brief Flag for when action is set
    const static uint8_t OBJECT      = 2;  //!<\brief Flag for when object is set
    const static uint8_t IDENTIFIER  = 4;  //!<\brief Flag for when identifier is set
    const static uint8_t ARG         = 8;  //!<\brief Flag for when arg is set
    const static uint8_t VALUE       = 16; //!<\brief Flag for when value is set

    inline void setAction(MoguSyntax action_)
    {
        action = action_;
        flags |= ACTION;
    }

    inline void setObject(MoguSyntax object_)
    {
        object = object_;
        flags |= OBJECT;
    }

    inline void setIdentifier(const NodeValue& identifier_)
    {
        identifier = identifier_;
        flags |= IDENTIFIER;
    }

    inline void setArg(const NodeValue& arg_)
    {
        arg = arg_;
        flags |= ARG;
    }

    inline void setValue(const NodeValue& value_)
    {
        value = value_;
        flags |= VALUE;
    }

    inline const MoguSyntax& getAction() const    { return action;}
    inline const MoguSyntax& getObject() const    { return object;}
    inline const std::string& getIdentifier() const 
        { return identifier.getString();}
    inline NodeValue& getArg()         { return arg;}
    inline NodeValue& getValue()       { return value;}
    inline const uint8_t& getFlags() const        { return flags;}
    inline Moldable& getWidget()  { return broadcaster;}

    /* To meet minimum requirements, both the action and object must
     * be set.
     */
    inline bool valid()
    {
        return (flags & ACTION) && (flags & OBJECT);
    }

private:
    /*!\brief The enumerated action that starts the command. This is guaranteed
     * to be a valid action by the MoguScript import stack. All commands
     * require an action.
     */
    MoguSyntax action = MoguSyntax::__NONE__;

    /*!\brief The enumerated object type which will be changed by the command.
     * This is required for any mogu command (commands must DO something TO
     * something).
     */
    MoguSyntax object = MoguSyntax::__NONE__;

    /*!\brief Most objects will require some sort of identifier, but not all.
     *  The 'self' (or 'own') object is its own identifier, and refers to the
     *  widget that was passed into this class. 'app' refers to the specific
     *  application instance (although this may be deprecated). This information
     *  is not needed when passing information into the CommandValue, but is
     *  useful when extracting data from it.
     */
    NodeValue identifier;

    /*!\brief Some objects will contain some sort of argument. For instance,
     * hash or list nodes will have an argument representing the location
     * within that databse node that is to be interacted with. Widgets may
     * have a state argument which will be altered by this command.
     */
    NodeValue arg;

    /*!\brief Many commands will have a value. The value will be applied
     * to the object [identifier [arg]] set.
     */
    NodeValue value;

    /*!\brief Holds a reference to the widget that started the broadcast.
     * This is necessary for when a widget refers to itself.
     */
    Moldable& broadcaster;

    /*!\brief Used for boolean operations pertaining to which fields are set,
     * when handling the commands.
     */
    uint8_t flags       =  0;
};

#endif /* COMMANDVALUE_H_ */
