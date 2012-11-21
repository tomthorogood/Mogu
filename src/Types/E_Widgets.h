/*
 * E_Widgets.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef E_WIDGETS_H_
#define E_WIDGETS_H_

/*!\brief The types of widgets creatable with Mogu. */

namespace WidgetTypes{
enum WidgetTypes {
    container			=0x0,
    stack				=0x1,
    text				=0x2,
    image				=0x3,
    link				=0x4,
    image_link			=0x5,
    mediaplayer         =0x6,
    button				=0x7,
    input_text			=0x8,
    textarea			=0x9,
    dropdown_menu		=0xA,
    radio_button_menu	=0xB,
    checkbox_menu		=0xC,
    multi_select		=0xD,
    password			=0xE,
    foreach				=0x10,
    memberlist			=0x11
};

enum Properties
{
    has_children        =0x1,
    has_events          =0x2,
    is_stacked          =0x4,
    has_animation       =0x8,
    blocks_actions      =0x10,
    is_named			=0x20,
    is_validated		=0x40,
    is_dynamic			=0x80,
    property_txfr		=0x100,
    has_tooltip			=0x200
};

enum States {
	num_children		=0x1
	,registered_name	=0x2
	,current_index		=0x3
	,is_hidden			=0x4
	,value				=0x5
	,currentstyle		=0x6
};
} //namespace WidgetTypes


#endif /* E_WIDGETS_H_ */
