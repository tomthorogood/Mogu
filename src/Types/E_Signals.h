/*
 * E_Signals.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef E_SIGNALS_H_
#define E_SIGNALS_H_

typedef uint8_t byte;

namespace Family {
enum _Family {
    self            =0,
    parent          =1,
    children        =2,
    siblings        =3,
    child           =4,
    sibling         =5,
    application		=6
};
} //namespace Family

namespace SignalTypes{
enum SignalType{
    /*!\brief Propagated relays are signals that are triggered by
     * other signals, and only carry information about the next signal node
     * in the database that will be read from.
     *
     * These signal types are useful when one widget needs to trigger an
     * unrelated widget to  do something independent of the calling widget's
     * state.
     */
    propagated_relay               =0xCA,

    /*!\brief Catalyzed relay types arethose that are triggered by actual
     * Wt::Signal emissions. These are useful when one widget is not related
     * to the receiver in the tree, but they are related by some common
     * functionality or UI style.
     */
    catalyzed_relay                 =0xCB,

    /*!\brief A propagated targeted reaction carries a payload across many
     * widgets in one direction of the widget tree; upon degradation, the
     * receiving widget(s) carry out the next action.
     */
    propagated_targeted_reaction    =0xC6,
    /*!\brief A catalyzed targeted reaction is launched from an on-page event,
     * and carries its payload in one direction across many widgetsl upon
     * degradation, the receiving widget(s) carry out the next action.
     */
    catalyzed_targeted_reaction     =0xC7,

    /*!\brief A propagated nuclear event is one that originates from a relay
     * signal, and when activated causes its listeners (which can be any of its
     * nuclear family or itself) to take the associated action.
     */
    propagated_nuclear_event        =0xD0,

    /*!\brief A catalzed nuclear event is the most basic of all signals;
     * after an on-page event takes place, the broadcasting widget transmits
     * its payload to any of its nuclear family (or itself) and carries out
     * the specified action.
     */
    catalyzed_nuclear_event         =0xD1,

    /*!\brief A propagated nuclear reaction is similar to a propagated
     * targeted reaction, except that when the payload is delivered, it
     * can be delivered to any of the receiving widget(s)'s nuclear family.
     * A propagated targeted reaction is in fact the same as the propagated
     * nuclear reaction, if the 'listener' is 'self'.
     */
    propagated_nuclear_reaction     =0xD6,

    /*!\brief A catalyzed nuclear reaction is similar to a catalyzed targeted
     * reaction, except that when the payload is delivered, it can be delivered
     * to any of the receiving widget(s)'s nuclear family.
     * A catalyzed targeted reaction is in fact the same as this, if the
     * 'listener' is 'self'.
     */
    catalyzed_nuclear_reaction      =0xD7,

    /*!\brief A propagated nuclear relay is the same as a propagated relay,
     * except nuclear family listeners of the final node can be suggested. Each
     * of these listeners will act as the broadcaster of the next message,
     * so this can have huge side effects if not handled with care.
     */
    propagated_nuclear_relay        =0xDA,

    /*!\brief A catalyzed nuclear relay is the same as a catalyzed relay,
     * except nuclear family listeners of the final node can be suggested. Each
     * of these listeners will act as the broadcaster of the next message, so
     * this can have huge side effects if not handled with care.
     */
    catalyzed_nuclear_relay         =0xDB,

    /*!\brief A catalyzed contracted event is one that is a much simpler signal
     * to program, and much more efficient to perform, at the cost of having
     * the target widget's information stored persistently in memory. Instead of
     * a `listeners` field, it has a `name` field, and the setup is thus
     * exactly the same as with perspective molding.
     */
    catalyzed_contracted_event		=0xE1,

    /*!\brief A catalyzed contracted relay is similar to a contracted event,
     * but instructs the named widget to broadcast another signal. This can be
     * an efficient means of broadcasting to specific region of the widget tree.
     */
    catalyzed_contracted_relay		=0xE9
};

enum SignalTypeBits
{
    catalyst            =0x1,
    signal_repeats      =0x2,
    transforms          =0x4,
    pointer             =0x8,
    specific_listeners  =0x10,
    registered_listener =0x20,
    carries_payload     =0x40,
    has_effect          =0x80
};

} //namespace SignalTypes

namespace SignalActions{
	const byte NUM_ACTIONS = 19;
enum SignalAction{
    NO_ACTION			=0x0
    ,rebroadcast		=0x1
    ,trickle			=0x2
    ,bubble				=0x3
    ,set_style			=0x4
    ,add_class			=0x5
    ,remove_class		=0x6
    ,set_index			=0x8
    ,increment_index	=0x9
    ,decrement_index	=0xA
    ,add_widget			=0xB
    ,set_internal_path	=0xC
    ,remove_child		=0xD
    ,delete_child		=0xE
    ,clear				=0xF
    ,store_value		=0x10
    ,increment_value	=0x11
    ,change_session		=0x12
    ,slot				=0x13
    ,register_user		=0x14
    ,match				=0x15
    ,test_text			=0x16
    ,store_abstract		=0x17
    ,delete_value		=0x1F
    ,set_text			=0x20
    ,reload				=0x21
    ,read_state			=0x22
    ,javascript			=0x40
    ,email_user			=0x41
    ,reset_password		=0x42
    ,STDOUT				=0xFD
    ,TERM				=0xFE
    ,BLOCK				=0xFF
};
}//namespace SignalActions


namespace SignalTriggers{
enum SignalTrigger{
    click			=0x1
    ,style_changed	=0x2
    ,hidden_changed	=0x3
    ,mouseover		=0x4
    ,mouseout		=0x8
    ,fail			=0x10
    ,succeed		=0x20
    ,keyup			=0x40
    ,enter_pressed	=0x80
    ,index_changed	=0x100
    ,debugged		=0x200
};
} //namespace SignalTriggers

namespace Labels{
enum Labels
{
    message			=0
    ,trigger		=1
    ,action			=2
    ,nextAction 	=3
    ,listeners		=4
    ,degradation 	=5
    ,signal			=6
    ,animation		=7
    ,block			=8
    ,interrupt		=9
};
}//namespace Labels


#endif /* E_SIGNALS_H_ */
