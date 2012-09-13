/*
 * E_Widgets.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef E_WIDGETS_H_
#define E_WIDGETS_H_

/*!\brief The types of widgets creatable with Goo. */

namespace BitMasks{
enum BitMasks{
	widget_accepts_children		=0x1,
	widget_has_content			=0x2,
	widget_usually_clicked		=0x4,
	widget_accepts_input		=0x8,
	widget_write_permissions	=0x40,
	widget_is_dynamic			=0x80
};

}

namespace WidgetTypes{
enum WidgetTypes {
    container			=0x0,
    stack				=0x1,
    text				=0x2,
    image				=0x3,
    link				=0x4,
    image_link			=0x5,
    button				=0x7,
    input_text			=0x8,
    textarea			=0x9,
    dropdown_menu		=0xA,
    radio_button_menu	=0xB,
    checkbox_menu		=0xC,
    multi_select		=0xD,
    password			=0xE,
    template_group		=0x10

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
    is_dynamic			=0x80
};

enum States {
	num_children		=0x1
	,registered_name	=0x2
	,current_index		=0x3
	,is_hidden			=0x4
};
} //namespace WidgetTypes


#endif /* E_WIDGETS_H_ */
