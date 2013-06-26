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

enum class CommandFlags {
    ACTION          =1
    , OBJECT        =2
    , IDENTIFIER    =4
    , ARG           =8
    , VALUE         =16
    , R_OBJECT      =32
    , R_IDENTIFIER  =64
    , R_ARG         =128
};

class CommandValue
{
public:
    CommandValue(Moldable& widget);

    void set(const CommandFlags& flag, const NodeValue& v);
    void set(const CommandFlags& flag, MoguSyntax& v);
    NodeValue get ( CommandFlags& flag) const ;
    inline NodeValue get (CommandFlags&& flag) {
        return get(flag);
    }

    inline void setAction(MoguSyntax action_)
    {
        set(CommandFlags::ACTION, action_);
        if (action == MoguSyntax::append) reversed = true;
    }


    inline const MoguSyntax& getAction() const    { return action;}
    inline const MoguSyntax& getObject() const    { return object;}
    inline const std::string& getIdentifier() const 
        { return identifier.getString();}
    inline const uint8_t& getFlags() const        { return flags;}
    inline Moldable& getWidget()  { return broadcaster;}

    inline std::string stitchState()
    {
        std::string str;
        if (!flags & (uint8_t) CommandFlags::OBJECT) return EMPTY;
        str += std::to_string((int) object);

        if (flags & (uint8_t) CommandFlags::IDENTIFIER) {
            str += " ";
            str += (std::string) identifier;
        }

        if (flags & (uint8_t) CommandFlags::ARG)
        {
            str += " ";
            str += (std::string) arg;
        }
        return str;
    }

    /* To meet minimum requirements, both the action and object must
     * be set.
     */
    inline bool valid()
    {
        return (flags & (uint8_t) CommandFlags::ACTION) &&
            (flags & (uint8_t) CommandFlags::OBJECT);
    }

    inline bool isReversed() const { return reversed;}

    inline bool test (CommandFlags flag) const {
        return flags & (uint8_t) flag; 
    }

private:
    bool reversed = false;
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
    MoguSyntax r_object = MoguSyntax::__NONE__;

    /*!\brief Most objects will require some sort of identifier, but not all.
     *  The 'self' (or 'own') object is its own identifier, and refers to the
     *  widget that was passed into this class. 'app' refers to the specific
     *  application instance (although this may be deprecated). This information
     *  is not needed when passing information into the CommandValue, but is
     *  useful when extracting data from it.
     */
    NodeValue identifier;
    NodeValue r_identifier;

    /*!\brief Some objects will contain some sort of argument. For instance,
     * hash or list nodes will have an argument representing the location
     * within that databse node that is to be interacted with. Widgets may
     * have a state argument which will be altered by this command.
     */
    NodeValue arg;
    NodeValue r_arg;

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
