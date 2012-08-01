#ifndef ENUMS_H_
#define ENUMS_H_

#include <stdint.h>
typedef uint8_t byte;

namespace Enums{

/*! \brief An enum describing the options available in the side menu
 * for the Financial Firsts portal.
 */
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

/*!\brief The signals that can be transmitted from one Moldable widget
 * to another.
 */
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


// TODO Why is this here? This should not be in this namespace!
enum Properties
{
    has_children        =0x1,
    has_events          =0x2,
    is_stacked          =0x4,
    has_animation       =0x8,
    blocks_actions      =0x10,
    is_named			=0x20
};
} //namespace SignalTypes

namespace SignalActions{
const byte NUM_ACTIONS = 5;
enum SignalAction{
    NO_ACTION			=0x0,
    rebroadcast			=0x1,
    trickle				=0x2,
    bubble				=0x3,
    set_style			=0x4,
    add_class			=0x5,
    remove_class		=0x6,
    set_index			=0x8,
    increment_index		=0x9,
    decrement_index		=0xA,
    add_widget			=0xB,
    set_internal_path	=0xC,
    remove_child		=0xD,
    delete_child		=0xE,
    clear				=0xF,
    store_plain			=0x10,
    increment_plain		=0x11,
    store_enc			=0x18,
    increment_enc		=0x19,
    delete_value		=0x1F

};
}

namespace SignalTriggers{
enum SignalTrigger{
    click,
    action_name,
    style_changed,
    mouseover,
    mouseout,
};
} //namespace SignalTriggers

namespace NodeValueTypes{
    /*!\brief In the database, values which should be interpreted
     * as hash fields should be appear [like this]
     */
    const char FIELD_CONTAINER[2] = {'[',']'};

    /*!\brief In the database, values which should be interpreted
     * as integers should appear ^like this^
     */
    const char INTEGER_CONTAINER[2] = {'^','^'};

    /*!\brief In the database, values which should be interpreted
     * as enumerated types should appear {like this}
     */
    const char ENUM_CONTAINER[2] = {'{','}'};

    /*!\brief In the database, values which should be interpreted
     * as static nodes should appear <like this>.
     */
    const char STATIC_NODE_CONTAINER[2] = {'%','%'};

    /*!\brief In the database, values which should be interpreted
     * as dynamic nodes should appear @like this@
     */
    const char DYNAMIC_NODE_CONTAINER[2] = {'@','@'};

    /*!\brief In the database, values which should be interpreted
     * as floating point values should appear ~like this~
     */
    const char FLOAT_CONTAINER[2] = {'~','~'};

    /*!\brief In the database, values which should be interpreted as
     * pointers to a file in the fileststem should appear
     * `like/this/`
     */
    const char FILE_CONTAINER[2] = {'`','`'};

    /*!\brief In the database, values can instead use the enumerated
     * type name instead of any of the above notations by placing
     * the $ symbol with the enum name as the first part:
     * $field_value, $enum_value, etc.
     */
    const char ENUM_REPR_CAST = '$';

    /*!\brief In the database, values can instead use the actual
     * number representation of the enumerated type instead of
     * any of the above notations by placing the # symbol with
     * the integral value as the first part of the value:
     * #1, #2, #0x1, #0x2, etc.
     */
    const char ENUM_INT_CAST = '#';

enum NodeValueTypes{
    /*!\brief When read in from the database, string values are interpreted
     * exactly as they are read.
     */
    string_value        =0x0,//!< string_value

    /*!\brief When read in from the database, field values are attributed
     * to a field in the node being read.
     */
    field_value         =0x1,//!< field_value

    /*!\brief When read in from the database, integer values are transformed
     * into integers.
     */
    integer_value       =0x2,//!< integer_value

    /*!\brief When read in from the database, enum values are parsed with a
     * callback that returns an integer representation of the enumerated type.
     */
    enum_value          =0x3,//!< enum_value

    /*!\brief When readin from the database, the value from another node
     * in the database is read in.
     */
    static_node_value   =0x5,

    /*!\brief When read in from the database, dynamic node values themselves
     * contain variables; these are used mainly to access values specific to
     * a given user. This functionality has not yet been implemented.
     */
    dynamic_node_value  =0xD,//!< dynamic_node_value

    /*!\brief When read in from the database, float_values are parsed as
     * floats.
     */
    float_value         =0x12,//!< float_value

    /*!\brief When read in from the datbase, file_values will attempt to open
     * a file at the listed location, and will read the contents of the file.
     */
    file_value          =0x18
};
} //namespace NodeValueTypes

namespace WidgetTypeFlags{
enum WidgetTypeFlags {
    is_link         =0x1, //!< is_link
    is_image        =0x2, //!< is_image
    is_input        =0x4, //!< is_input
    has_options     =0x8, //!< has_options
    is_select       =0x10,//!< is_select
    is_multi        =0x20,//!< is_multi
    is_freeform	    =0x40 //!< is_freeform
};
} //namespace WidgetTypeFlags

/*!\brief The types of widgets creatable with Goo. */
namespace WidgetTypes{
enum WidgetTypes {
    text                =0x0,
    link                =0x1,
    image               =0x2,
    image_link          =0x3,
    submit_button       =0x4,
    submit_image        =0x6,
    radio_button_menu   =0xC,
    input_text          =0x44,
    text_area           =0x64,
    dropdown_menu       =0x1C,
    checkbox_menu       =0x2C,
    multi_select        =0x3C,
    generic_container   =0x40,
    stacked_container   =0x60
};
} //namespace WidgetTypes

/*!\brief The flags associated with the
 * style keys.
 */

namespace StyleDirectivesFlags{
enum StyleDirectivesFlags {
    is_node             =0x1,
    is_positioning      =0x2,
    is_content          =0x4,
    is_external         =0x8,
    is_uri              =0x10,
    is_event            =0x20,
    is_widget_type      =0x40
};
} //namespace StyleDirectivesFlags

/*!\brief The style keys available to Goo */
namespace StyleDirectives{
enum StyleDirectives {
    css_class           =0x0,
    children            =0x1,
    content_alignment   =0x2,
    text_content        =0x4,
    content_dimensions  =0x6,
    text_file           =0xC,
    source              =0x14,
    location            =0x18,
    events              =0x21,
    widget_type         =0x40,
    content_node        =0x5,
    internal_path       =0xF
};
} //namespace StyleDirectives

/*! \brief Possible states of the hashable answer properties */
namespace HashableAnswer_ErrorFlags{
enum HashableAnswer_ErrorFlags {
    is_hashable,                    // A value of 0 means no error flags!
    err_no_hashable_data,           // Only the database was given
    err_no_hashable_question,       // Not enough question data given
    err_no_hashable_form,           // Not enough form data given
    err_no_hashable_answer,         // Not enough answer data given
    err_no_database_selected,       // No database data given
    err_hash_already_set,           // Don't hash a pre-hashed hash!
    err_unknown                     // What did you do?!
};
} //namespace HashableAnswer_ErrorFlags

namespace Labels{
enum Labels
{
    message,
    trigger,
    action,
    nextAction,
    listeners,
    degradation,
    signal,
    animation,
    block
};

}//namespace Labels;

} // <-- namespace Enums

#endif // ENUMS_H_
