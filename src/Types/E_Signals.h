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
enum _Family
{
    self = 0,
    parent = 1,
    children = 2,
    siblings = 3,
    child = 4,
    sibling = 5,
    application = 6
};
}    //namespace Family

namespace SignalActions {
enum SignalAction
{
    NO_ACTION = 0x0

    /*!\deprecated */
    ,
    rebroadcast = 0x1

    /*!\deprecated */
    ,
    trickle = 0x2

    /*!\brief Send a signal upward
     * listener: The final nuclear reaction (self,parent, etc...)
     * message: The final message
     * degradation: required
     */
    ,
    bubble = 0x3

    /*!\brief Change the style of the listener
     * listener: A Moldable widget
     * message: The css style list "foo bar baz"
     */
    ,
    set_style = 0x4

    /*!\brief Append a style class to the listener
     * listener: A moldable widget
     * message: The css style "foo"
     */
    ,
    add_class = 0x5

    /*!\brief Remove a style class from the listener
     * listener: A moldable widget
     * message: The css style "foo"
     */
    ,
    remove_class = 0x6

    /*!\brief Emit another signal
     * listener: A moldable widget
     * message: The signal (e.g. "click")
     */
    ,
    emit = 0x7

    /*!\brief Change the index of a stacked widget
     * listener: A stacked widget
     * message: The number to set it to.
     */
    ,
    set_index = 0x8

    /*!\brief Increase a stacked widget by 1.
     * listener: A stacked widget
     * message: NONE
     */
    ,
    increment_index = 0x9

    /*!\brief Decrease a stacked widget idnex by 1.
     * listener: A stacked widget
     * message: NONE
     */
    ,
    decrement_index = 0xA

    /*!\brief Creates and adds a new widget to the listener widget
     * listener: A container widget
     * message: The node contaning the template information
     */
    ,
    add_widget = 0xB

    /*!\brief Changes the internal path to be displayed in the users'
     * browser bar.
     * message: The path name
     * listener: NONE
     */
    ,
    set_internal_path = 0xC,
    remove_child = 0xD,
    delete_child = 0xE,
    clear = 0xF,
    store_value = 0x10,
    increment_value = 0x11,
    change_session = 0x12,
    slot = 0x13,
    register_user = 0x14,
    match = 0x15,
    test_text = 0x16,
    store_abstract = 0x17,
    delete_value = 0x1F,
    set_text = 0x20,
    reload = 0x21,
    read_state = 0x22,
    javascript = 0x40,
    email_user = 0x41,
    reset_password = 0x42,
    onload = 0x43,
    activate_global = 0x44,
    change_group = 0x45,
    add_self_to_group = 0x46,
    add_other_to_group = 0x47,
    STDOUT = 0xFD,
    TERM = 0xFE,
    BLOCK = 0xFF
};
}    //namespace SignalActions

namespace SignalTriggers {
enum SignalTrigger
{
    click = 0x1,
    style_changed = 0x2,
    hidden_changed = 0x3,
    onload = 0x5,
    mouseover = 0x4,
    mouseout = 0x8,
    fail = 0x10,
    succeed = 0x20,
    keyup = 0x40,
    enter_pressed = 0x80,
    index_changed = 0x100
};
}    //namespace SignalTriggers

namespace Labels {
enum Labels
{
    message = 0,
    trigger = 1,
    action = 2,
    nextAction = 3,
    listeners = 4,
    degradation = 5,
    signal = 6,
    animation = 7,
    block = 8,
    interrupt = 9
};
}    //namespace Labels

#endif /* E_SIGNALS_H_ */
