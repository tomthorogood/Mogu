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
#include <Parsers/NodeValueParser.h>
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

    uint8_t set(const CommandFlags& flag, NodeValue v);
    uint8_t set(const CommandFlags& flag, const MoguSyntax v);
    NodeValue get ( CommandFlags flag) const ;


    inline const std::string& getIdentifier() const 
        { return identifier.getString();}
    inline const uint8_t& getFlags() const        { return flags;}
    inline Moldable& getWidget()  { return broadcaster;}
    
    std::string stitchState(bool reverse=false);

    inline bool isReversed() const { return reversed;}

    inline bool test (CommandFlags flag) const {
        return flags & (uint8_t) flag; 
    }

    bool objectIsReduceable(bool reverse_object=false);
    inline void reduceObject(bool reverse=false, Moldable* broadcaster=nullptr)
    {
        Parsers::NodeValueParser nvp;
        nvp.giveInput(stitchState(reverse), value);
        set(CommandFlags::VALUE, value);

        CommandFlags arg_flag = reverse ? CommandFlags::R_ARG : CommandFlags::ARG;
        CommandFlags obj_flag = reverse ? CommandFlags::R_OBJECT : CommandFlags::OBJECT;
        CommandFlags id_flag  = reverse ? CommandFlags::R_IDENTIFIER : CommandFlags::IDENTIFIER;

        if (test(arg_flag)) flags -= (uint8_t) arg_flag;
        if (test(obj_flag)) flags -= (uint8_t) obj_flag;
        if (test(id_flag)) flags -= (uint8_t) id_flag;

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
