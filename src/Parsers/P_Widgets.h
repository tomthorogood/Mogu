/*
 * P_Widgets.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef P_WIDGETS_H_
#define P_WIDGETS_H_

#include <declarations.h>
#include <Parsers/TokenGenerator.h>

namespace Parsers{

class BitMaskParser :
	public TurnLeft::Utils::EnumParser <Enums::BitMasks::BitMasks>
{
public:
	BitMaskParser()
	{
		namespace Mask = Enums::BitMasks;
		enumMap["dynamic"]			= Mask::widget_is_dynamic;
		enumMap["rw"]				= Mask::widget_write_permissions;
	}
};

class WidgetTypeParser :
	public TurnLeft::Utils::EnumParser <Enums::WidgetTypes::WidgetTypes>
{
public:
	WidgetTypeParser()
	{
	    namespace WidgetTypes = Enums::WidgetTypes;
	    enumMap["text"]            = WidgetTypes::text;
	    enumMap["link"]            = WidgetTypes::link;
	    enumMap["image"]           = WidgetTypes::image;
	    enumMap["image link"]      = WidgetTypes::image_link;
	    enumMap["image_link"]      = WidgetTypes::image_link;
	    enumMap["submit button"]   = WidgetTypes::button;
	    enumMap["submit"]          = WidgetTypes::button;
	    enumMap["radio button"]    = WidgetTypes::radio_button_menu;
	    enumMap["text area"]       = WidgetTypes::textarea;
	    enumMap["textarea"]        = WidgetTypes::textarea;
	    enumMap["input"]           = WidgetTypes::input_text;
	    enumMap["input text"]      = WidgetTypes::input_text;
	    enumMap["dropdown menu"]   = WidgetTypes::dropdown_menu;
	    enumMap["dropdown"]        = WidgetTypes::dropdown_menu;
	    enumMap["checkbox menu"]   = WidgetTypes::checkbox_menu;
	    enumMap["checkbox"]        = WidgetTypes::checkbox_menu;
	    enumMap["multi select"]    = WidgetTypes::multi_select;
	    enumMap["container"]       = WidgetTypes::container;
	    enumMap["password"]			= WidgetTypes::password;
	    enumMap["stacked"]         = WidgetTypes::stack;
	    enumMap["stack"]			= WidgetTypes::stack;
	    enumMap["foreach"]			= WidgetTypes::foreach;
	}

	virtual Enums::WidgetTypes::WidgetTypes parse(const std::string& val)
	{
		std::string val_cpy = val;
		TokenGenerator tgen(val_cpy);
		return TurnLeft::Utils::EnumParser
				<Enums::WidgetTypes::WidgetTypes>::parse(tgen.next('|'));

	}

};

class WidgetStateParser:
	public TurnLeft::Utils::EnumParser<Enums::WidgetTypes::States>
{
public:
	WidgetStateParser()
	{
		using namespace Enums::WidgetTypes;
		enumMap["current_index"]  	= current_index;
		enumMap["num_children"] 	= num_children;
		enumMap["name"] 			= registered_name;
		enumMap["hidden"] 			= is_hidden;
		enumMap["value"]			= value;
	}
};
}//namespace Parsers

#endif /* P_WIDGETS_H_ */
